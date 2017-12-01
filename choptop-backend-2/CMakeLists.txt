cmake_minimum_required(VERSION 3.7)
project(choptop)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(SOURCE_FILES
        src/hx711.cpp
        src/hx711.h
        src/main.cpp
        src/hx711_mock.cpp)

add_executable(choptop ${SOURCE_FILES})

target_link_libraries(choptop Threads::Threads)

if (IS_RPI)
    find_library(wiringPi_LIB wiringPi)
    target_link_libraries(choptop ${wiringPi_LIB})
    target_compile_definitions(choptop PRIVATE IS_RPI=1)
endif()