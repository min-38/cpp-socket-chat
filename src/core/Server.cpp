#include "core/Server.h"

#include "utils/Env.h"
#include "utils/Logger.h"

#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <algorithm>
#include <vector>
#include <ctime>
#include <fcntl.h> // fcntl 함수를 사용하기 위해 추가

#define MAX_EVENTS 64
#define RECV_BUFFER_SIZE 4096

Server::Server() : m_port(8877), m_epoll_fd(-1), m_is_running(false)
{
    std::string envServerPort = Env::getInstance().get("SERVER_PORT");
    if (!envServerPort.empty())
        m_port = stoi(envServerPort);

    m_listener = std::make_unique<Listener>(m_port);
}

Server::~Server()
{
    m_is_running = false;

    if (m_io_thread.joinable())
        m_io_thread.join();

    if (m_listener)
        m_listener->close();

    if (m_epoll_fd != -1)
        close(m_epoll_fd);
    
    Logger::info("Server has shut down gracefully.");
}

void Server::run()
{
    size_t num_threads = std::thread::hardware_concurrency();
    m_thread_pool = std::make_unique<ThreadPool>(num_threads);

    if (!m_listener->initialize())
    {
        Logger::error("Listener initialization failed.");
        return;
    }

    m_epoll_fd = epoll_create1(0);
    if (m_epoll_fd == -1)
    {
        Logger::error("Failed to create epoll instance.");
        return;
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = m_listener->getFd();
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listener->getFd(), &event) == -1)
    {
        Logger::error("Failed to add listener fd to epoll.");
        return;
    }

    m_is_running = true;
    m_io_thread = std::thread(&Server::eventLoop, this);

    Logger::info("Server started on port: " + std::to_string(m_port));
    Logger::info("Using " + std::to_string(num_threads) + " worker threads.");

    if (m_io_thread.joinable())
        m_io_thread.join();
}

void Server::eventLoop()
{
    struct epoll_event events[MAX_EVENTS];

    while (m_is_running)
    {
        int num_events = epoll_wait(m_epoll_fd, events, MAX_EVENTS, -1);
        if (num_events < 0 && errno != EINTR)
        {
            Logger::error("epoll_wait error.");
            continue;
        }

        for (int i = 0; i < num_events; ++i)
        {
            if (events[i].data.fd == m_listener->getFd())
            {
                int new_client_fd = m_listener->acceptConnection();
                if (new_client_fd > 0) {
                    // 1. 논블로킹 소켓으로 설정
                    int flags = fcntl(new_client_fd, F_GETFL, 0);
                    fcntl(new_client_fd, F_SETFL, flags | O_NONBLOCK);

                    // 2. epoll 감시 목록에 추가
                    struct epoll_event client_event;
                    client_event.events = EPOLLIN;
                    client_event.data.fd = new_client_fd;
                    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, new_client_fd, &client_event) == -1) {
                        Logger::error("Failed to add client fd to epoll.");
                        close(new_client_fd);
                    } else {
                        // 3. 클라이언트 정보 맵에 추가
                        std::lock_guard<std::mutex> lock(m_clients_mutex);
                        Logger::info("New client connected: " + std::to_string(new_client_fd));
                    }
                }
            }
            else
            {
                // 기존 클라이언트의 데이터 수신 처리
                int client_fd = events[i].data.fd;
                char buffer[RECV_BUFFER_SIZE];
                ssize_t bytes = recv(client_fd, buffer, sizeof(buffer), 0);

                if (bytes <= 0) {
                    disconnectClient(client_fd);
                    continue;
                }

                std::lock_guard<std::mutex> lock(m_clients_mutex);
                auto it = m_clients.find(client_fd);
                if (it != m_clients.end())
                {
                    ClientInfo& client = it->second;
                    client.recvBuffer.insert(client.recvBuffer.end(), buffer, buffer + bytes);

                    while (client.recvBuffer.size() >= sizeof(PacketHeader)) {
                        PacketHeader* header = reinterpret_cast<PacketHeader*>(client.recvBuffer.data());
                        uint16_t payload_size = ntohs(header->size);
                        size_t packet_size = sizeof(PacketHeader) + payload_size;

                        if (client.recvBuffer.size() < packet_size)
                            break;

                        Packet packet;
                        memcpy(&packet, client.recvBuffer.data(), packet_size);
                        packet.header.type = (PacketType)ntohs((uint16_t)packet.header.type);
                        packet.header.size = payload_size;

                        m_thread_pool->enqueue([this, client_fd, packet]{
                            processPacket(client_fd, packet);
                        });

                        client.recvBuffer.erase(client.recvBuffer.begin(), client.recvBuffer.begin() + packet_size);
                    }
                }
            }
        }
    }
}

