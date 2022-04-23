#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "common.h"

#define VALIDATE_CONFIG_ERROR 0
#define VALIDATE_SUCCESS 1

/**
 * @author yzl
 * 在命令行展示程序的用法
 */
void showUsage() {
    printf("\n[限制相关]\n");
    printf("\
  -t,     [可选] 限制实际时间，单位 ms，请注意和 CPU 时间区分，默认值为 4000\n\
  -c,     [可选] 限制 CPU 时间，单位 ms，默认值为 4000\n\
  -m,     [可选] 限制运行内存，单位 KB，默认值为 1024 * 1024\n\
  -u,     [可选] 执行目标可执行文件对应的用户 ID，默认值为 3000\n\
  -g,     [可选] 是否开启 seccomp 限制沙箱，默认为 0 (不开启)，设置为 1 则开启\n\
  -f,     [可选] 限制代码最大输出，单位 B，默认值为 20000\n");

    printf("[输入/输出相关]\n");

    printf("\
  -r,     [可选] 目标可执行文件\n\
  -l,     [可选] 日志文件\n\
  -o,     [可选] 标准输出文件\n\
  -e,     [可选] 标准错误文件\n\
  -i,     [可选] 标准输入文件\n");

    printf("[其他]\n");
    printf("\
  -h,     查看帮助\n\n");
}

/**
 * @author yzl
 * @param execConfig 运行配置
 * @return void
 * 初始化用户配置
 */

void initExecConfigAndJudgeResult(struct execConfig *execConfig, struct judgeResult *judgeResult) {
    execConfig->memoryLimit = MEMORY_LIMIT_DEFAULT;
    execConfig->cpuTimeLimit = TIME_LIMIT_DEFAULT;
    execConfig->realTimeLimit = WALL_TIME_DEFAULT;
    execConfig->processLimit = PROCESS_LIMIT_DEFAULT;
    execConfig->outputLimit = OUTPUT_LIMIT_DEFAULT;
    execConfig->wallMemoryLimit = WALL_MEMORY_DEFAULT;
    execConfig->uid = UID_DEFAULT;
    execConfig->guard = GUARD_DEFAULT;
    execConfig->execPath = "\0";
    execConfig->stderrPath = "\0";
    execConfig->stdoutPath = "\0";
    execConfig->stdinPath = "\0";
    execConfig->loggerPath = "\0";
    execConfig->execPath = "\0";
    execConfig->loggerFile = NULL;
    judgeResult->condition = 1;
    judgeResult->memoryCost = 0;
    judgeResult->realTimeCost = 0;
    judgeResult->cpuTimeCost = 0;
}

/**
 * @author yzl
 * @param execConfig 用户提供的运行的配置
 * @return void
 * 验证用户配置的合法性
 */

int validateForExecConfig(struct execConfig *execConfig) {
    if (execConfig->cpuTimeLimit < 0
        || execConfig->memoryLimit < 1024
        || execConfig->realTimeLimit < 0
        || execConfig->processLimit < 0
        || execConfig->outputLimit < 0
        || execConfig->execPath[0] == '\0') {
        return VALIDATE_CONFIG_ERROR;
    }
    return VALIDATE_SUCCESS;
}

/**
 * 获取用户配置，并设置用户配置
 *
 * @author yzl
 * @param argc 用户传入参数的个数
 * @param argv 用户传入的参数
 * @param execConfig 运行配置
 * @return int 是否设置成功，如果成功，程序将继续执行
 */

int getAndSetOptions(int argc, char *argv[], struct execConfig *execConfig) {
    int opt;
    if (argc == 1) {
        showUsage();
        return 0;
    }
    while ((opt = getopt(argc, argv, "t:c:m:f:o:e:i:r:l:h:u:g:p:")) != -1) {
        switch (opt) {
            case 't':
                execConfig->realTimeLimit = atoi(optarg);
                break;
            case 'c':
                execConfig->cpuTimeLimit = atoi(optarg);
                break;
            case 'm':
                execConfig->memoryLimit = atoi(optarg);
                break;
            case 'f':
                execConfig->outputLimit = atoi(optarg);
                break;
            case 'o':
                execConfig->stdoutPath = optarg;
                break;
            case 'e':
                execConfig->stderrPath = optarg;
                break;
            case 'i':
                execConfig->stdinPath = optarg;
                break;
            case 'r':
                execConfig->execPath = optarg;
                break;
            case 'l':
                execConfig->loggerPath = optarg;
                execConfig->loggerFile = fopen(execConfig->loggerPath, "w");
                break;
            case 'u':
                execConfig->uid = atoi(optarg);
                break;
            case 'g':
                execConfig->guard = atoi(optarg);
                break;
            case 'p':
                execConfig->processLimit = atoi(optarg);
                break;
            case 'h':
                showUsage();
                return 0;
            default:
                printf("Unknown option: %c\n", (char) optopt);
                return 0;
        }
    }
    return 1;
}

/**
 * 运行结束，输出结果
 *
 * @author yzl
 * @param execConfig 运行参数
 * @param judgeResult 运行结果
 */

void generateResult(struct execConfig *execConfig, struct judgeResult *judgeResult) {
    // 此处的stdout将被调用者处理 应该以json字符串形式表示
    printf("{\n"
           "    \"realTimeCost\": %llu,\n"
           "    \"cpuTimeCost\": %llu,\n"
           "    \"memoryCost\": %llu,\n"
           "    \"condition\": %d,\n"
           "    \"stdinPath\": \"%s\",\n"
           "    \"stdoutPath\": \"%s\",\n"
           "    \"stderrPath\": \"%s\",\n"
           "    \"loggerPath\": \"%s\"\n"
           "}\n",
           judgeResult->realTimeCost,
           judgeResult->cpuTimeCost,
           judgeResult->memoryCost,
           judgeResult->condition,
           execConfig->stdinPath,
           execConfig->stdoutPath,
           execConfig->stderrPath,
           execConfig->loggerPath
    );
}