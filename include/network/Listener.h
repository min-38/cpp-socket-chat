#pragma once

#include "utils/pch.h"

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
    int acceptConnection();

    int getPort() const { return m_port; }
    int getFd() const { return m_socket_fd; }

    void close();

private:
    int m_port;
    int m_socket_fd;
};
