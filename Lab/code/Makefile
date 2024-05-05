# 指定编译器
CC = gcc

# 指定编译标志
CFLAGS = -Wall -g 

# Bison生成的文件
BISON_FILE = Parse.tab.c
BISON_OUTPUT = Parse.output
BISON_HEADER = Parse.tab.h
BISON_SRC = Parse.y

# Flex生成的文件
FLEX_FILE = lex.yy.c
FLEX_SRC = Lexer.l

# 语义分析文件
SEMANTIC_FILE = Semantic.c
SEMANTIC_HEADER = Semantic.h
SEMANTIC_ENUM = SemanTypeEnum.h
# 中间代码生成文件
INTER_CODE_FILE = InterCode.c
INTER_CODE_HEADER = InterCode.h
INTER_CODE_ENUM = InterCodeEnum.h
# 目标文件
TARGET = parser

# TODO: 源文件，排除语义分析文件
SRCS = $(wildcard *.c)
HDRS = $(wildcard *.h)
#SRCS := $(filter-out $(SEMANTIC_FILE), $(SRCS))
#HDRS := $(filter-out $(SEMANTIC_HEADER), $(HDRS))



# 编译规则
# TODO不能加入lex.yy.c，因为flex会生成一个main函数，同时造成重复定义
$(TARGET): $(SRCS) $(HDRS) $(BISON_FILE) $(BISON_HEADER) $(FLEX_FILE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(BISON_FILE) -lfl -ly

Lab1DBG: $(SRCS) $(HDRS) $(BISON_FILE) $(BISON_HEADER) $(FLEX_FILE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(BISON_FILE) -lfl -ly -DDEBUG_NEW_NODES 

Lab2DBG: $(SRCS) $(HDRS) $(BISON_FILE) $(BISON_HEADER) $(FLEX_FILE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(BISON_FILE) -lfl -ly -DDEBUG_SEMANTIC_ANALYSIS

Lab3DBG: $(SRCS) $(HDRS) $(BISON_FILE) $(BISON_HEADER) $(FLEX_FILE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(BISON_FILE) -lfl -ly -DDEBUG_INTER_CODE_GENERATION
# Bison规则
$(BISON_FILE) $(BISON_HEADER) $(BISON_OUTPUT): $(BISON_SRC)
	bison -d -v -t $(BISON_SRC)

# Flex规则
$(FLEX_FILE): $(FLEX_SRC)
	flex $(FLEX_SRC)


.PHONY: clean test
# 清理规则
clean:
	rm -f $(BISON_FILE) $(BISON_HEADER) $(BISON_OUTPUT) $(FLEX_FILE) $(TARGET)
