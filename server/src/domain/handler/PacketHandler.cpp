#include "domain/handler/PacketHandler.h"
#include "domain/handler/NicknameHandler.h"
#include "domain/handler/MessageHandler.h"
#include "domain/handler/RoomHandler.h"

#include "logger/Logger.h"

PacketHandler::PacketHandler()
{
    // 닉네임 관련 패킷
    m_handlers[PacketType::NICKNAME_REQUEST] = std::make_unique<NicknameHandler>();

    // 메시지 관련 패킷
    m_handlers[PacketType::MESSAGE_SEND] = std::make_unique<MessageHandler>();

    // Room 관련 패킷
    m_handlers[PacketType::CREATE_ROOM] = std::make_unique<RoomHandler>();
    m_handlers[PacketType::JOIN_ROOM] = std::make_unique<RoomHandler>();
    m_handlers[PacketType::EXIT_ROOM] = std::make_unique<RoomHandler>();
}

void PacketHandler::handle(Server& server, int client_fd, const Packet& packet, ClientManager& client_manager)
{
    auto it = m_handlers.find(packet.header.type);
    if (it != m_handlers.end())
    {
        // 해당 역할을 담당하는 핸들러에 위임
        Logger::info("Handling packet of type " + std::to_string(static_cast<uint16_t>(packet.header.type)) + " from client " + std::to_string(client_fd));
        it->second->handle(server, client_fd, packet, client_manager);
        return;
    }

    Logger::warn("Unknown packet type received from " + std::to_string(client_fd));
}
