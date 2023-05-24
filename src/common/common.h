#ifndef Y_JUDGER_COMMON_H
#define Y_JUDGER_COMMON_H

#include <stdio.h>
#include <unistd.h>
#include <sys/resource.h>

enum RUNNING_CONDITION {
    RUN_SUCCESS = 1, // 程序通过
    RUNTIME_ERROR, // 运行时错误
    TIME_LIMIT_EXCEED, // 时间超限
    MEMORY_LIMIT_EXCEED, // 内存超限
    OUTPUT_LIMIT_EXCEED, // 输出超过限制
    SEGMENTATION_FAULT,  // 段错误
    FLOAT_ERROR, // 浮点错误
    UNKNOWN_ERROR, // 未知错误
    INPUT_FILE_NOT_FOUND, // 找不到输入文件
    CAN_NOT_MAKE_OUTPUT, // 无法寻找输出
    SET_LIMIT_ERROR, // 设限失败
    NOT_ROOT_USER,  // 非管理员用户
    FORK_ERROR, //fork失败
    CREATE_THREAD_ERROR, //监控线程创建失败
    VALIDATE_ERROR // 数据验证失败
};

// 对于内存限制的一些实践和解释请参考child.c
enum EXEC_SETTING_DEFAULT {
    TIME_LIMIT_DEFAULT = 4000,  //cpu实践限制，默认为 4000 ms
    MEMORY_LIMIT_DEFAULT = 1024 * 64, // 限制默认内存为64 mb
    WALL_MEMORY_DEFAULT = 1024 * 1024 * 3L, // 内存硬限制，请参考child.c
    WALL_TIME_DEFAULT = 4000,  // 实际时间限制，默认为 4000 ms
    PROCESS_LIMIT_DEFAULT = 100, // 进程限制
    OUTPUT_LIMIT_DEFAULT = 20000,  // 输出限制，默认为 20000
    UID_DEFAULT = 3000,
    GUARD_DEFAULT = 0
};

struct timeoutKillerConfig {
    int pid;
    int limitTime;
};

struct execConfig {
    rlim_t cpuTimeLimit;
    rlim_t memoryLimit;
    rlim_t wallMemoryLimit;
    rlim_t processLimit;
    rlim_t outputLimit;
    rlim_t realTimeLimit;
    char *execPath;
    char *stdinPath;
    char *stdoutPath;
    char *stderrPath;
    char *loggerPath;
    FILE *loggerFile;
    uid_t uid;
    int guard;
};

struct judgeResult {
    // 实际消耗时间
    rlim_t realTimeCost;
    // 消耗内存
    rlim_t memoryCost;
    // 消耗 CPU 时间
    rlim_t cpuTimeCost;
    // 执行状态，请参考 RUNNING_CONDITION 枚举类型
    int condition;
};

void initExecConfigAndJudgeResult(struct execConfig *execConfig, struct judgeResult *judgeResult);

int validateForExecConfig(struct execConfig *execConfig);

void showUsage();

int getAndSetOptions(int argc, char *argv[], struct execConfig *execConfig);

void generateResult(struct execConfig *execConfig, struct judgeResult *judgeResult);

#endif //Y_JUDGER_COMMON_H
