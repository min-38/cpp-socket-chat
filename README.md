# C++ Socket Chat

고성능 멀티스레드 TCP 소켓 기반 채팅 애플리케이션

## 목차

- [개요](#개요)
- [주요 기능](#주요-기능)
- [기술 스택](#기술-스택)
- [아키텍처](#아키텍처)
- [프로젝트 구조](#프로젝트-구조)
- [요구사항](#요구사항)
- [빌드 및 실행](#빌드-및-실행)
- [사용법](#사용법)
- [프로토콜 명세](#프로토콜-명세)

## 개요

C++17로 구현된 실시간 채팅 시스템입니다. epoll 기반의 이벤트 주도 아키텍처와 스레드풀을 활용하여 높은 동시성과 성능을 제공합니다. 서버는 레이어드 아키텍처로 설계되어 유지보수성과 확장성이 뛰어납니다.

## 주요 기능

### 서버
- **고성능 비동기 I/O**: epoll을 활용한 이벤트 주도 아키텍처
- **멀티스레드 처리**: 하드웨어 코어 수에 맞춘 스레드풀 활용
- **레이어드 아키텍처**: Application, Domain, Network, I/O, Concurrency 계층 분리
- **실시간 메시지 브로드캐스팅**: 모든 연결된 클라이언트에게 메시지 전송
- **환경 변수 기반 설정**: .env 파일을 통한 포트 설정
- **로깅 시스템**: 서버 상태 및 이벤트 추적

### 클라이언트
- **콘솔 기반 UI**: 직관적인 터미널 인터페이스
- **닉네임 설정**: 사용자 식별을 위한 닉네임 등록
- **실시간 채팅**: 메시지 송수신 및 시스템 알림 수신
- **계층화된 서비스**: Network, Core, UI 계층 분리

## 기술 스택

- **언어**: C++17
- **빌드 시스템**: CMake 3.16+
- **네트워킹**: POSIX Socket API, epoll
- **동시성**: std::thread, ThreadPool
- **데이터 형식**: JSON (nlohmann/json)
- **플랫폼**: Linux/macOS

## 아키텍처

### 서버 레이어드 아키텍처

```
┌─────────────────────────────────────┐
│      Application Layer              │  Server
│                                     │
├─────────────────────────────────────┤
│      Domain Layer                   │  PacketHandler, ClientManager
│                                     │  NicknameHandler, MessageHandler
├─────────────────────────────────────┤
│      Network Layer                  │  Listener, ConnectionManager
│                                     │  PacketReceiver, PacketSender
├─────────────────────────────────────┤
│      I/O Layer                      │  EpollManager
│                                     │
├─────────────────────────────────────┤
│      Concurrency Layer              │  ThreadPool, ThreadSafeQueue
│                                     │
└─────────────────────────────────────┘
```

### 클라이언트 아키텍처

```
┌─────────────────────────────────────┐
│      UI Layer                       │  ChatClient, ConsoleUI
│                                     │
├─────────────────────────────────────┤
│      Core Layer                     │  ConnectionService, ChatService
│                                     │  Message
├─────────────────────────────────────┤
│      Network Layer                  │  NetworkClient, PacketHandler
│                                     │
└─────────────────────────────────────┘
```

## 프로젝트 구조

```
cpp-socket-chat/
├── server/                 # 서버 애플리케이션
│   ├── include/
│   │   ├── application/    # Server 클래스
│   │   ├── domain/         # 비즈니스 로직 (Handler, ClientManager)
│   │   ├── network/        # 네트워크 통신 (Listener, PacketReceiver/Sender)
│   │   ├── io/             # I/O 관리 (EpollManager)
│   │   ├── concurrency/    # 동시성 (ThreadPool, ThreadSafeQueue)
│   │   ├── config/         # 환경 설정 (Env)
│   │   └── logger/         # 로깅 시스템
│   ├── src/                # 구현 파일
│   ├── .env                # 서버 설정 (포트 등)
│   └── CMakeLists.txt
│
├── client/                 # 클라이언트 애플리케이션
│   ├── include/
│   │   ├── ui/             # 사용자 인터페이스
│   │   ├── core/           # 핵심 서비스 로직
│   │   ├── network/        # 네트워크 통신
│   │   └── common/         # 공통 유틸리티
│   ├── src/                # 구현 파일
│   └── CMakeLists.txt
│
└── README.md
```

## 요구사항

### 시스템 요구사항
- Linux 또는 macOS
- CMake 3.16 이상
- GCC 7+ 또는 Clang 5+ (C++17 지원)

### 빌드 도구
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake

# macOS
brew install cmake
```

## 빌드 및 실행

### 서버 빌드 및 실행

```bash
# 서버 디렉토리로 이동
cd server

# 빌드 디렉토리 생성 및 빌드
cmake .
make

# 서버 실행
./server
```

**환경 변수 설정** (선택사항):
```bash
# server/.env 파일 수정
SERVER_PORT=8877
```

### 클라이언트 빌드 및 실행

```bash
# 클라이언트 디렉토리로 이동
cd client

# 빌드 디렉토리 생성 및 빌드
cmake .
make

# 클라이언트 실행
./client
```

## 사용법

### 1. 서버 시작
```bash
cd server
./server
```
서버가 시작되면 다음과 같은 로그를 확인할 수 있습니다:
```
[INFO] Server started on port: 8877
[INFO] Using 8 worker threads.
```

### 2. 클라이언트 접속
```bash
cd client
./client
```

### 3. 채팅 시작
1. 클라이언트 실행 시 서버 IP와 포트 입력
2. 닉네임 설정
3. 메시지 입력 및 전송
4. `/quit` 명령어로 종료

### 예시 세션
```
서버의 IP를 입력해주세요: 127.0.0.1
서버의 PORT를 입력해주세요: 8877
닉네임을 입력하세요: Alice
[시스템] 서버에 연결되었습니다.
사용할 닉네임을 입력하세요 (32자 이하, 띄어쓰기 불가): 아무개
닉네임 [아무개] 사용이 승인되었습니다.
[시스템] 채팅이 시작되었습니다! (종료: /exit)  
(입력창) > 
[2025-11-09 21:57:34][아무개2(127.0.0.2)] > 안녕하세요
```

## 프로토콜 명세

### 패킷 구조

```cpp
struct PacketHeader {
    PacketType type;    // 2 bytes
    uint16_t size;      // 2 bytes (페이로드 크기)
};

struct Packet {
    PacketHeader header;
    char payload[MAX_PAYLOAD_SIZE];  // 1024 bytes (JSON)
};
```

### 패킷 타입

#### 클라이언트 → 서버
| 타입 | 코드 | 설명 |
|------|------|------|
| NICKNAME_REQUEST | 101 | 닉네임 등록 요청 |
| MESSAGE_SEND | 201 | 채팅 메시지 전송 |

#### 서버 → 클라이언트
| 타입 | 코드 | 설명 |
|------|------|------|
| NICKNAME_RESPONSE_OK | 1001 | 닉네임 등록 성공 |
| NICKNAME_RESPONSE_FAIL | 1002 | 닉네임 등록 실패 |
| MESSAGE_CHAT | 1101 | 일반 채팅 메시지 |
| MESSAGE_SYSTEM | 1102 | 시스템 알림 메시지 |

### 페이로드 형식 (JSON)

#### 닉네임 요청
```json
{
  "nickname": "Alice"
}
```

#### 메시지 전송
```json
{
  "message": "Hello, World!"
}
```

#### 채팅 메시지 수신
```json
{
  "nickname": "Bob",
  "message": "Hi there!"
}
```

## 주요 특징

### 서버 성능 최적화
- **epoll 이벤트 루프**: 수천 개의 동시 연결 처리 가능
- **스레드풀**: CPU 코어 수에 최적화된 워커 스레드
- **논블로킹 I/O**: 단일 스레드로 다중 클라이언트 모니터링
- **제로 카피**: 효율적인 메모리 관리

### 코드 품질
- **레이어드 아키텍처**: 명확한 책임 분리
- **RAII 패턴**: 자동 리소스 관리
- **스마트 포인터**: 메모리 누수 방지
- **예외 처리**: 안정적인 에러 핸들링

### 확장성
- 새로운 패킷 타입 추가 용이
- 핸들러 패턴으로 비즈니스 로직 확장 가능
- 계층 분리로 각 레이어 독립적 개선 가능

## 개발 환경

### 디버깅
```bash
# 디버그 빌드
cmake -DCMAKE_BUILD_TYPE=Debug .
make

# GDB로 디버깅
gdb ./server
```

### 로그 확인
서버는 실행 중 다음과 같은 로그를 출력합니다:
- `[INFO]`: 일반 정보
- `[WARN]`: 경고
- `[ERROR]`: 오류

## 라이선스

이 프로젝트는 MIT 라이선스를 따릅니다.

## 기여

버그 리포트, 기능 제안, 풀 리퀘스트를 환영합니다.

---

**Made with C++17**
