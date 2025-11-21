#include "domain/handler/RoomHandler.h"
#include "application/Server.h"
#include "logger/Logger.h"
#include "vendor/nlohmann/json.hpp"
#include <cstring>

void RoomHandler::handle(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager)
{
    switch (packet.header.type)
    {
        case PacketType::CREATE_ROOM:
            handleCreateRoom(server, client_fd, packet, client_manager);
            break;
        case PacketType::EXIT_ROOM:
            handleExitRoom(server, client_fd, packet, client_manager);
            break;
        case PacketType::JOIN_ROOM:
            handleJoinRoom(server, client_fd, packet, client_manager);
            break;
        default:
            Logger::warn("RoomHandler received unknown packet type from " + std::to_string(client_fd));
            break;
    }
}

void RoomHandler::handleCreateRoom(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager)
{
    Packet response;

    try
    {
        // JSON 파싱
        std::string json_string(packet.payload, packet.header.size);
        nlohmann::json json_data = nlohmann::json::parse(json_string);

        // 방 이름 검증
        if (!json_data.contains("roomName") || !json_data["roomName"].is_string())
        {
            response.header.type = PacketType::ROOM_CREATE_FAIL;
            const char* reason = "Invalid JSON format: missing 'roomName' field.";
            response.header.size = strlen(reason);
            memcpy(response.payload, reason, response.header.size);
            server.sendPacket(client_fd, response);
            return;
        }

        // 최대 인원 수 검증
        if (!json_data.contains("maxUsers") || !json_data["maxUsers"].is_number())
        {
            response.header.type = PacketType::ROOM_CREATE_FAIL;
            const char* reason = "Invalid JSON format: missing 'maxUsers' field.";
            response.header.size = strlen(reason);
            memcpy(response.payload, reason, response.header.size);
            server.sendPacket(client_fd, response);
            return;
        }

        std::string roomName = json_data["roomName"];
        int maxUsers = json_data["maxUsers"];

        // 방 생성
        std::string roomCode = server.getRoomManager().addRoom(roomName, maxUsers);

        // 방 코드가 빈 문자열이라면 실패
        if (roomCode.empty())
        {
            response.header.type = PacketType::ROOM_CREATE_FAIL;
            const char* reason = "Failed to create room. Maximum room count reached.";
            response.header.size = strlen(reason);
            memcpy(response.payload, reason, response.header.size);
            Logger::warn("Room creation failed for client " + std::to_string(client_fd));
        }
        else
        {
            // 방 생성자를 방에 추가
            ClientInfo* client = client_manager.getClient(client_fd);
            if (client)
            {
                bool joined = server.getRoomManager().joinRoom(client_fd, roomCode);
                if (joined)
                    client->roomCode = roomCode; // ClientInfo에 roomCode 저장
            }

            response.header.type = PacketType::ROOM_CREATE_OK;

            // payload 설정
            nlohmann::json response_json;
            response_json["roomCode"] = roomCode;
            std::string response_str = response_json.dump();

            response.header.size = std::min(response_str.length(), (size_t)MAX_PAYLOAD_SIZE);
            memcpy(response.payload, response_str.c_str(), response.header.size);

            Logger::info("Room created: " + roomCode + " (name: " + roomName + ", max: " + std::to_string(maxUsers) + ") by client " + std::to_string(client_fd));
        }
    }
    catch (const nlohmann::json::exception& e)
    {
        response.header.type = PacketType::ROOM_CREATE_FAIL;
        std::string error_msg = "JSON parse error: " + std::string(e.what());
        response.header.size = std::min(error_msg.length(), (size_t)MAX_PAYLOAD_SIZE);
        memcpy(response.payload, error_msg.c_str(), response.header.size);
        Logger::error("handleCreateRoom JSON error: " + std::string(e.what()));
    }

    server.sendPacket(client_fd, response);
}

