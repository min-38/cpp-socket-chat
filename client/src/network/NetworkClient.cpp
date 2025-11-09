#include "network/NetworkClient.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

NetworkClient::NetworkClient() : m_sockfd(-1), m_connected(false) {}

NetworkClient::~NetworkClient()
{
    if (m_connected)
        Close();
}

bool NetworkClient::Connect(const std::string& host, int port)
{
    m_sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (m_sockfd < 0)
    {
        perror("소켓 생성 실패");
        m_connected = false;
        return false;
    }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(host.c_str());

    if (connect(m_sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("서버 연결 실패");
        ::close(m_sockfd);
        m_connected = false;
        return false;
    }

    m_connected = true;
    return true;
}

bool NetworkClient::Receive(Packet& packet)
{
    PacketHeader header;
    ssize_t bytes_received = recv(m_sockfd, &header, sizeof(PacketHeader), MSG_WAITALL);

    if (bytes_received != sizeof(PacketHeader))
    {
        if (m_connected)
        {
            if (bytes_received == 0)
                std::cout << "서버와의 연결이 종료되었습니다." << std::endl;
            else
                perror("recv header error");
            Close();
        }
        return false;
    }

    header.type = (PacketType)ntohs((uint16_t)header.type);
    header.size = ntohs(header.size);
    packet.header = header;

    if (header.size > 0)
    {
        if (header.size > MAX_PAYLOAD_SIZE)
        {
            std::cerr << "Error: Received oversized payload. size=" << header.size << std::endl;
            Close();
            return false;
        }

        bytes_received = recv(m_sockfd, packet.payload, header.size, MSG_WAITALL);
        if (bytes_received != header.size)
        {
            perror("recv payload error");
            Close();
            return false;
        }
    }

    return true;
}

bool NetworkClient::Send(const Packet& packet)
{
    if (!m_connected)
        return false;

    if (packet.header.size > MAX_PAYLOAD_SIZE)
    {
        std::cerr << "Payload 크기가 너무 큽니다: " << packet.header.size
                  << " (최대 " << MAX_PAYLOAD_SIZE << " 바이트)" << std::endl;
        return false;
    }

    // Logger::debug("Sending packet of type " + std::to_string(static_cast<uint16_t>(packet.header.type)) +
    //               " with size " + std::to_string(packet.header.size) + " bytes.");

    Packet send_packet = packet;
    send_packet.header.type = (PacketType)htons((uint16_t)packet.header.type);
    send_packet.header.size = htons(packet.header.size);

    const char* buffer = reinterpret_cast<const char*>(&send_packet);
    size_t total_send_size = sizeof(PacketHeader) + packet.header.size;
    size_t total_sent_size = 0;

    while (total_sent_size < total_send_size)
    {
        ssize_t sent_bytes = send(m_sockfd, buffer + total_sent_size,
                                   total_send_size - total_sent_size, 0);
        if (sent_bytes == -1)
        {
            perror("send error");
            return false;
        }
        total_sent_size += sent_bytes;
    }

    return true;
}

void NetworkClient::Close()
{
    if (m_connected)
    {
        ::close(m_sockfd);
        m_connected = false;
    }
}

bool NetworkClient::IsConnected() const
{
    return m_connected;
}
