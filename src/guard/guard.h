#ifndef Y_JUDGER_GUARD_H
#define Y_JUDGER_GUARD_H

// 请参阅Linux系统调用列表
// https://www.ibm.com/developerworks/cn/linux/kernel/syscall/part1/appendix.html
#include <seccomp.h>


void setSeccompGuard();

#endif //Y_JUDGER_GUARD_H
