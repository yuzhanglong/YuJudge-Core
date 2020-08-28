#!/bin/sh

# 将用户的输出和正确输出进行比较 可供外部项目使用
# 本程序不负责对比，只负责运行用户代码并输出
# 具体的对比或者specialJudge应该交给调用者处理

# example: ./compare <用户输出> <期望输出>


SUBMISSION_OUTPUT_PATH="$1"
RESOLUTION_PATH="$2"

# 判断文件是否存在
if [ ! -f $SUBMISSION_OUTPUT_PATH ]; then
  echo 2
  exit 0
elif [ ! -f $RESOLUTION_PATH ]; then
  echo 2
  exit 0
fi

# 文件对比
diff -q -b "$SUBMISSION_OUTPUT_PATH" "$RESOLUTION_PATH"
EXIT_CODE=$?
echo $EXIT_CODE