#include "Semantic.h"
#include "SyntaxTree.h"
#include "TokenTypeEnum.h"
#include "SemanTypeEnum.h"

TablePtr RootTable = NULL;
FieldListPtr RootFieldList = NULL;

// 功能函数
unsigned int getHashCode(char *name)
{
	unsigned int val = 0, i;
	for (; *name; ++name)
	{
		val = (val << 2) + *name;
		if ((i = val & ~0x3fff) != 0)
		{
			val = (val ^ (i >> 12)) & HASH_TABLE_SIZE;
		}
	}
	return val;
}

void reportError(ErrorTypeEnum errorType, int line, const char *msg)
{
	printf("Error type %d at Line %d: %s\n", errorType, line, msg);
}
void freeSymbolTable(TablePtr table)
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Free symbol table\n");
#endif
	if (table == NULL)
	{
		return;
	}
	freeTable(table);
}
void initSymbolTable()
{
	RootTable = createTable();
#ifdef DEBUG_SEMANTIC_ANALYSIS
	assert(RootTable != NULL);
	printf("Symbol table initialized\n");
#endif
}
void startSemantic(NodePtr node)
{
	if (node == NULL)
	{
		return;
	}
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Semantic analysing node: %s\n", node->name);
#endif
	// 共有三个入口，其中前两个完成所有变量的声明以及部分表达式的分析，第三个完成声明之外的表达式分析
	if (!strcmp(node->name, "ExtDef"))
	{
		ExtDef(node);
	}
	if (!strcmp(node->name, "Def"))
	{
		Def(node);
	}
	if (!strcmp(node->name, "Exp"))
	{
		Exp(node);
	}
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Semantic analysing node: %s finished\n", node->name);
#endif
	// 递归分析
	startSemantic(node->child);
	startSemantic(node->sibling);
}
// 类型相关函数
TypePtr createType(Kind kind, int argNum, ...)
{
	TypePtr type = (TypePtr)malloc(sizeof(Type));

	type->kind = kind;
	va_list argList;
	// 根据kind读入参数，初始化type
	switch (kind)
	{
	case BASIC_KIND:
		va_start(argList, argNum);
		type->u.basic = va_arg(argList, BasicTypeEnum);
		break;
	case ARRAY_KIND:
		va_start(argList, argNum);
		// 数组类型
		type->u.array.elementType = va_arg(argList, TypePtr);
		type->u.array.size = va_arg(argList, int);
	case STRUCTURE_KIND:
		va_start(argList, argNum);
		// 结构体类型
		type->u.structure.structName = va_arg(argList, char *);
		type->u.structure.field = va_arg(argList, FieldListPtr);
		break;
	case FUNCTION_KIND:
		va_start(argList, argNum);
		// 函数类型
		type->u.function.argc = va_arg(argList, int);
		type->u.function.argv = va_arg(argList, FieldListPtr);
		type->u.function.retType = va_arg(argList, TypePtr);
		break;
	default:
		break;
	}
	va_end(argList);
	return type;
}

int compareType(TypePtr type1, TypePtr type2)
{
	if (type1 == NULL || type2 == NULL)
	{
		return TRUE;
	}

	if (type1->kind == FUNCTION_KIND || type2->kind == FUNCTION_KIND)
	{
		return FALSE;
	}

	if (type1->kind != type2->kind)
	{
		return FALSE;
	}
	if (type1->kind == type2->kind)
	{
		switch (type1->kind)
		{
		case BASIC_KIND:
			if (type1->u.basic == type2->u.basic)
			{
				return TRUE;
			}
			break;
		default:
			fprintf(stderr, "Traceback: compareType().\nInvalid type kind %d\n", type1->kind);
			break;
		}
	}
	return FALSE;
}
void printType(TypePtr type)
{
	if (type == NULL)
	{
		printf("Type: NULL\n");
	}
	else
	{
		printf("Type: %d\n", type->kind);
		switch (type->kind)
		{
		case BASIC_KIND:
			printf("Basic Type: %d\n", type->u.basic);
			break;
		case ARRAY_KIND:
			printf("Array Type: \n");
			printType(type->u.array.elementType);
			printf("Array Size: %d\n", type->u.array.size);
			break;
		case STRUCTURE_KIND:
			printf("Structure Type: %s\n", type->u.structure.structName);
			printFieldList(type->u.structure.field);
			break;
		case FUNCTION_KIND:
			printf("Function Type: \n");
			printf("Argc: %d\n", type->u.function.argc);
			printFieldList(type->u.function.argv);
			printf("Return Type: \n");
			printType(type->u.function.retType);
			break;
		}
	}
}

