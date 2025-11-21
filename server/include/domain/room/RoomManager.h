#pragma once

#include "Room.h"
#include "common/pch.h"
#include <mutex>

#define MAX_ROOM_CNT 100

class RoomManager
{
public:
    // 방 생성
    std::string addRoom(const std::string& roomName, int maxUserCnt);

    // 방 삭제
    void removeRoom(const std::string& roomCode);

    // 방 참여
    bool joinRoom(int clientFd, const std::string& roomCode);

    // 방 나가기
    void leaveRoom(int clientFd, const std::string& roomCode);

    // 방 정보 가져오기
    Room* getRoom(const std::string& roomCode);

private:
    std::string generateRoomCode();

    int maxRoomCnt = MAX_ROOM_CNT;
    std::unordered_map<std::string, Room> rooms;
    std::mutex mtx;
};