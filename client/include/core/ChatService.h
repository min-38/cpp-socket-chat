#pragma once

#include "network/INetworkClient.h"
#include "core/Message.h"
#include "common/pch.h"

class ChatService
{
public:
    using MessageCallback = std::function<void(const Message&)>;

    explicit ChatService(std::shared_ptr<INetworkClient> network_client);

    // 메시지 송신
    bool SendMessage(const std::string& content);

    // 메시지 수신 시작
    void StartReceiving(MessageCallback callback);

    // 메시지 수신 중지
    void StopReceiving();

    // 수신 중인지 여부 확인
    bool IsReceiving() const { return m_is_receiving; }

private:
    void ReceiveLoop();

private:
    std::shared_ptr<INetworkClient> m_network_client;
    bool m_is_receiving;
    MessageCallback m_message_callback;
};
