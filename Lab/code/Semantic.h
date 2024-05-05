#ifndef SEMANTIC_H
#define SEMANTIC_H

#define HASH_TABLE_SIZE 0x3fff
#define STACK_DEEP

#include "SyntaxTree.h"
#include "SemanTypeEnum.h"

#define boolean int


typedef struct type* TypePtr;
typedef struct fieldList* FieldListPtr;
typedef struct tableItem* ItemPtr;
typedef struct hashTable* HashPtr;
typedef struct stack* StackPtr;
typedef struct table* TablePtr;

typedef struct type {
    Kind kind;
    union {
        // 基本类型
        BasicType basic;
        // 数组类型信息包括元素类型与数组大小构成
        struct {
            TypePtr elem;
            int size;
        } array;
        // 结构体类型信息是一个链表
        struct {
            char* structName;
            FieldListPtr field;
        } structure;

        struct {
            int argc;          // argument counter
            FieldListPtr argv;   // argument vector
            TypePtr returnType;  // returnType
        } function;
    } u;
} Type;

typedef struct fieldList {
    char* name;       // 域的名字
    TypePtr type;       // 域的类型
    FieldListPtr tail;  // 下一个域
} FieldList;

typedef struct tableItem {
    int symbolDepth;
    FieldListPtr field;
    ItemPtr nextSymbol;  
    ItemPtr nextHash;    
} TableItem;

typedef struct hashTable {
    ItemPtr* hashArray;
} HashTable;

typedef struct stack {
    ItemPtr* stackArray;
    int curStackDepth;
} Stack;

typedef struct table {
    HashPtr hash;
    StackPtr stack;
    int unNamedStructNum;
} Table;

extern TablePtr table;

TypePtr newType(Kind kind, int argNum,...);
TypePtr copyType(TypePtr src);
void deleteType(TypePtr type);
boolean checkType(TypePtr type1, TypePtr type2);

FieldListPtr newFieldList(char* newName, TypePtr newType);
FieldListPtr copyFieldList(FieldListPtr src);
void deleteFieldList(FieldListPtr fieldList);
void setFieldListName(FieldListPtr p, char* newName);


ItemPtr newItem(int symbolDepth, FieldListPtr pfield);
void deleteItem(ItemPtr item);
boolean isStructDef(ItemPtr src);

// Hash functions
HashPtr newHash();
void deleteHash(HashPtr hash);
ItemPtr getHashHead(HashPtr hash, int index);
void setHashHead(HashPtr hash, int index, ItemPtr newVal);

// Stack functions
StackPtr newStack();
void deleteStack(StackPtr stack);
void addStackDepth(StackPtr stack);
void minusStackDepth(StackPtr stack);
ItemPtr getCurDepthStackHead(StackPtr stack);
void setCurDepthStackHead(StackPtr stack, ItemPtr newVal);

// Table functions
TablePtr initTable();
void deleteTable(TablePtr table);
ItemPtr searchTableItem(TablePtr table, char* name);
boolean checkTableItemConflict(TablePtr table, ItemPtr item);
void addTableItem(TablePtr table, ItemPtr item);
void deleteTableItem(TablePtr table, ItemPtr item);
void clearCurDepthStackList(TablePtr table);


// Global functions
unsigned int getHashCode(char* name);
void reportError(ErrorType type, int line, char* msg);


void preTree(NodePtr node);


void ExtDef(NodePtr node);
void ExtDecList(NodePtr node, TypePtr specifier);
TypePtr Specifier(NodePtr node);
TypePtr StructSpecifier(NodePtr node);
ItemPtr VarDec(NodePtr node, TypePtr specifier);
void FunDec(NodePtr node, TypePtr returnType);
void VarList(NodePtr node, ItemPtr func);
FieldListPtr ParamDec(NodePtr node);
void CompSt(NodePtr node, TypePtr returnType);
void StmtList(NodePtr node, TypePtr returnType);
void Stmt(NodePtr node, TypePtr returnType);
void DefList(NodePtr node, ItemPtr structInfo);
void Def(NodePtr node, ItemPtr structInfo);
void DecList(NodePtr node, TypePtr specifier, ItemPtr structInfo);
void Dec(NodePtr node, TypePtr specifier, ItemPtr structInfo);
TypePtr Exp(NodePtr node);
void Args(NodePtr node, ItemPtr funcInfo);

#endif // SEMANTIC_H