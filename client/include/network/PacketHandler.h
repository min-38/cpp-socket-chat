#pragma once

#include "network/Protocol.h"
#include "core/Message.h"
#include <string>

class PacketHandler
{
public:
    // 메시지 패킷 파싱
    static Message ParseMessagePacket(const Packet& packet);

    // 닉네임 요청 패킷 생성
    static Packet CreateNicknameRequestPacket(const std::string& nickname);

    // 메시지 전송 패킷 생성
    static Packet CreateMessageSendPacket(const std::string& message);

    // Room 관련 패킷 생성
    static Packet CreateRoomCreatePacket(const std::string& roomName, int maxUsers);
    static Packet CreateRoomJoinPacket(const std::string& roomCode);
    static Packet CreateRoomExitPacket();

    // Room 응답 패킷 파싱 (roomCode 반환)
    static std::string ParseRoomResponsePacket(const Packet& packet);
};
