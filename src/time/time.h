#include <sys/time.h>


#ifndef Y_JUDGE_TIME_H
#define Y_JUDGE_TIME_H

long getTimeMillisecondByTimeval(struct timeval timeval);

int getGapMillsecond(struct timeval startTime, struct timeval endTime);

#endif //Y_JUDGE_TIME_H
