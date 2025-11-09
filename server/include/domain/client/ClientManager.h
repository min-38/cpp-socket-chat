#pragma once

#include "common/pch.h"
#include <unordered_map>
#include <mutex>
#include <string>
#include <vector>

struct ClientInfo
{
    int fd;
    std::string ip;
    std::string nickname;
    bool isAuthenticated = false;
    std::vector<char> recvBuffer; // 수신 버퍼
};

class ClientManager
{
public:
    // 클라이언트 추가
    void addClient(int client_fd, const std::string& ip);
    
    // 클라이언트 제거
    void removeClient(int client_fd);
    
    // 클라이언트 정보 가져오기
    ClientInfo* getClient(int client_fd);
    
    // 인증된 모든 클라이언트 fd 가져오기
    std::vector<int> getAllAuthenticatedClientFds(int exclude_fd = -1);

    // 닉네임 중복 확인
    bool isNicknameTaken(const std::string& nickname);
    
    // 닉네임 설정
    void setNickname(int client_fd, const std::string& nickname);
    
    // 닉네임 제거
    void removeNickname(const std::string& nickname);

private:
    std::unordered_map<int, ClientInfo> m_clients;
    std::unordered_set<std::string> m_nicknames;
    std::mutex m_mutex;
};
