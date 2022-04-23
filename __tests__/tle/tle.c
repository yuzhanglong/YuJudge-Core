/*
 *  期望的结果: 时间超限(TIME_LIMIT_EXCEED)
 *  本程序会一直执行while循环
 *  在达到设置的时间限制之后，它会被杀死
 * */

#include <stdio.h>

int main(void) {
    while (1);
    printf("you can not reach here.....");
    return 0;
}