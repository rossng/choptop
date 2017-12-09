cmake_minimum_required(VERSION 3.6)
project(choptop)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost REQUIRED COMPONENTS system thread filesystem regex coroutine context)

set(SOURCE_FILES
        src/hx711.cpp
        src/hx711.h
        src/main.cpp
        src/hx711_mock.cpp
        src/LoadCellReader.cpp
        src/LoadCellReader.h
        src/LoadCellsProcessor.cpp
        src/LoadCellsProcessor.h
        src/PositionProcessor.cpp
        src/PositionProcessor.h)

add_executable(choptop ${SOURCE_FILES})

target_link_libraries(choptop Threads::Threads)

if (IS_RPI)
    find_library(wiringPi_LIB wiringPi)
    target_link_libraries(choptop ${wiringPi_LIB})
    target_compile_definitions(choptop PRIVATE IS_RPI=1)
endif()

if(Boost_FOUND)
    message(${Boost_INCLUDE_DIRS})
    target_include_directories(choptop PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(choptop ${Boost_LIBRARIES})
endif()

add_library(sws INTERFACE)
target_include_directories(sws INTERFACE vendor)

find_package(OpenSSL REQUIRED)
target_link_libraries(sws INTERFACE ${OPENSSL_LIBRARIES})
target_include_directories(sws INTERFACE ${OPENSSL_INCLUDE_DIR})

if(WIN32)
    target_link_libraries(sws INTERFACE ws2_32 wsock32)
endif()

target_include_directories(sws INTERFACE ${Boost_INCLUDE_DIRS})
target_link_libraries(sws INTERFACE ${Boost_LIBRARIES})

add_library(ws_server ws_server/WebsocketServer.cpp ws_server/WebsocketServer.h)
target_include_directories(ws_server PUBLIC ws_server)
target_link_libraries(ws_server sws)

target_link_libraries(choptop ws_server)

add_executable(server_test src/server_test.cpp)
target_link_libraries(server_test ws_server)