#ifndef Y_JUDGER_LOGGER_H
#define Y_JUDGER_LOGGER_H
typedef int logType;

#define FATAL       5
#define ERROR       4
#define WARNING     3
#define INFO        2
#define DEBUG       1

void makeLog(logType type, char *content, FILE *loggerFile);

#endif //Y_JUDGER_LOGGER_H
