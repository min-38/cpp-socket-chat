#pragma once

#include "domain/client/ClientManager.h"
#include "network/Protocol.h"

class PacketSender
{
public:
    PacketSender(ClientManager& client_manager);

    // 단일 클라이언트에게 패킷 전송
    void send(int client_fd, const Packet& packet);

    // 인증된 모든 클라이언트에게 브로드캐스트 (exclude_fd 제외)
    void broadcast(const Packet& packet, int exclude_fd = -1);

private:
    ClientManager& m_client_manager;
};
