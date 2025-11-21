#include "network/PacketHandler.h"
#include "vendor/nlohmann/json.hpp"
#include "common/pch.h"

Message PacketHandler::ParseMessagePacket(const Packet& packet)
{
    std::string payload_str(packet.payload, packet.header.size);

    try
    {
        nlohmann::json data = nlohmann::json::parse(payload_str);

        // 패킷 타입에 따라 MessageType 결정
        MessageType msg_type = MessageType::CHAT;

        if (packet.header.type == PacketType::ROOM_USER_JOIN)
            msg_type = MessageType::USER_JOIN;
        else if (packet.header.type == PacketType::ROOM_USER_EXIT)
            msg_type = MessageType::USER_EXIT;
        else if (packet.header.type == PacketType::MESSAGE_SYSTEM)
            msg_type = MessageType::SYSTEM;

        std::string timestamp = data.value("timestamp", "");
        std::string nickname = data.value("sender_nickname", "");
        std::string ip = data.value("sender_ip", "");
        std::string content = data.value("message", "");
        int sender_fd = data.value("sender_fd", -1);

        // 입장과 퇴장 메시지는 nickname 필드가 다를 수 있음
        if (msg_type == MessageType::USER_JOIN || msg_type == MessageType::USER_EXIT)
        {
            nickname = data.value("nickname", "");
            int client_fd = data.value("clientFd", -1);
            sender_fd = client_fd;
        }

        return Message(nickname, ip, content, timestamp, sender_fd, msg_type);
    }
    catch (const json::parse_error& e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return Message();
    }
}

Packet PacketHandler::CreateNicknameRequestPacket(const std::string& nickname)
{
    nlohmann::json data;
    data["nickname"] = nickname;
    std::string payload = data.dump();

    Packet packet;
    packet.header.type = PacketType::NICKNAME_REQUEST;
    packet.header.size = payload.length();
    memcpy(packet.payload, payload.c_str(), payload.length());

    return packet;
}

Packet PacketHandler::CreateMessageSendPacket(const std::string& message)
{
    nlohmann::json data;
    data["message"] = message;
    std::string payload = data.dump();

    Packet packet;
    packet.header.type = PacketType::MESSAGE_SEND;
    packet.header.size = payload.length();
    memcpy(packet.payload, payload.c_str(), payload.length());

    return packet;
}

Packet PacketHandler::CreateRoomCreatePacket(const std::string& roomName, int maxUsers)
{
    nlohmann::json data;
    data["roomName"] = roomName;
    data["maxUsers"] = maxUsers;
    std::string payload = data.dump();

    Packet packet;
    packet.header.type = PacketType::CREATE_ROOM;
    packet.header.size = payload.length();
    memcpy(packet.payload, payload.c_str(), payload.length());

    return packet;
}

Packet PacketHandler::CreateRoomJoinPacket(const std::string& roomCode)
{
    nlohmann::json data;
    data["roomCode"] = roomCode;
    std::string payload = data.dump();

    Packet packet;
    packet.header.type = PacketType::JOIN_ROOM;
    packet.header.size = payload.length();
    memcpy(packet.payload, payload.c_str(), payload.length());

    return packet;
}

Packet PacketHandler::CreateRoomExitPacket()
{
    Packet packet;
    packet.header.type = PacketType::EXIT_ROOM;
    packet.header.size = 0;

    return packet;
}

std::string PacketHandler::ParseRoomResponsePacket(const Packet& packet)
{
    // payload가 비어있으면 빈 문자열 반환
    if (packet.header.size == 0) return "";

    std::string payload_str(packet.payload, packet.header.size);

    try
    {
        nlohmann::json data = nlohmann::json::parse(payload_str);
        return data.value("roomCode", "");
    }
    catch (const nlohmann::json::parse_error& e)
    {
        std::cerr << "Room response parse error or error message: " << payload_str << std::endl;
        return "";
    }
}
