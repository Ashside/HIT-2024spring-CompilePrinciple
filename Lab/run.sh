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
# 结果输出到outputres目录下
for file in testcases/*
do
#如果第一个参数是-all，则输出所有的结果
	if [ "$1" = "-all" ]; then
		echo "Running $file"
		./code/parser $file > outputres/$(basename $file .cmm).ir
		continue
	#否则打印所有结果，不写入文件
	else
		echo "Running $file"
		./code/parser $file
	fi
	

done

# 清理现场
cd code
echo "Cleaning up"
make clean
cd ..


echo "Done"