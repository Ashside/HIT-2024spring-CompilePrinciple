#include "InterCode.h"
#include "SyntaxTree.h"
#include "InterCodeEnum.h"
#include "Semantic.h"

int InterError = FALSE;
InterCodeList *interCodeList;

Operand *newOperand(OperandEnum kind, ...)
{
	// 如果是OP_CONSTANT，那么value是int类型
	// 否则name是char *类型
	// TODO 考虑使用void *类型传入参数
#ifdef DEBUG_INTER_CODE_GENERATION
	printf("newOperand: %d\n", kind);
#endif
	Operand *op = (Operand *)malloc(sizeof(Operand));
	op->kind = kind;
	va_list ap;
	va_start(ap, kind);
	switch (kind)
	{
	case OP_CONSTANT:
		op->u.value = va_arg(ap, int);
		break;
	case OP_VARIABLE:
	case OP_ADDRESS:
	case OP_LABEL:
	case OP_FUNCTION:
	case OP_RELOP:
		op->u.name = va_arg(ap, char *);
		break;
	default:
		break;
	}
	va_end(ap);
#ifdef DEBUG_INTER_CODE_GENERATION
	printf("end newOperand\n");
#endif
	return op;
}

void freeOperand(Operand *op)
{
	if (op == NULL)
	{
		return;
	}
	free(op);
}

void setOperand(Operand *op, OperandEnum kind, void *value)
{
#ifdef DEBUG_INTER_CODE_GENERATION
	printf("setOperand: %d\n", kind);
#endif
	op->kind = kind;
	switch (kind)
	{
	case OP_CONSTANT:
		op->u.value = *(int *)value;
#ifdef DEBUG_INTER_CODE_GENERATION
		printf("setOperand: %d\n", op->u.value);
#endif
		break;
	case OP_VARIABLE:
	case OP_ADDRESS:
	case OP_LABEL:
	case OP_FUNCTION:
	case OP_RELOP:
		op->u.name = (char *)value;
#ifdef DEBUG_INTER_CODE_GENERATION
		printf("setOperand: %s\n", op->u.name);
#endif
		break;
	default:
		break;
	}
#ifdef DEBUG_INTER_CODE_GENERATION
	printf("end setOperand\n");
#endif
}

void printOperand(FILE *file, Operand *op)
{
	if (op == NULL)
	{
		return;
	}
	if (file == NULL)
	{
		switch (op->kind)
		{
		case OP_CONSTANT:
			printf("#%d", op->u.value);
			break;
		case OP_VARIABLE:
		case OP_ADDRESS:
		case OP_LABEL:
		case OP_FUNCTION:
		case OP_RELOP:
			printf("%s", op->u.name);
			break;
		}
	}
	else
	{
		switch (op->kind)
		{
		case OP_CONSTANT:
			fprintf(file, "#%d", op->u.value);
			break;
		case OP_VARIABLE:
		case OP_ADDRESS:
		case OP_LABEL:
		case OP_FUNCTION:
		case OP_RELOP:
			fprintf(file, "%s", op->u.name);
			break;
		}
	}
}

InterCode *newInterCode(InterCodeEnum kind, ...)
{
	InterCode *code = (InterCode *)malloc(sizeof(InterCode));
	code->kind = kind;
	va_list ap;
	switch (kind) {
        case IR_LABEL:
        case IR_FUNCTION:
        case IR_GOTO:
        case IR_RETURN:
        case IR_ARG:
        case IR_PARAM:
        case IR_READ:
        case IR_WRITE:
            va_start(ap, 1);
            code->u.singleOp.op = va_arg(ap, Operand*);
            break;
        case IR_ASSIGN:
        case IR_GET_ADDR:
        case IR_READ_ADDR:
        case IR_WRITE_ADDR:
        case IR_CALL:
            va_start(ap, 2);
            code->u.assign.left = va_arg(ap, Operand*);
            code->u.assign.right = va_arg(ap, Operand*);
            break;
        case IR_ADD:
        case IR_SUB:
        case IR_MUL:
        case IR_DIV:
            va_start(ap, 3);
            code->u.doubleOp.result = va_arg(ap, Operand*);
            code->u.doubleOp.op1 = va_arg(ap, Operand*);
            code->u.doubleOp.op2 = va_arg(ap, Operand*);
            break;
        case IR_DEC:
            va_start(ap, 2);
            code->u.decSize.op = va_arg(ap, Operand*);
            code->u.decSize.size = va_arg(ap, int);
            break;
        case IR_IF_GOTO:
            va_start(ap, 4);
            code->u.condJmp.x = va_arg(ap, Operand*);
            code->u.condJmp.relop = va_arg(ap, Operand*);
            code->u.condJmp.y = va_arg(ap, Operand*);
            code->u.condJmp.z = va_arg(ap, Operand*);
    }
	va_end(ap);	
	return code;
}

