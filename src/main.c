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
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "utils/logger/logger.h"


struct TimeoutkillerConfig {
    int pid;
    int limitTime;
};

struct scriptConfig {
    rlim_t timeLimit;
    rlim_t memoryLimit;
    rlim_t processLimit;
    rlim_t outputLimit;
};

/**
 * @author yzl
 * @param startTime: 开始点的timeval结构体
 * @param endTime:  结束点的timeval结构体
 * @return int 相差的毫秒数
 */

int getTimeMillisecond(struct timeval startTime, struct timeval endTime) {
    long startMillisecond = startTime.tv_sec * 1000 + startTime.tv_usec / 1000;
    long endMillisecond = endTime.tv_sec * 1000 + endTime.tv_usec / 1000;
    return (int) (startMillisecond - endMillisecond);
}

/**
 * @author yzl
 * @param pid: 杀死某个进程id
 * @param killType:  类型
 * @return int  是否杀死
 */

int killPid(pid_t pid, int killType) {
    // kill pid -9 cannot be caught or ignored
    int isKill = kill(pid, killType);
    return isKill;
}


/**
 * @author yzl
 * @return int 当前用户是否为管理员
 */

int isRoot() {
    uid_t uid = getuid();
    return uid == 0;
}

/**
 * @author yzl
 * @return void *
 * 创建一个监控线程
 */

void *monitorThread(void *timeoutkillerConfig) {
    struct TimeoutkillerConfig *timeConf = (struct TimeoutkillerConfig *) (timeoutkillerConfig);
    // 单独的线程 用来在超时的时候杀死进程

    printf("监控线程已开启\n");
    pid_t pid = timeConf->pid;
    int limitTime = timeConf->limitTime;
    printf("监控限制时间%d\n", limitTime);

    sleep((unsigned int) (limitTime));

    printf("sleep结束\n");
    killPid(pid, SIGKILL);
    printf("进程被杀死了");
    return NULL;
}


void setLimitation() {
    // 内存超限
    struct rlimit maxMemory;
    maxMemory.rlim_cur = maxMemory.rlim_max = 10000;

    if (setrlimit(RLIMIT_AS, &maxMemory) == 0) {
        printf("限制内存成功\n");
        // TODO:内存超限会成为段错误，注意区分
    }

    // 时间超限
    struct rlimit maxTime;
    maxTime.rlim_cur = maxTime.rlim_max = 10;                    //test : 5
    if (setrlimit(RLIMIT_CPU, &maxTime) == 0) {
        printf("限制时间成功\n");
    }

    // 进程超限
    struct rlimit maxProcessAmount;
    maxProcessAmount.rlim_cur = maxProcessAmount.rlim_max = 1;  //test : 1
    if (setrlimit(RLIMIT_NPROC, &maxProcessAmount) == 0) {
        printf("限制进程成功\n");
    }


    // 输出超限
    struct rlimit maxOutput;
    maxOutput.rlim_cur = maxOutput.rlim_max = 100000000000;
    if (setrlimit(RLIMIT_FSIZE, &maxOutput) == 0) {
        printf("限制输出成功\n");
    }
}


int runSubmission() {
    struct timeval startTime, endTime;

    if (!isRoot()) {
        makeLog(WARNING, "非root用户");
    }

    pid_t childPid = fork();
    pthread_t pthread = 0;

    gettimeofday(&startTime, NULL);

    //如果出现错误，fork返回一个负值
    if (childPid < 0) {
        //TODO:报错
    }

    if (childPid == 0) {
        //TODO:初始化孩子进程 用户提交的代码脚本将在这里执行

        setLimitation();

        FILE *inputFile = fopen("test/2.in", "r");
        FILE *outputFile = fopen("test/demo.out", "w");

        int f = fileno(inputFile);
        int f2 = fileno(outputFile);

        // 获取文件描述符
        dup2(f, STDIN_FILENO);
        dup2(f2, STDOUT_FILENO);
        execve("test/a.out", NULL, NULL);
        exit(EXIT_SUCCESS);
    }

    if (childPid > 0) {
        // 父亲进程
        printf("我是父进程PID=%d\n", childPid);
        struct TimeoutkillerConfig timeoutkillerConfig;
        timeoutkillerConfig.limitTime = 10;
        timeoutkillerConfig.pid = childPid;

        // 若线程创建成功，则返回0。若线程创建失败，则返回出错编号
        int t = pthread_create(&pthread, NULL, monitorThread, (void *) (&timeoutkillerConfig));


        if (t != 0) {
            //TODO:线程没有创建
        }


        int status = 0;
        struct rusage costResource;

        // 等待孩子进程执行 孩子被杀死或者执行都会走到这里
        wait4(childPid, &status, WSTOPPED, &costResource);
        // 非正常返回
        if (WIFSIGNALED(status)) {
            printf("非正常返回\n");

            if (WTERMSIG(status) == SIGXCPU) {
                printf("时间超限\n");
            }
            if (WTERMSIG(status) == SIGFPE) {
                printf("浮点错误\n");
            }
            if (WTERMSIG(status) == SIGSEGV) {
                //TODO: 内存超限也需要在这里处理
                printf("段错误\n");
            }
            if (WTERMSIG(status) == SIGKILL) {
                printf("运行时错误\n");
            }
            if (WTERMSIG(status) == SIGXFSZ) {
                printf("输出超限\n");
            }

            //TODO:特殊情况处理
            printf("%d", WTERMSIG(status));
        }

        // 正常返回
        if (WIFEXITED(status)) {
            printf("正常返回\n");
        }

        // cpu时间/ 注意和真实时间区分
        int cpuTime = (int) (costResource.ru_utime.tv_sec * 1000 + costResource.ru_utime.tv_usec / 1000);

        // 销毁监控进程
        pthread_cancel(pthread);
        printf("监控进程已被销毁\n");

        gettimeofday(&endTime, NULL);

        int timeCostInMillisecond = getTimeMillisecond(startTime, endTime);

        //WARNING：ru_maxrss的值在mac和linux有区别！
        printf("花费内存%ld\n", costResource.ru_maxrss);
        printf("花费CPU时间%d\n", cpuTime);
        printf("花费实际时间%d\n", timeCostInMillisecond);

        //TODO:处理status 捕捉退出信号 并根据信号 来判断退出的情况
    }
    return 0;
}

int main(int argc, char *argv[]) {
    runSubmission();
    return 0;
}