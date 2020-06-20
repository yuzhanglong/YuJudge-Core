//
// Created by yuzhanglong on 2020/6/19.
//

#include <stdio.h>
#include "common.h"

void showUsage() {
    printf("[程序限制相关]\n");
    printf("\
  -t, --walltime=TIME    限制实际时间为t秒，请注意和cpu时间区分\n\
  -c, --cputime=TIME     限制cpu时间为t秒\n\
  -m, --memsize=SIZE     限制运行内存为mKB\n\
  -f, --filesize=SIZE     限制代码最大输出为f B\n");

    printf("[输入输出相关]\n");
    printf("\
  -o, --stdout=TIME    标准输出文件\n\
  -e, --stderr=TIME     标准错误文件\n\
  -i, --stdin=SIZE     标准输入文件\n");
}