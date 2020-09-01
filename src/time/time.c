#include "time.h"

/**
 * @author yzl
 * @param timeval: timeval 结构体
 * @return long 总毫秒数
 */

long getTimeMillisecondByTimeval(struct timeval timeval) {
    //  startTime.tv_sec : s
    //  startTime.tv_usec : us
    return timeval.tv_sec * 1000 + timeval.tv_usec / 1000;
}


/**
 * @author yzl
 * @param startTime: 开始点的timeval结构体
 * @param endTime:  结束点的timeval结构体
 * @return int 相差的毫秒数
 */

int getGapMillsecond(struct timeval startTime, struct timeval endTime) {
    long startMillisecond = getTimeMillisecondByTimeval(startTime);
    long endMillisecond = getTimeMillisecondByTimeval(endTime);
    return (int) (endMillisecond - startMillisecond);
}




