#include "InterCode.h"
#include "SyntaxTree.h"
#include "InterCodeEnum.h"
#include "Semantic.h"

InterCodeList *interCodeList;

Operand *newOperand(OperandEnum kind, ...)
{
	// 如果是OP_CONSTANT，那么value是int类型
	// 否则value是char *类型
	// 使用va_list来处理可变参数，此处可以不指明参数个数
	// 实际上相当于void*
#ifdef DEBUG_INTER_CODE_GENERATION
	printf("newOperand Kind: %d\n", kind);
#endif
	Operand *op = (Operand *)malloc(sizeof(Operand));
	op->kind = kind;
	va_list ap;
	va_start(ap, kind);

	// 仅常数类型的操作数需要处理int类型的参数
	switch (kind)
	{
	case OP_CONSTANT:
		op->u.value = va_arg(ap, int);
#ifdef DEBUG_INTER_CODE_GENERATION
		printf("newOperand: %d\n", op->u.value);
#endif
		break;
	case OP_VARIABLE:
	case OP_ADDRESS:
	case OP_LABEL:
	case OP_FUNCTION:
	case OP_RELOP:
		op->u.name = va_arg(ap, char *);
#ifdef DEBUG_INTER_CODE_GENERATION
		printf("newOperand: %s\n", op->u.name);
#endif
		break;
	default:
#ifdef DEBUG_INTER_CODE_GENERATION
		printf("newOperand: default\n");
#endif
		break;
	}

	va_end(ap);
#ifdef DEBUG_INTER_CODE_GENERATION
	printf("end newOperand\n");
#endif
	return op;
}

/// 释放操作数
/// @param op 操作数
void freeOperand(Operand *op)
{
	if (op == NULL)
	{
		return;
	}
	free(op);
}

