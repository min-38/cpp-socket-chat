#pragma once

#include <cstdint>

struct Packet;

class INetworkClient
{
public:
    virtual ~INetworkClient() = default;

    virtual bool Connect(const std::string& host, int port) = 0;
    virtual bool Send(const Packet& packet) = 0;
    virtual bool Receive(Packet& packet) = 0;
    virtual void Close() = 0;
    virtual bool IsConnected() const = 0;
};
