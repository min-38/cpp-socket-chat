#pragma once

#include "IPacketHandler.h"

class RoomHandler : public IPacketHandler
{
public:
    void handle(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager) override;

private:
    void handleCreateRoom(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager);
    void handleExitRoom(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager);
    void handleJoinRoom(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager);
};
