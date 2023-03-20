cmake_minimum_required(VERSION 3.0)

project("hello")

add_executable(my_program main.c data.c)

target_include_directories(my_program PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")