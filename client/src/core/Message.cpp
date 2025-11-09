#include "core/Message.h"
#include <sstream>

std::string Message::FormatForDisplay() const
{
    std::stringstream ss;
    ss << "[" << m_timestamp << "]"
       << "[" << m_sender_nickname << "(" << m_sender_ip << ")] > "
       << m_content;
    return ss.str();
}
