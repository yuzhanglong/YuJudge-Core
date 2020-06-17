/**
 *  @author:yuzhanglong
 *  @email:yuzl1123@163.com
 *  @version: demo-test
 *
 *  项目日志记录模块
 */
#include <sys/time.h>
#include <stdio.h>
#include "logger.h"


/**
 * @author yzl
 * 输出本日志的创建时间
 */
void outputCurrentTime() {
    time_t rawTime;
    struct tm *currentTime = localtime(&rawTime);
    printf("[%s]", asctime(currentTime));
}

void outputLoggerType(logType type) {
    printf("[");
    if (type == DEBUG) {
        printf("DEBUG");
    } else if (type == INFO) {
        printf("INFO");
    } else if (type == WARNING) {
        printf("WARNING");
    } else if (type == ERROR) {
        printf("ERROR");
    } else if (type == FATAL) {
        printf("FATAL");
    }
    printf("]");
}


void makeLog(logType type, char *content) {
    /*输出类型*/
    outputLoggerType(type);
    /*获取生成日志的时间*/
    outputCurrentTime();
    printf("    %s", content);
};

/**
 * @author yzl
 * 初始化日志记录模块
 */
int initLoggger() {
    //TODO:处理日志文件名、路径 格式等功能
    return 1;
}