#include "guard.h"

int ALLOW_LIST[] = {
        // 进程控制
        SCMP_SYS(fork),
        SCMP_SYS(clone),
        SCMP_SYS(vfork),
        SCMP_SYS(kill)
};


/**
 * @author yzl
 * @return int
 * 获取白名单的长度
 */
int getAllowListLength() {
    int len = sizeof(ALLOW_LIST) / sizeof(int);
    return len;
}

/**
 * @author yzl
 * @param ctx scmp_filter_ctx
 * @return int
 * 添加限制规则
 */
int addSeccompRules(scmp_filter_ctx ctx) {
    int len = getAllowListLength();
    for (int i = 0; i < len; i++) {
        if (seccomp_rule_add(ctx, SCMP_ACT_KILL, ALLOW_LIST[i], 0) != 0) {
            return 0;
        }
    }
    return 1;
}

/**
 * @author yzl
 * @return int
 * 执行规则限制
 */
void setSeccompGuard() {
    scmp_filter_ctx ctx;
    ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (!ctx) {
        //TODO：限制失效
    }
    addSeccompRules(ctx);
    seccomp_load(ctx);
}