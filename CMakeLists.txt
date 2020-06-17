cmake_minimum_required(VERSION 3.16)
project(y_judger C)

set(CMAKE_C_STANDARD 99)

add_executable(y_judger src/main.c src src/utils/logger/logger.c src/utils/logger/logger.h)