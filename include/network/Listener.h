#pragma once

#include "common/pch.h"
#include <string>

struct ConnectionInfo
{
    int fd = -1;
    std::string ip;
    int port = 0;

    bool isValid() const { return fd > 0; }
};

class Listener
{
public:
    explicit Listener(int port);
    ~Listener();

    // 복사, 이동 금지
    Listener(const Listener&) = delete;
    Listener& operator=(const Listener&) = delete;
    Listener(Listener&&) = delete;
    Listener& operator=(Listener&&) = delete;

    bool initialize();
    ConnectionInfo acceptConnection();

    int getPort() const { return m_port; }
    int getFd() const { return m_socket_fd; }

    void close();

private:
    int m_port;
    int m_socket_fd;
};
