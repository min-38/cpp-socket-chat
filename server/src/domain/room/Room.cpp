#include "domain/room/Room.h"

#include <algorithm>

Room::Room(const std::string& code, const std::string& name, int maxUsers)
    : roomCode(code), roomName(name), maxUserCnt(maxUsers)
{
}

JoinResult Room::acceptNewUser(int clientFd)
{
    std::lock_guard<std::mutex> lock(mtx);
    if (clients.size() >= maxUserCnt)
        return JoinResult::ROOM_FULL;

    auto it = std::find(clients.begin(), clients.end(), clientFd);
    if (it != clients.end())
        return JoinResult::ALREADY_IN;

    // 성공
    clients.push_back(clientFd);
    return JoinResult::SUCCESS;
}

void Room::leaveUser(int clientFd)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto it = std::find(clients.begin(), clients.end(), clientFd);
    if(it != clients.end())
        clients.erase(it);
}