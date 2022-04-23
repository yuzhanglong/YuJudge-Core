#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/wait.h>
#include "../child/child.h"
#include "../time/time.h"
#include "../system/system.h"
#include "../logger/logger.h"
#include "judge.h"

/**
 * 创建一个监控线程
 *
 * @author yzl
 * @param timeoutKillerConfig 超时状态配置
 * @return void *
 */

void *monitorThread(void *timeoutKillerConfig) {
    struct timeoutKillerConfig *timeConf = (struct timeoutKillerConfig *) (timeoutKillerConfig);
    // 单独的线程 用来在超时的时候杀死进程 防止超时
    pid_t pid = timeConf->pid;
    int limitTime = timeConf->limitTime;
    sleep((unsigned int) (limitTime));
    killPid(pid, SIGKILL);
    return NULL;
}

/**
 * 获取子进程运行状态，应该在wait之后调用
 *
 * @author yzl
 * @param status 子进程状态，一般通过wait4得到
 * @param judgeResult 运行结果
 * @param execConfig 用户提供的运行的配置
 * @return enum RUNNING_CONDITION *
 */

enum RUNNING_CONDITION getRunningCondition(int status, struct execConfig *execConfig, struct judgeResult *judgeResult) {
    // 正常终止 主动抛出的exit都会到这里来
    if (WIFEXITED(status)) {
        // 对于和期望输出比对的业务逻辑
        // 我们交给调动者（判题服务器）来实现
        if (WEXITSTATUS(status) == 0) {
            // 细粒度判断内存限制，通过杀进程的方式并不准确，详见child.c文件
            int isMemoryExceeded = (unsigned long long) (judgeResult->memoryCost) > execConfig->memoryLimit;
            if (isMemoryExceeded) {
                return MEMORY_LIMIT_EXCEED;
            }

            // 细粒度的时间限制(ms)
            int isCpuTimeExceeded = execConfig->cpuTimeLimit < judgeResult->cpuTimeCost;
            int isRealTimeExceeded = execConfig->realTimeLimit < judgeResult->realTimeCost;
            if (isCpuTimeExceeded || isRealTimeExceeded) {
                return TIME_LIMIT_EXCEED;
            }
            return RUN_SUCCESS;
        }
        return UNKNOWN_ERROR;
    }

    // 异常终止
    if (WIFSIGNALED(status)) {
        if (WTERMSIG(status) == SIGXCPU) {
            return TIME_LIMIT_EXCEED;
        }
        if (WTERMSIG(status) == SIGFPE) {
            return FLOAT_ERROR;
        }
        if (WTERMSIG(status) == SIGSEGV) {
            return SEGMENTATION_FAULT;
        }
        if (WTERMSIG(status) == SIGKILL) {
            // 经测试 cpu的时间超限也会出现在此处
            if (execConfig->cpuTimeLimit < judgeResult->cpuTimeCost) {
                return TIME_LIMIT_EXCEED;
            }
            return RUNTIME_ERROR;
        }
        if (WTERMSIG(status) == SIGXFSZ) {
            return OUTPUT_LIMIT_EXCEED;
        }
        return RUNTIME_ERROR;
    }
    return UNKNOWN_ERROR;
}


/**
 * 运行程序, 写入运行结果
 * @author yzl
 * @param  judgeResult 最终运行结果
 * @param  execConfig 用户提供的运行的配置
 * @return void
 */

void runJudge(struct execConfig *execConfig, struct judgeResult *judgeResult) {
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);
    if (!isRoot()) {
        makeLog(WARNING, "非root用户", execConfig->loggerFile);
        judgeResult->condition = NOT_ROOT_USER;
        return;
    }

    pid_t childPid = fork();
    pthread_t pthread = 0;

    if (childPid < 0) {
        //如果出现错误，fork返回一个负值
        makeLog(ERROR, "fork error!", execConfig->loggerFile);
        judgeResult->condition = FORK_ERROR;
        return;
    }

    if (childPid == 0) {
        runChild(execConfig);
    }

    if (childPid > 0) {
        // 父亲进程
        makeLog(DEBUG, "父进程已创建", execConfig->loggerFile);
        struct timeoutKillerConfig killerConfig;
        killerConfig.limitTime = execConfig->realTimeLimit / 1000;
        killerConfig.pid = childPid;

        // 若线程创建成功，则返回0。若线程创建失败，则返回出错编号
        int t = pthread_create(&pthread, NULL, monitorThread, (void *) (&killerConfig));
        if (t != 0) {
            judgeResult->condition = CREATE_THREAD_ERROR;
            return;
        }
        int status = 0;
        struct rusage costResource;

        // 等待孩子进程执行 孩子被杀死或者正常执行都会走到这里
        wait4(childPid, &status, WSTOPPED, &costResource);

        // 销毁监控进程
        pthread_cancel(pthread);
        makeLog(DEBUG, "监控线程被销毁", execConfig->loggerFile);
        gettimeofday(&endTime, NULL);

        // cpu时间 注意和真实时间区分
        judgeResult->cpuTimeCost = (int) getTimeMillisecondByTimeval(costResource.ru_utime);
        judgeResult->realTimeCost = getGapMillsecond(startTime, endTime);
        //WARNING：该值在mac和linux系统上是有区别的！
        judgeResult->memoryCost = costResource.ru_maxrss;
        judgeResult->condition = getRunningCondition(status, execConfig, judgeResult);
    }
}