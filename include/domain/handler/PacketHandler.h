#pragma once

#include "IPacketHandler.h"
#include <unordered_map>
#include <memory>

class Server;

class PacketHandler
{
public:
    PacketHandler();
    void handle(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager);

private:
    std::unordered_map<PacketType, std::unique_ptr<IPacketHandler>> m_handlers;
};
