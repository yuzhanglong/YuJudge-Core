# 基于C + Linux的判题核心



## 总述

这是本人的OnlineJudge项目的一部分，我称之为**判题核心**，基于**c语言**编写，支持在**Linux系统**下运行。


## 项目结构

```
y-judger                                               
├─ .gitignore                                          
├─ CMakeLists.txt                                      // cmake相关
├─ README.md                                           // 自述文件
├─ src                                                 // 代码
│  ├─ child                                            // 孩子进程相关
│  │  ├─ child.c                                       
│  │  └─ child.h                                       
│  ├─ common                                           // 一般性/公用相关
│  │  ├─ common.c                                      
│  │  └─ common.h                                      
│  ├─ guard                                            // 安全性相关
│  │  ├─ guard.c                                       
│  │  └─ guard.h                                       
│  ├─ judger                                           // 程序总线
│  │  ├─ judger.c                                      
│  │  └─ judger.h                                      
│  ├─ logger                                           // 日志记录
│  │  ├─ logger.c                                      
│  │  └─ logger.h                                      
│  ├─ main.c                                           // 程序入口
│  ├─ sysHelper                                        // 系统调用相关
│  │  ├─ sysHelper.c                                   
│  │  └─ sysHelper.h                                   
│  ├─ timeHelper                                       // 时间处理相关
│  │  ├─ timeHelper.c                                  
│  │  └─ timeHelper.h                                  
│  └─ utils                                            // 工具相关
│     └─ checker.sh                                   
└─ tests                                               // 测试文件
```



## 本程序的流程

本程序的流程(**一次执行**)如下图所示，这也是当今绝大部分oj的执行的基础方案(可能部分细节处理方式不同):

