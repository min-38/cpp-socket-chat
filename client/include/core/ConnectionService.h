#pragma once

#include "common/pch.h"
#include "network/INetworkClient.h"
#include "core/User.h"

class ConnectionService
{
public:
    explicit ConnectionService(std::shared_ptr<INetworkClient> network_client);

    // 서버 연결
    bool ConnectToServer(const std::string& host, int port);

    // 닉네임 등록
    bool RegisterNickname(const std::string& nickname);

    // 닉네임 유효성 검사
    bool ValidateNickname(const std::string& nickname) const;

    // 서버 연결 종료
    void Disconnect();

    // 연결 상태 리턴
    bool IsConnected() const;

    // 현재 사용자 정보 리턴
    const User& GetCurrentUser() const { return m_current_user; }

private:
    std::shared_ptr<INetworkClient> m_network_client;
    User m_current_user;
};
