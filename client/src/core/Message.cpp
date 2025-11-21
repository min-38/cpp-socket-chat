#include "core/Message.h"
#include <sstream>

std::string Message::FormatForDisplay(bool is_mine) const
{
    std::stringstream ss;

    switch (m_type)
    {
        case MessageType::USER_JOIN:
            ss << "[시스템] " << m_sender_nickname << "님이 입장하셨습니다.";
            break;

        case MessageType::USER_EXIT:
            ss << "[시스템] " << m_sender_nickname << "님이 퇴장하셨습니다.";
            break;

        case MessageType::SYSTEM:
            ss << "[시스템] " << m_content;
            break;

        case MessageType::CHAT:
        default:
            ss << "[" << m_timestamp << "]";

            // 자기가 보낸 메시지는 '나'로 표시
            if (is_mine)
                ss << "[나] > " << m_content;
            else
                ss << "[" << m_sender_nickname << "(" << m_sender_ip << ")] > " << m_content;
            break;
    }

    return ss.str();
}
