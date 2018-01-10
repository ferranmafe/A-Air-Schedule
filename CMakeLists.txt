cmake_minimum_required(VERSION 3.6)
project(A_Air_Schedule)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    edmondsKarp.cc
    main.cpp
    Makefile
    test_entry.txt)

add_executable(A_Air_Schedule ${SOURCE_FILES})