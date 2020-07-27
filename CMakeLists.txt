cmake_minimum_required(VERSION 3.16)
project(api_project C)

set(CMAKE_C_STANDARD 11)

set(GCC_COVERAGE_COMPILE_FLAGS "-DEVAL -std=gnu11 -O2 -pipe -static -s -lm")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}${GCC_COVERAGE_COMPILE_FLAGS}")

add_executable(api_project main.c)
