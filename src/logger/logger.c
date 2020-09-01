#include <time.h>
#include <stdio.h>
#include "logger.h"

/**
 * @author yzl
 * 输出本日志的创建时间
 */
void outputCurrentTime(FILE *loggerFile) {

    time_t rawTime = time(NULL);
    struct tm *currentTime;
    currentTime = localtime(&rawTime);
    fprintf(loggerFile, "[%d-%d-%d %d:%d:%d]",
            currentTime->tm_year + 1900,
            currentTime->tm_mon + 1,
            currentTime->tm_mday,
            currentTime->tm_hour,
            currentTime->tm_min,
            currentTime->tm_sec
    );
}

void outputLoggerType(logType type, FILE *loggerFile) {
    fprintf(loggerFile, "[");
    if (type == DEBUG) {
        fprintf(loggerFile, "DEBUG");
    } else if (type == INFO) {
        fprintf(loggerFile, "INFO");
    } else if (type == WARNING) {
        fprintf(loggerFile, "WARNING");
    } else if (type == ERROR) {
        fprintf(loggerFile, "ERROR");
    } else if (type == FATAL) {
        fprintf(loggerFile, "FATAL");
    }
    fprintf(loggerFile, "]");
}


void makeLog(logType type, char *content, FILE *loggerFile) {
    if (loggerFile == NULL) return;
    /*输出类型*/
    outputLoggerType(type, loggerFile);
    /*获取生成日志的时间*/
    outputCurrentTime(loggerFile);
    printf("%s\n", content);
}

/**
 * @author yzl
 * 初始化日志记录模块
 */
int initLogger() {
    //TODO:处理日志文件名、路径 格式等功能
    return 1;
}