void freeType(TypePtr type)
{
	if (type == NULL)
	{
		return;
	}
	FieldListPtr nextToFree = NULL;
	switch (type->kind)
	{
	case BASIC_KIND:
		break;
	case ARRAY_KIND:
		freeType(type->u.array.elementType);
		type->u.array.elementType = NULL;
		break;
	case STRUCTURE_KIND:
		if (type->u.structure.structName != NULL)
		{
			free(type->u.structure.structName);
		}
		type->u.structure.structName = NULL;
		nextToFree = type->u.structure.field;

		while (nextToFree != NULL)
		{
			FieldListPtr cur = nextToFree;
			nextToFree = nextToFree->nextField;
			freeFieldList(cur);
		}
		type->u.structure.field = NULL;
		break;
	case FUNCTION_KIND:
		freeType(type->u.function.retType);
		type->u.function.retType = NULL;
		nextToFree = type->u.function.argv;
		while (nextToFree != NULL)
		{
			FieldListPtr cur = nextToFree;
			nextToFree = nextToFree->nextField;
			freeFieldList(cur);
		}
		type->u.function.argv = NULL;
		break;
	default:
		fprintf(stderr, "Traceback: freeType(). Invalid type kind %d\n", type->kind);
		break;
	}
}

FieldListPtr createFieldList(char *name, TypePtr type)
{

	FieldListPtr fieldList = (FieldListPtr)malloc(sizeof(FieldList));

	// 复制字符串
	int len = strlen(name) + 1; // 包括'\0'
	fieldList->name = (char *)malloc(len * sizeof(char));
	strncpy(fieldList->name, name, len);

	fieldList->type = type;
	fieldList->nextField = NULL;

	return fieldList;
}

// 域相关函数
void printFieldList(FieldListPtr fieldList)
{
	if (fieldList == NULL)
	{
		printf("NULL FieldList\n");
	}
	else
	{
		while (fieldList != NULL)
		{
			printf("FieldList: \n");
			printf("Name: %s\n", fieldList->name);
			printType(fieldList->type);
			fieldList = fieldList->nextField;
		}
		// printFieldList(fieldList->nextField);
	}
}

void freeFieldList(FieldListPtr fieldList)
{
	assert(fieldList != NULL);
	if (fieldList->name != NULL)
	{
		free(fieldList->name);
		fieldList->name = NULL;
	}
	if (fieldList->type != NULL)
	{
		freeType(fieldList->type);
	}
	fieldList->type = NULL;
	free(fieldList);
}

void setFieldName(FieldListPtr fieldList, char *newName)
{
	assert(fieldList != NULL && newName != NULL);

	if (fieldList->name != NULL)
	{
		free(fieldList->name);
		fieldList->name = NULL;
	}

	int len = strlen(newName) + 1; // 包括'\0'
	fieldList->name = (char *)malloc(len * sizeof(char));
	strncpy(fieldList->name, newName, len);
}

ItemPtr createItem(FieldListPtr fieldList)
{
	ItemPtr item = (ItemPtr)malloc(sizeof(TableItem));

	assert(fieldList != NULL);
	item->fieldList = fieldList;
	item->nextHashItem = NULL;

	return item;
}

void printItem(ItemPtr item)
{
	if (item == NULL)
	{
		printf("NULL Item\n");
	}
	else
	{
		printf("Item: \n");

		printFieldList(item->fieldList);
	}
}

void freeItem(ItemPtr item)
{
	assert(item != NULL);
	if (item->fieldList != NULL)
	{
		freeFieldList(item->fieldList);
	}
	item->fieldList = NULL;
	free(item);
}

HashTablePtr createHash()
{
	HashTablePtr hashTable = (HashTablePtr)malloc(sizeof(HashTable));

	hashTable->table = (ItemPtr *)malloc(HASH_TABLE_SIZE * sizeof(ItemPtr));

	for (int i = 0; i < HASH_TABLE_SIZE; i++)
	{
		hashTable->table[i] = NULL;
	}

	return hashTable;
}

