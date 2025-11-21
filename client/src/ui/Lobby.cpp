#include "ui/Lobby.h"
#include "ui/ConsoleUI.h"
#include "network/NetworkClient.h"
#include "core/Message.h"
#include <iostream>
#include <limits>
#include <chrono>
#include <ctime>

Lobby::Lobby()
    : m_is_running(false)
{
    m_network_client = std::make_shared<NetworkClient>();
    m_connection_service = std::make_unique<ConnectionService>(m_network_client);
    m_room_service = std::make_unique<RoomService>(m_network_client);
    m_chat_service = std::make_unique<ChatService>(m_network_client);
}

Lobby::~Lobby()
{
    if (m_receiver_thread.joinable())
    {
        m_chat_service->StopReceiving();
        m_receiver_thread.join();
    }

    if (m_connection_service->IsConnected())
        m_connection_service->Disconnect();
}

void Lobby::Run()
{
    if (!SetupConnection()) return;

    if (!RegisterUser()) return;

    m_is_running = true;
    ShowLobbyMenu();
}

bool Lobby::SetupConnection()
{
    std::string host;
    int port;

    ConsoleUI::GetConnectionInfo(host, port);

    if (!m_connection_service->ConnectToServer(host, port))
    {
        ConsoleUI::DisplayError("서버 연결에 실패했습니다.");
        return false;
    }

    ConsoleUI::DisplaySystemMessage("서버에 연결되었습니다.");
    return true;
}

bool Lobby::RegisterUser()
{
    while (m_connection_service->IsConnected())
    {
        std::string nickname = ConsoleUI::GetNickname();

        if (m_connection_service->RegisterNickname(nickname))
            return true;
    }

    return false;
}

void Lobby::ShowLobbyMenu()
{
    while (m_is_running && m_connection_service->IsConnected())
    {
        std::cout << "\n========== 로비 ==========" << std::endl;
        std::cout << "1. 채팅방 생성" << std::endl;
        std::cout << "2. 채팅방 참여" << std::endl;
        std::cout << "3. 나가기" << std::endl;
        std::cout << "==========================" << std::endl;
        std::cout << "선택: ";

        int choice;
        std::cin >> choice;

        // 입력 버퍼 클리어
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice)
        {
            case 1:
                HandleCreateRoom();
                break;
            case 2:
                HandleJoinRoom();
                break;
            case 3:
                std::cout << "로비를 나갑니다." << std::endl;
                m_is_running = false;
                return;
            default:
                std::cout << "잘못된 선택입니다. 다시 선택해주세요." << std::endl;
                break;
        }
    }
}

void Lobby::HandleCreateRoom()
{
    std::string roomName;
    int maxUsers;

    std::cout << "\n--- 채팅방 생성 ---" << std::endl;
    std::cout << "방 이름: ";
    std::getline(std::cin, roomName);

    if (roomName.empty())
    {
        std::cout << "방 이름을 입력해주세요." << std::endl;
        return;
    }

    std::cout << "최대 인원 (2-10): ";
    std::cin >> maxUsers;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (maxUsers < 2 || maxUsers > 10)
    {
        std::cout << "최대 인원은 2명에서 10명 사이로 설정해주세요." << std::endl;
        return;
    }

    std::string roomCode;
    if (m_room_service->CreateRoom(roomName, maxUsers, roomCode))
    {
        std::cout << "\n방이 생성되었습니다!" << std::endl;
        std::cout << "방 코드: " << roomCode << std::endl;
        std::cout << "\n채팅을 시작합니다..." << std::endl;
        StartChatSession();
    }
    else
    {
        std::cout << "방 생성에 실패했습니다." << std::endl;
    }
}

void Lobby::HandleJoinRoom()
{
    std::string roomCode;

    std::cout << "\n--- 채팅방 참여 ---" << std::endl;
    std::cout << "방 코드: ";
    std::getline(std::cin, roomCode);

    if (roomCode.empty())
    {
        std::cout << "방 코드를 입력해주세요." << std::endl;
        return;
    }

    if (m_room_service->JoinRoom(roomCode))
    {
        std::cout << "\n방에 참여했습니다!" << std::endl;
        std::cout << "\n채팅을 시작합니다..." << std::endl;
        StartChatSession();
    }
    else
    {
        std::cout << "방 참여에 실패했습니다." << std::endl;
    }
}

void Lobby::StartChatSession()
{
    ConsoleUI::DisplaySystemMessage("채팅이 시작되었습니다! (종료: /exit)");

    m_receiver_thread = std::thread([this]()
    {
        m_chat_service->StartReceiving(
            [this](const Message& message) {
                // 서버로부터 받은 메시지는 다른 사람의 메시지 (입장/퇴장 알림 포함)
                OnMessageReceived(message.FormatForDisplay(false));
            }
        );

        ConsoleUI::ClearLine();
        ConsoleUI::DisplaySystemMessage(
            "서버와의 연결이 종료되었습니다. Enter 키를 눌러 프로그램을 종료하세요."
        );
    });

    InputLoop();
}

void Lobby::InputLoop()
{
    ConsoleUI::ShowInputPrompt();

    while (m_connection_service->IsConnected())
    {
        std::string input = ConsoleUI::GetMessageInput();

        if (std::cin.eof() || !m_connection_service->IsConnected())
            break;

        if (input == "/exit")
        {
            // 방에서 나가기
            if (m_room_service->IsInRoom())
            {
                m_room_service->ExitRoom();
            }
            break;
        }

        if (!input.empty())
        {
            // 메시지 전송
            if (m_chat_service->SendMessage(input))
            {
                // 성공하면 자기 메시지를 즉시 화면에 출력
                
                auto now = std::chrono::system_clock::now();
                std::time_t now_time = std::chrono::system_clock::to_time_t(now);
                char time_buffer[100];
                std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now_time));

                Message my_message("", "", input, std::string(time_buffer));
                ConsoleUI::ClearLine();
                std::cout << my_message.FormatForDisplay(true) << std::endl;
            }
        }

        ConsoleUI::ShowInputPrompt();
    }

    m_chat_service->StopReceiving();

    // 수신 스레드 종료 대기
    if (m_receiver_thread.joinable())
        m_receiver_thread.join();

    // 채팅 종료 후 자동으로 로비로 돌아감
    if (m_connection_service->IsConnected() && m_is_running)
    {
        std::cout << "\n로비로 돌아갑니다...\n" << std::endl;
        ShowLobbyMenu();
    }
}

void Lobby::OnMessageReceived(const std::string& message)
{
    ConsoleUI::ClearLine();
    std::cout << message << std::endl;
    ConsoleUI::ShowInputPrompt();
}