![image-20200621010643020](https://github.com/yuzhanglong/YuJudge-Core/blob/master/example/2.png)

本程序执行用户的代码，并向标准输出定位到用户提供的文件路径中，并返回结果。

【存疑】值得注意的是，我没有将**stdout**文件和出题者提供的xxx.out文件来进行比对，我觉得这一步交给**调用者**（judgeHost）比较好。当然，实现这一功能也非常简单，只需执行以下脚本，你只需传入需要比对文件的**路径**即可。

```sh
#!/bin/sh

PROGRAM="$1"
TESTOUT="$2"

diff -a "$PROGRAM" "$TESTOUT"
EXITCODE=$?

[ $EXITCODE -gt 1 ] && exit 1

exit 0
```



## 如何使用（开发环境下）

我们以**centos7**系统来举例：

### 安装基本的依赖(各种编译器/cmake)，不再赘述

```
yum -y install gcc
....
```

### 进入项目目录

```
cd 你的项目目录
```

### 编译

```
mkdir build
cd build
cmake ..
make
```

执行 `make` 命令会在 `build` 目录下编译生成可执行文件：y-judge

### 运行

```
sudo ./y-judger
```



## 数据结构

一次执行完成，输出如下图所示，调用者只需读取下面的字符串，将其jsonify即可，未来会提供更多的运行数据。

```json
{
    "realTimeCost": 1,
    "cpuTimeCost": 0,
    "memoryCost": 552,
    "condition": 1,
    "stdinPath": "../tests/hello/hello-c/hello.in",
    "stdoutPath": "../tests/hello/hello-c/hello.err",
    "stderrPath": "../tests/hello/hello-c/hello.out"
}
```

其中对condition的解释如下：

```c
enum RUNNING_CONDITION {
    RUN_SUCCESS = 1, // 程序通过
    RUNTIME_ERROR, // 运行时错误
    TIME_LIMIT_EXCEED, // 时间超限
    MEMORY_LIMIT_EXCEED, // 内存超限
    OUTPUT_LIMIT_EXCEED, // 输出超过限制
    SEGMENTATION_FAULT,  // 段错误
    FLOAT_ERROR, // 浮点错误
    UNKNOWN_ERROR, // 未知错误
    INPUT_FILE_NOT_FOUND, // 找不到输入文件
    CAN_NOT_MAKE_OUTPUT, // 无法寻找输出
    SET_LIMIT_ERROR, // 设限失败
    NOT_ROOT_USER,  // 非管理员用户
    FORK_ERROR, //fork失败
    CREATE_THREAD_ERROR, //监控线程创建失败
    VALIDATE_ERROR // 数据验证失败
};
```





## 对一些实践方案的思考



一个**onlineJudge**的基本架构应该如下图所示：

![image-20200621002852899](https://github.com/yuzhanglong/YuJudge-Core/blob/master/example/1.png)

### 封装 -- 在JudgeHost下运行本程序

本程序可以**单独执行**（仅限于开发者调试）。执行者传入一系列参数（时间限制、内存限制、可执行文件路径、stdout/in/err输出路径等）即可完成一次判题流程。

在生产环境下，**本程序**就是上图中**judgeHost**的**核心部分**，每个**judgeHost**可以调用本程序来进行判题（当然，judgeHost使用何框架、语言，这个可以自由选择，接下来我准备用Springboot来完成judgehost）

**judgeHost**向**judgeServer**暴露API，**接受用户的提交**，**编译用户的代码**，**生成相应的运行脚本（编译失败无需执行）**，然后**多次/并行执行本程序**，让本程序来执行运行脚本即可（编译失败无需执行），即可实现一次判题。



### 集群 -- 多个JudgeHost

为了提高效率，可能需要多个judgeHost，用于处理**大量用户在某个时间段提交**的情况，针对此，judgeServer可能要维护一个**队列**，根据用户提交的顺序、每个**judgehost**的“**是否忙碌**”状态来处理用户提交的代码。



### 多线程 -- 在judgeHost内并行执行本程序

线程是实现并发机制的一种有效手段。进程和线程一样，都是实现并发的一个基本单位。线程是比进程更小的执行单位，线程是进程的基础之上进行进一步的划分。所谓多线程是指一个进程在执行过程中可以产生多个更小的程序单元，这些更小的单元称为线程。

一个调用者（judgehost）可以**并行执行**多个本程序，这样大大提高了一道题的判题效率（因为一道题可能有多个测试点，每个测试点可能也要多跑几遍）



## 执行脚本的处理

为了降低**本程序**和**judgehost**的耦合度，我们不会和judgehost来约定用户提交的文件的目录结构，你只需要传入相关文件的**绝对路径**即可。另外，请不要使用相对路径。

对于c/c++，我们执行的是编译出来的二进制

对于Java、python，由于不能直接运行，我们可以利用脚本，运行这样的脚本就相当于运行代码，例如：

```sh
#!/bin/sh
java -cp /Users/yuzhanglong/Desktop/y-judger/tests/hello/hello-java hello
```

```
#!/bin/sh
python3 /Users/yuzhanglong/Desktop/y-judger/tests/hello/hello-python/hello.py
```

执行脚本的生成不是本程序所关心的内容（这是judgehost的事情）

本程序关心的是需要运行的**可执行文件的路径**。



## 参考资料

| 介绍/标题/主题                                               | 网址                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| **linux-manual-page**                                        | https://man7.org                                             |
| **How does Online Judge Work?**                              | https://www.linkedin.com/pulse/how-does-online-judge-works-ahmad-faiyaz |
| **How do I implement an online judge or autograder that both can be completed in 1–2 months and that is similar to CodeChef, TopCoder, etc. from scratch？** | https://www.quora.com/How-do-I-implement-an-online-judge-or-autograder-that-both-can-be-completed-in-1%E2%80%932-months-and-that-is-similar-to-CodeChef-TopCoder-etc-from-scratch](https://www.quora.com/How-do-I-implement-an-online-judge-or-autograder-that-both-can-be-completed-in-1–2-months-and-that-is-similar-to-CodeChef-TopCoder-etc-from-scratch |
| **CLion 实现远程调试**                                       | https://blog.csdn.net/lihao21/article/details/87425187?utm_medium=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-2.nonecase&depth_1-utm_source=distribute.pc_relevant.none-task-blog-BlogCommendFromMachineLearnPai2-2.nonecase |
| **domJudge代码**                                             | https://github.com/DOMjudge/domjudge                         |
| **青岛大学onlineJudge源代码**                                | https://github.com/QingdaoU/OnlineJudge                      |



 

 



