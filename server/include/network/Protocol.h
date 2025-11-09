#pragma once

#include <cstdint>
#include <string>

const int RECV_BUFFER_SIZE = 4096;
const int MAX_NICKNAME_LEN = 32;
const int IP_ADDRESS_LEN = 16;
const int MAX_PAYLOAD_SIZE = 1024;

enum class PacketType : uint16_t
{
    // Client -> Server
    NICKNAME_REQUEST = 101,
    MESSAGE_SEND = 201,

    // Server -> Client
    NICKNAME_RESPONSE_OK = 1001,
    NICKNAME_RESPONSE_FAIL = 1002,

    // Server -> Client (메시지들)
    MESSAGE_CHAT = 1101,        // 일반 채팅 메시지
    MESSAGE_SYSTEM = 1102,      // 시스템 알림
};

// 바이트 하나하나 딱 맞춰야 하는 파일/네트워크 데이터 다룰 때 유용
#pragma pack(push, 1) // push, 1 = “지금부터 구조체 패딩 끄기(1바이트 정렬), 그리고 이전 설정 저장”.

struct PacketHeader
{
    PacketType type;
    uint16_t size; // payload에 담긴 JSON 문자열의 길이
};

struct Packet
{
    PacketHeader header;
    char payload[MAX_PAYLOAD_SIZE]; // JSON 문자열이 복사될 공간
};

#pragma pack(pop) // pop = “이전 설정 복원, 구조체 패딩 원래대로 돌려놓기”.
