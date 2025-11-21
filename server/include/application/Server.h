#pragma once

#include "io/EpollManager.h"
#include "network/Listener.h"
#include "network/ConnectionManager.h"
#include "network/PacketReceiver.h"
#include "network/PacketSender.h"
#include "concurrency/ThreadPool.h"
#include "domain/client/ClientManager.h"
#include "domain/handler/PacketHandler.h"
#include "domain/room/RoomManager.h"

#include <atomic>
#include <memory>
#include <thread>

class Server
{
public:
    Server();
    ~Server();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;
    Server(Server&&) = delete;
    Server& operator=(Server&&) = delete;

    void run();

    // PacketHandler에서 사용할 수 있도록 공개
    void sendPacket(int client_fd, const Packet& packet);
    void broadcastPacket(const Packet& packet, int exclude_fd = -1);
    void broadcastPacketForRoom(const Packet& packet, const std::string& roomCode, int exclude_fd = -1);

    // Getter
    RoomManager& getRoomManager() { return m_room_manager; }

private:
    void eventLoop();
    void handleEvent(int fd, uint32_t events);

    int m_port;

    // 네트워크 컴포넌트
    std::unique_ptr<Listener>           m_listener;
    std::unique_ptr<EpollManager>       m_epoll_manager;
    std::unique_ptr<ConnectionManager>  m_connection_manager;
    std::unique_ptr<PacketReceiver>     m_packet_receiver;
    std::unique_ptr<PacketSender>       m_packet_sender;

    // 도메인
    ClientManager   m_client_manager;
    PacketHandler   m_packet_handler;
    RoomManager     m_room_manager;

    // concurrency 컴포넌트
    std::unique_ptr<ThreadPool> m_thread_pool;
    std::thread m_io_thread;
};
