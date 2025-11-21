#pragma once

#include "common/pch.h"
#include "network/INetworkClient.h"
#include <string>

class RoomService
{
public:
    explicit RoomService(std::shared_ptr<INetworkClient> network_client);

    // 방 생성
    bool CreateRoom(const std::string& roomName, int maxUsers, std::string& outRoomCode);

    // 방 참여
    bool JoinRoom(const std::string& roomCode);

    // 방 나가기
    bool ExitRoom();

    // 현재 방 코드 리턴
    const std::string& GetCurrentRoomCode() const { return m_current_room_code; }

    // 방에 있는지 여부
    bool IsInRoom() const { return !m_current_room_code.empty(); }

private:
    std::shared_ptr<INetworkClient> m_network_client;
    std::string m_current_room_code;
};
