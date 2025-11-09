#pragma once

#include "domain/client/ClientManager.h"
#include "network/Protocol.h"
#include <functional>

class PacketReceiver
{
public:
    PacketReceiver(ClientManager& client_manager);

    // 소켓에서 데이터 수신 (버퍼에 추가)
    // 반환값: true = 계속 수신, false = 연결 종료
    bool receive(int client_fd);

    // 버퍼에서 완전한 패킷들을 파싱하고 콜백 호출
    void parsePackets(int client_fd, std::function<void(int fd, const Packet&)> callback);

    // 편의 메서드: receive + parsePackets
    bool receiveAndParse(int client_fd, std::function<void(int fd, const Packet&)> callback);

private:
    ClientManager& m_client_manager;
};
