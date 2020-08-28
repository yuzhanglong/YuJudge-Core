#include <unistd.h>
#include <signal.h>
#include "system.h"

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