#include <stdio.h>
#include<getopt.h>
#include "common.h"

#define VALIDATE_CONFIG_ERROR 0
#define VALIDATE_SUCCESS 1

void showUsage() {
    printf("[限制相关]\n");
    printf("\
  -t, --walltime=TIME    限制实际时间为t秒，请注意和cpu时间区分\n\
  -c, --cputime=TIME     限制cpu时间为t秒\n\
  -m, --memsize=SIZE     限制运行内存为mKB\n\
  -f, --filesize=SIZE    限制代码最大输出为f B\n");

    printf("[输入/输出相关]\n");
    printf("\
  -o, --stdout=TIME      标准输出文件\n\
  -e, --stderr=TIME      标准错误文件\n\
  -i, --stdin=SIZE       标准输入文件\n");
    printf("[其他信息]\n");
    printf("\
  -v, --stdout=TIME      标准输出文件\n\
  -e, --stderr=TIME      标准错误文件\n\
  -i, --stdin=SIZE       标准输入文件\n");
}

/**
 * @author yzl
 * @param execConfig 运行配置
 * @return void
 * 初始化用户配置
 */

void initExecConfig(struct execConfig *execConfig) {
    execConfig->memoryLimit = MEMORY_LIMIT_DEFAULT;
    execConfig->timeLimit = TIME_LIMIT_DEFAULT;
    execConfig->wallTime = WALL_TIME_DEFAULT;
    execConfig->processLimit = PROCESS_LIMIT_DEFAULT;
    execConfig->outputLimit = OUTPUT_LIMIT_DEFAULT;
}

/**
 * @author yzl
 * @param execConfig 用户提供的运行的配置
 * @return void
 * 验证用户配置的合法性
 */

int validateForExecConfig(struct execConfig *execConfig) {
    if (execConfig->timeLimit < 0
        || execConfig->memoryLimit < 1024
        || execConfig->wallTime < 0
        || execConfig->processLimit < 0
        || execConfig->outputLimit < 0) {
        return VALIDATE_CONFIG_ERROR;
    }
    return VALIDATE_SUCCESS;
}

void getAndSetOptions() {

}