cmake_minimum_required(VERSION 3.10)
project(multithread CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(../googletest-master/googletest/include)
include_directories(../googletest-master/googletest/)
include_directories(header)

add_executable(multithread image_fifo.h image_fifo.cpp main.cpp)

target_link_libraries(multithread /home/valmit/CLionProjects/googletest-master/lib/libgtest.a)

find_package(Threads)

target_link_libraries(multithread pthread)
