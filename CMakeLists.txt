cmake_minimum_required(VERSION 3.16)
project(cpp-socket-chat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the parent directory of 'utils' to the module path to find the 'utils' CMakeLists.txt
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../")

add_executable(server_bin
    src/main.cpp
    src/core/Server.cpp
    src/network/Listener.cpp
)

target_include_directories(server_bin PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(server_bin PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
