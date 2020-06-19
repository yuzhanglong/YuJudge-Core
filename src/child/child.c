//
// Created by yuzhanglong on 2020/6/19.
//


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "child.h"


void setLimitation(struct execConfig *execConfig) {
    // 内存超限
    struct rlimit maxMemory;
    maxMemory.rlim_cur = maxMemory.rlim_max = execConfig->memoryLimit;

    if (setrlimit(RLIMIT_AS, &maxMemory) != 0) {
        //TODO：返回错误
    }

    // 时间超限
    struct rlimit maxTime;
    maxTime.rlim_cur = maxTime.rlim_max = execConfig->timeLimit;
    if (setrlimit(RLIMIT_CPU, &maxTime) != 0) {
        //TODO：返回错误
    }

    // 进程超限
    struct rlimit maxProcessAmount;
    maxProcessAmount.rlim_cur = maxProcessAmount.rlim_max = execConfig->processLimit;
    if (setrlimit(RLIMIT_NPROC, &maxProcessAmount) != 0) {
        //TODO：返回错误
    }


    // 输出超限
    struct rlimit maxOutput;
    maxOutput.rlim_cur = maxOutput.rlim_max = execConfig->outputLimit;
    if (setrlimit(RLIMIT_FSIZE, &maxOutput) != 0) {
        //TODO：返回错误
    }
}


void runChild(struct execConfig *execConfig) {
    setLimitation(execConfig);
    FILE *inputFile = NULL;
    FILE *outputFile = NULL;

    if (execConfig->stdinPath[0] != '\0') {
        inputFile = fopen(execConfig->stdinPath, "r");
        int f = fileno(inputFile);
        dup2(f, STDIN_FILENO);
    }
    if (execConfig->stdoutPath[0] != '\0') {
        outputFile = fopen(execConfig->stdoutPath, "w");
        int f2 = fileno(outputFile);
        dup2(f2, STDOUT_FILENO);
    }
    execve(execConfig->execPath, NULL, NULL);
    // 成功退出子进程
    exit(EXIT_SUCCESS);
}