#pragma once

class User
{
public:
    User() = default;
    User(const std::string& nickname)
        : m_nickname(nickname) {}

    User(const std::string& nickname, const std::string& ip_address)
        : m_nickname(nickname), m_ip_address(ip_address) {}

    // Getter
    const std::string& GetNickname() const { return m_nickname; }
    const std::string& GetIpAddress() const { return m_ip_address; }

    // Setter
    void SetNickname(const std::string& nickname) { m_nickname = nickname; }
    void SetIpAddress(const std::string& ip_address) { m_ip_address = ip_address; }

    // Validation
    bool IsValid() const { return !m_nickname.empty(); }

private:
    std::string m_nickname;
    std::string m_ip_address;
};
