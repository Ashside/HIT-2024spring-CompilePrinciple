#ifndef SEMANTIC_H
#define SEMANTIC_H

#define HASH_TABLE_SIZE 0x3fff
#include "SemanTypeEnum.h"
#include "SyntaxTree.h"

typedef struct _Type *TypePtr;
typedef struct _FieldList *FieldListPtr;
typedef struct _TableItem *TableItemPtr;

typedef struct _Type
{
	Kind kind; // 类型的种类
	union
	{
		// 基本类型
		int basic;
		// 数组类型信息包括元素类型与数组大小构成
		struct
		{
			Type elem;
			int size;
		} array;
		// 结构体类型信息是一个链表
		FieldList structure;
	} u;
} Type;

typedef struct _FieldList
{
	char *name; // 域的名字
	Type type;	// 域的类型

	FieldListPtr tail; // 下一个域
} FieldList;


typedef struct _TableItem
{
	char *name; // 变量名
	Type type;	// 变量类型
	TableItemPtr next;
} TableItem;

unsigned int getHash(char *name);
void reportError(ErrorTypeEnum errorType, int line, const char *msg);

void buildSymbolTable(TreeNodePtr root);
#endif // SEMANTIC_H