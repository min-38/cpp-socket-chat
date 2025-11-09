#include "domain/handler/NicknameHandler.h"
#include "application/Server.h"
#include "logger/Logger.h"
#include "network/Protocol.h"
#include "vendor/nlohmann/json.hpp"
#include <cstring>

void NicknameHandler::handle(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager)
{
    Packet response;

    try
    {
        // JSON 파싱
        std::string json_string(packet.payload, packet.header.size);
        nlohmann::json json_data = nlohmann::json::parse(json_string);

        // nickname 필드 추출
        if (!json_data.contains("nickname") || !json_data["nickname"].is_string())
        {
            response.header.type = PacketType::NICKNAME_RESPONSE_FAIL;
            const char* reason = "Invalid JSON format: missing 'nickname' field.";
            response.header.size = strlen(reason);
            memcpy(response.payload, reason, response.header.size);
            server.sendPacket(client_fd, response);
            return;
        }

        std::string nickname = json_data["nickname"];

        // 닉네임 중복 확인
        if (client_manager.isNicknameTaken(nickname))
        {
            response.header.type = PacketType::NICKNAME_RESPONSE_FAIL;
            const char* reason = "Duplicated nickname.";
            response.header.size = strlen(reason);
            memcpy(response.payload, reason, response.header.size);
        }
        else
        {
            client_manager.setNickname(client_fd, nickname);
            response.header.type = PacketType::NICKNAME_RESPONSE_OK;
            response.header.size = 0;
            Logger::info("Client " + std::to_string(client_fd) + " set nickname to " + nickname);
        }
    }
    catch (const nlohmann::json::exception& e)
    {
        response.header.type = PacketType::NICKNAME_RESPONSE_FAIL;
        std::string error_msg = "JSON parse error: " + std::string(e.what());
        response.header.size = std::min(error_msg.length(), (size_t)MAX_PAYLOAD_SIZE);
        memcpy(response.payload, error_msg.c_str(), response.header.size);
        Logger::error("NicknameHandler JSON error: " + std::string(e.what()));
    }

    server.sendPacket(client_fd, response);
}