void freeInterCode(InterCode *code)
{
	if (code == NULL)
	{
		return;
	}
	free(code);
}

InterCodeSS *newInterCodeSS(InterCode *code)
{
	InterCodeSS *codeSS = (InterCodeSS *)malloc(sizeof(InterCodeSS));
	codeSS->code = *code;
	codeSS->prev = NULL;
	codeSS->next = NULL;
	return codeSS;
}

void freeInterCodeSS(InterCodeSS *codeSS)
{
	if (codeSS == NULL)
	{
		return;
	}
	freeInterCode(&codeSS->code);
	free(codeSS);
}

void printInterCodeSS(FILE *file, InterCodeSS *codeSS)
{
	if (codeSS == NULL)
	{
		switch (codeSS->code.kind)
		{
		case IR_LABEL:
			printf("LABEL ");
			printOperand(file, codeSS->code.u.singleOp.op);
			printf(" :");
			break;
		case IR_FUNCTION:
			printf("FUNCTION ");
			printOperand(file, codeSS->code.u.singleOp.op);
			printf(" :");
			break;
		case IR_ASSIGN:
			printOperand(file, codeSS->code.u.assign.left);
			printf(" := ");
			printOperand(file, codeSS->code.u.assign.right);
			break;
		case IR_ADD:
			printOperand(file, codeSS->code.u.doubleOp.result);
			printf(" := ");
			printOperand(file, codeSS->code.u.doubleOp.op1);
			printf(" + ");
			printOperand(file, codeSS->code.u.doubleOp.op2);
			break;
		case IR_SUB:
			printOperand(file, codeSS->code.u.doubleOp.result);
			printf(" := ");
			printOperand(file, codeSS->code.u.doubleOp.op1);
			printf(" - ");
			printOperand(file, codeSS->code.u.doubleOp.op2);
			break;
		case IR_MUL:
			printOperand(file, codeSS->code.u.doubleOp.result);
			printf(" := ");
			printOperand(file, codeSS->code.u.doubleOp.op1);
			printf(" * ");
			printOperand(file, codeSS->code.u.doubleOp.op2);
			break;
		case IR_DIV:
			printOperand(file, codeSS->code.u.doubleOp.result);
			printf(" := ");
			printOperand(file, codeSS->code.u.doubleOp.op1);
			printf(" / ");
			printOperand(file, codeSS->code.u.doubleOp.op2);
			break;
		case IR_GET_ADDR:
			printOperand(file, codeSS->code.u.assign.left);
			printf(" := &");
			printOperand(file, codeSS->code.u.assign.right);
			break;
		case IR_READ_ADDR:
			printOperand(file, codeSS->code.u.assign.left);
			printf(" := *");
			printOperand(file, codeSS->code.u.assign.right);
			break;
		case IR_WRITE_ADDR:
			printf("*");
			printOperand(file, codeSS->code.u.assign.left);
			printf(" := ");
			printOperand(file, codeSS->code.u.assign.right);
			break;
		case IR_GOTO:
			printf("GOTO ");
			printOperand(file, codeSS->code.u.singleOp.op);
			break;
		case IR_IF_GOTO:
			printf("IF ");
			printOperand(file, codeSS->code.u.condJmp.x);
			printf(" ");
			printOperand(file, codeSS->code.u.condJmp.relop);
			printf(" ");
			printOperand(file, codeSS->code.u.condJmp.y);
			printf(" GOTO ");
			printOperand(file, codeSS->code.u.condJmp.z);
			break;
		case IR_RETURN:
			printf("RETURN ");
			printOperand(file, codeSS->code.u.singleOp.op);
			break;
		case IR_DEC:
			printf("DEC ");
			printOperand(file, codeSS->code.u.decSize.op);
			printf(" %d", codeSS->code.u.decSize.size);
			break;
		case IR_ARG:
			printf("ARG ");
			printOperand(file, codeSS->code.u.singleOp.op);
			break;
		case IR_CALL:
			printOperand(file, codeSS->code.u.assign.left);
			printf(" := CALL ");
			printOperand(file, codeSS->code.u.assign.right);
			break;
		case IR_PARAM:
			printf("PARAM ");
			printOperand(file, codeSS->code.u.singleOp.op);
			break;
		case IR_READ:
			printf("READ ");
			printOperand(file, codeSS->code.u.singleOp.op);
			break;
		case IR_WRITE:
			printf("WRITE ");
			printOperand(file, codeSS->code.u.singleOp.op);
			break;
		}
		printf("\n");
	}
	else {
		switch (codeSS->code.kind)
		{
		case IR_LABEL:
			fprintf(file, "LABEL ");
			printOperand(file, codeSS->code.u.singleOp.op);
			fprintf(file, " :");
			break;
		case IR_FUNCTION:
			fprintf(file, "FUNCTION ");
			printOperand(file, codeSS->code.u.singleOp.op);
			fprintf(file, " :");
			break;
		case IR_ASSIGN:
			printOperand(file, codeSS->code.u.assign.left);
			fprintf(file, " := ");
			printOperand(file, codeSS->code.u.assign.right);
			break;
		case IR_ADD:
			printOperand(file, codeSS->code.u.doubleOp.result);
			fprintf(file, " := ");
			printOperand(file, codeSS->code.u.doubleOp.op1);
			fprintf(file, " + ");
			printOperand(file, codeSS->code.u.doubleOp.op2);
			break;
		case IR_SUB:
			printOperand(file, codeSS->code.u.doubleOp.result);
			fprintf(file, " := ");
			printOperand(file, codeSS->code.u.doubleOp.op1);
			fprintf(file, " - ");
			printOperand(file, codeSS->code.u.doubleOp.op2);
			break;
		case IR_MUL:
			printOperand(file, codeSS->code.u.doubleOp.result);
			fprintf(file, " := ");
			printOperand(file, codeSS->code.u.doubleOp.op1);
			fprintf(file, " * ");
			printOperand(file, codeSS->code.u.doubleOp.op2);
			break;
		case IR_DIV:
			printOperand(file, codeSS->code.u.doubleOp.result);
			fprintf(file, " := ");
			printOperand(file, codeSS->code.u.doubleOp.op1);
			fprintf(file, " / ");
			printOperand(file, codeSS->code.u.doubleOp.op2);
			break;
		case IR_GET_ADDR:
			printOperand(file, codeSS->code.u.assign.left);
			fprintf(file, " := &");
			printOperand(file, codeSS->code.u.assign.right);
			break;
		case IR_READ_ADDR:
			printOperand(file, codeSS->code.u.assign.left);
			fprintf(file, " := *");
			printOperand(file, codeSS->code.u.assign.right);
			break;
		case IR_WRITE_ADDR:
			fprintf(file, "*");
			printOperand(file, codeSS->code.u.assign.left);
			fprintf(file, " := ");
			printOperand(file, codeSS->code.u.assign.right);
			break;
		case IR_GOTO:
			fprintf(file, "GOTO ");
			printOperand(file, codeSS->code.u.singleOp.op);
			break;
		case IR_IF_GOTO:
			fprintf(file, "IF ");
			printOperand(file, codeSS->code.u.condJmp.x);
			fprintf(file, " ");
			printOperand(file, codeSS->code.u.condJmp.relop);
			fprintf(file, " ");
			printOperand(file, codeSS->code.u.condJmp.y);
			fprintf(file, " GOTO ");
			printOperand(file, codeSS->code.u.condJmp.z);
			break;
		case IR_RETURN:
			fprintf(file, "RETURN ");
			printOperand(file, codeSS->code.u.singleOp.op);
			break;
		case IR_DEC:
			fprintf(file, "DEC ");
			printOperand(file, codeSS->code.u.decSize.op);
			fprintf(file, " %d", codeSS->code.u.decSize.size);
			break;
		case IR_ARG:
			fprintf(file, "ARG ");
			printOperand(file, codeSS->code.u.singleOp.op);
			break;
		case IR_CALL:
			printOperand(file, codeSS->code.u.assign.left);
			fprintf(file, " := CALL ");
			printOperand(file, codeSS->code.u.assign.right);
			break;
		case IR_PARAM:
			fprintf(file, "PARAM ");
			printOperand(file, codeSS->code.u.singleOp.op);
			break;
		case IR_READ:
			fprintf(file, "READ ");
			printOperand(file, codeSS->code.u.singleOp.op);
			break;
		case IR_WRITE:
			fprintf(file, "WRITE ");
			printOperand(file, codeSS->code.u.singleOp.op);
			break;
		}
		fprintf(file, "\n");
	}


}

