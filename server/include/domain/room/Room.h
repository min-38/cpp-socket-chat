#pragma once

#include "common/pch.h"
#include <mutex>
#include <string>
#include <vector>

enum class JoinResult
{
    SUCCESS,      // 성공
    ROOM_FULL,    // 방이 꽉 참
    ALREADY_IN    // 이미 방에 있음
};

class Room
{
public:
    Room(const std::string& code, const std::string& name, int maxUsers);

    JoinResult acceptNewUser(int clientFd);
    void leaveUser(int clientFd);

    // Getter
    std::string getRoomCode() const { return roomCode; }
    std::string getRoomName() const { return roomName; }
    int getMaxUserCount() const { return maxUserCnt; }
    int getCurrentUserCount() const { return clients.size(); }
    std::vector<int> getClientFds() const { return clients; }

private:
    std::string roomCode;
    std::string roomName;
    int maxUserCnt;

    std::vector<int> clients;
    std::mutex mtx;
};
