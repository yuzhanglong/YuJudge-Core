#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/wait.h>
#include "../child/child.h"
#include "../timeHelper/timeHelper.h"
#include "../sysHelper/sysHelper.h"
#include "../logger/logger.h"
#include "judger.h"

/**
 * @author yzl
 * @return void *
 * 创建一个监控线程
 */

void *monitorThread(void *timeoutkillerConfig) {
    struct timeoutkillerConfig *timeConf = (struct timeoutkillerConfig *) (timeoutkillerConfig);
    // 单独的线程 用来在超时的时候杀死进程 防止超时
    pid_t pid = timeConf->pid;
    int limitTime = timeConf->limitTime;
    makeLog(DEBUG, "监控线程已开启");
    sleep((unsigned int) (limitTime));
    killPid(pid, SIGKILL);
    makeLog(DEBUG, "运行超时");
    return NULL;
}

/**
 * @author yzl
 * @param status 子进程状态，一般通过wait4得到
 * @param costResource 进程资源消耗结构体，一般通过wait4得到
 * @param execConfig 用户提供的运行的配置
 * @return enum RUNNING_CONDITION *
 * 获取子进程运行状态，应该在wait之后调用
 */

enum RUNNING_CONDITION getRunningConditon(int status, struct rusage costResource, struct execConfig *execConfig) {
    // 正常终止 主动抛出的exit都会到这里来
    if (WIFEXITED(status)) {
        // 注意：此处的accept并不是真正的accept
        return WEXITSTATUS(status) == 0 ? RUN_SUCCESS : WEXITSTATUS(status);
    }
    // 异常终止
    if (WIFSIGNALED(status)) {
        if (WTERMSIG(status) == SIGXCPU) {
            return TIME_LIMIT_EXCEEDED;
        }
        if (WTERMSIG(status) == SIGFPE) {
            return FLOAT_ERROR;
        }
        if (WTERMSIG(status) == SIGSEGV) {
            //内存超限也需要在这里处理
            if (execConfig->memoryLimit < costResource.ru_maxrss) {
                return MEMORY_LIMIT_EXCEED;
            }
            return SEGMENTATION_FAULT;
        }
        if (WTERMSIG(status) == SIGKILL) {
            return RUNTIME_ERROR;
        }
        if (WTERMSIG(status) == SIGXFSZ) {
            return OUTPUT_LIMIT_EXCEED;
        }
    }
    return UNKNOWN_ERROR;
}


/**
 * @author yzl
 * @param judgeResult 最终运行结果
 * @param execConfig 用户提供的运行的配置
 * @return void
 * 运行程序， 写入运行结果
 */

void runJudger(struct execConfig *execConfig, struct judgeResult *judgeResult) {
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    if (!isRoot()) {
        makeLog(WARNING, "非root用户");
        judgeResult->condtion = UNROOT_USER;
        return;
    }
    pid_t childPid = fork();
    pthread_t pthread = 0;

    if (childPid < 0) {
        //如果出现错误，fork返回一个负值
        makeLog(ERROR, "fork error!");
        judgeResult->condtion = FORK_ERROR;
        return;
    }
    if (childPid == 0) {
        runChild(execConfig);
    }

    if (childPid > 0) {
        // 父亲进程
        makeLog(DEBUG, "父进程已创建");
        struct timeoutkillerConfig killerConfig;
        killerConfig.limitTime = execConfig->wallTime;
        killerConfig.pid = childPid;

        // 若线程创建成功，则返回0。若线程创建失败，则返回出错编号
        int t = pthread_create(&pthread, NULL, monitorThread, (void *) (&killerConfig));
        if (t != 0) {
            judgeResult->condtion = CREATE_THREAD_ERROR;
            return;
        }
        int status = 0;
        struct rusage costResource;

        // 等待孩子进程执行 孩子被杀死或者正常执行都会走到这里
        wait4(childPid, &status, WSTOPPED, &costResource);
        // 销毁监控进程
        pthread_cancel(pthread);
        makeLog(DEBUG, "监控线程被销毁");

        gettimeofday(&endTime, NULL);
        int timeCostInMillisecond = getGapMillsecond(startTime, endTime);

        // cpu时间/ 注意和真实时间区分
        int cpuTime = (int) (costResource.ru_utime.tv_sec * 1000 + costResource.ru_utime.tv_usec / 1000);
        judgeResult->cpuTimeCost = cpuTime;
        judgeResult->realTimeCost = timeCostInMillisecond;
        judgeResult->condtion = getRunningConditon(status, costResource, execConfig);
        //WARNING：ru_maxrss的值在mac和linux有区别！
        judgeResult->memoryCost = costResource.ru_maxrss;
    }
}