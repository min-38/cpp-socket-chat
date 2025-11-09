#include "domain/client/ClientManager.h"

void ClientManager::addClient(int client_fd, const std::string& ip)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_clients[client_fd] = {client_fd, ip, "", false, {}};
}

void ClientManager::removeClient(int client_fd)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_clients.find(client_fd);
    if (it != m_clients.end())
    {
        if (it->second.isAuthenticated)
            m_nicknames.erase(it->second.nickname);
        m_clients.erase(it);
    }
}

ClientInfo* ClientManager::getClient(int client_fd)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_clients.find(client_fd);
    if (it != m_clients.end())
        return &it->second;
    return nullptr;
}

std::vector<int> ClientManager::getAllAuthenticatedClientFds(int exclude_fd)
{
    std::vector<int> fds;
    std::lock_guard<std::mutex> lock(m_mutex);
    for (const auto& pair : m_clients)
    {
        if (pair.first != exclude_fd && pair.second.isAuthenticated)
            fds.push_back(pair.first);
    }
    return fds;
}

bool ClientManager::isNicknameTaken(const std::string& nickname)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_nicknames.count(nickname);
}

void ClientManager::setNickname(int client_fd, const std::string& nickname)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_clients.find(client_fd);
    if (it != m_clients.end())
    {
        it->second.nickname = nickname;
        it->second.isAuthenticated = true;
        m_nicknames.insert(nickname);
    }
}

void ClientManager::removeNickname(const std::string& nickname)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_nicknames.erase(nickname);
}