InterCodeList *newInterCodeList()
{
	InterCodeList *list = (InterCodeList *)malloc(sizeof(InterCodeList));
	list->head = NULL;
	list->cur = NULL;
	// 临时变量编号
	// TODO 临时变量编号的生成
	list->tempVarNum = 0;
	list->labelNum = 0;
	return list;
}

void freeInterCodeList(InterCodeList *list)
{
	if (list == NULL)
	{
		return;
	}
	InterCodeSS *p = list->head;
	while (p != NULL)
	{
		InterCodeSS *q = p;
		p = p->next;
		freeInterCodeSS(q);
	}
	free(list);
}

void addInterCodeSS(InterCodeList *list, InterCodeSS *codeSS)
{
	if (list == NULL || codeSS == NULL)
	{
		return;
	}
	if (list->head == NULL)
	{
		list->head = codeSS;
		list->cur = codeSS;
	}
	else
	{
		// 将codeSS插入到list的游标cur之后
		// 涉及到插入中间节点的操作
		list->cur->next = codeSS;
		codeSS->prev = list->cur;
		list->cur = codeSS;
	}
}

void printInterCodeList(FILE *file, InterCodeList *list)
{
	InterCodeSS *p = list->head;
	if (file == NULL)
	{
		while (p != NULL)
		{
			printInterCodeSS(file, p);
			p = p->next;
		}
	}
	else
	{
		while (p != NULL)
		{
			printInterCodeSS(file, p);
			p = p->next;
		}
	}
}

