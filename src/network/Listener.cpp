#include "network/Listener.h"

#include "utils/Logger.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

int make_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

Listener::Listener(int port) : m_port(port), m_socket_fd(-1){}

Listener::~Listener()
{
    close();
}

bool Listener::initialize()
{
    m_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_socket_fd < 0) return false;

    int opt = 1;
    setsockopt(m_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    setsockopt(m_socket_fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));

    struct sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(m_port);

    if (bind(m_socket_fd, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
        close();
        return false;
    }

    if (listen(m_socket_fd, SOMAXCONN) < 0)
    {
        close();
        return false;
    }

    if (make_nonblocking(m_socket_fd) == -1)
        return false;

    return true;
}

int Listener::acceptConnection()
{
    struct sockaddr_in client_addr {};
    socklen_t len = sizeof(client_addr);
    
    int client_fd = accept(m_socket_fd, (sockaddr*)&client_addr, &len);
    if (client_fd < 0)
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            Logger::error("accept failed");
        }
        return -1; // 에러 또는 논블러킹 수락 완료
    }
    
    // 클라이언트 주소 정보 로깅 (옵션)
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
    Logger::info("New client connected: " + std::string(client_ip) + ":" + std::to_string(ntohs(client_addr.sin_port)));

    return client_fd;
}

void Listener::close()
{
    if (m_socket_fd != -1)
        ::close(m_socket_fd);
}