void RoomHandler::handleJoinRoom(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager)
{
    Packet response;

    try
    {
        // JSON 파싱
        std::string json_string(packet.payload, packet.header.size);
        nlohmann::json json_data = nlohmann::json::parse(json_string);

        // 방 코드 검증
        if (!json_data.contains("roomCode") || !json_data["roomCode"].is_string())
        {
            response.header.type = PacketType::ROOM_JOIN_FAIL;
            const char* reason = "Invalid JSON format: missing 'roomCode' field.";
            response.header.size = strlen(reason);
            memcpy(response.payload, reason, response.header.size);
            server.sendPacket(client_fd, response);
            return;
        }

        std::string roomCode = json_data["roomCode"];

        // 클라이언트 정보 가져오기
        ClientInfo* client = client_manager.getClient(client_fd);
        if (!client)
        {
            response.header.type = PacketType::ROOM_JOIN_FAIL;
            const char* reason = "Client not found.";
            response.header.size = strlen(reason);
            memcpy(response.payload, reason, response.header.size);
            server.sendPacket(client_fd, response);
            return;
        }

        // 이미 다른 방에 있는지 체크
        if (!client->roomCode.empty())
        {
            response.header.type = PacketType::ROOM_JOIN_FAIL;
            const char* reason = "Already in another room. Please exit first.";
            response.header.size = strlen(reason);
            memcpy(response.payload, reason, response.header.size);
            server.sendPacket(client_fd, response);
            return;
        }

        // 방 입장 시도
        bool success = server.getRoomManager().joinRoom(client_fd, roomCode);
        if (!success)
        {
            // 실패 (방이 없거나 꽉 참)
            response.header.type = PacketType::ROOM_JOIN_FAIL;
            const char* reason = "Failed to join room. Room not found or full.";
            response.header.size = strlen(reason);
            memcpy(response.payload, reason, response.header.size);
            Logger::warn("Client " + std::to_string(client_fd) + " failed to join room " + roomCode);
        }
        else
        {
            client->roomCode = roomCode; // ClientInfo에 roomCode 저장

            response.header.type = PacketType::ROOM_JOIN_OK;
            nlohmann::json response_json;
            response_json["roomCode"] = roomCode;
            std::string response_str = response_json.dump();

            response.header.size = std::min(response_str.length(), (size_t)MAX_PAYLOAD_SIZE);
            memcpy(response.payload, response_str.c_str(), response.header.size);

            Logger::info("Client " + std::to_string(client_fd) + " joined room " + roomCode);

            // 방에 있는 다른 유저들에게 브로드캐스트 (ROOM_USER_JOIN) - 자기 자신 제외
            Packet broadcastPacket;
            broadcastPacket.header.type = PacketType::ROOM_USER_JOIN;

            // 입장한 유저 정보 전송
            nlohmann::json broadcast_json;
            broadcast_json["clientFd"] = client_fd;
            if (client->nickname.empty())
                broadcast_json["nickname"] = "Anonymous";
            else
                broadcast_json["nickname"] = client->nickname;

            std::string broadcast_str = broadcast_json.dump();
            broadcastPacket.header.size = std::min(broadcast_str.length(), (size_t)MAX_PAYLOAD_SIZE);
            memcpy(broadcastPacket.payload, broadcast_str.c_str(), broadcastPacket.header.size);

            server.broadcastPacketForRoom(broadcastPacket, roomCode, client_fd);
        }
    }
    catch (const nlohmann::json::exception& e)
    {
        response.header.type = PacketType::ROOM_JOIN_FAIL;
        std::string error_msg = "JSON parse error: " + std::string(e.what());
        response.header.size = std::min(error_msg.length(), (size_t)MAX_PAYLOAD_SIZE);
        memcpy(response.payload, error_msg.c_str(), response.header.size);
        Logger::error("handleJoinRoom JSON error: " + std::string(e.what()));
    }

    server.sendPacket(client_fd, response);
}

void RoomHandler::handleExitRoom(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager)
{
    Packet response;

    try
    {
        // 클라이언트 정보 가져오기
        ClientInfo* client = client_manager.getClient(client_fd);
        if (!client)
        {
            response.header.type = PacketType::ROOM_EXIT_FAIL;
            const char* reason = "Client not found.";
            response.header.size = strlen(reason);
            memcpy(response.payload, reason, response.header.size);
            server.sendPacket(client_fd, response);
            return;
        }

        // 현재 방에 있는지 체크
        if (client->roomCode.empty())
        {
            response.header.type = PacketType::ROOM_EXIT_FAIL;
            const char* reason = "Not in any room.";
            response.header.size = strlen(reason);
            memcpy(response.payload, reason, response.header.size);
            server.sendPacket(client_fd, response);
            return;
        }

        std::string roomCode = client->roomCode;

        // 방에 있는 다른 유저들에게 먼저 방에서 나갔다고 전송
        Packet broadcastPacket;
        broadcastPacket.header.type = PacketType::ROOM_USER_EXIT;

        // 퇴장한 유저 정보
        nlohmann::json broadcast_json;
        broadcast_json["clientFd"] = client_fd;
        if (client->nickname.empty())
            broadcast_json["nickname"] = "Anonymous";
        else
            broadcast_json["nickname"] = client->nickname;

        std::string broadcast_str = broadcast_json.dump();
        broadcastPacket.header.size = std::min(broadcast_str.length(), (size_t)MAX_PAYLOAD_SIZE);
        memcpy(broadcastPacket.payload, broadcast_str.c_str(), broadcastPacket.header.size);

        // 나가는 사람을 제외하고 방에 있는 다른 유저들에게만 브로드캐스트
        server.broadcastPacketForRoom(broadcastPacket, roomCode, client_fd);

        // 방에서 나가기
        server.getRoomManager().leaveRoom(client_fd, roomCode);

        // ClientInfo의 roomCode 초기화
        client->roomCode = "";

        response.header.type = PacketType::ROOM_EXIT_OK;
        response.header.size = 0;

        Logger::info("Client " + std::to_string(client_fd) + " exited room " + roomCode);
    }
    catch (const std::exception& e)
    {
        response.header.type = PacketType::ROOM_EXIT_FAIL;
        std::string error_msg = "Error: " + std::string(e.what());
        response.header.size = std::min(error_msg.length(), (size_t)MAX_PAYLOAD_SIZE);
        memcpy(response.payload, error_msg.c_str(), response.header.size);
        Logger::error("handleExitRoom error: " + std::string(e.what()));
    }

    server.sendPacket(client_fd, response);
}
