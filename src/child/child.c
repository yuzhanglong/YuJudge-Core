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
    /* 内存超限
     * 经过测试发现，内存的限制的精确度较低，
     * 我们可以在这里限制一个较大的内存（默认设置了128mb，一般无需更改）
     * 我们可以采用如下的方案，
     * 为了安全性，硬限制内存确实需要，但是做不到像限制时间那样完美
     * 所以我们先为程序设置一个较大的限制值，
     * 这个限制值能让用户的代码完整地正常运行
     * 同时又不会因为各种奇奇怪怪的情况退出
     * 程序运行完成之后我们再来进行比对
     */
    struct rlimit maxMemory;
    // kb to bytes
    maxMemory.rlim_cur = maxMemory.rlim_max = (execConfig->wallMemoryLimit) * 1024;

    if (setrlimit(RLIMIT_AS, &maxMemory) != 0) {
        CHILD_EXIT(SET_LIMIT_ERROR);
    }

    // 时间超限，传入毫秒单位，我们将其转成setrlimit要求的秒单位，并进位(粗略)
    struct rlimit maxTime;
    maxTime.rlim_cur = maxTime.rlim_max = (execConfig->cpuTimeLimit / 1000) + 1;
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

    // 堆栈
    struct rlimit maxStack;
    maxStack.rlim_cur = maxStack.rlim_max = RLIM_INFINITY;
    if (setrlimit(RLIMIT_STACK, &maxStack) != 0) {
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

    // 重定向输入
    if (execConfig->stdinPath[0] != '\0') {
        inputFile = fopen(execConfig->stdinPath, "r");
        if (!inputFile) {
            CHILD_EXIT(INPUT_FILE_NOT_FOUND);
        }
        int f = fileno(inputFile);
        dup2(f, STDIN_FILENO);
    }

    // 重定向标准输出
    if (execConfig->stdoutPath[0] != '\0') {
        outputFile = fopen(execConfig->stdoutPath, "w");
        if (!outputFile) {
            CHILD_EXIT(CAN_NOT_MAKE_OUTPUT);
        }
        int f2 = fileno(outputFile);
        dup2(f2, STDOUT_FILENO);
    }

    // 重定向错误输出
    if (execConfig->stderrPath[0] != '\0') {
        errFile = fopen(execConfig->stderrPath, "w");
        if (!errFile) {
            CHILD_EXIT(CAN_NOT_MAKE_OUTPUT);
        }
        int f3 = fileno(errFile);
        dup2(f3, STDERR_FILENO);
    }

    setLimitation(execConfig);

    // 设置uid
    if (execConfig->uid > 0) {
        if (setuid(execConfig->uid) == -1) {
            CHILD_EXIT(RUNTIME_ERROR);
        }
    }

    if (execConfig->guard) {
        setSeccompGuard();
    }

    char *envp[] = {"PATH=/bin", 0};
    // 执行用户的提交
    execve(execConfig->execPath, NULL, envp);
    CHILD_EXIT(EXIT_SUCCESS);
}