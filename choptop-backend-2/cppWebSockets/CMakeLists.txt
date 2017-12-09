cmake_minimum_required(VERSION 3.6)
project(choptop)

set(CMAKE_CXX_STANDARD 14)

add_library(ws Util.cpp Util.h WebSocketServer.cpp WebSocketServer.h)
target_link_libraries(ws -lwebsockets)
target_include_directories(ws INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})