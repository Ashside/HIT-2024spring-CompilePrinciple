# 设置CMake的最小版本
cmake_minimum_required(VERSION 3.22)

# 设置你的项目名
project(Lab1 C)

# 设置C标准
set(CMAKE_C_STANDARD 11)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# 添加你的头文件目录
include_directories(include)

# 添加你的源文件目录
file(GLOB SOURCES "src/*.c")

# 添加自定义命令来执行Makefile
add_custom_command(
  OUTPUT lex.yy.c Parse.tab.c Parse.tab.h
  COMMAND make -C ${CMAKE_SOURCE_DIR}/src
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src
)

# 创建一个始终被构建的目标
add_custom_target(
  RunMakefile ALL
  DEPENDS lex.yy.c Parse.tab.c Parse.tab.h
)
# 添加你的执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} fl)