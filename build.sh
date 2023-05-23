# 编译可执行文件
echo '[TIP] 正在编译判题核心 (build/y_judge)'
rm -rf build && mkdir build
cd build || exit
cmake ..
make
cd ..

echo '[TIP] 编译成功，编译可执行文件位于 (build/y_judge)'
