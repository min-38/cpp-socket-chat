cmake_minimum_required(VERSION 3.16)
project(cpp-socket-chat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server
    src/main.cpp

    # Application Layer
    src/application/Server.cpp

    # Domain Layer
    src/domain/client/ClientManager.cpp
    src/domain/handler/MessageHandler.cpp
    src/domain/handler/NicknameHandler.cpp
    src/domain/handler/PacketHandler.cpp

    # Network Layer
    src/network/Listener.cpp
    src/network/ConnectionManager.cpp
    src/network/PacketReceiver.cpp
    src/network/PacketSender.cpp

    # I/O Layer
    src/io/EpollManager.cpp

    # Concurrency Layer
    src/concurrency/ThreadPool.cpp

    # Config Layer
    src/config/Env.cpp

    # Logger Layer
    src/logger/Logger.cpp
)

target_include_directories(server PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)