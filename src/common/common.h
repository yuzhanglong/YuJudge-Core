#ifndef Y_JUDGER_COMMON_H
#define Y_JUDGER_COMMON_H


#include <sys/resource.h>

enum RUNNING_CONDITION {
    RUN_SUCCESS = 1, // 程序通过
    RUNTIME_ERROR, // 运行时错误
    TIME_LIMIT_EXCEEDED, // 时间超限
    MEMORY_LIMIT_EXCEED, // 内存超限
    OUTPUT_LIMIT_EXCEED, // 输出超过限制
    SEGMENTATION_FAULT,  // 段错误
    FLOAT_ERROR, // 浮点错误
    UNKNOWN_ERROR, // 未知错误
    INPUT_FILE_NOT_FOUND, // 找不到输入文件
    CAN_NOT_MAKE_OUTPUT, // 无法寻找输出
    SET_LIMIT_ERROR,
    UNROOT_USER,  // 非管理员用户
    FORK_ERROR, //fork失败
    CREATE_THREAD_ERROR, //监控线程创建失败
    VALIDATE_ERROR // 数据验证失败
};

enum EXEC_SETTIN_DEFAULT {
    TIME_LIMIT_DEFAULT = 4,
    MEMORY_LIMIT_DEFAULT = 1024 * 1024 * 32, // 2000kb
    WALL_TIME_DEFAULT = 6,
    PROCESS_LIMIT_DEFAULT = 1,
    OUTPUT_LIMIT_DEFAULT = 100000000,
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
    rlim_t wallTime;
    char *execPath;
    char *stdinPath;
    char *stdoutPath;
    char *stderrPath;
};

struct judgeResult {
    rlim_t realTimeCost;
    rlim_t memoryCost;
    rlim_t cpuTimeCost;
    int condtion;
};

void initExecConfig(struct execConfig *execConfig);

int validateForExecConfig(struct execConfig *execConfig);

void showUsage();

#endif //Y_JUDGER_COMMON_H
