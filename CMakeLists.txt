cmake_minimum_required(VERSION 3.4)
project(eventServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES source/main.cc source/protocol/ntprotocol.cc source/protocol/ntprotocol.cc source/protocol/ntprotocol.h source/process/SONEEventProcess.cc source/process/SONEEventProcess.h source/msgdecoder/ntmsgdecoder.cpp source/msgdecoder/ntmsgdecoder.h source/c_type/c_type.h)
add_executable(eventServer ${SOURCE_FILES})

#加载静态库 libevent.a
target_link_libraries(eventServer event)