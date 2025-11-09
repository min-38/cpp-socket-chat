#pragma once

#include "network/Protocol.h"
#include "core/Message.h"

class PacketHandler
{
public:
    // 메시지 패킷 파싱
    static Message ParseMessagePacket(const Packet& packet);

    // 닉네임 요청 패킷 생성
    static Packet CreateNicknameRequestPacket(const std::string& nickname);

    // 메시지 전송 패킷 생성
    static Packet CreateMessageSendPacket(const std::string& message);
};
