#include "InterCode.h"
#include "SyntaxTree.h"
#include "InterCodeEnum.h"
#include "Semantic.h"

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
		op->u.value = (int)value;
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

InterCode *newInterCode(InterCodeEnum kind, int argNum,...)
{
	InterCode *code = (InterCode *)malloc(sizeof(InterCode));
	code->kind = kind;
	va_list ap;
	if (IS_SINGLE_OP_INTERCODE(kind))
	{
		va_start(ap, argNum);
		code->u.singleOp.op = va_arg(ap, Operand *);
	}
	else if (IS_DOUBLE_OP_INTERCODE(kind))
	{
		va_start(ap, argNum);
		code->u.assign.left = va_arg(ap, Operand *);
		code->u.assign.right = va_arg(ap, Operand *);
	}
	else if (IS_THREE_OP_INTERCODE(kind))
	{
		va_start(ap, argNum);
		code->u.doubleOp.result = va_arg(ap, Operand *);
		code->u.doubleOp.op1 = va_arg(ap, Operand *);
		code->u.doubleOp.op2 = va_arg(ap, Operand *);
	}
	else if (IS_DEC_INTERCODE(kind))
	{
		va_start(ap, argNum);
		code->u.decSize.op = va_arg(ap, Operand *);
		code->u.decSize.size = va_arg(ap, int);
	}
	else if (IS_COND_JUMP_INTERCODE(kind))
	{
		va_start(ap, argNum);
		code->u.condJmp.x = va_arg(ap, Operand *);
		code->u.condJmp.relop = va_arg(ap, Operand *);
		code->u.condJmp.y = va_arg(ap, Operand *);
		code->u.condJmp.z = va_arg(ap, Operand *);
	}
	else
	{
		va_start(ap, argNum);
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
	if (file == NULL)
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
	else
	{
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
	list->tempVarNum = 1;
	list->labelNum = 1;
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
	
		while (p != NULL)
		{
			printInterCodeSS(file, p);
			p = p->next;
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

void generateInterCodeSS(Node *root)
{
	if (root == NULL)
	{
		return;
	}
	if (!strcmp(root->name, "ExtDefList"))
	{
		translateExtDefList(root);
	}
	else
	{
		generateInterCodeSS(root->child);
		generateInterCodeSS(root->sibling);
	}
}

// 生成中间代码
// 参数顺序：kind, argNum, arg1, arg2, arg3
// 1参中间码 LABEL x
// 2参中间码 x := y
// 3参中间码 x := y + z
// 4参中间码 IF x [relop] y GOTO z
void generateInterCode(InterCodeEnum kind, int argNum, ...)
{

	va_list ap;
	int size = 0;

	InterCodeSS *codeSS = (InterCodeSS *)malloc(sizeof(InterCodeSS));

	Operand *op1 = NULL;
	Operand *op2 = NULL;
	Operand *relop = NULL;
	Operand *result = NULL;
	Operand *temp = NULL;

	if (IS_SINGLE_OP_INTERCODE(kind))
	{
		va_start(ap, argNum);
		op1 = va_arg(ap, Operand *);
		// 正常情况下，直接生成中间代码
		// 但是如果op1是一个地址，那么需要先读取地址
		// 生成一个新的临时变量，然后再生成中间代码
		if (op1->kind == OP_ADDRESS)
		{
			temp = newTempVar();
			generateInterCode(IR_READ_ADDR, 2, temp, op1);
			op1 = temp;
		}
		codeSS = newInterCodeSS(newInterCode(kind,1 ,op1));
		addInterCodeSS(interCodeList, codeSS);
	}
	else if (IS_DOUBLE_OP_INTERCODE(kind))
	{
		va_start(ap, argNum);
		op1 = va_arg(ap, Operand *);
		op2 = va_arg(ap, Operand *);
		// 首先考察赋值中的地址操作
		if ((op1->kind == OP_ADDRESS || op2->kind == OP_ADDRESS) && (kind == IR_ASSIGN))
		{
			// 如果op1是地址，op2不是，那么，*op1 := op2
			if (op1->kind == OP_ADDRESS && op2->kind != OP_ADDRESS)
			{
				generateInterCode(IR_WRITE_ADDR, 2, op1, op2);
			}
			// 如果op1不是地址，op2是，那么，op1 := *op2
			else if (op1->kind != OP_ADDRESS && op2->kind == OP_ADDRESS)
			{
				generateInterCode(IR_READ_ADDR, 2, op1, op2);
			}
			// 如果op1和op2都是地址，那么，*op1 := *op2
			// 即读取op2的值，然后写入op1地址对应的位置
			else if (op1->kind == OP_ADDRESS && op2->kind == OP_ADDRESS)
			{
				temp = newTempVar();
				generateInterCode(IR_READ_ADDR, 2, temp, op2);
				generateInterCode(IR_WRITE_ADDR, 2, op1, temp);
			}
		}
		// 不是赋值操作，直接生成中间代码
		else
		{
			codeSS = newInterCodeSS(newInterCode(kind, 2,op1, op2));
			addInterCodeSS(interCodeList, codeSS);
		}
	}
	else if (IS_THREE_OP_INTERCODE(kind))
	{
		va_start(ap, argNum);
		result = va_arg(ap, Operand *);
		op1 = va_arg(ap, Operand *);
		op2 = va_arg(ap, Operand *);
		if (op1->kind == OP_ADDRESS)
		{
			temp = newTempVar();
			generateInterCode(IR_READ_ADDR, 2, temp, op1);
			op1 = temp;
		}
		if (op2->kind == OP_ADDRESS)
		{
			temp = newTempVar();
			generateInterCode(IR_READ_ADDR, 2, temp, op2);
			op2 = temp;
		}
		codeSS = newInterCodeSS(newInterCode(kind, 3,result, op1, op2));
		addInterCodeSS(interCodeList, codeSS);
	}
	else if (IS_DEC_INTERCODE(kind))
	{
		va_start(ap, argNum);
		op1 = va_arg(ap, Operand *);
		size = va_arg(ap, int);
		codeSS = newInterCodeSS(newInterCode(kind, 2,op1, size));
		addInterCodeSS(interCodeList, codeSS);
	}
	else if (IS_COND_JUMP_INTERCODE(kind))
	{
		va_start(ap, argNum);
		result = va_arg(ap, Operand *);
		relop = va_arg(ap, Operand *);
		op1 = va_arg(ap, Operand *);
		op2 = va_arg(ap, Operand *);
		codeSS = newInterCodeSS(newInterCode(kind, 4,result, relop, op1, op2));
		addInterCodeSS(interCodeList, codeSS);
	}
	else
	{
		va_start(ap, argNum);
	}
	va_end(ap);
}

void translateExtDefList(Node *node)
{
	// ExtDefList -> ExtDef ExtDefList
	// ExtDefList -> e
	while (node != NULL)
	{
		if (!strcmp(node->child->name, "ExtDef"))
		{
			translateExtDef(node->child);
		}
		node = node->child->sibling;
	}
}

void translateExtDef(Node *node)
{
	// ExtDef -> Specifier ExtDecList SEMI
	// ExtDef -> Specifier FunDec CompSt
	if (node == NULL)
	{
		return;
	}
	Node *specifier = node->child;
	// TODO: 这里没有全局变量的使用
	// 直接解析函数定义
	Node *funDec = specifier->sibling;
	Node *compSt = funDec->sibling;

	if (!strcmp(funDec->name, "FunDec"))
	{
		translateFunDec(funDec);
		translateCompSt(compSt);
	}
}

void translateFunDec(Node *node)
{
	// FunDec -> ID LP VarList RP
	// FunDec -> ID LP RP
	if (node == NULL)
	{
		return;
	}
	generateInterCode(IR_FUNCTION, 1, newOperand(OP_FUNCTION, newString(node->child->value)));
	if (node->child->sibling->sibling != NULL)
	{
		// 有参数
		// translateVarList(node->child->sibling->sibling);
		ItemPtr item = searchTableItem(table, node->child->value);
		FieldListPtr field = item->field->type->u.function.argv;
		while (field != NULL)
		{
			generateInterCode(IR_PARAM, 1, newOperand(OP_VARIABLE, newString(field->name)));
			field = field->tail;
		}
	}
}

void translateCompSt(Node *node)
{
	// CompSt -> LC DefList StmtList RC
	// DefList -> e
	if (node == NULL)
	{
		return;
	}
	Node* secondChild = node->child->sibling;

	if (!strcmp(secondChild->name, "DefList"))
	{
		translateDefList(secondChild);
		translateStmtList(secondChild->sibling);
	}
	else if (!strcmp(secondChild->name, "StmtList"))
	{
		translateStmtList(secondChild);
	}
}

void translateArgs(Node *node, ArgList *argList)
{
	// Args -> Exp COMMA Args
	// Args -> Exp
	Argument *arg = newArgument(newTempVar());
	translateExp(node->child, arg->op);

	addArg(argList, arg);
	if (node->child->sibling != NULL)
	{
		translateArgs(node->child->sibling->sibling, argList);
	}
}

void translateDefList(Node *node)
{
	// DefList -> Def DefList
	// DefList -> e
	if (node == NULL)
	{
		return;
	}
	while (node != NULL)
	{
		if (!strcmp(node->child->name, "Def"))
		{
			translateDef(node->child);
		}
		node = node->child->sibling;
	}
}

void translateDef(Node *node)
{
	// Def -> Specifier DecList SEMI
	if (node == NULL)
	{
		return;
	}
	Node *specifier = node->child;
	Node *decList = specifier->sibling;
	if (!strcmp(decList->name, "DecList"))
	{
		translateDecList(decList);
	}
}

void translateDecList(Node *node)
{
	// DecList -> Dec
	// DecList -> Dec COMMA DecList
	if (node == NULL)
	{
		return;
	}
	while (node != NULL)
	{
		if (!strcmp(node->child->name, "Dec"))
		{
			translateDec(node->child);
		}
		if (node->child->sibling != NULL)
		{
			node = node->child->sibling->sibling;
		}
		else
		{
			break;
		}
	}
}

void translateDec(Node *node)
{
	// Dec -> VarDec
	// Dec -> VarDec ASSIGN Exp
	if (node == NULL)
	{
		return;
	}
	Node *varDec = node->child;
	if (varDec->sibling != NULL)
	{
		Node *exp = varDec->sibling->sibling;
		// 变量定义并赋值
		Operand *varTemp = newTempVar();
		translateVarDec(varDec, varTemp);
		Operand *expTemp = newTempVar();
		translateExp(exp, expTemp);
		generateInterCode(IR_ASSIGN, 2, varTemp, expTemp);
	}
	else
	{
		// 变量定义
		translateVarDec(varDec, NULL);
	}
}
// 变量定义
// 如果var不为NULL，那么生成赋值中间代码
void translateVarDec(Node *node, Operand *var)
{
	// VarDec -> ID
	// VarDec -> VarDec LB INT RB
	if (node == NULL)
	{
		return;
	}

	NodePtr idNode = node->child;
	if (!strcmp(idNode->name, "ID"))
	{
		ItemPtr idItem = searchTableItem(table, idNode->value);
		TypePtr idType = idItem->field->type;
		if (idType->kind == BASIC)
		{
			if (var != NULL)
			{
				interCodeList->tempVarNum--; // 临时变量编号减一，因为不需要生成临时变量
				setOperand(var, OP_VARIABLE, (void *)newString(idNode->value));
			}
		}
		else if (idType->kind == ARRAY)
		{
			// 数组类型
			// 生成DEC中间代码
			generateInterCode(IR_DEC, 2, newOperand(OP_VARIABLE, newString(idItem->field->name)), sizeOfType(idType));
		}
	}
	else if (!strcmp(idNode->name, "VarDec"))
	{
		translateVarDec(idNode, var);
	}
}

void translateStmtList(Node *node)
{
	// StmtList -> Stmt StmtList
	// StmtList -> e
	if (node == NULL)
	{
		return;
	}
	while (node != NULL)
	{
		if (!strcmp(node->child->name, "Stmt"))
		{
			translateStmt(node->child);
		}
		node = node->child->sibling;
	}
}

void translateStmt(Node *node)
{
	// Stmt -> Exp SEMI
	// Stmt -> CompSt
	// Stmt -> RETURN Exp SEMI
	// Stmt -> IF LP Exp RP Stmt
	// Stmt -> IF LP Exp RP Stmt ELSE Stmt
	// Stmt -> WHILE LP Exp RP Stmt
	if (node == NULL)
	{
		return;
	}
	Node *child = node->child;

	if (!strcmp(child->name, "Exp"))
	{
		translateExp(child, NULL);
	}
	else if (!strcmp(child->name, "CompSt"))
	{
		translateCompSt(child);
	}
	else if (!strcmp(child->name, "RETURN"))
	{
		Operand *temp = newTempVar();
		translateExp(child->sibling, temp);
		generateInterCode(IR_RETURN, 1, temp);
	}
	else if (!strcmp(child->name, "IF"))
	{
		// IF LP Exp RP Stmt
		// IF LP Exp RP Stmt ELSE Stmt

		NodePtr expNode = child->sibling->sibling;
		NodePtr stmtNode = expNode->sibling->sibling;
		Operand *label1 = newLabel();
		Operand *label2 = newLabel();
		translateCond(expNode, label1, label2);
		generateInterCode(IR_LABEL, 1, label1);
		translateStmt(stmtNode);

		if (stmtNode->sibling == NULL)
		{
			generateInterCode(IR_LABEL, 1, label2);
		}
		else
		{
			Operand *label3 = newLabel();
			generateInterCode(IR_GOTO, 1, label3);
			generateInterCode(IR_LABEL, 1, label2);
			translateStmt(stmtNode->sibling->sibling);
			generateInterCode(IR_LABEL, 1, label3);
		}
	}
	else if (!strcmp(child->name, "WHILE"))
	{
		// WHILE LP Exp RP Stmt
		Operand *label1 = newLabel();
		Operand *label2 = newLabel();
		Operand *label3 = newLabel();
		NodePtr expNode = child->sibling->sibling;
		NodePtr stmtNode = expNode->sibling->sibling;
		generateInterCode(IR_LABEL, 1, label1);
		translateCond(expNode, label2, label3);
		generateInterCode(IR_LABEL, 1, label2);
		translateStmt(stmtNode);
		generateInterCode(IR_GOTO, 1, label1);
		generateInterCode(IR_LABEL, 1, label3);
	}
}

void translateExp(Node *node, Operand *var)
{
	// Exp -> Exp ASSIGN Exp
	// Exp -> Exp AND Exp
	// Exp -> Exp OR Exp
	// Exp -> Exp RELOP Exp
	// Exp -> Exp PLUS Exp
	// Exp -> Exp MINUS Exp
	// Exp -> Exp STAR Exp
	// Exp -> Exp DIV Exp

	// Exp -> Exp LB Exp RB
	// Exp -> Exp DOT ID // 结构体访问不要求
	// Exp -> NOT Exp
	// Exp -> MINUS Exp

	// Exp -> LP Exp RP

	// Exp -> ID LP Args RP
	// Exp -> ID LP RP

	// Exp -> ID
	// Exp -> INT
	// Exp -> FLOAT
	if (node == NULL)
	{
		return;
	}
	Node *child = node->child;
	if (!strcmp(child->name, "Exp"))
	{
		// Exp -> Exp ASSIGN Exp
		// Exp -> Exp AND Exp
		// Exp -> Exp OR Exp
		// Exp -> Exp RELOP Exp
		// Exp -> Exp PLUS Exp
		// Exp -> Exp MINUS Exp
		// Exp -> Exp STAR Exp
		// Exp -> Exp DIV Exp

		// Exp -> Exp LB Exp RB

		// Exp -> Exp DOT ID

		// 参照指导书AND OR RELOP和NOT采取相同的翻译方式
		if (!strcmp(node->child->sibling->name, "AND") || !strcmp(node->child->sibling->name, "OR") ||
			!strcmp(node->child->sibling->name, "RELOP"))
		{
			Operand *label1 = newLabel();
			Operand *label2 = newLabel();
			Operand *true_num = newOperand(OP_CONSTANT, 1);
			Operand *false_num = newOperand(OP_CONSTANT, 0);
			generateInterCode(IR_ASSIGN, 2, var, false_num);
			translateCond(node, label1, label2);
			generateInterCode(IR_LABEL, 1, label1);
			generateInterCode(IR_ASSIGN, 2, var, true_num);
		}
		// Exp -> Exp PLUS Exp
		// Exp -> Exp ASSIGNOP Exp
		else if (!strcmp(node->child->sibling->name, "PLUS") || !strcmp(node->child->sibling->name, "MINUS") ||
				 !strcmp(node->child->sibling->name, "STAR") || !strcmp(node->child->sibling->name, "DIV") ||
				 !strcmp(node->child->sibling->name, "ASSIGNOP"))
		{
			// Exp -> Exp ASSIGNOP Exp
			if (!strcmp(node->child->sibling->name, "ASSIGNOP"))
			{
				Operand *t2 = newTempVar();
				translateExp(node->child->sibling->sibling, t2);
				Operand *t1 = newTempVar();
				translateExp(node->child, t1);
				generateInterCode(IR_ASSIGN, 2, t1, t2);
			}
			// Exp -> Exp PLUS Exp
			else
			{

				Operand *t1 = newTempVar();
				translateExp(node->child, t1);
				Operand *t2 = newTempVar();
				translateExp(node->child->sibling->sibling, t2);
				// Exp -> Exp PLUS Exp
				if (!strcmp(node->child->sibling->name, "PLUS"))
				{
					generateInterCode(IR_ADD, 3, var, t1, t2);
				}
				// Exp -> Exp MINUS Exp
				else if (!strcmp(node->child->sibling->name, "MINUS"))
				{
					generateInterCode(IR_SUB, 3, var, t1, t2);
				}
				// Exp -> Exp STAR Exp
				else if (!strcmp(node->child->sibling->name, "STAR"))
				{
					generateInterCode(IR_MUL, 3, var, t1, t2);
				}
				// Exp -> Exp DIV Exp
				else if (!strcmp(node->child->sibling->name, "DIV"))
				{
					generateInterCode(IR_DIV, 3, var, t1, t2);
				}
			}
		}
		else if (!strcmp(node->child->sibling->name, "LB"))
		{
			// Exp -> Exp LB Exp RB
			// 数组访问
			// 生成中间代码
			Operand *t1 = newTempVar();
			Operand *t2 = newTempVar();
			translateExp(node->child, t1);					 // 数组名，即数组首地址
			translateExp(node->child->sibling->sibling, t2); // 数组下标
			Operand *t3 = newTempVar();						 // 数组下标乘以数组元素大小
			// 数组下标乘以数组元素大小
			Operand *size = newOperand(OP_CONSTANT, sizeOfType(searchTableItem(table, t1->u.name)->field->type->u.array.elem));
			generateInterCode(IR_MUL, 3, t3, t2, size);
			Operand *t4 = newTempVar(); // 获取数组首地址
			generateInterCode(IR_GET_ADDR, 2, t4, t1);
			Operand *t5 = newTempVar(); // 计算数组元素地址
			generateInterCode(IR_ADD, 3, t5, t4, t3);
			// 将数组元素地址返回给var
			generateInterCode(IR_ASSIGN, 2, var, t5);
			var->kind = OP_ADDRESS;


		}
	}
	else if (!strcmp(child->name, "NOT"))
	{
		// NOT Exp
		Operand *label1 = newLabel();
		Operand *label2 = newLabel();
		Operand *true_num = newOperand(OP_CONSTANT, 1);
		Operand *false_num = newOperand(OP_CONSTANT, 0);
		generateInterCode(IR_ASSIGN, 2, var, false_num);
		translateCond(node, label1, label2);
		generateInterCode(IR_LABEL, 1, label1);
		generateInterCode(IR_ASSIGN, 2, var, true_num);
	}
	else if (!strcmp(child->name, "LP"))
	{
		translateExp(child->sibling, var);
	}
	else if (!strcmp(child->name, "MINUS"))
	{
		// MINUS Exp
		Operand *t1 = newTempVar();
		translateExp(child->sibling, t1);
		Operand *t2 = newTempVar();
		Operand *zero = newOperand(OP_CONSTANT, 0);
		generateInterCode(IR_SUB, 3, t2, zero, t1);
		generateInterCode(IR_ASSIGN, 2, var, t2);
	}
	else if (!strcmp(child->name, "ID"))
	{
		// Exp -> ID
		// Exp -> ID LP Args RP
		// Exp -> ID LP RP
		// 函数调用
		if (child->sibling != NULL)
		{
			// Exp -> ID LP Args RP
			// Exp -> ID LP RP

			Operand *func = newOperand(OP_FUNCTION, newString(child->value));

			if (!strcmp(child->sibling->sibling->name, "Args"))
			{
				ArgList *argList = newArgList();
				translateArgs(child->sibling->sibling, argList);
				if (!strcmp(child->value, "write"))
				{
					generateInterCode(IR_WRITE, 1, argList->head->op);
				}
				else
				{
					// 生成ARG中间代码
					Argument *arg = argList->head;
					while (arg != NULL)
					{
						generateInterCode(IR_ARG, 1, arg->op);
						arg = arg->next;
					}
					// 生成CALL中间代码
					if (var != NULL)
					{
						generateInterCode(IR_CALL, 2, var, func);
					}
					else
					{
						Operand *temp = newTempVar();
						generateInterCode(IR_CALL, 2, temp, func);
					}
				}
			}
			else if (!strcmp(child->sibling->sibling->name, "RP"))
			{
				// 生成CALL中间代码
				if (!strcmp(child->value, "read"))
				{
					generateInterCode(IR_READ, 1, var);
				}
				else
				{
					if (var != NULL)
					{
						generateInterCode(IR_CALL, 2, var, func);
					}
					else
					{
						Operand *temp = newTempVar();
						generateInterCode(IR_CALL, 2, temp, func);
					}
				}
			}
		}
		// Exp -> ID
		else
		{
			ItemPtr item = searchTableItem(table, child->value);
			if (item == NULL)
			{
				return;
			}
			interCodeList->tempVarNum--; // 临时变量编号减一，因为不需要生成临时变量
			setOperand(var, OP_VARIABLE, (void *)newString(child->value));
		}
	}
	else if (!strcmp(child->name, "INT"))
	{
		// INT
		interCodeList->tempVarNum--; // 临时变量编号减一，因为不需要生成临时变量
		setOperand(var, OP_CONSTANT, (void *)atoi(child->value));
	}
	else if (!strcmp(child->name, "FLOAT"))
	{
		// FLOAT
		// TODO 浮点数应该是不存在的
		fprintf(stderr, "FLOAT is not supported\n");
	}
}

void translateCond(Node *node, Operand *labelTrue, Operand *labelFalse)
{
	// Exp -> Exp RELOP Exp
	// Exp -> NOT Exp
	// Exp -> Exp AND Exp
	// Exp -> Exp OR Exp

	// 仅在以上四种布尔表达式中使用

	NodePtr child = node->child;
	NodePtr sibling = child->sibling;

	if (!strcmp(child->name, "NOT"))
	{
		// 倒置labelTrue和labelFalse
		translateCond(child->sibling, labelFalse, labelTrue);
	}
	else if (!strcmp(sibling->name, "RELOP"))
	{
		Operand *t1 = newTempVar();
		Operand *t2 = newTempVar();
		translateExp(child, t1);
		translateExp(child->sibling->sibling, t2);
		Operand *relop = newOperand(OP_RELOP, newString(sibling->value));

		// 如果t1和t2是地址，那么需要先读取地址
		if (t1->kind == OP_ADDRESS)
		{
			Operand *temp = newTempVar();
			generateInterCode(IR_READ_ADDR, 2, temp, t1);
			t1 = temp;
		}
		if (t2->kind == OP_ADDRESS)
		{
			Operand *temp = newTempVar();
			generateInterCode(IR_READ_ADDR, 2, temp, t2);
			t2 = temp;
		}
		generateInterCode(IR_IF_GOTO, 4, t1, relop, t2, labelTrue);
		generateInterCode(IR_GOTO, 1, labelFalse);
	}
	else if (!strcmp(sibling->name, "AND"))
	{
		Operand *label1 = newLabel();
		translateCond(child, label1, labelFalse);
		generateInterCode(IR_LABEL, 1, label1);
		translateCond(child->sibling->sibling, labelTrue, labelFalse);
	}
	else if (!strcmp(sibling->name, "OR"))
	{
		Operand *label1 = newLabel();
		translateCond(child, labelTrue, label1);
		generateInterCode(IR_LABEL, 1, label1);
		translateCond(child->sibling->sibling, labelTrue, labelFalse);
	}
	else
	{
		Operand *t1 = newTempVar();
		translateExp(node, t1);
		Operand *zero = newOperand(OP_CONSTANT, 0);
		Operand *neq = newOperand(OP_RELOP, newString("!="));

		// 如果t1是地址，那么需要先读取地址
		if (t1->kind == OP_ADDRESS)
		{
			Operand *temp = newTempVar();
			generateInterCode(IR_READ_ADDR, 2, temp, t1);
			t1 = temp;
		}
		generateInterCode(IR_IF_GOTO, 4, t1, neq, zero, labelTrue);
		generateInterCode(IR_GOTO, 1, labelFalse);
	}
}