/// 设置操作数的值
/// @param op 操作数
/// @param kind 操作数类型
/// @param argNum 参数个数
/// @param ... 参数列表，如果是OP_CONSTANT，那么是int类型，否则是char *类型
void setOperand(Operand *op, OperandEnum kind, int argNum, ...)
{
#ifdef DEBUG_INTER_CODE_GENERATION
	printf("setOperand: %d\n", kind);
#endif

	va_list ap;
	op->kind = kind;
	switch (kind)
	{
	case OP_CONSTANT:
		va_start(ap, argNum);
		op->u.value = va_arg(ap, int);
		va_end(ap);
#ifdef DEBUG_INTER_CODE_GENERATION
		printf("setOperand: %d\n", op->u.value);
#endif
		break;
	case OP_VARIABLE:
	case OP_ADDRESS:
	case OP_LABEL:
	case OP_FUNCTION:
	case OP_RELOP:
		va_start(ap, argNum);
		op->u.name = va_arg(ap, char *);
		va_end(ap);
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

/// 打印操作数/
///
/// 根据操作数的类型，向文件中打印操作数的值，这里文件指针一般是stdout
/// @param file 文件指针
/// @param op 操作数
///
void printOperand(FILE *file, Operand *op)
{
	if (op == NULL)
	{
		return;
	}

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

/// 生成中间代码InterCode
/// @param kind 中间代码类型(InterCodeEnum)
/// @param argNum 参数个数
/// @param ... 参数列表，参照指导书中的顺序从左到右给出
InterCode *newInterCode(InterCodeEnum kind, int argNum, ...)
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

/// 释放中间代码
/// @param code 中间代码
void freeInterCode(InterCode *code)
{
	if (code == NULL)
	{
		return;
	}
	free(code);
}

/// 生成中间代码双向链表节点
/// @param code 中间代码
InterCodeSS *newInterCodeSS(InterCode *code)
{
	InterCodeSS *codeSS = (InterCodeSS *)malloc(sizeof(InterCodeSS));
	codeSS->code = *code;
	codeSS->prev = NULL;
	codeSS->next = NULL;
	return codeSS;
}

/// 释放中间代码双向链表节点
/// @param codeSS 中间代码双向链表节点
void freeInterCodeSS(InterCodeSS *codeSS)
{
	if (codeSS == NULL)
	{
		return;
	}
	freeInterCode(&codeSS->code);
	free(codeSS);
}

/// 打印中间代码节点
/// @param file 文件指针
/// @param codeSS 中间代码节点
void printInterCodeSS(FILE *file, InterCodeSS *codeSS)
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

/// 生成中间代码双向链表
/// @return 中间代码双向链表/
InterCodeList *newInterCodeList()
{
	InterCodeList *list = (InterCodeList *)malloc(sizeof(InterCodeList));
	list->head = NULL;
	list->cur = NULL;
	// 临时变量编号
	// TODO 临时变量编号的生成
	list->tempCnt = 1;
	list->labeCnt = 1;
	return list;
}

/// 释放中间代码双向链表
/// @param list 中间代码双向链表
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

/// 将一个中间代码节点插入到中间代码双向链表cur指针之后
/// @param list 中间代码双向链表
/// @param codeSS 中间代码节点
void addInterCodeSS(InterCodeList *list, InterCodeSS *codeSS)
{
	if (list == NULL || codeSS == NULL)
	{
		return;
	}
	// 如果链表为空，那么直接插入
	if (list->head == NULL)
	{
		list->head = codeSS;
		list->cur = codeSS;
	}
	else
	{
		// 将codeSS插入到list的游标cur之后
		list->cur->next = codeSS;
		codeSS->prev = list->cur;
		list->cur = codeSS;
	}
}

/// 打印中间代码双向链表
/// @param file 文件指针
/// @param list 中间代码双向链表
void printInterCodeList(FILE *file, InterCodeList *list)
{
	InterCodeSS *p = list->head;

	while (p != NULL)
	{
		printInterCodeSS(file, p);
		p = p->next;
	}
}

/// 在函数声明中单个参数的翻译，将参数处理为一个链表
/// @param op 作为参数的操作数
Argument *newArgument(Operand *op)
{
	Argument *arg = (Argument *)malloc(sizeof(Argument));
	arg->op = op;
	arg->next = NULL;
	return arg;
}

/// 释放参数
/// @param arg 参数
void freeArgument(Argument *arg)
{
	if (arg == NULL)
	{
		return;
	}
	freeOperand(arg->op);
	free(arg);
}

/// 生成参数链表
ArgList *newArgList()
{
	ArgList *list = (ArgList *)malloc(sizeof(ArgList));
	list->head = NULL;
	list->cur = NULL;
	return list;
}

/// 释放参数链表
/// @param argList 参数链表
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

/// @brief 向参数链表中添加参数
/// @param argList
/// @param arg
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

/// @brief 新生成一个临时操作数变量，该变量的名字为t+编号
/// @return  返回一个临时操作数变量
Operand *newTempVar()
{
	Operand *op = newOperand(OP_VARIABLE, "t");
	op->u.name = (char *)malloc(sizeof(char) * 10);
	sprintf(op->u.name, "t%d", interCodeList->tempCnt);
	interCodeList->tempCnt++; // 临时变量编号加一
	return op;
}

/// @brief  新生成一个标签操作数变量，该变量的名字为label+编号
/// @return  返回一个标签操作数变量
Operand *newLabel()
{
	Operand *op = newOperand(OP_LABEL, "label");
	op->u.name = (char *)malloc(sizeof(char) * 10);
	sprintf(op->u.name, "label%d", interCodeList->labeCnt);
	interCodeList->labeCnt++;
	return op;
}

/// @brief 遍历并启动翻译
/// @param root
void travelTranslate(Node *root)
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
		travelTranslate(root->child);
		travelTranslate(root->sibling);
	}
}

// 生成中间代码
// 参数顺序：kind, argNum, arg1, arg2, arg3, arg4
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
		codeSS = newInterCodeSS(newInterCode(kind, 1, op1));
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
			codeSS = newInterCodeSS(newInterCode(kind, 2, op1, op2));
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
		codeSS = newInterCodeSS(newInterCode(kind, 3, result, op1, op2));
		addInterCodeSS(interCodeList, codeSS);
	}
	else if (IS_DEC_INTERCODE(kind))
	{
		va_start(ap, argNum);
		op1 = va_arg(ap, Operand *);
		size = va_arg(ap, int);
		codeSS = newInterCodeSS(newInterCode(kind, 2, op1, size));
		addInterCodeSS(interCodeList, codeSS);
	}
	else if (IS_COND_JUMP_INTERCODE(kind))
	{
		va_start(ap, argNum);
		result = va_arg(ap, Operand *);
		relop = va_arg(ap, Operand *);
		op1 = va_arg(ap, Operand *);
		op2 = va_arg(ap, Operand *);
		codeSS = newInterCodeSS(newInterCode(kind, 4, result, relop, op1, op2));
		addInterCodeSS(interCodeList, codeSS);
	}
	else
	{
		va_start(ap, argNum);
	}
	va_end(ap);
}

