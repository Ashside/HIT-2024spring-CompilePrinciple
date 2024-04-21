#ifndef SEMANTIC_H
#define SEMANTIC_H

#define HASH_TABLE_SIZE 0x3fff
#include "SemanTypeEnum.h"
#include "SyntaxTree.h"

typedef struct _Type *TypePtr;
typedef struct _FieldList *FieldListPtr;
typedef struct _TableItem *ItemPtr;
typedef struct _HashTable *HashTablePtr;
typedef struct _Table *TablePtr;
typedef struct _Stack *StackPtr;




typedef struct _Type
{
	Kind kind; // 类型的种类
	// 基本类型 数组 结构体 函数
	union
	{	
		// 基本类型
		BasicTypeEnum basic;

		// 数组类型
		struct
		{
			TypePtr elementType;
			int size;
		} array;

		// 结构体
		struct 
		{
			char* structName;
			FieldListPtr field;
		} structure;

		// 函数
		struct 
		{
			int argc;
			FieldListPtr argv;
			TypePtr retType;
		} function;
		
	} u;
} Type;

typedef struct _FieldList
{
	char *name; // 域的名字
	TypePtr type;	// 域的类型

	FieldListPtr nextField; // 下一个域
} FieldList;


typedef struct _TableItem
{
	int depth; // 作用域深度
	FieldListPtr fieldList; // 变量的作用域

	ItemPtr nextSymbol; // 下一个符号，深度相同
	ItemPtr nextHashItem; // 下一个哈希值相同的符号
} TableItem;

typedef struct _HashTable
{
	ItemPtr *table; // 哈希表，注意类型是指针数组
} HashTable;

typedef struct _Stack
{
	ItemPtr* stackList; // 栈
	int top; // 栈顶
} Stack;

typedef struct _Table
{
	HashTablePtr hashTable; // 符号表
	StackPtr stack; // 栈
	int anonymousCount; // 匿名结构体计数
} Table;

// 导入符号表
extern TablePtr RootTable;

// 功能函数
unsigned int getHashCode(char *name);
void reportError(ErrorTypeEnum errorType, int line, const char *msg);
// 语义分析功能函数
void freeSymbolTable();
void initSymbolTable();



// 类型相关函数
TypePtr createType(Kind kind, ...);
int compareType(TypePtr type1, TypePtr type2);
void printType(TypePtr type);
void freeType(TypePtr type);



// 域相关函数
FieldListPtr createFieldList(char* name, TypePtr type);
void printFieldList(FieldListPtr fieldList);
void freeFieldList(FieldListPtr fieldList);
void setFieldName(FieldListPtr fieldList,char * newName);



// 表项相关函数
ItemPtr createItem(FieldListPtr fieldList);
void printItem(ItemPtr item);
void freeItem(ItemPtr item);


// 哈希表相关函数
HashTablePtr createHash();
void freeHash(HashTablePtr hashTable);
ItemPtr getHashHeadItem(HashTablePtr hashTable, int index);
void setHashHeadItem(HashTablePtr hashTable,int index,ItemPtr newItemValue);



// 栈相关函数
/*
StackPtr createStack();
void freeStack(StackPtr stack);
*/

// 符号表相关函数
TablePtr createTable();
void freeTable(TablePtr table);
ItemPtr searchTable(TablePtr table, char *name);
int isConflict(TablePtr table, ItemPtr item);
void insertTable(TablePtr table, ItemPtr item);
void deleteTableItem(TablePtr table, ItemPtr item);
void printTable(TablePtr table);




// 产生式相关函数
//void Program(NodePtr node);

void ExtDef(NodePtr node);

//弃用
void ExtDefList(NodePtr node,TypePtr spec);

TypePtr Specifier(NodePtr node);

void ExtDecList(NodePtr node,TypePtr type);

//弃用
TypePtr StructSpecifier(NodePtr node);

ItemPtr VarDec(NodePtr node, TypePtr spec);

//弃用
void FunDec(NodePtr node, TypePtr ret);

//弃用
void VarList(NodePtr node, ItemPtr func);

//弃用
FieldListPtr ParamDec(NodePtr node);

//弃用
void CompSt(NodePtr node, TypePtr ret);
//弃用
void StmtList(NodePtr node, TypePtr ret);
//弃用
void Stmt(NodePtr node, TypePtr ret);

//弃用
void DefList(NodePtr node, ItemPtr stru);

void Def(NodePtr node);
void DecList(NodePtr node, TypePtr spec);
void Dec(NodePtr node, TypePtr spec);

TypePtr Exp(NodePtr node);

//弃用
void Args(NodePtr node, ItemPtr func);



#endif // SEMANTIC_H