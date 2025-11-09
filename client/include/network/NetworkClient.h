#pragma once

#include "network/INetworkClient.h"
#include "network/Protocol.h"
#include "common/pch.h"

class NetworkClient : public INetworkClient
{
public:
    NetworkClient();
    ~NetworkClient() override;

    bool Connect(const std::string& host, int port) override;
    bool Send(const Packet& packet) override;
    bool Receive(Packet& packet) override;
    void Close() override;
    bool IsConnected() const override;

private:
    int m_sockfd;
    bool m_connected;
    static const int BUFFER_SIZE = 4096;
};