/// @brief 由于没有全局变量的使用，直接解析函数定义
/// @param node
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

/// @brief 在产生式ExtDef -> Specifier FunDec CompSt中，解析函数定义
/// @param node
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

	// 对应进入解析函数
	if (!strcmp(funDec->name, "FunDec"))
	{
		translateFunDec(funDec);
		translateCompSt(compSt);
	}
}

/// @brief 解析函数定义
/// @param node
void translateFunDec(Node *node)
{
	// FunDec -> ID LP VarList RP
	// FunDec -> ID LP RP
	if (node == NULL)
	{
		return;
	}

	// 首先生成函数定义
	generateInterCode(IR_FUNCTION, 1, newOperand(OP_FUNCTION, newString(node->child->value)));

	// 如果有参数
	if (node->child->sibling->sibling != NULL)
	{
		// 有参数
		// 没必要单独翻译
		// translateVarList(node->child->sibling->sibling);

		// 已经存在符号表中
		ItemPtr item = searchTableItem(table, node->child->value);
		FieldListPtr field = item->field->type->u.function.argv;
		// 生成参数中间代码
		// 注意顺序问题，PARAM是从左到右，ARG是从右到左
		while (field != NULL)
		{
			// 生成参数中间代码
			generateInterCode(IR_PARAM, 1, newOperand(OP_VARIABLE, newString(field->name)));
			field = field->tail;
		}
	}
}
/// @brief 翻译CompSt，注意代码块中可能没有定义，即DefList为空
/// @param node
void translateCompSt(Node *node)
{
	// CompSt -> LC DefList StmtList RC
	// DefList -> e
	if (node == NULL)
	{
		return;
	}
	// 检查第二个孩子是否是DefList
	Node *secondChild = node->child->sibling;

	if (!strcmp(secondChild->name, "DefList"))
	{
		// 有定义
		translateDefList(secondChild);
		translateStmtList(secondChild->sibling);
	}
	else if (!strcmp(secondChild->name, "StmtList"))
	{
		translateStmtList(secondChild);
	}
}

/// @brief 翻译参数列表
/// @param node
/// @param argList
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

/// @brief 翻译定义列表
/// @param node
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

/// @brief 翻译定义
/// @param node
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

/// @brief 翻译声明列表
/// @param node
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

/// @brief 翻译声明，注意可能存在初始化赋值
/// @param node
void translateDec(Node *node)
{
	// Dec -> VarDec
	// Dec -> VarDec ASSIGN Exp
	if (node == NULL)
	{
		return;
	}
	Node *varDec = node->child;

	// 如果有赋值
	if (varDec->sibling != NULL)
	{
		Node *exp = varDec->sibling->sibling;
		// 变量定义并赋值
		Operand *varTemp = newTempVar();
		translateVarDec(varDec, varTemp);
		Operand *expTemp = newTempVar();
		translateExp(exp, expTemp);
		// 生成赋值中间代码
		generateInterCode(IR_ASSIGN, 2, varTemp, expTemp);
	}
	else
	{
		// 变量定义
		translateVarDec(varDec, NULL);
	}
}