void freeHash(HashTablePtr hashTable)
{
	assert(hashTable != NULL);

	for (int i = 0; i < HASH_TABLE_SIZE; i++)
	{
		ItemPtr next = hashTable->table[i];
		while (next != NULL)
		{
			ItemPtr cur = next;
			next = next->nextHashItem;
			freeItem(cur);
		}
		hashTable->table[i] = NULL;
	}
	free(hashTable->table);
	hashTable->table = NULL;
	free(hashTable);
}

ItemPtr getHashHeadItem(HashTablePtr hashTable, int index)
{
	assert(hashTable != NULL);
	assert(index >= 0 && index < HASH_TABLE_SIZE);

	return hashTable->table[index];
}

void setHashHeadItem(HashTablePtr hashTable, int index, ItemPtr newItemValue)
{
	assert(hashTable != NULL);
	assert(index >= 0 && index < HASH_TABLE_SIZE);

	hashTable->table[index] = newItemValue;
}

TablePtr createTable()
{
	TablePtr table = (TablePtr)malloc(sizeof(Table));

	table->hashTable = createHash();

	return table;
}

void freeTable(TablePtr table)
{
	if (table == NULL)
	{
		return;
	}

	freeHash(table->hashTable);
	table->hashTable = NULL;
	free(table);
}

ItemPtr findItemByName(TablePtr table, char *name)
{
	unsigned int index = getHashCode(name);
	ItemPtr headHashItem = getHashHeadItem(table->hashTable, index);
	if (headHashItem == NULL)
	{
		return NULL;
	}
	while (headHashItem != NULL)
	{
		if (strcmp(headHashItem->fieldList->name, name) == 0)
		{
			return headHashItem;
		}
		headHashItem = headHashItem->nextHashItem;
	}
	return NULL;
}

int isConflict(TablePtr table, ItemPtr item)
{
	ItemPtr itemInTable = findItemByName(table, item->fieldList->name);
	if (itemInTable == NULL)
	{
		return FALSE;
	}
	else
	{
		// TODO: 弃用结构体域和函数参数的比较
		return TRUE;
	}
}

void insertTable(TablePtr table, ItemPtr item)
{
	assert(table != NULL && item != NULL);
	unsigned int index = getHashCode(item->fieldList->name);
	ItemPtr headHashItem = getHashHeadItem(table->hashTable, index);
	if (headHashItem == NULL)
	{
		setHashHeadItem(table->hashTable, index, item);
	}
	else
	{
		item->nextHashItem = headHashItem;
		setHashHeadItem(table->hashTable, index, item);
	}
}

void deleteTableItem(TablePtr table, ItemPtr item)
{
	assert(table != NULL && item != NULL);
	unsigned int index = getHashCode(item->fieldList->name);
	ItemPtr headHashItem = getHashHeadItem(table->hashTable, index);

	if (headHashItem == NULL)
	{
		return;
	}
	else if (headHashItem == item)
	{
		setHashHeadItem(table->hashTable, index, item->nextHashItem);
	}
	else
	{
		ItemPtr temp = headHashItem;
		while (temp->nextHashItem != item)
		{
			temp = temp->nextHashItem;
		}
		temp->nextHashItem = item->nextHashItem;
	}
	freeItem(item);
}

void printTable(TablePtr table)
{
	assert(table != NULL);
	for (int i = 0; i < HASH_TABLE_SIZE; i++)
	{
		ItemPtr temp = getHashHeadItem(table->hashTable, i);
		while (temp != NULL)
		{
			printItem(temp);
			temp = temp->nextHashItem;
		}
	}
}

void Program()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Program\n");
#endif
}

void ExtDefList()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("ExtDefList\n");
#endif
}

void StructSpecifier()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("StructSpecifier\n");
#endif
}

void FunDec()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("FunDec\n");
#endif
}

void VarList()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("VarList\n");
#endif
}

void ParamDec()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("ParamDec\n");
#endif
}

void CompSt()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("CompSt\n");
#endif
}

void StmtList()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("StmtList\n");
#endif
}

void Stmt()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Stmt\n");
#endif
}

void DefList()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("DefList\n");
#endif
}

void Args()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Args\n");
#endif
}

