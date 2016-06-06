cmake_minimum_required(VERSION 3.4)
project(eventServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES source/main.cpp)
add_executable(eventServer ${SOURCE_FILES})