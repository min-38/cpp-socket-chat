#include "network/PacketReceiver.h"
#include "logger/Logger.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

PacketReceiver::PacketReceiver(ClientManager& client_manager) : m_client_manager(client_manager) {}

bool PacketReceiver::receive(int client_fd)
{
    ClientInfo* client = m_client_manager.getClient(client_fd);
    if (!client)
        return false;

    char buffer[RECV_BUFFER_SIZE];
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer), 0);

    Logger::info("Received " + std::to_string(bytes) + " bytes from client " + std::to_string(client_fd));

    // 연결 종료 또는 에러
    if (bytes <= 0)
        return false;

    // 수신 버퍼에 추가
    client->recvBuffer.insert(client->recvBuffer.end(), buffer, buffer + bytes);
    return true;
}

void PacketReceiver::parsePackets(int client_fd, std::function<void(int fd, const Packet&)> callback)
{
    // 1. 클라이언트 정보 가져오기
    ClientInfo* client = m_client_manager.getClient(client_fd);
    if (!client)
        return;

    // 2. 버퍼에서 완전한 패킷 파싱
    while (client->recvBuffer.size() >= sizeof(PacketHeader))
    {
        PacketHeader* header = reinterpret_cast<PacketHeader*>(client->recvBuffer.data());
        uint16_t payload_size = ntohs(header->size);
        size_t packet_size = sizeof(PacketHeader) + payload_size;

        Logger::info("Parsing packet of size " + std::to_string(packet_size) + " from client " + std::to_string(client_fd));

        // 아직 전체 패킷이 도착하지 않음
        if (client->recvBuffer.size() < packet_size)
            break;

        Logger::info("Received complete packet from client " + std::to_string(client_fd));

        // 패킷 복사 및 변환
        Packet packet;
        memcpy(&packet, client->recvBuffer.data(), packet_size);
        packet.header.type = (PacketType)ntohs((uint16_t)packet.header.type);
        packet.header.size = payload_size;

        // 콜백 호출
        callback(client_fd, packet);

        // 처리된 패킷 제거
        client->recvBuffer.erase(client->recvBuffer.begin(), client->recvBuffer.begin() + packet_size);
    }
}

bool PacketReceiver::receiveAndParse(int client_fd, std::function<void(int fd, const Packet&)> callback)
{
    if (!receive(client_fd))
        return false;

    parsePackets(client_fd, callback);
    return true;
}
