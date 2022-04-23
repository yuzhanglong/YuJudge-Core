# CentOS 7
echo '[TIP] 正在安装项目必要依赖...'

yum -y install java-1.8.0-openjdk.x86_64 java-1.8.0-openjdk-devel python3 gcc gcc-c++ libseccomp-devel git cmake make zip unzip

# 编译可执行文件
echo '[TIP] 正在编译判题核心 (build/y_judge)'
cmake -B build
make -C build


gcc -Wall -O2 -std=gnu11 examples/quick-start/hello-world.c -o examples/quick-start/run-hello-world.out -lm
build/y_judge -r /home/yuzhanglong/examples/quick-start/run-hello-world.out \
              -i /home/yuzhanglong/examples/quick-start/hello-world.in \
              -o /home/yuzhanglong/test.out \
              -e /home/yuzhanglong/error.out \
              -t '10000' \
              -c '10000' \
              -m '50000' \
              -f '1200' \
              -g '0'
