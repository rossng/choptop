cmake_minimum_required(VERSION 3.6)
project(choptop)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost REQUIRED COMPONENTS system)

set(SOURCE_FILES
        src/hx711.cpp
        src/hx711.h
        src/main.cpp
        src/hx711_mock.cpp src/LoadCellReader.cpp src/LoadCellReader.h src/LoadCellsProcessor.cpp src/LoadCellsProcessor.h src/PositionProcessor.cpp src/PositionProcessor.h)

add_executable(choptop ${SOURCE_FILES})

target_link_libraries(choptop Threads::Threads)

if(Boost_FOUND)
    message(${Boost_INCLUDE_DIRS})
    target_include_directories(choptop PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(choptop ${Boost_LIBRARIES})
endif()

if (IS_RPI)
    find_library(wiringPi_LIB wiringPi)
    target_link_libraries(choptop ${wiringPi_LIB})
    target_compile_definitions(choptop PRIVATE IS_RPI=1)
endif()