void ExtDef(NodePtr node)
{
	// ExtDef -> Specifier ExtDecList SEMI
	// ExtDef -> Specifier SEMI
	// ExtDef -> Specifier FunDec CompSt
	assert(node != NULL);

	TypePtr spec = Specifier(node->child);

	if (!strcmp(node->child->sibling->name, "ExtDecList"))
	{
		ExtDecList(node->child->sibling, spec);
	}
}

TypePtr Specifier(NodePtr node)
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Specifier: %s\n", node->child->value);
#endif
	// Specifier -> TYPE
	// Specifier -> StructSpecifier
	assert(node != NULL);

	if (!strcmp(node->child->name, "TYPE"))
	{
		if (!strcmp(node->child->value, "int"))
		{
			return createType(BASIC_KIND, 1, INT_TYPE);
		}
		else if (!strcmp(node->child->value, "float"))
		{
			return createType(BASIC_KIND, 1, FLOAT_TYPE);
		}
	}
	else
	{
		// Specifier -> StructSpecifier
		// 弃用结构体
		// return StructSpecifier(node->child);
	}
	return NULL;
}

void ExtDecList(NodePtr node, TypePtr type)
{
	// ExtDecList -> VarDec
	// ExtDecList -> VarDec COMMA ExtDecList
	assert(node != NULL);

	NodePtr varDecNode = node;
	while (varDecNode != NULL)
	{
		VarDec(varDecNode->child, type);
		// 如果有多个变量声明
		if (varDecNode->child->sibling != NULL)
		{
			// COMMA VarDec
			varDecNode = varDecNode->child->sibling->sibling;
		}
		else
		{
			break;
		}
	}
}

void VarDec(NodePtr node, TypePtr spec)
{
	// VarDec -> ID
	// VarDec -> VarDec LB INT RB
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("VarDec: %s\n", node->child->name);
#endif
	NodePtr idNode = node;

	// 找到最后一个ID节点
	while (idNode->child != NULL)
	{
		idNode = idNode->child;
	}
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("VarDec: %s\n", node->child->name);
#endif

	ItemPtr item = createItem(createFieldList(idNode->value, NULL));

#ifdef DEBUG_SEMANTIC_ANALYSIS
	printItem(item);
#endif
	// 判断类型
	if (!strcmp(node->child->name, "ID"))
	{
// VarDec -> ID
#ifdef DEBUG_SEMANTIC_ANALYSIS
		printf("VarDec -> ID\n");
#endif
		item->fieldList->type = spec;
	}
	else
	{
// VarDec -> VarDec LB INT RB
#ifdef DEBUG_SEMANTIC_ANALYSIS
		printf("VarDec -> VarDec LB INT RB\n");
#endif

		NodePtr varDecNode = node->child;
		TypePtr tempType = spec;
		// 找到最后一个VarDec节点
		while (varDecNode->sibling != NULL)
		{
			item->fieldList->type = createType(ARRAY_KIND, 2, tempType, atoi(varDecNode->sibling->sibling->value));
			tempType = item->fieldList->type; // TODO: 有问题
			varDecNode = varDecNode->child;
		}
	}

	if (isConflict(RootTable, item))
	{
		reportError(RE_DEFINED_VARIABLE, node->line, "Redefined variable");
	}
	else
	{
		insertTable(RootTable, item);
	}

	return;
}

void Def(NodePtr node)
{
	// Def -> Specifier DecList SEMI
	assert(node != NULL);

	TypePtr spec = Specifier(node->child);

	DecList(node->child->sibling, spec);

	if (spec != NULL)
	{
		freeType(spec);
	}
}

void DecList(NodePtr node, TypePtr spec)
{
	// DecList -> Dec
	// DecList -> Dec COMMA DecList
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("DecList: %s\n", node->child->name);
#endif

	NodePtr decNode = node;
	while (decNode != NULL)
	{
		Dec(decNode->child, spec);
#ifdef DEBUG_SEMANTIC_ANALYSIS
		printf("Dec finished\n");
#endif
		if (decNode->child->sibling != NULL)
		{
			// COMMA Dec
			decNode = decNode->child->sibling->sibling;
		}
		else
		{
			break;
		}
	}
}

void Dec(NodePtr node, TypePtr spec)
{
	// Dec -> VarDec
	// Dec -> VarDec ASSIGNOP Exp
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Dec: %s\n", node->child->name);
#endif

	VarDec(node->child, spec);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("VarDec finished\n");
#endif
}

