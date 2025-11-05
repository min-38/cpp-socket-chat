#pragma once

#include "domain/client/ClientManager.h"
#include "io/EpollManager.h"
#include "network/Listener.h"

class ConnectionManager
{
public:
    ConnectionManager(EpollManager& epoll_manager, ClientManager& client_manager);

    // 새 클라이언트 연결 수락
    void acceptNewConnection(Listener& listener);

    // 클라이언트 연결 종료
    void closeConnection(int client_fd);

private:
    EpollManager& m_epoll_manager;
    ClientManager& m_client_manager;
};
