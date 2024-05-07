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

unsigned int getHashCode(char* name);
void reportError(ErrorType type, int line, char* msg);
void preTree(NodePtr node);


boolean checkType(TypePtr type1, TypePtr type2);
int sizeOfType(TypePtr type);
void setFieldListName(FieldListPtr p, char* newName);
boolean isStructDef(ItemPtr src);
ItemPtr getHashHead(HashPtr hash, int index);
void setHashHead(HashPtr hash, int index, ItemPtr newVal);
void addStackDepth(StackPtr stack);
void minusStackDepth(StackPtr stack);
ItemPtr getCurDepthStackHead(StackPtr stack);
void setCurDepthStackHead(StackPtr stack, ItemPtr newVal);
ItemPtr searchTableItem(TablePtr table, char* name);
int checkTableItemConflict(TablePtr table, ItemPtr item);
void addTableItem(TablePtr table, ItemPtr item);
void clearCurDepthStackList(TablePtr table);
void addWRtoTable(TablePtr table);

TypePtr newType(Kind kind, int argNum,...);
ItemPtr newItem(int symbolDepth, FieldListPtr pfield);
FieldListPtr newFieldList(char* newName, TypePtr newType);
HashPtr newHash();
StackPtr newStack();
TablePtr newTable();

TypePtr copyType(TypePtr src);
FieldListPtr copyFieldList(FieldListPtr src);


void deleteType(TypePtr type);
void deleteFieldList(FieldListPtr fieldList);
void deleteItem(ItemPtr item);
void deleteHash(HashPtr hash);
void deleteStack(StackPtr stack);
void deleteTable(TablePtr table);
void deleteTableItem(TablePtr table, ItemPtr item);



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