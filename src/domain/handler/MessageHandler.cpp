#include "domain/handler/MessageHandler.h"
#include "application/Server.h"
#include "logger/Logger.h"
#include "vendor/nlohmann/json.hpp"
#include <ctime>
#include <iostream>

void MessageHandler::handle(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager)
{
    ClientInfo* sender_info = client_manager.getClient(client_fd);
    if (!sender_info || !sender_info->isAuthenticated) return;

    try
    {
        // 메시지 파싱
        nlohmann::json client_json = nlohmann::json::parse(std::string(packet.payload, packet.header.size));

        // 브로드캐스트용 페이로드 생성
        nlohmann::json payload;
        payload["sender_ip"] = sender_info->ip;
        payload["sender_nickname"] = sender_info->nickname;
        payload["message"] = client_json["message"];

        // 보내는 시간 설정 (현재 서버 시간)
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        char time_buffer[100];
        std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));
        payload["timestamp"] = std::string(time_buffer);

        Logger::info("Parsed message from client " + std::to_string(client_fd) + ": " + payload.dump());

        std::string json_string = payload.dump();

        Packet broadcast_packet;
        broadcast_packet.header.type = PacketType::MESSAGE_CHAT;
        broadcast_packet.header.size = json_string.length();

        if (json_string.length() > MAX_PAYLOAD_SIZE)
        {
            Logger::error("Serialized JSON exceeds maximum payload size for broadcasting.");
            Logger::error("JSON size: " + std::to_string(json_string.length()) + ", Max allowed: " + std::to_string(MAX_PAYLOAD_SIZE));
        }
        else
        {
            memcpy(broadcast_packet.payload, json_string.c_str(), json_string.length());
            server.broadcastPacket(broadcast_packet, client_fd);
        }
    }
    catch (const nlohmann::json::exception& e)
    {
        Logger::error("JSON Error: " + std::string(e.what()));
    }
    catch (const std::exception& e)
    {
        Logger::error("Standard Error: " + std::string(e.what()));
    }
}
