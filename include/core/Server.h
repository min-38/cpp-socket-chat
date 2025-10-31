#pragma once

#include "utils/pch.h"
#include "utils/Protocol.h"

#include "network/Listener.h"

#include "utils/thread/ThreadPool.h"

#include <mutex>
#include <atomic>
#include <memory>
#include <sys/epoll.h>

struct ClientInfo
{
    int fd;
    std::string ip;
    std::string nickname;
    bool isAuthenticated = false;
    std::vector<char> recvBuffer; // TCP 스트림 처리를 위한 버퍼
};

class Server
{
public:
    Server();
    ~Server();

    // 복사, 이동 금지
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    void run();

private:
    void eventLoop();
    void processPacket(int client_fd, const Packet& packet);
    void sendPacket(int client_fd, const Packet& packet);
    void broadcastPacket(const Packet& packet, int exclude_fd = -1);
    void disconnectClient(int client_fd);

private:
    int m_port;

    int m_epoll_fd;

    std::unordered_map<int, ClientInfo> m_clients;
    std::mutex m_clients_mutex;
    std::atomic<bool> m_is_running;

    std::unique_ptr<Listener> m_listener;
    std::unique_ptr<ThreadPool> m_thread_pool;

    std::thread m_io_thread;

    std::unordered_set<std::string> m_nicknames;
};
