//
// Created by yuzhanglong on 2020/6/19.
//

#ifndef Y_JUDGER_COMMON_H
#define Y_JUDGER_COMMON_H


#include <sys/resource.h>

enum JUDGE_CONDITION {
    ACCEPTED = 1, // 程序通过
    COMPILE_ERROR, // 编译错误
    WRONG_ANSWER,  // 答案错误
    RUNTIME_ERROR, // 运行时错误
    TIME_LIMIT_EXCEEDED, // 时间超限
    MEMORY_LIMIT_EXCEED, // 内存超限
    OUTPUT_LIMIT_EXCEED, // 输出超过限制
    SEGMENTATION_FAULT,  // 段错误
    FLOAT_ERROR, // 浮点错误
    UNKNOWN_ERROR // 未知错误
};

struct timeoutkillerConfig {
    int pid;
    int limitTime;
};

struct execConfig {
    rlim_t timeLimit;
    rlim_t memoryLimit;
    rlim_t processLimit;
    rlim_t outputLimit;
    char *execPath;
    char *stdinPath;
    char *stdoutPath;
};

struct judgeResult {
    rlim_t realTimeCost;
    rlim_t memoryCost;
    rlim_t cpuTimeCost;
    int condtion;
};


#endif //Y_JUDGER_COMMON_H
