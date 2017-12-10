cmake_minimum_required(VERSION 3.6)
project(choptop)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost REQUIRED COMPONENTS system thread filesystem regex coroutine context)

set(SOURCE_FILES
        src/hx711.h
        src/main.cpp
        src/LoadCellReader.cpp
        src/LoadCellReader.h
        src/WeightProcessor.cpp
        src/WeightProcessor.h
        src/PositionProcessor.cpp
        src/PositionProcessor.h)

if (HX711_GPIO)
    set(SOURCE_FILES ${SOURCE_FILES} src/hx711_gpio.cpp)
elseif (HX711_SERIAL)
    set(SOURCE_FILES ${SOURCE_FILES} src/hx711_serial.cpp)
else ()
    set(SOURCE_FILES ${SOURCE_FILES} src/hx711_mock.cpp)
endif ()

add_executable(choptop ${SOURCE_FILES})

target_link_libraries(choptop Threads::Threads server)

if (Boost_FOUND)
    message(${Boost_INCLUDE_DIRS})
    target_include_directories(choptop PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(choptop ${Boost_LIBRARIES})
endif ()

if (HX711_GPIO)
    find_library(wiringPi_LIB wiringPi)
    target_link_libraries(choptop ${wiringPi_LIB})
endif ()

add_library(rs232 rs232/rs232-linux.c rs232/rs232-win.c rs232/rs232.h)

add_library(server server/ChoptopServer.cpp server/ChoptopServer.h)
target_include_directories(server PUBLIC ${Boost_INCLUDE_DIRS} ${WEBSOCKETPP_INCLUDE_DIR} server/)
target_link_libraries(server ${Boost_LIBRARIES})
if (WIN32)
    target_link_libraries(server ws2_32 wsock32)
endif ()

add_executable(server_test src/test_server.cpp)
target_link_libraries(server_test server)
target_include_directories(server_test PUBLIC server/)