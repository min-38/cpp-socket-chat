#include "core/RoomService.h"
#include "network/PacketHandler.h"
#include "network/Protocol.h"
#include "common/pch.h"

RoomService::RoomService(std::shared_ptr<INetworkClient> network_client) : m_network_client(network_client) {}

bool RoomService::CreateRoom(const std::string& roomName, int maxUsers, std::string& outRoomCode)
{
    if (!m_network_client)
    {
        std::cerr << "Network client is null" << std::endl;
        return false;
    }

    // CREATE_ROOM 패킷 생성 및 전송
    Packet request = PacketHandler::CreateRoomCreatePacket(roomName, maxUsers);
    if (!m_network_client->Send(request))
    {
        std::cerr << "Failed to send CREATE_ROOM packet" << std::endl;
        return false;
    }

    // 응답 수신
    Packet response;
    if (!m_network_client->Receive(response))
    {
        std::cerr << "Failed to receive CREATE_ROOM response" << std::endl;
        return false;
    }

    // 응답 확인
    if (response.header.type == PacketType::ROOM_CREATE_OK)
    {
        std::string roomCode = PacketHandler::ParseRoomResponsePacket(response);
        if (!roomCode.empty())
        {
            m_current_room_code = roomCode;
            outRoomCode = roomCode;
            std::cout << "Room created successfully! Room Code: " << roomCode << std::endl;
            return true;
        }
        else
        {
            std::cerr << "Failed to parse room code from response" << std::endl;
            return false;
        }
    }
    else if (response.header.type == PacketType::ROOM_CREATE_FAIL)
    {
        std::string error_msg(response.payload, response.header.size);
        std::cerr << "Room creation failed: " << error_msg << std::endl;
        return false;
    }
    else
    {
        std::cerr << "Unexpected response type for CREATE_ROOM" << std::endl;
        return false;
    }
}

bool RoomService::JoinRoom(const std::string& roomCode)
{
    if (!m_network_client)
    {
        std::cerr << "Network client is null" << std::endl;
        return false;
    }

    // JOIN_ROOM 패킷 생성 및 전송
    Packet request = PacketHandler::CreateRoomJoinPacket(roomCode);
    if (!m_network_client->Send(request))
    {
        std::cerr << "Failed to send JOIN_ROOM packet" << std::endl;
        return false;
    }

    // 응답 수신
    Packet response;
    if (!m_network_client->Receive(response))
    {
        std::cerr << "Failed to receive JOIN_ROOM response" << std::endl;
        return false;
    }

    // 응답 확인
    if (response.header.type == PacketType::ROOM_JOIN_OK)
    {
        std::string joinedRoomCode = PacketHandler::ParseRoomResponsePacket(response);
        if (!joinedRoomCode.empty())
        {
            m_current_room_code = joinedRoomCode;
            std::cout << "Successfully joined room: " << joinedRoomCode << std::endl;
            return true;
        }
        else
        {
            std::cerr << "Failed to parse room code from response" << std::endl;
            return false;
        }
    }
    else if (response.header.type == PacketType::ROOM_JOIN_FAIL)
    {
        std::string error_msg(response.payload, response.header.size);
        std::cerr << "Room join failed: " << error_msg << std::endl;
        return false;
    }
    else
    {
        std::cerr << "Unexpected response type for JOIN_ROOM" << std::endl;
        return false;
    }
}

bool RoomService::ExitRoom()
{
    if (!m_network_client)
    {
        std::cerr << "Network client is null" << std::endl;
        return false;
    }

    if (m_current_room_code.empty())
    {
        std::cerr << "Not currently in any room" << std::endl;
        return false;
    }

    // EXIT_ROOM 패킷 생성 및 전송
    Packet request = PacketHandler::CreateRoomExitPacket();
    if (!m_network_client->Send(request))
    {
        std::cerr << "Failed to send EXIT_ROOM packet" << std::endl;
        return false;
    }

    // 응답 수신
    Packet response;
    if (!m_network_client->Receive(response))
    {
        std::cerr << "Failed to receive EXIT_ROOM response" << std::endl;
        return false;
    }

    // 응답 확인
    if (response.header.type == PacketType::ROOM_EXIT_OK)
    {
        std::cout << "Successfully exited room: " << m_current_room_code << std::endl;
        m_current_room_code = "";
        return true;
    }
    else if (response.header.type == PacketType::ROOM_EXIT_FAIL)
    {
        std::string error_msg(response.payload, response.header.size);
        std::cerr << "Room exit failed: " << error_msg << std::endl;
        return false;
    }
    else
    {
        std::cerr << "Unexpected response type for EXIT_ROOM" << std::endl;
        return false;
    }
}