Argument *newArgument(Operand *op)
{
	Argument *arg = (Argument *)malloc(sizeof(Argument));
	arg->op = op;
	arg->next = NULL;
	return arg;
}

void freeArgument(Argument *arg)
{
	if (arg == NULL)
	{
		return;
	}
	freeOperand(arg->op);
	free(arg);
}

ArgList *newArgList()
{
	ArgList *list = (ArgList *)malloc(sizeof(ArgList));
	list->head = NULL;
	list->cur = NULL;
	return list;
}

void freeArgList(ArgList *argList)
{
	if (argList == NULL)
	{
		return;
	}
	Argument *p = argList->head;
	while (p != NULL)
	{
		Argument *q = p;
		p = p->next;
		freeArgument(q);
	}
	free(argList);
}

void addArg(ArgList *argList, Argument *arg)
{
	if (argList == NULL || arg == NULL)
	{
		return;
	}
	if (argList->head == NULL)
	{
		argList->head = arg;
		argList->cur = arg;
	}
	else
	{
		argList->cur->next = arg;
		argList->cur = arg;
	}
}

Operand *newTempVar()
{
	Operand *op = newOperand(OP_VARIABLE, "t");
	op->u.name = (char *)malloc(sizeof(char) * 10);
	sprintf(op->u.name, "t%d", interCodeList->tempVarNum);
	interCodeList->tempVarNum++;
	return op;
}

Operand *newLabel()
{
	Operand *op = newOperand(OP_LABEL, "label");
	op->u.name = (char *)malloc(sizeof(char) * 10);
	sprintf(op->u.name, "label%d", interCodeList->labelNum);
	interCodeList->labelNum++;
	return op;
}