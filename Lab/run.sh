#!/bin/bash

cd code
ls
make clean
make 

cd ..

# 清理outputres目录
rm -rf outputres
# 创建outputres目录
mkdir outputres


# 遍历testcases目录下的所有文件，逐个执行
# 结果输出到outputres目录下，文件名为testcase的文件名去掉.cmm后缀后加上.res后缀
for file in testcases/*
do
	echo "Running $file"
	./code/parser $file > outputres/$(basename $file .cmm).res
done

# 清理现场
cd code
echo "Cleaning up"
make clean
cd ..


echo "Done"