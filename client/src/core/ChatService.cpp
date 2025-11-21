#include "core/ChatService.h"
#include "network/PacketHandler.h"
#include "network/Protocol.h"

ChatService::ChatService(std::shared_ptr<INetworkClient> network_client)
    : m_network_client(network_client), m_is_receiving(false) {}

bool ChatService::SendMessage(const std::string& content)
{
    if (content.empty())
        return false;

    Packet packet = PacketHandler::CreateMessageSendPacket(content);
    return m_network_client->Send(packet);
}

void ChatService::StartReceiving(MessageCallback callback)
{
    m_message_callback = callback;
    m_is_receiving = true;
    ReceiveLoop();
}

void ChatService::StopReceiving()
{
    m_is_receiving = false;
}

void ChatService::ReceiveLoop()
{
    while (m_is_receiving && m_network_client->IsConnected())
    {
        Packet packet;
        if (!m_network_client->Receive(packet))
            break;

        // 채팅 메시지, 입퇴장 알림, 시스템 메시지 처리
        if (packet.header.type == PacketType::MESSAGE_CHAT ||
            packet.header.type == PacketType::ROOM_USER_JOIN ||
            packet.header.type == PacketType::ROOM_USER_EXIT ||
            packet.header.type == PacketType::MESSAGE_SYSTEM)
        {
            Message message = PacketHandler::ParseMessagePacket(packet);
            if (m_message_callback)
                m_message_callback(message);
        }
    }

    m_is_receiving = false;
}
