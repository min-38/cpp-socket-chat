#include "domain/room/RoomManager.h"
#include "logger/Logger.h"
#include <random>

std::string RoomManager::generateRoomCode()
{
    // 영문 대소문자 + 숫자 조합
    static const char chars[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, sizeof(chars) - 2);

    std::string code;
    code.reserve(8);

    for (int i = 0; i < 8; ++i) {
        code += chars[dis(gen)];
    }

    return code;
}

std::string RoomManager::addRoom(const std::string& roomName, int maxUserCnt)
{
    std::lock_guard<std::mutex> lock(mtx);

    // 최대 방 개수 확인
    if (rooms.size() >= maxRoomCnt) return "";  // 실패 시 빈 문자열 리턴

    // Room Code 생성
    std::string roomCode;
    int attempts = 0;
    do
    {
        roomCode = generateRoomCode();
        attempts++;

        // 100번 시도했는데도 중복이면 실패로 처리 (임시?)
        if (attempts > 100) return "";  
        
    } while (rooms.find(roomCode) != rooms.end());  // 중복 체크

    // Room 생성 및 추가
    rooms.try_emplace(roomCode, roomCode, roomName, maxUserCnt);

    return roomCode;
}

void RoomManager::removeRoom(const std::string& roomCode)
{
    std::lock_guard<std::mutex> lock(mtx);

    // 방 삭제
    rooms.erase(roomCode);
}

bool RoomManager::joinRoom(int clientFd, const std::string& roomCode)
{
    std::lock_guard<std::mutex> lock(mtx);

    // 방이 존재하는지 확인
    auto it = rooms.find(roomCode);
    if (it == rooms.end())
        return false;  // 방이 없음

    // 사용자를 Room에 추가
    JoinResult result = it->second.acceptNewUser(clientFd);

    return result == JoinResult::SUCCESS;
}

void RoomManager::leaveRoom(int clientFd, const std::string& roomCode)
{
    std::lock_guard<std::mutex> lock(mtx);

    // 방이 존재하는지 확인
    auto it = rooms.find(roomCode);
    if (it == rooms.end())
        return;  // 방이 없으면 종료

    // 사용자를 Room에서 제거
    it->second.leaveUser(clientFd);

    // 방에 아무도 없으면 삭제 처리
    if (it->second.getCurrentUserCount() == 0)
    {
        rooms.erase(it);
        Logger::info("Room " + roomCode + " deleted (empty)");
    }
}

Room* RoomManager::getRoom(const std::string& roomCode)
{
    std::lock_guard<std::mutex> lock(mtx);

    auto it = rooms.find(roomCode);
    if (it == rooms.end())
        return nullptr;

    return &(it->second);
}
