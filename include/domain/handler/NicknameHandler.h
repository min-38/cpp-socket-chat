#pragma once

#include "IPacketHandler.h"

class NicknameHandler : public IPacketHandler
{
public:
    void handle(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager) override;
};
