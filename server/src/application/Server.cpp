#include "application/Server.h"
#include "config/Env.h"
#include "logger/Logger.h"

Server::Server() : m_port(8877)
{
    std::string envServerPort = Env::getInstance().get("SERVER_PORT");
    if (!envServerPort.empty())
        m_port = stoi(envServerPort);

    // 컴포넌트 초기화
    m_listener = std::make_unique<Listener>(m_port);
    m_epoll_manager = std::make_unique<EpollManager>();
    m_connection_manager = std::make_unique<ConnectionManager>(*m_epoll_manager, m_client_manager);
    m_packet_receiver = std::make_unique<PacketReceiver>(m_client_manager);
    m_packet_sender = std::make_unique<PacketSender>(m_client_manager);
}

Server::~Server()
{
    m_epoll_manager->stop();

    if (m_io_thread.joinable())
        m_io_thread.join();

    if (m_listener)
        m_listener->close();

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

    if (!m_epoll_manager->initialize())
    {
        Logger::error("EpollManager initialization failed.");
        return;
    }

    // Listener fd를 epoll에 추가
    if (!m_epoll_manager->addFd(m_listener->getFd(), EPOLLIN))
    {
        Logger::error("Failed to add listener fd to epoll.");
        return;
    }

    m_io_thread = std::thread(&Server::eventLoop, this);

    Logger::info("Server started on port: " + std::to_string(m_port));
    Logger::info("Using " + std::to_string(num_threads) + " worker threads.");

    if (m_io_thread.joinable())
        m_io_thread.join();
}

void Server::eventLoop()
{
    // 이벤트 루프 시작
    m_epoll_manager->wait([this](int fd, uint32_t events) {
        handleEvent(fd, events);
    });
}

void Server::handleEvent(int fd, uint32_t events)
{
    // Listener fd인 경우: 새 연결 수락
    if (fd == m_listener->getFd())
    {
        m_connection_manager->acceptNewConnection(*m_listener);
        return;
    }
    
    // 클라이언트 fd인 경우: 데이터 수신 및 처리
    bool success = m_packet_receiver->receiveAndParse(fd, [this](int client_fd, const Packet& packet){
        // 패킷 처리를 스레드풀에서 실행
        m_thread_pool->enqueue([this, client_fd, packet] {
            m_packet_handler.handle(*this, client_fd, packet, m_client_manager);
        });
    });

    // 연결 종료
    if (!success)
        m_connection_manager->closeConnection(fd);
}

void Server::sendPacket(int client_fd, const Packet& packet)
{
    m_packet_sender->send(client_fd, packet);
}

void Server::broadcastPacket(const Packet& packet, int exclude_fd)
{
    m_packet_sender->broadcast(packet, exclude_fd);
}

void Server::broadcastPacketForRoom(const Packet& packet, const std::string& roomCode, int exclude_fd)
{
    // 방 찾기
    Room* room = m_room_manager.getRoom(roomCode);
    if (!room) return; // 방이 없으면 종료

    // 방에 있는 모든 클라이언트에게 전송
    std::vector<int> clientFds = room->getClientFds();
    for (int fd : clientFds)
        if (fd != exclude_fd)
            m_packet_sender->send(fd, packet);
}