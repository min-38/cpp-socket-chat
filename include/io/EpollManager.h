#pragma once

#include <sys/epoll.h>
#include <functional>

class EpollManager
{
public:
    EpollManager();
    ~EpollManager();

    EpollManager(const EpollManager&) = delete;
    EpollManager& operator=(const EpollManager&) = delete;
    EpollManager(EpollManager&&) = delete;
    EpollManager& operator=(EpollManager&&) = delete;

    bool initialize();
    bool addFd(int fd, uint32_t events = EPOLLIN);
    bool removeFd(int fd);

    // 이벤트 루프: 콜백으로 각 이벤트 처리
    void wait(std::function<void(int fd, uint32_t events)> callback);

    void stop() { m_is_running = false; }
    bool isRunning() const { return m_is_running; }
    int getFd() const { return m_epoll_fd; }

private:
    int m_epoll_fd;
    bool m_is_running;
    static constexpr int MAX_EVENTS = 64;
};