/// @brief 变量定义 如果var不为NULL，那么生成赋值中间代码
/// @param node
/// @param var 传入的一个操作数，用于获取本次变量定义
void translateVarDec(Node *node, Operand *var)
{
	// VarDec -> ID
	// VarDec -> VarDec LB INT RB
	if (node == NULL)
	{
		return;
	}

	NodePtr idNode = node->child;

	// 可能是个普通变量，也可能是个数组名
	// 在这里不能区分，需要借助符号表
	if (!strcmp(idNode->name, "ID"))
	{
		ItemPtr idItem = searchTableItem(table, idNode->value);
		TypePtr idType = idItem->field->type;

		// 对于普通变量，直接生成中间代码
		if (idType->kind == BASIC)
		{
			if (var != NULL)
			{
				// 由于上层传来的是一个临时变量
				// 而此处的变量有自己的名字
				// 所以此处不需要生成临时变量
				interCodeList->tempCnt--; // 临时变量编号减一，因为不需要生成临时变量
				// 修改操作数的名字
				setOperand(var, OP_VARIABLE, 1, newString(idNode->value));
			}
		}
		// 对于数组，需要生成DEC中间代码
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

/// @brief 翻译语句序列
/// @param node
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

/// @brief 翻译语句
/// @param node
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
		// 生成条件中间代码
		translateCond(expNode, label1, label2);

		// 生成标签1
		generateInterCode(IR_LABEL, 1, label1);

		// 标签1对应的语句
		translateStmt(stmtNode);

		// 标签2对应的语句
		if (stmtNode->sibling == NULL)
		{
			generateInterCode(IR_LABEL, 1, label2);
		}
		else
		{
			// 如果有else
			Operand *label3 = newLabel();
			// 这样翻译是因为
			// 在前面不跳转到else对应的语句
			// 就要前往下一个代码块
			// 所以形如
			// goto label3
			// label2:
			// stmt
			// label3:
			// 离开代码块
			generateInterCode(IR_GOTO, 1, label3);
			generateInterCode(IR_LABEL, 1, label2);
			translateStmt(stmtNode->sibling->sibling);
			generateInterCode(IR_LABEL, 1, label3);
		}
	}
	else if (!strcmp(child->name, "WHILE"))
	{
		// WHILE LP Exp RP Stmt
		Operand *label1 = newLabel(); // 循环开始
		Operand *label2 = newLabel(); // 循环体
		Operand *label3 = newLabel(); // 循环结束
		NodePtr expNode = child->sibling->sibling;
		NodePtr stmtNode = expNode->sibling->sibling;
		generateInterCode(IR_LABEL, 1, label1); // 循环开始标志
		translateCond(expNode, label2, label3); // 注意传入的是label2和label3，检查exp是否为真，并跳转到label2或label3
		generateInterCode(IR_LABEL, 1, label2); // 循环体标志
		translateStmt(stmtNode);				// 翻译循环体
		generateInterCode(IR_GOTO, 1, label1);	// 跳转到循环开始
		generateInterCode(IR_LABEL, 1, label3); // 循环结束标志
	}
}
/// @brief 翻译表达式
/// @param node
/// @param var 上层传入一个操作数，用于获取本次表达式的值
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
		// 根据条件跳转决定var的值
		if (!strcmp(node->child->sibling->name, "AND") || !strcmp(node->child->sibling->name, "OR") ||
			!strcmp(node->child->sibling->name, "RELOP"))
		{
			Operand *label1 = newLabel();
			Operand *label2 = newLabel();
			Operand *true_num = newOperand(OP_CONSTANT, 1);
			Operand *false_num = newOperand(OP_CONSTANT, 0);
			generateInterCode(IR_ASSIGN, 2, var, false_num); // 先将var赋值为假
			translateCond(node, label1, label2);			 // 翻译条件，如果条件为真，跳转到label1，否则跳转到label2
			// label2是一个假标签，用于跳出条件判断
			// 在translateCond中，最后一步是生成跳转到label2的中间代码
			generateInterCode(IR_LABEL, 1, label1);			// 标签1
			generateInterCode(IR_ASSIGN, 2, var, true_num); // 赋值为真
			generateInterCode(IR_LABEL, 1, label2);			// 标签2
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
				Operand *t2 = newTempVar(); // 计算右侧表达式的值
				translateExp(node->child->sibling->sibling, t2);
				Operand *t1 = newTempVar(); // 接收右侧表达式的值
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
		generateInterCode(IR_LABEL, 1, label2);
	}
	else if (!strcmp(child->name, "LP"))
	{
		// LP Exp RP
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
					Argument *headArg = argList->head;
					Argument *lastArg = argList->cur;
					Argument *tempArg = headArg;
					// 倒序生成ARG中间代码
					// 双指针法
					while (tempArg != NULL)
					{
						if (headArg == lastArg)
						{
							generateInterCode(IR_ARG, 1, tempArg->op);
							break;
						}
						if (tempArg->next == lastArg)
						{
							generateInterCode(IR_ARG, 1, lastArg->op);
							lastArg = tempArg;
							tempArg = headArg;
						}
						else
						{
							tempArg = tempArg->next;
						}
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
			// 该变量有名字
			ItemPtr item = searchTableItem(table, child->value);
			if (item == NULL)
			{
				return;
			}
			interCodeList->tempCnt--; // 临时变量编号减一，因为不需要生成临时变量
			setOperand(var, OP_VARIABLE, 1, newString(child->value));
		}
	}
	else if (!strcmp(child->name, "INT"))
	{
		// INT
		interCodeList->tempCnt--; // 临时变量编号减一，因为不需要生成临时变量
		setOperand(var, OP_CONSTANT, 1, atoi(child->value));
	}
	else if (!strcmp(child->name, "FLOAT"))
	{
		// FLOAT
		// TODO 浮点数应该是不存在的
		fprintf(stderr, "FLOAT is not supported\n");
	}
}

