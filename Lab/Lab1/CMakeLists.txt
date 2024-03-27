# 设置CMake的最小版本
cmake_minimum_required(VERSION 3.22)

# 设置你的项目名
project(Lab1 C)

# 设置C标准
set(CMAKE_C_STANDARD 11)

# 添加你的头文件目录
include_directories(include)

# 添加你的源文件目录
file(GLOB SOURCES "src/*.c")

# 添加你的执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} fl)