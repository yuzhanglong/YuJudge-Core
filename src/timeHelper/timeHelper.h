//
// Created by yuzhanglong on 2020/6/19.
//
#include <sys/time.h>


#ifndef Y_JUDGER_TIMEHELPER_H
#define Y_JUDGER_TIMEHELPER_H

long getTimeMillisecondByTimeval(struct timeval timeval);

int getGapMillsecond(struct timeval startTime, struct timeval endTime);

#endif //Y_JUDGER_TIMEHELPER_H
