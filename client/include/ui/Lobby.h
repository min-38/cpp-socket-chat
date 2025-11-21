#pragma once

#include "core/ConnectionService.h"
#include "core/RoomService.h"
#include "core/ChatService.h"
#include "network/INetworkClient.h"
#include <thread>
#include <memory>

class Lobby
{
public:
    Lobby();
    ~Lobby();

    void Run();

private:
    // 서버와 연결 설정
    bool SetupConnection();

    // 사용자 등록
    bool RegisterUser();

    // 로비 메뉴 표시 및 처리
    void ShowLobbyMenu();

    // 방 생성
    void HandleCreateRoom();

    // 방 참여
    void HandleJoinRoom();

    // 채팅 시작
    void StartChatSession();

    // 사용자 입력 처리 루프
    void InputLoop();

    // 메시지 수신 처리
    void OnMessageReceived(const std::string& message);

private:
    std::shared_ptr<INetworkClient> m_network_client;

    std::unique_ptr<ConnectionService> m_connection_service;
    std::unique_ptr<RoomService> m_room_service;
    std::unique_ptr<ChatService> m_chat_service;

    std::thread m_receiver_thread;
    bool m_is_running;
};
