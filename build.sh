# CentOS 7
echo '[TIP] 正在安装项目必要依赖...'

yum -y install java-1.8.0-openjdk.x86_64 java-1.8.0-openjdk-devel python3 gcc gcc-c++ libseccomp-devel git cmake make zip unzip

# 编译可执行文件
echo '[TIP] 正在编译判题核心 (build/y_judge)'
rm -rf build && mkdir build
cd build || exit
cmake ..
make
cd ..

echo '[TIP] 编译成功，编译可执行文件位于 (build/y_judge)'
