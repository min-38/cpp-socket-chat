#include "network/PacketHandler.h"
#include "vendor/nlohmann/json.hpp"
#include "common/pch.h"

using json = nlohmann::json;

Message PacketHandler::ParseMessagePacket(const Packet& packet)
{
    std::string payload_str(packet.payload, packet.header.size);

    try
    {
        json data = json::parse(payload_str);

        std::string timestamp = data.value("timestamp", "");
        std::string nickname = data.value("sender_nickname", "");
        std::string ip = data.value("sender_ip", "");
        std::string content = data.value("message", "");

        return Message(nickname, ip, content, timestamp);
    }
    catch (const json::parse_error& e)
    {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return Message();
    }
}

Packet PacketHandler::CreateNicknameRequestPacket(const std::string& nickname)
{
    json data;
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
    json data;
    data["message"] = message;
    std::string payload = data.dump();

    Packet packet;
    packet.header.type = PacketType::MESSAGE_SEND;
    packet.header.size = payload.length();
    memcpy(packet.payload, payload.c_str(), payload.length());

    return packet;
}