void Server::processPacket(int client_fd, const Packet& packet)
{
    std::lock_guard<std::mutex> lock(m_clients_mutex);
    auto it = m_clients.find(client_fd);
    if (it == m_clients.end()) return;

    ClientInfo& sender_info = it->second;

    switch (packet.header.type)
    {
        case PacketType::NICKNAME_REQUEST:
        {
            std::string nickname(packet.payload, packet.header.size);
            Packet response;
            if (m_nicknames.count(nickname)) {
                response.header.type = PacketType::NICKNAME_RESPONSE_FAIL;
                const char* reason = "Duplicated nickname.";
                response.header.size = strlen(reason);
                memcpy(response.payload, reason, response.header.size);
            } else {
                sender_info.nickname = nickname;
                sender_info.isAuthenticated = true;
                m_nicknames.insert(nickname);
                response.header.type = PacketType::NICKNAME_RESPONSE_OK;
                response.header.size = 0;
                Logger::info("Client " + std::to_string(client_fd) + " set nickname to " + nickname);
            }
            sendPacket(client_fd, response);
            break;
        }
        case PacketType::MESSAGE_SEND_REQUEST:
        {
            if (!sender_info.isAuthenticated) break;

            MessageBroadcastPayload broadcast_payload;
            broadcast_payload.timestamp = time(nullptr);
            strncpy(broadcast_payload.senderIp, sender_info.ip.c_str(), IP_ADDRESS_LEN - 1);
            strncpy(broadcast_payload.senderNickname, sender_info.nickname.c_str(), MAX_NICKNAME_LEN - 1);
            
            broadcast_payload.senderIp[IP_ADDRESS_LEN - 1] = '\0';
            broadcast_payload.senderNickname[MAX_NICKNAME_LEN - 1] = '\0';
            
            size_t message_len = packet.header.size;
            size_t max_msg_len = sizeof(broadcast_payload.message);
            memcpy(broadcast_payload.message, packet.payload, std::min(message_len, max_msg_len));

            Packet broadcast_packet;
            broadcast_packet.header.type = PacketType::MESSAGE_BROADCAST;
            broadcast_packet.header.size = sizeof(broadcast_payload);
            memcpy(broadcast_packet.payload, &broadcast_payload, sizeof(broadcast_payload));

            broadcastPacket(broadcast_packet, client_fd);
            break;
        }
        default:
            Logger::warn("Unknown packet type received from " + std::to_string(client_fd));
            break;
    }
}

void Server::sendPacket(int client_fd, const Packet& packet)
{
    Packet send_packet = packet;
    send_packet.header.type = (PacketType)htons((uint16_t)packet.header.type);
    send_packet.header.size = htons(packet.header.size);

    const char* buffer = reinterpret_cast<const char*>(&send_packet);
    size_t total_to_send = sizeof(PacketHeader) + packet.header.size;
    size_t total_sent = 0;

    while (total_sent < total_to_send)
    {
        ssize_t sent_bytes = send(client_fd, buffer + total_sent, total_to_send - total_sent, 0);
        if (sent_bytes < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                continue;
            }
            Logger::error("send() error on fd " + std::to_string(client_fd) + ": " + strerror(errno));
            return;
        }
        total_sent += sent_bytes;
    }
}

void Server::broadcastPacket(const Packet& packet, int exclude_fd)
{
    std::vector<int> fds_to_send;
    {
        std::lock_guard<std::mutex> lock(m_clients_mutex);
        for (const auto& pair : m_clients) {
            if (pair.first != exclude_fd && pair.second.isAuthenticated) {
                fds_to_send.push_back(pair.first);
            }
        }
    }
    for (int fd : fds_to_send) {
        sendPacket(fd, packet);
    }
}

void Server::disconnectClient(int client_fd)
{
    std::lock_guard<std::mutex> lock(m_clients_mutex);
    auto it = m_clients.find(client_fd);
    if (it != m_clients.end()) {

        Logger::info("Client " + it->second.nickname + " (" + std::to_string(client_fd) + ") disconnected.");

        if (it->second.isAuthenticated)
            m_nicknames.erase(it->second.nickname);

        m_clients.erase(it);
    }
    epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr);

    close(client_fd);
}
