cmake_minimum_required(VERSION 3.6)
project(choptop)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost REQUIRED COMPONENTS system thread filesystem regex coroutine context)

set(SOURCE_FILES
        src/main.cpp
        src/DataProcessor.cpp
        src/DataProcessor.h)

add_executable(choptop ${SOURCE_FILES})

target_link_libraries(choptop Threads::Threads server sensor_reader)
target_include_directories(choptop PUBLIC sensor_reader/)

if (Boost_FOUND)
    message(${Boost_INCLUDE_DIRS})
    target_include_directories(choptop PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(choptop ${Boost_LIBRARIES})
endif ()

add_library(sensor_reader
        sensor_reader/rs232-linux.c
        sensor_reader/rs232-win.c
        sensor_reader/rs232.h
        sensor_reader/SensorReader.cpp
        sensor_reader/SensorReader.h)

target_include_directories(sensor_reader PUBLIC ${Boost_INCLUDE_DIRS})
target_link_libraries(sensor_reader ${Boost_LIBRARIES})

add_library(server server/ChoptopServer.cpp server/ChoptopServer.h)
target_include_directories(server PUBLIC ${Boost_INCLUDE_DIRS} ${WEBSOCKETPP_INCLUDE_DIR} server/)
target_link_libraries(server ${Boost_LIBRARIES})
if (WIN32)
    target_link_libraries(server ws2_32 wsock32)
endif ()

add_executable(server_test src/test_server.cpp)
target_link_libraries(server_test server)
target_include_directories(server_test PUBLIC server/)

add_executable(serial_test src/test_serial.cpp)
target_link_libraries(serial_test sensor_reader)
target_include_directories(serial_test PUBLIC sensor_reader/)