/// @brief 翻译条件表达式，若为真跳转到labelTrue，否则跳转到labelFalse
/// @param node
/// @param labelTrue
/// @param labelFalse
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
		// 重新传入表达式
		translateCond(child->sibling, labelFalse, labelTrue);
	}
	else if (sibling != NULL)
	{
		if ((!strcmp(sibling->name, "RELOP")))
		{
			Operand *t1 = newTempVar();
			Operand *t2 = newTempVar();
			translateExp(child, t1);
			translateExp(child->sibling->sibling, t2);
			Operand *relop = newOperand(OP_RELOP, newString(sibling->value));

			// 此处涉及数值计算需要计算结果
			// 如果t1和t2是地址，那么需要先读取地址
			CALCULATE_ADDRESS(t1)
			CALCULATE_ADDRESS(t2)
			// 在relop处理中，如果条件为真，跳转到labelTrue，否则跳转到labelFalse
			generateInterCode(IR_IF_GOTO, 4, t1, relop, t2, labelTrue);
			generateInterCode(IR_GOTO, 1, labelFalse);
		}
		else if (!strcmp(sibling->name, "AND"))
		{
			Operand *label1 = newLabel();
			translateCond(child, label1, labelFalse);					   // 如果第一个条件为真，跳转到label1，再判断第二个条件
			generateInterCode(IR_LABEL, 1, label1);						   // 标签1
			translateCond(child->sibling->sibling, labelTrue, labelFalse); // 如果第二个条件为真，跳转到labelTrue，否则跳转到labelFalse
		}
		else if (!strcmp(sibling->name, "OR"))
		{
			Operand *label1 = newLabel();
			translateCond(child, labelTrue, label1);					   // 如果第一个条件为真，跳转到labelTrue，若为假，才跳转到标签判断第二个条件
			generateInterCode(IR_LABEL, 1, label1);						   // 标签1
			translateCond(child->sibling->sibling, labelTrue, labelFalse); // 如果第二个条件为真，跳转到labelTrue，否则跳转到labelFalse
		}
	}

	else // 检查条件真假并对应跳转，也是其他条件的基础
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
		// 如果t1不等于0，跳转到labelTrue，否则跳转到labelFalse
		generateInterCode(IR_IF_GOTO, 4, t1, neq, zero, labelTrue);
		// 无条件跳转到labelFalse
		generateInterCode(IR_GOTO, 1, labelFalse);
	}
}
