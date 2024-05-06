#ifndef INTERCODE_H
#define INTERCODE_H

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
	}u;

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
	Operand* op;
	struct Argument *next;
} Argument;

typedef struct ArgList_
{
	Argument *head;
	Argument *cur;
} ArgList;


extern int InterError;
extern InterCodeList *interCodeList;

Operand *newOperand(OperandEnum kind, ...);
void freeOperand(Operand *op);
void setOperand(Operand *op, OperandEnum kind, void *value);
void printOperand(FILE* file, Operand *op);

InterCode *newInterCode(InterCodeEnum kind, ...);
void freeInterCode(InterCode *code);


InterCodeSS *newInterCodeSS(InterCode *code);
void freeInterCodeSS(InterCodeSS *codeSS);
void printInterCodeSS(FILE* file, InterCodeSS *codeSS);


InterCodeList *newInterCodeList();
void freeInterCodeList(InterCodeList *list);
void addInterCodeSS(InterCodeList *list, InterCodeSS *codeSS);
void printInterCodeList(FILE* file, InterCodeList *list);

Argument *newArgument(Operand* op);
void freeArgument(Argument *arg);

ArgList *newArgList();
void freeArgList(ArgList *argList);
void addArg(ArgList *argList, Argument *arg);

Operand *newTempVar();
Operand *newLabel();

void generateInterCodeSS(Node *root);
void generateInterCode(InterCodeEnum kind, ...);

void translateExp(Node* node, Operand* place);
void translateArgs(Node* node, ArgList* argList);
void translateCond(Node* node, Operand* labelTrue, Operand* labelFalse);
void translateVarDec(Node* node, Operand* place);
void translateDec(Node* node);
void translateDecList(Node* node);
void translateDef(Node* node);
void translateDefList(Node* node);
void translateCompSt(Node* node);
void translateStmt(Node* node);
void translateStmtList(Node* node);
void translateFunDec(Node* node);
void translateExtDef(Node* node);
void translateExtDefList(Node* node);


#endif