#include "ui/ChatClient.h"
#include "ui/ConsoleUI.h"
#include "network/NetworkClient.h"

ChatClient::ChatClient()
{
    m_network_client = std::make_shared<NetworkClient>();
    m_connection_service = std::make_unique<ConnectionService>(m_network_client);
    m_chat_service = std::make_unique<ChatService>(m_network_client);
}

ChatClient::~ChatClient()
{
    if (m_receiver_thread.joinable())
    {
        m_chat_service->StopReceiving();
        m_receiver_thread.join();
    }

    if (m_connection_service->IsConnected())
        m_connection_service->Disconnect();
}

void ChatClient::Run()
{
    if (!SetupConnection()) return;

    if (!RegisterUser()) return;

    StartChatSession();
}

bool ChatClient::SetupConnection()
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

bool ChatClient::RegisterUser()
{
    while (m_connection_service->IsConnected())
    {
        std::string nickname = ConsoleUI::GetNickname();

        if (m_connection_service->RegisterNickname(nickname))
            return true;
    }

    return false;
}

void ChatClient::StartChatSession()
{
    ConsoleUI::DisplaySystemMessage("채팅이 시작되었습니다! (종료: /exit)");

    m_receiver_thread = std::thread([this]()
    {
        m_chat_service->StartReceiving(
            [this](const Message& message) {
                OnMessageReceived(message);
            }
        );

        ConsoleUI::ClearLine();
        ConsoleUI::DisplaySystemMessage(
            "서버와의 연결이 종료되었습니다. Enter 키를 눌러 프로그램을 종료하세요."
        );
    });

    InputLoop();
}

void ChatClient::InputLoop()
{
    ConsoleUI::ShowInputPrompt();

    while (m_connection_service->IsConnected())
    {
        std::string input = ConsoleUI::GetMessageInput();

        if (std::cin.eof() || !m_connection_service->IsConnected())
            break;

        if (input == "/exit")
            break;

        if (!input.empty())
            m_chat_service->SendMessage(input);

        ConsoleUI::ShowInputPrompt();
    }

    m_chat_service->StopReceiving();
}

void ChatClient::OnMessageReceived(const Message& message)
{
    ConsoleUI::DisplayMessage(message);
}
