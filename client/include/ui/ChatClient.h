#pragma once

#include "core/ConnectionService.h"
#include "core/ChatService.h"
#include "network/INetworkClient.h"
#include <thread>

class ChatClient
{
public:
    ChatClient();
    ~ChatClient();

    void Run();

private:
    // 서버와 연결 설정
    bool SetupConnection();

    // 사용자 등록
    bool RegisterUser();

    // 채팅 시작
    void StartChatSession();

    // 사용자 입력 처리 루프
    void InputLoop();

    // 메시지 수신 처리
    void OnMessageReceived(const Message& message);

private:
    std::shared_ptr<INetworkClient> m_network_client;

    std::unique_ptr<ConnectionService> m_connection_service;
    std::unique_ptr<ChatService> m_chat_service;

    std::thread m_receiver_thread;
};
