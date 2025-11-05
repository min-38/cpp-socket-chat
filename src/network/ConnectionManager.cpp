#include "network/ConnectionManager.h"
#include "logger/Logger.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

ConnectionManager::ConnectionManager(EpollManager& epoll_manager, ClientManager& client_manager)
    : m_epoll_manager(epoll_manager), m_client_manager(client_manager) {}

void ConnectionManager::acceptNewConnection(Listener& listener)
{
    ConnectionInfo conn_info = listener.acceptConnection();
    if (!conn_info.isValid())
        return;

    // 논블로킹 설정
    int flags = fcntl(conn_info.fd, F_GETFL, 0);
    fcntl(conn_info.fd, F_SETFL, flags | O_NONBLOCK);

    // epoll에 추가
    if (!m_epoll_manager.addFd(conn_info.fd, EPOLLIN))
    {
        Logger::error("Failed to add client fd to epoll.");
        close(conn_info.fd);
        return;
    }

    // ClientManager에 추가
    m_client_manager.addClient(conn_info.fd, conn_info.ip);
    Logger::info("Client fd " + std::to_string(conn_info.fd) + " (" + conn_info.ip + ") connected.");
}

void ConnectionManager::closeConnection(int client_fd)
{
    ClientInfo* client = m_client_manager.getClient(client_fd);
    if (client)
    {
        Logger::info("Client " + client->nickname + " (" + std::to_string(client_fd) + ") disconnected.");
        m_client_manager.removeClient(client_fd);
    }

    m_epoll_manager.removeFd(client_fd);
    close(client_fd);
}
