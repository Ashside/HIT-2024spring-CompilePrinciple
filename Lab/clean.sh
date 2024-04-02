#!/bin/bash

cd code
make clean

cd ..

# 清理outputres目录
rm -rf outputres
# 创建outputres目录
mkdir outputres