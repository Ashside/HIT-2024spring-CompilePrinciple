#ifndef INTERCODE_H
#define INTERCODE_H
#define DEBUG_INTER_CODE_GENERATION
#include "SyntaxTree.h"
#include "Semantic.h"
#include "InterCodeEnum.h"

typedef struct Operand_
{
	OperandEnum kind;
	union
	{
		/* data */
		int value;	// for constant
		char *name; // for variable, function, label
	} u;

} Operand;

typedef struct InterCode_
{
	InterCodeEnum kind;
	union
	{
		struct
		{
			Operand *op;
		} singleOp;
		struct
		{
			Operand *left;
			Operand *right;
		} assign;
		struct
		{
			Operand *result;
			Operand *op1;
			Operand *op2;
		} doubleOp;
		struct
		{
			Operand *x;
			Operand *relop;
			Operand *y;
			Operand *z;
		} condJmp;
		struct
		{
			Operand *op;
			int size;
		} decSize;
	} u;

} InterCode; // intermediate code

typedef struct InterCodeSS_
{
	InterCode code;
	struct InterCodeSS_ *prev, *next;
} InterCodeSS; // double linked list

typedef struct InterCodeList_
{
	InterCodeSS *head;
	InterCodeSS *cur;
	int tempVarNum;
	int labelNum;
} InterCodeList;

typedef struct Argument
{
	Operand *op;
	struct Argument *next;
} Argument;

typedef struct ArgList_
{
	Argument *head;
	Argument *cur;
} ArgList;

extern int InterError;
extern InterCodeList *interCodeList;

// 单操作数中间码
// LABEL, FUNCTION, GOTO, RETURN, ARG, PARAM, READ, WRITE
#define IS_SINGLE_OP_INTERCODE(kind) \
	(kind == IR_LABEL || kind == IR_FUNCTION || kind == IR_GOTO || kind == IR_RETURN || kind == IR_ARG || kind == IR_PARAM || kind == IR_READ || kind == IR_WRITE)

// 双操作数中间码
// ASSIGN, GET_ADDR, READ_ADDR, WRITE_ADDR, CALL
#define IS_DOUBLE_OP_INTERCODE(kind) \
	(kind == IR_ASSIGN || kind == IR_GET_ADDR || kind == IR_READ_ADDR || kind == IR_WRITE_ADDR || kind == IR_CALL)

// 三操作数中间码
// ADD, SUB, MUL, DIV
#define IS_THREE_OP_INTERCODE(kind) \
	(kind == IR_ADD || kind == IR_SUB || kind == IR_MUL || kind == IR_DIV)

// 条件跳转中间码
// IF_GOTO
#define IS_COND_JUMP_INTERCODE(kind) \
	(kind == IR_IF_GOTO)

// 申请内存空间中间码
// DEC
#define IS_DEC_INTERCODE(kind) \
	(kind == IR_DEC)

Operand *newOperand(OperandEnum kind, ...);
void freeOperand(Operand *op);
void setOperand(Operand *op, OperandEnum kind, void *value);
void printOperand(FILE *file, Operand *op);

InterCode *newInterCode(InterCodeEnum kind, ...);
void freeInterCode(InterCode *code);

InterCodeSS *newInterCodeSS(InterCode *code);
void freeInterCodeSS(InterCodeSS *codeSS);
void printInterCodeSS(FILE *file, InterCodeSS *codeSS);

InterCodeList *newInterCodeList();
void freeInterCodeList(InterCodeList *list);
void addInterCodeSS(InterCodeList *list, InterCodeSS *codeSS);
void printInterCodeList(FILE *file, InterCodeList *list);

Argument *newArgument(Operand *op);
void freeArgument(Argument *arg);

ArgList *newArgList();
void freeArgList(ArgList *argList);
void addArg(ArgList *argList, Argument *arg);

Operand *newTempVar();
Operand *newLabel();

void generateInterCodeSS(Node *root);
void generateInterCode(InterCodeEnum kind, int argNum, ...);


void translateExtDefList(Node *node);
void translateExtDef(Node *node);
void translateFunDec(Node *node);
void translateCompSt(Node *node);
void translateDefList(Node *node);
void translateDef(Node *node);
void translateDecList(Node *node);
void translateDec(Node *node);
void translateVarDec(Node *node, Operand *var);
void translateStmtList(Node *node);
void translateStmt(Node *node);
void translateExp(Node *node, Operand *var);
void translateCond(Node *node, Operand *labelTrue, Operand *labelFalse);
void translateCompSt(Node *node);
void translateArgs(Node *node, ArgList *argList);
#endif