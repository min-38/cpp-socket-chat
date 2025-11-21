#pragma once

enum class MessageType
{
    CHAT,           // 일반 채팅 메시지
    SYSTEM,         // 시스템 메시지
    USER_JOIN,      // 유저 입장 알림
    USER_EXIT       // 유저 퇴장 알림
};

class Message
{
public:
    Message() = default;
    Message(const std::string& content)
        : m_content(content), m_type(MessageType::CHAT) {}

    Message(const std::string& sender_nickname,
            const std::string& sender_ip,
            const std::string& content,
            const std::string& timestamp,
            int sender_fd = -1,
            MessageType type = MessageType::CHAT)
        : m_sender_nickname(sender_nickname),
          m_sender_ip(sender_ip),
          m_content(content),
          m_timestamp(timestamp),
          m_sender_fd(sender_fd),
          m_type(type) {}

    // Getter
    const std::string& GetContent() const { return m_content; }
    const std::string& GetSenderNickname() const { return m_sender_nickname; }
    const std::string& GetSenderIp() const { return m_sender_ip; }
    const std::string& GetTimestamp() const { return m_timestamp; }
    int GetSenderFd() const { return m_sender_fd; }
    MessageType GetType() const { return m_type; }

    // Setter
    void SetContent(const std::string& content) { m_content = content; }
    void SetSenderNickname(const std::string& nickname) { m_sender_nickname = nickname; }
    void SetSenderIp(const std::string& ip) { m_sender_ip = ip; }
    void SetTimestamp(const std::string& timestamp) { m_timestamp = timestamp; }
    void SetSenderFd(int fd) { m_sender_fd = fd; }
    void SetType(MessageType type) { m_type = type; }

    std::string FormatForDisplay(bool is_mine = false) const;

private:
    std::string m_content;
    std::string m_sender_nickname;
    std::string m_sender_ip;
    std::string m_timestamp;
    int m_sender_fd = -1;
    MessageType m_type = MessageType::CHAT;
};
