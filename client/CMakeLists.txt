cmake_minimum_required(VERSION 3.16)
project(cpp-socket-chat LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files organized by functionality
set(NETWORK_SOURCES
    src/network/NetworkClient.cpp
    src/network/PacketHandler.cpp
)

set(CORE_SOURCES
    src/core/Message.cpp
    src/core/ConnectionService.cpp
    src/core/ChatService.cpp
    src/core/RoomService.cpp
)

set(UI_SOURCES
    src/ui/ChatClient.cpp
    src/ui/ConsoleUI.cpp
    src/ui/Lobby.cpp
)

set(COMMON_SOURCES
    src/common/logger.cpp
)

# Executable
add_executable(client
    src/main.cpp
    ${NETWORK_SOURCES}
    ${CORE_SOURCES}
    ${UI_SOURCES}
    ${COMMON_SOURCES}
)

target_include_directories(client PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/common
)

# Precompiled Headers for faster compilation
target_precompile_headers(client
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include/common/pch.h
)

set_target_properties(client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