TypePtr Exp(NodePtr node)
{
	// Exp -> Exp ASSIGNOP Exp
	// Exp -> Exp AND Exp
	// Exp -> Exp OR Exp
	// Exp -> Exp RELOP Exp
	// Exp -> Exp PLUS Exp
	// Exp -> Exp MINUS Exp
	// Exp -> Exp STAR Exp
	// Exp -> Exp DIV Exp
	// Exp -> LP Exp RP
	// Exp -> MINUS Exp
	// Exp -> NOT Exp
	// Exp -> ID LP Args RP
	// Exp -> ID LP RP
	// Exp -> Exp LB Exp RB
	// Exp -> Exp DOT ID
	// Exp -> ID
	// Exp -> INT
	// Exp -> FLOAT

	assert(node != NULL);

	NodePtr expNode = node->child;

	// Exp -> ID
	// Exp -> INT
	// Exp -> FLOAT
	if (!strcmp(expNode->name, "INT"))
	{
		return createType(BASIC_KIND, 1, INT_TYPE);
	}
	else if (!strcmp(expNode->name, "FLOAT"))
	{
		return createType(BASIC_KIND, 1, FLOAT_TYPE);
	}
	else if (!strcmp(expNode->name, "ID"))
	{
		ItemPtr item = findItemByName(RootTable, expNode->value);
		if (item == NULL)
		{
			reportError(NOT_DEFINED_VARIABLE, node->line, "Undefined variable");
			// 添加进符号表，防止重复报错
			insertTable(RootTable, createItem(createFieldList(expNode->value, NULL)));
			return NULL;
		}
		else
		{
			return item->fieldList->type;
		}
	}
	// Exp 运算符 Exp
	// Exp -> Exp LB Exp RB
	// Exp -> Exp DOT ID
	else if (!strcmp(expNode->name, "Exp"))
	{
		if (!strcmp(expNode->sibling->name, "LB"))
		{
			// Exp -> Exp LB Exp RB
			TypePtr leftType = Exp(expNode);
			TypePtr rightType = Exp(expNode->sibling->sibling);
			if (leftType == NULL || rightType == NULL)
			{
				return NULL;
			}
			// 错误类型 NOT_ARRAY, NOT_INTEGER_INDEX
			if (leftType->kind != ARRAY_KIND)
			{
				reportError(NOT_ARRAY, node->line, "Not array variable");
				return NULL;
			}
			if (rightType->kind != BASIC_KIND || rightType->u.basic != INT_TYPE)
			{
				reportError(NOT_INTEGER_INDEX, node->line, "Not integer index");
				return NULL;
			}
		}
		//
		else if (!strcmp(expNode->sibling->name, "DOT"))
		{
			// Exp -> Exp DOT ID
			// 弃用结构体域的比较
			TypePtr leftType = Exp(expNode);
			if (leftType == NULL)
			{
				return NULL;
			}
			// printf("Exp -> Exp DOT ID\n");
		}
		else
		{
			// Exp 运算符 Exp
			TypePtr leftType = Exp(expNode);
			TypePtr rightType = Exp(expNode->sibling->sibling);
			if (leftType == NULL || rightType == NULL)
			{
				return NULL;
			}
			// Exp -> Exp ASSIGNOP Exp
			if (!strcmp(expNode->sibling->name, "ASSIGNOP"))
			{
				// Exp -> INT ASSIGNOP Exp
				// Exp -> FLOAT ASSIGNOP Exp
				if (!strcmp(expNode->child->name, "INT") || !strcmp(expNode->child->name, "FLOAT"))
				{
					reportError(NOT_LEFT_ASSIGNMENT, node->line, "The left-hand side of an assignment must be a variable.");
				}
				// Exp -> ID ASSIGNOP Exp
				else if (!strcmp(expNode->child->name, "ID"))
				{
					if (!compareType(leftType, rightType))
					{
						reportError(MISMATCHED_ASSIGNMENT, node->line, "Type mismatched for assignment.");
					}
				}
			}
			// Exp -> Exp 运算符 Exp
			else
			{
				// 直接比较类型
				if (!compareType(leftType, rightType))
				{
					reportError(MISMATCHED_OPERAND, node->line, "Type mismatched for operands.");
				}
			}
		}
	}
	return NULL;
}