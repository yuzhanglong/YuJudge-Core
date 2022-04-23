/*
 * C 语言的 hello world 项目
 * 程序接受用户输入一个整数，
 * 程序只有一行输出，为 'hello world' 字符串
 */

#include<stdio.h>

int main(void) {
    int a;
    scanf("%d", &a);
    printf("hello world!\n");
    return 0;
}