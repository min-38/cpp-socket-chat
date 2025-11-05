#include "io/EpollManager.h"
#include "logger/Logger.h"
#include <unistd.h>
#include <errno.h>
#include <cstring>

EpollManager::EpollManager() : m_epoll_fd(-1), m_is_running(false) {}

EpollManager::~EpollManager()
{
    if (m_epoll_fd != -1)
        close(m_epoll_fd);
}

bool EpollManager::initialize()
{
    m_epoll_fd = epoll_create1(0);
    if (m_epoll_fd == -1)
    {
        Logger::error("Failed to create epoll instance.");
        return false;
    }
    m_is_running = true;
    return true;
}

bool EpollManager::addFd(int fd, uint32_t events)
{
    struct epoll_event event;
    event.events = events;
    event.data.fd = fd;

    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1)
    {
        Logger::error("Failed to add fd " + std::to_string(fd) + " to epoll.");
        return false;
    }
    return true;
}

bool EpollManager::removeFd(int fd)
{
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, nullptr) == -1)
    {
        Logger::error("Failed to remove fd " + std::to_string(fd) + " from epoll.");
        return false;
    }
    return true;
}

void EpollManager::wait(std::function<void(int fd, uint32_t events)> callback)
{
    struct epoll_event events[MAX_EVENTS];

    while (m_is_running)
    {
        int num_events = epoll_wait(m_epoll_fd, events, MAX_EVENTS, -1);
        if (num_events < 0 && errno != EINTR)
        {
            Logger::error("epoll_wait error: " + std::string(strerror(errno)));
            continue;
        }

        for (int i = 0; i < num_events; ++i)
            callback(events[i].data.fd, events[i].events);
    }
}
