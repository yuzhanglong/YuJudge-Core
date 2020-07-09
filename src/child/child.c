#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "child.h"

#include "../guard/guard.h"

#define CHILD_EXIT exit

/**
 * @author yzl
 * @param execConfig 用户提供的运行的配置
 * @return void
 * 向（子）进程设限，例如内存、输出限制等限制
 */

void setLimitation(struct execConfig *execConfig) {
    // 内存超限
    struct rlimit maxMemory;
    maxMemory.rlim_cur = maxMemory.rlim_max = execConfig->memoryLimit;

    if (setrlimit(RLIMIT_AS, &maxMemory) != 0) {
        CHILD_EXIT(SET_LIMIT_ERROR);
    }

    // 时间超限
    struct rlimit maxTime;
    maxTime.rlim_cur = maxTime.rlim_max = execConfig->cpuTimeLimit;
    if (setrlimit(RLIMIT_CPU, &maxTime) != 0) {
        CHILD_EXIT(SET_LIMIT_ERROR);
    }

    // 进程超限
    struct rlimit maxProcessAmount;
    maxProcessAmount.rlim_cur = maxProcessAmount.rlim_max = execConfig->processLimit;
    if (setrlimit(RLIMIT_NPROC, &maxProcessAmount) != 0) {
        CHILD_EXIT(SET_LIMIT_ERROR);
    }


    // 输出超限
    struct rlimit maxOutput;
    maxOutput.rlim_cur = maxOutput.rlim_max = execConfig->outputLimit;
    if (setrlimit(RLIMIT_FSIZE, &maxOutput) != 0) {
        CHILD_EXIT(SET_LIMIT_ERROR);
    }
}

/**
 * @author yzl
 * @param execConfig 用户提供的运行的配置
 * @return void
 * 在fork出孩子进程之后调用之 运行孩子进程（用户提交的代码会在这里被运行）
 */

void runChild(struct execConfig *execConfig) {

    FILE *inputFile = NULL;
    FILE *outputFile = NULL;
    FILE *errFile = NULL;

    if (execConfig->stdinPath[0] != '\0') {
        inputFile = fopen(execConfig->stdinPath, "r");
        if (!inputFile) {
            CHILD_EXIT(INPUT_FILE_NOT_FOUND);
        }
        int f = fileno(inputFile);
        dup2(f, STDIN_FILENO);
    }
    if (execConfig->stdoutPath[0] != '\0') {
        outputFile = fopen(execConfig->stdoutPath, "w");
        if (!inputFile) {
            CHILD_EXIT(CAN_NOT_MAKE_OUTPUT);
        }
        int f2 = fileno(outputFile);
        dup2(f2, STDOUT_FILENO);
    }
    if (execConfig->stderrPath[0] != '\0') {
        errFile = fopen(execConfig->stderrPath, "w");
        if (!inputFile) {
            CHILD_EXIT(CAN_NOT_MAKE_OUTPUT);
        }
        int f3 = fileno(errFile);
        dup2(f3, STDERR_FILENO);
    }
    setLimitation(execConfig);

    // TODO: 目前安全性的实现还有一些问题，暂时注释
    // setSeccompGuard();

    // 执行用户的提交
    execve(execConfig->execPath, NULL, NULL);
    CHILD_EXIT(EXIT_SUCCESS);
}