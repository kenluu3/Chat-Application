cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(chatroom VERSION 1.0)

file(GLOB_RECURSE SRC_F src/*.cpp)
add_executable(chatroom ${SRC_F})

target_include_directories(chatroom PUBLIC src/Client src/Server)