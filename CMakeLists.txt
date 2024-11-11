cmake_minimum_required(VERSION 3.29)
project(debug_api_test C)

set(CMAKE_C_STANDARD 11)

add_executable(detach_before_continue detach_before_continue.c)
add_executable(continue_before_detach continue_before_detach.c)
