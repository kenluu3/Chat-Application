cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(chatroom-client VERSION 1.0)

file(GLOB_RECURSE SRC_F *.cpp)
add_executable(chat-client ${SRC_F})

target_include_directories(chat-client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/client)