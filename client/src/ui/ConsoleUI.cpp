#include "ui/ConsoleUI.h"
#include <limits>

void ConsoleUI::DisplayMessage(const Message& message)
{
    ClearLine();
    std::cout << message.FormatForDisplay() << std::endl;
    ShowInputPrompt();
}

void ConsoleUI::DisplaySystemMessage(const std::string& message)
{
    ClearLine();
    std::cout << "[시스템] " << message << std::endl;
}

void ConsoleUI::DisplayError(const std::string& error)
{
    std::cerr << "[오류] " << error << std::endl;
}

void ConsoleUI::ShowInputPrompt()
{
    std::cout << "(입력창) > " << std::flush;
}

void ConsoleUI::ClearLine()
{
    std::cout << "\r" << std::string(80, ' ') << "\r";
}

void ConsoleUI::GetConnectionInfo(std::string& host, int& port)
{
    std::cout << "서버의 IP를 입력해주세요: ";
    std::cin >> host;

    std::cout << "서버의 PORT를 입력해주세요: ";
    std::cin >> port;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string ConsoleUI::GetNickname()
{
    std::string nickname;
    std::cout << "사용할 닉네임을 입력하세요 (32자 이하, 띄어쓰기 불가): ";
    std::cin >> nickname;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return nickname;
}

std::string ConsoleUI::GetMessageInput()
{
    std::string line;
    std::getline(std::cin, line);
    return line;
}
