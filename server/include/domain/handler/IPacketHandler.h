#pragma once

#include "network/Protocol.h"
#include "domain/client/ClientManager.h"

class Server;

class IPacketHandler
{
public:
    virtual ~IPacketHandler() = default;
    virtual void handle(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager) = 0;
};
