cmake_minimum_required(VERSION 3.16)
project(y_judger C)
LINK_LIBRARIES("pthread")
LINK_LIBRARIES("seccomp")
set(CMAKE_C_STANDARD 99)
add_executable(
        y_judger
        src/main.c
        src/logger/logger.c
        src/logger/logger.h
        src/common/common.c
        src/common/common.h
        src/timeHelper/timeHelper.c
        src/timeHelper/timeHelper.h
        src/sysHelper/sysHelper.c
        src/sysHelper/sysHelper.h
        src/child/child.c
        src/child/child.h
        src/judger/judger.c
        src/judger/judger.h
        src/guard/guard.c
        src/guard/guard.h)