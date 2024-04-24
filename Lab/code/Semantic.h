#ifndef SEMANTIC_H
#define SEMANTIC_H

#define HASH_TABLE_SIZE 0x3fff
#include "SemanTypeEnum.h"
#include "SyntaxTree.h"

//#define DEBUG_SEMANTIC_ANALYSIS

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
	FieldListPtr fieldList; // 变量的作用域，同时存放了变量的类型和名字
	ItemPtr nextHashItem; // 下一个哈希值相同的符号
} TableItem;

typedef struct _HashTable
{
	ItemPtr *table; // 哈希表，类型是TableItem数组
} HashTable;

typedef struct _Table
{
	HashTablePtr hashTable; // 符号表的哈希表
} Table;

// 导入符号表
extern TablePtr RootTable;

// 功能函数

// 哈希函数，根据名字生成哈希值
unsigned int getHashCode(char *name);
// 错误报告函数
void reportError(ErrorTypeEnum errorType, int line, const char *msg);

// 语义分析功能函数

// 释放符号表
void freeSymbolTable(TablePtr table);
// 初始化符号表
void initSymbolTable();
// 语义分析
void startSemantic(NodePtr node);


// 类型相关函数
TypePtr createType(Kind kind,int argNum, ...);
// 比较两个类型是否相同，相同返回TRUE，不同返回FALSE
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



// 符号表相关函数
TablePtr createTable();
void freeTable(TablePtr table);
ItemPtr findItemByName(TablePtr table, char *name);
int isConflict(TablePtr table, ItemPtr item);
void insertTable(TablePtr table, ItemPtr item);
void deleteTableItem(TablePtr table, ItemPtr item);
void printTable(TablePtr table);





// 产生式相关函数

//弃用
void Program();
//弃用
void ExtDefList();
//弃用
void StructSpecifier();
//弃用
void FunDec();
//弃用
void VarList();
//弃用
void ParamDec();
//弃用
void CompSt();
//弃用
void StmtList();
//弃用
void Stmt();
//弃用
void DefList();
//弃用
void Args();

// ExtDef -> Specifier ExtDecList SEMI
// ExtDef -> Specifier SEMI
// ExtDef -> Specifier FunDec CompSt
void ExtDef(NodePtr node);

// Specifier -> TYPE
TypePtr Specifier(NodePtr node);

// ExtDecList -> VarDec
// ExtDecList -> VarDec COMMA ExtDecList
void ExtDecList(NodePtr node,TypePtr type);

// VarDec -> ID
// VarDec -> VarDec LB INT RB
void VarDec(NodePtr node, TypePtr spec);


// Def -> Specifier DecList SEMI
void Def(NodePtr node);

// DecList -> Dec
// DecList -> Dec COMMA DecList
void DecList(NodePtr node, TypePtr spec);

// Dec -> VarDec
// Dec -> VarDec ASSIGNOP Exp
void Dec(NodePtr node, TypePtr spec);

// Exp -> Exp ASSIGNOP Exp
// Exp -> Exp AND Exp
TypePtr Exp(NodePtr node);



#endif // SEMANTIC_H