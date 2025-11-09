#pragma once

#include "common/pch.h"
#include "core/Message.h"

class ConsoleUI
{
public:

    // 메시지 출력
    static void DisplayMessage(const Message& message);

    // 시스템 메시지 출력
    static void DisplaySystemMessage(const std::string& message);

    // 오류 메시지 출력
    static void DisplayError(const std::string& error);

    // 입력 프롬프트 표시
    static void ShowInputPrompt();

    static void ClearLine();

    // 연결 정보 입력 ui 표시
    static void GetConnectionInfo(std::string& host, int& port);

    // 닉네임 입력 ui 표시
    static std::string GetNickname();

    // 메시지 입력 ui 표시
    static std::string GetMessageInput();
};
