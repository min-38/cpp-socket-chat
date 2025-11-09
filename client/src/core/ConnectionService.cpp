#include "core/ConnectionService.h"
#include "network/PacketHandler.h"
#include "network/Protocol.h"
#include "vendor/nlohmann/json.hpp"

using json = nlohmann::json;

ConnectionService::ConnectionService(std::shared_ptr<INetworkClient> network_client)
    : m_network_client(network_client) {}

bool ConnectionService::ConnectToServer(const std::string& host, int port)
{
    return m_network_client->Connect(host, port);
}

bool ConnectionService::ValidateNickname(const std::string& nickname) const
{
    if (nickname.empty() || nickname.length() > MAX_NICKNAME_LEN)
    {
        std::cout << "닉네임이 비어있거나 너무 깁니다." << std::endl;
        return false;
    }
    return true;
}

bool ConnectionService::RegisterNickname(const std::string& nickname)
{
    if (!ValidateNickname(nickname))
        return false;

    Packet request_packet = PacketHandler::CreateNicknameRequestPacket(nickname);
    if (!m_network_client->Send(request_packet))
    {
        std::cerr << "닉네임 요청 전송에 실패했습니다." << std::endl;
        return false;
    }

    Packet response_packet;
    if (!m_network_client->Receive(response_packet))
    {
        std::cerr << "응답 대기 중 서버와의 연결이 끊어졌습니다." << std::endl;
        return false;
    }

    if (response_packet.header.type == PacketType::NICKNAME_RESPONSE_OK)
    {
        m_current_user.SetNickname(nickname);
        std::cout << "닉네임 [" << nickname << "] 사용이 승인되었습니다." << std::endl;
        return true;
    }
    else
    {
        try
        {
            std::string payload_str(response_packet.payload, response_packet.header.size);
            json data = json::parse(payload_str);
            std::string reason = data.value("reason", "알 수 없는 이유");
            std::cerr << "닉네임 설정 실패: " << reason << std::endl;
        }
        catch (const json::parse_error& e)
        {
            std::string reason(response_packet.payload, response_packet.header.size);
            std::cerr << "닉네임 설정 실패: " << reason << std::endl;
        }
        return false;
    }
}

void ConnectionService::Disconnect()
{
    m_network_client->Close();
}

bool ConnectionService::IsConnected() const
{
    return m_network_client->IsConnected();
}
