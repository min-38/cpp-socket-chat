#pragma once

class Message
{
public:
    Message() = default;
    Message(const std::string& content)
        : m_content(content) {}

    Message(const std::string& sender_nickname,
            const std::string& sender_ip,
            const std::string& content,
            const std::string& timestamp)
        : m_sender_nickname(sender_nickname),
          m_sender_ip(sender_ip),
          m_content(content),
          m_timestamp(timestamp) {}

    // Getter
    const std::string& GetContent() const { return m_content; }
    const std::string& GetSenderNickname() const { return m_sender_nickname; }
    const std::string& GetSenderIp() const { return m_sender_ip; }
    const std::string& GetTimestamp() const { return m_timestamp; }

    // Setter
    void SetContent(const std::string& content) { m_content = content; }
    void SetSenderNickname(const std::string& nickname) { m_sender_nickname = nickname; }
    void SetSenderIp(const std::string& ip) { m_sender_ip = ip; }
    void SetTimestamp(const std::string& timestamp) { m_timestamp = timestamp; }

    std::string FormatForDisplay() const;

private:
    std::string m_content;
    std::string m_sender_nickname;
    std::string m_sender_ip;
    std::string m_timestamp;
};
