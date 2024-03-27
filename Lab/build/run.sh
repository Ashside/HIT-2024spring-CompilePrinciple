#!/bin/bash

# 删除output文件夹下的所有文件
rm -f output/*
# 编译Lab1
make all

# 遍历testcases文件夹下的所有文件
for file in testcases/*
do
  # 获取文件名，不包含路径
  filename=$(basename -- "$file")

  # 执行./Lab1 < 测试文件，并将结果重定向到output文件夹下的同名文件
  ./Lab1 < "$file" > "output/$filename.opt"
done