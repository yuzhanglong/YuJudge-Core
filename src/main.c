/**
 *  @author:yuzhanglong
 *  @email:yuzl1123@163.com
 *  @version: demo-test
 *
 *
 *  注意：请在linux系统下调试运行，切记切记！
 *  若你在macos系统下运行(虽然可以跑)，会出现你不期望的情况，
 *  例如：某些量的单位会不同(eg.costResource.ru_maxrss)
 *       或者某些功能无法实现（eg.内存超限检测）
 *
 *  另外，在调试的时候请使用日志记录（当然可以printf，但是在生产环境下不建议）
 *
 */
#include <stdio.h>
#include "judger/judger.h"


int main(int argc, char *argv[]) {
    struct execConfig execConfig;
    execConfig.memoryLimit = 100000000;
    execConfig.timeLimit = 5;
    execConfig.outputLimit = 100000000;
    execConfig.processLimit = 1;
    execConfig.wallTime = 10;
    execConfig.stdoutPath = "../tests/hello/hello-c/hello.out";
    execConfig.stdinPath = "../tests/hello/hello-c/hello.in";
    execConfig.execPath = "../tests/hello/hello-c/run";
    struct judgeResult judgeResult;
    runJudger(&execConfig, &judgeResult);


    // 此处的stdout将被调用者处理 应该以json字符串形式表示
    printf("realTime: %llu  cpuTime: %llu  memory: %llu  condition: %d\n",
           judgeResult.realTimeCost,
           judgeResult.cpuTimeCost,
           judgeResult.memoryCost,
           judgeResult.condtion
    );
    return 0;
}