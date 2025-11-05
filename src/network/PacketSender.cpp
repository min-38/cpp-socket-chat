#include "network/PacketSender.h"
#include "logger/Logger.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include <vector>

PacketSender::PacketSender(ClientManager& client_manager) : m_client_manager(client_manager) {}

void PacketSender::send(int client_fd, const Packet& packet)
{
    // 전송 패킷 생성
    Packet send_packet = packet;
    send_packet.header.type = (PacketType)htons((uint16_t)packet.header.type);
    send_packet.header.size = htons(packet.header.size);

    const char* buffer = reinterpret_cast<const char*>(&send_packet);
    size_t total_to_send = sizeof(PacketHeader) + packet.header.size;
    size_t total_sent = 0;

    // 데이터 전송
    while (total_sent < total_to_send)
    {
        ssize_t sent_bytes = ::send(client_fd, buffer + total_sent, total_to_send - total_sent, 0);
        if (sent_bytes < 0)
        {
            // 일시적 오류 처리
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                continue;

            Logger::error("send() error on fd " + std::to_string(client_fd) + ": " + std::string(strerror(errno)));
            return;
        }
        total_sent += sent_bytes;
    }
}

void PacketSender::broadcast(const Packet& packet, int exclude_fd)
{
    std::vector<int> fds_to_send = m_client_manager.getAllAuthenticatedClientFds(exclude_fd);
    for (int fd : fds_to_send)
        send(fd, packet);
}
