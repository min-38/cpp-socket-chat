#pragma once

#include <cstdint>
#include <string>

const int RECV_BUFFER_SIZE = 4096;
const int MAX_NICKNAME_LEN = 32;
const int MAX_PAYLOAD_SIZE = 256;
const int IP_ADDRESS_LEN = 16;

enum class PacketType : uint16_t
{
    NICKNAME_REQUEST = 101,
    NICKNAME_RESPONSE_OK = 1001,
    NICKNAME_RESPONSE_FAIL = 1002,

    MESSAGE_SEND_REQUEST = 201,
    MESSAGE_RECEIVED = 202,
    MESSAGE_BROADCAST = 2001,
};

#pragma pack(push, 1)

struct PacketHeader
{
    PacketType type;
    uint16_t size;
};

struct Packet
{
    PacketHeader header; // 패킷의 종류
    char payload[MAX_PAYLOAD_SIZE]; // 실제 데이터가 복사될 공간
};

struct MessageReceivedPayload
{
    uint64_t timestamp; // Unix timestamp (e.g., from time(nullptr))
    char senderNickname[MAX_NICKNAME_LEN];
    char senderIp[IP_ADDRESS_LEN];
    char message[MAX_PAYLOAD_SIZE - MAX_NICKNAME_LEN];
};

#pragma pack(pop)