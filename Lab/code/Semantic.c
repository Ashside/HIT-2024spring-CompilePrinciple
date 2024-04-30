#include "Semantic.h"
#include "SyntaxTree.h"
#include "TokenTypeEnum.h"
#include "SemanTypeEnum.h"

TablePtr RootTable = NULL;
FieldListPtr RootFieldList = NULL;

// 功能函数
unsigned int getHashIndex(char *name)
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
int isStructItem(ItemPtr item)
{
	// 比较类型是否为结构体类型
	if (item == NULL)
	{
		return FALSE;
	}
	if (item->fieldList->type->kind != STRUCTURE_KIND)
	{
		return FALSE;
	}
	return TRUE;
}
int isFuncItem(ItemPtr item)
{
	if (item == NULL)
	{
		return FALSE;
	}
	if (item->fieldList->type->kind != FUNCTION_KIND)
	{
		return FALSE;
	}
	return TRUE;
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

	if (!strcmp(node->name, "ExtDef"))
	{
		ExtDef(node);
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
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Create type\n");
	printf("Kind: %d\n", kind);
#endif
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
		type->u.function.argvField = va_arg(argList, FieldListPtr);
		type->u.function.retType = va_arg(argList, TypePtr);
		break;
	default:
		fprintf(stderr, "Traceback: createType(). Invalid type kind %d\n", kind);
		break;
	}
	va_end(argList);
	return type;
}

int compareType(TypePtr type1, TypePtr type2)
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Compare type\n");
	printType(type1);
	printType(type2);
#endif
	if (type1 == NULL || type2 == NULL)
	{
		return TRUE;
	}
	// 函数类型不参与比较
	if (type1->kind == FUNCTION_KIND || type2->kind == FUNCTION_KIND)
	{
		return FALSE;
	}
	// 类型不同直接返回FALSE
	if (type1->kind != type2->kind)
	{
		return FALSE;
	}

	switch (type1->kind)
	{
	case BASIC_KIND:
		if (type1->u.basic == type2->u.basic)
		{
			return TRUE;
		}
		break;
	case ARRAY_KIND:
		return compareType(type1->u.array.elementType, type2->u.array.elementType);
	case STRUCTURE_KIND:
		return !strcmp(type1->u.structure.structName, type2->u.structure.structName);
	default:
		fprintf(stderr, "Traceback: compareType().\nInvalid type kind %d\n", type1->kind);

		break;
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
			printFieldList(type->u.function.argvField);
			printf("Return Type: \n");
			printType(type->u.function.retType);
			break;
		}
	}
}

void freeType(TypePtr type)
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Free type\n");
#endif
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
		nextToFree = type->u.function.argvField;
		while (nextToFree != NULL)
		{
			FieldListPtr cur = nextToFree;
			nextToFree = nextToFree->nextField;
			freeFieldList(cur);
		}
		type->u.function.argvField = NULL;
		break;
	default:
		fprintf(stderr, "Traceback: freeType(). Invalid type kind %d\n", type->kind);
		break;
	}
	free(type);
}

FieldListPtr createFieldList(char *name, TypePtr type)
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Create field list\n");
	printf("Name: %s\n", name);
	printType(type);
#endif

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
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Free field list\n");
	printf("FieldList: \n");
	printFieldList(fieldList);
#endif
	if (fieldList->name != NULL)
	{
		free(fieldList->name);
	}
	fieldList->name = NULL;
	if (fieldList->type != NULL)
	{
		freeType(fieldList->type);
	}
	fieldList->type = NULL;
	free(fieldList);
}

void setFieldName(FieldListPtr fieldList, char *name)
{
	assert(fieldList != NULL && name != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Set field name\n");
	printf("New name: %s\n", name);
#endif
	if (fieldList->name != NULL)
	{
		free(fieldList->name);
		fieldList->name = NULL;
	}

	int len = strlen(name) + 1; // 包括'\0'
	fieldList->name = (char *)malloc(len * sizeof(char));
	strncpy(fieldList->name, name, len);
}

ItemPtr createItem(FieldListPtr fieldList)
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Create item\n");
	printf("FieldList: \n");
	printFieldList(fieldList);
#endif
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
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Free item\n");
#endif
	if (item->fieldList != NULL)
	{
		freeFieldList(item->fieldList);
	}
	item->fieldList = NULL;
	free(item);
}

HashTablePtr createHash()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Create hash table\n");
#endif

	HashTablePtr hashTable = (HashTablePtr)malloc(sizeof(HashTable));

	hashTable->tableItems = (ItemPtr *)malloc(HASH_TABLE_SIZE * sizeof(ItemPtr));

	for (int i = 0; i < HASH_TABLE_SIZE; i++)
	{
		hashTable->tableItems[i] = NULL;
	}

	return hashTable;
}

void freeHash(HashTablePtr hashTable)
{
	assert(hashTable != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Free hash table\n");
#endif

	for (int i = 0; i < HASH_TABLE_SIZE; i++)
	{
		ItemPtr next = hashTable->tableItems[i];
		while (next != NULL)
		{
			ItemPtr cur = next;
			next = next->nextHashItem;
			freeItem(cur);
		}
		hashTable->tableItems[i] = NULL;
	}
	free(hashTable->tableItems);
	hashTable->tableItems = NULL;
	free(hashTable);
}

ItemPtr getHashHeadItem(HashTablePtr hashTable, int index)
{
	assert(hashTable != NULL);
	assert(index >= 0 && index < HASH_TABLE_SIZE);
#ifdef DEBUG_SEMANTIC_ANALYSIS

	printf("Get hash head item\n");
	printf("Index: %d\n", index);
	printItem(hashTable->tableItems[index]);
#endif
	return hashTable->tableItems[index];
}

void setHashHeadItem(HashTablePtr hashTable, int index, ItemPtr newItemValue)
{
	assert(hashTable != NULL);
	assert(index >= 0 && index < HASH_TABLE_SIZE);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Set hash head item\n");
	printf("Index: %d\n", index);
	printItem(newItemValue);
#endif
	hashTable->tableItems[index] = newItemValue;
}

TablePtr createTable()
{
#ifdef DEBUG_SEMANTIC_ANALYSIS

	printf("Create table\n");
#endif
	TablePtr table = (TablePtr)malloc(sizeof(Table));

	table->hashTable = createHash();
	table->structNum = 0;
	return table;
}

void freeTable(TablePtr table)
{
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Free table\n");
#endif
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
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Find item by name\n");
	printf("Name: %s\n", name);
#endif
	unsigned int index = getHashIndex(name);
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
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Is conflict\n");
#endif
	ItemPtr itemInTable = findItemByName(table, item->fieldList->name);
	if (itemInTable == NULL)
	{
		return FALSE;
	}
	while (itemInTable != NULL)
	{
		// 同名变量
		if (!strcmp(itemInTable->fieldList->name, item->fieldList->name))
		{
			// 对于结构体类型，只要名字相同就冲突
			if (itemInTable->fieldList->type != NULL && item->fieldList->type != NULL)
			{
				if (itemInTable->fieldList->type->kind == STRUCTURE_KIND && item->fieldList->type->kind == STRUCTURE_KIND)
				{
					return TRUE;
				}
			}
		}
		itemInTable = itemInTable->nextHashItem;
	}
	return FALSE;
}

void insertTableItem(TablePtr table, ItemPtr item)
{
	assert(table != NULL && item != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Insert table item\n");
	printItem(item);
#endif
	unsigned int index = getHashIndex(item->fieldList->name);
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
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Delete table item\n");
	printItem(item);
#endif
	unsigned int index = getHashIndex(item->fieldList->name);
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

TypePtr StructSpecifier(NodePtr node)
{
	// StructSpecifier -> STRUCT OptTag LC DefList RC
	// StructSpecifier -> STRUCT Tag
	// OptTag -> ID|e
	// Tag -> ID
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("StructSpecifier\n");
#endif
	NodePtr structNode = node->child;
	NodePtr tagNode = structNode->sibling;
	// 注意OptTag可能为空 先处理Tag
	if (!strcmp(tagNode->name, "Tag"))
	{
		// Tag -> ID
		// eg: struct A a;
		// 其中A是结构体名字 可能没有定义
		ItemPtr stcItem = findItemByName(RootTable, tagNode->child->value);
		// 如果未定义，报错
		if (stcItem == NULL || !isStructItem(stcItem))
		{
			reportError(NOT_DEFINED_STRUCT, node->line, "Undefined struct");
		}
		else
		{
			return createType(STRUCTURE_KIND, 2, stcItem->fieldList->name, stcItem->fieldList->type);
		}
	}
	// OptTag处理，可能不存在，需要判断匿名结构体
	else
	{
		ItemPtr stcItem = createItem(createFieldList("", createType(STRUCTURE_KIND, 2, NULL, NULL)));
		// OptTag -> ID
		if (!strcmp(tagNode->name, "OptTag"))
		{
			// OptTag -> ID
			// eg: struct A{int a; int b;} a;
			// 其中A是结构体名字
			// 读入结构体名字
			setFieldName(stcItem->fieldList, tagNode->child->value);
		}
		// OptTag -> e
		else
		{
			// OptTag -> e
			// eg: struct {int a; int b;} a;
			// 匿名结构体
			RootTable->structNum++;
			char *name = (char *)malloc(10 * sizeof(char));
			sprintf(name, "struct%d", RootTable->structNum);
			setFieldName(stcItem->fieldList, name);
		}
		// StructSpecifier -> STRUCT OptTag LC DefList RC
		if (!strcmp(tagNode->sibling->sibling->name, "DefList"))
		{
			// StructSpecifier -> STRUCT OptTag LC DefList RC
			// eg: struct A{int a; int b;}
			// 读入结构体域
			// 注意要求：全局变量，不允许重复定义
			DefList(tagNode->sibling->sibling, stcItem);
		}

		// 插入符号表
		if (isConflict(RootTable, stcItem))
		{
			reportError(RE_DEFINED_STRUCT, node->line, "Redefined struct");
			freeItem(stcItem);
			return NULL;
		}
		else
		{
			if (!strcmp(structNode->sibling->name, "OptTag"))
			{
				insertTableItem(RootTable, stcItem);
			}
			return createType(STRUCTURE_KIND, 2, stcItem->fieldList->name, stcItem->fieldList->type->u.structure.field);
		}
	}
	return NULL;
}

void FunDec(NodePtr node, TypePtr retType)
{
	// FunDec -> ID LP VarList RP
	// FunDec -> ID LP RP
	// eg: int a(int b, int c);
	// eg: int a();

	assert(node != NULL);

	NodePtr idNode = node->child;
	ItemPtr funcItem = createItem(createFieldList(idNode->value, createType(FUNCTION_KIND, 3, 0, NULL, retType)));
	// FunDec -> ID LP VarList RP
	if (!strcmp(idNode->sibling->sibling->name, "VarList"))
	{
		VarList(idNode->sibling->sibling, funcItem);
	}
	// 插入符号表
	if (isConflict(RootTable, funcItem))
	{
		reportError(RE_DEFINED_FUNCTION, node->line, "Redefined function");
		freeItem(funcItem);
	}
	else
	{
		insertTableItem(RootTable, funcItem);
	}
}

void VarList(NodePtr node, ItemPtr funcItem)
{
	// VarList -> ParamDec COMMA VarList
	// VarList -> ParamDec
	// eg: int a, int b, int c
	// eg: int a
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS

	printf("VarList: %s\n", node->child->name);
#endif
	int argc = 0;
	FieldListPtr curFiled = NULL;
	NodePtr paramDecNode = node->child;

	// VarList -> ParamDec COMMA VarList
	FieldListPtr paramField = ParamDec(paramDecNode);
	funcItem->fieldList->type->u.function.argvField = paramField;
	curFiled = paramField;
	argc++;
	// 如果有多个参数
	while (paramDecNode->sibling != NULL)
	{
		// COMMA ParamDec
		paramDecNode = paramDecNode->sibling->sibling->child;
		paramField = ParamDec(paramDecNode);
		if (paramField != NULL)
		{
			curFiled->nextField = paramField;
			curFiled = paramField;
			argc++;
		}
	}
	funcItem->fieldList->type->u.function.argc = argc;
}

FieldListPtr ParamDec(NodePtr node)
{
	// ParamDec -> Specifier VarDec
	// eg: int a
	// eg: float b
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS

	printf("ParamDec: %s\n", node->child->name);
#endif
	TypePtr spec = Specifier(node->child);
	FieldListPtr field = NULL;
	ItemPtr varDecItem = NULL;
	if (spec != NULL)
	{
		varDecItem = VarDec(node->child->sibling, spec);
		if (varDecItem != NULL)
		{
			field = varDecItem->fieldList;
		}
	}
	return field;
}

void CompSt(NodePtr node, TypePtr retType)
{
	// CompSt -> LC DefList StmtList RC
	// eg: {int a; int b; int c;}
	// eg: {int a; int b; int c; return 0;}
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("CompSt: %s\n", node->child->name);
#endif
	// CompSt -> LC DefList StmtList RC
	// DefList -> Def DefList
	// StmtList -> Stmt StmtList
	NodePtr defListNode = node->child->sibling;
	NodePtr stmtListNode = defListNode->sibling;
	if (!strcmp(defListNode->name, "DefList"))
	{
		DefList(defListNode, NULL);
	}
	if (!strcmp(stmtListNode->name, "StmtList"))
	{
		StmtList(stmtListNode, retType);
	}
}

void StmtList(NodePtr node, TypePtr retType)
{
	// StmtList -> Stmt StmtList
	// StmtList -> e
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("StmtList: %s\n", node->child->name);
#endif
	NodePtr stmtListNode = node;
	while (stmtListNode != NULL)
	{
		Stmt(stmtListNode->child, retType);
		if (stmtListNode->child->sibling != NULL)
		{
			// Stmt StmtList
			stmtListNode = stmtListNode->child->sibling;
		}
		else
		{
			break;
		}
	}
}

void Stmt(NodePtr node, TypePtr retType)
{
	// Stmt -> Exp SEMI
	// Stmt -> CompSt
	// Stmt -> RETURN Exp SEMI
	// Stmt -> IF LP Exp RP Stmt
	// Stmt -> IF LP Exp RP Stmt ELSE Stmt
	// Stmt -> WHILE LP Exp RP Stmt
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Stmt: %s\n", node->child->name);
#endif
	NodePtr childNode = node->child;
	if (!strcmp(childNode->name, "Exp"))
	{
		// Stmt -> Exp SEMI
		Exp(childNode);
	}
	else if (!strcmp(childNode->name, "CompSt"))
	{
		// Stmt -> CompSt
		CompSt(childNode, retType);
	}
	else if (!strcmp(childNode->name, "RETURN"))
	{
		// Stmt -> RETURN Exp SEMI
		TypePtr expType = Exp(childNode->sibling);
		if (expType != NULL)
		{
			if (!compareType(expType, retType))
			{
				reportError(MISMATCHED_RETURN, node->line, "Type mismatched for return");
			}
		}
	}
	else if (!strcmp(childNode->name, "IF"))
	{
		// Stmt -> IF LP Exp RP Stmt
		// Stmt -> IF LP Exp RP Stmt ELSE Stmt
		Exp(childNode->sibling->sibling);
		Stmt(childNode->sibling->sibling->sibling->sibling, retType);
		// ELSE
		if (childNode->sibling->sibling->sibling->sibling->sibling != NULL)
		{
			// ELSE Stmt
			Stmt(childNode->sibling->sibling->sibling->sibling->sibling->sibling, retType);
		}
	}
	else if (!strcmp(childNode->name, "WHILE"))
	{
		// Stmt -> WHILE LP Exp RP Stmt
		Exp(childNode->sibling->sibling);
		Stmt(childNode->sibling->sibling->sibling->sibling, retType);
	}
}

void DefList(NodePtr node, ItemPtr stcItem)
{
	// DefList -> Def DefList
	// DefList -> e
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("DefList: %s\n", node->child->name);
#endif
	NodePtr defListNode = node;
	while (defListNode != NULL)
	{
		Def(defListNode->child, stcItem);
		if (defListNode->child->sibling != NULL)
		{
			// Def DefList
			defListNode = defListNode->child->sibling;
		}
		else
		{
			break;
		}
	}
}

void Args(NodePtr node, ItemPtr funcItem) {
	// Args -> Exp COMMA Args
	// Args -> Exp
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Args: %s\n", node->child->name);
#endif
	NodePtr argNode = node;
	FieldListPtr argFiled = funcItem->fieldList->type->u.function.argvField;
	while (argNode != NULL)
	{
		/* code */
		if (argFiled == NULL)
		{
			reportError(MISMATCHED_PARAMETER, node->line, "Too many arguments");
			break;
		}
		TypePtr expType = Exp(argNode->child);
		if (expType != NULL)
		{
			if (!compareType(expType, argFiled->type))
			{
				reportError(MISMATCHED_PARAMETER, node->line, "Type mismatched for arguments");
			}
		}
		argFiled = argFiled->nextField;
		if (argNode->child->sibling != NULL)
		{
			// Exp COMMA Args
			argNode = argNode->child->sibling->sibling;
		}
		else
		{
			break;
		}


	}
	if (argFiled != NULL)
	{
		reportError(MISMATCHED_PARAMETER, node->line, "Too few arguments");
	}
	
}

void ExtDef(NodePtr node)
{
	// ExtDef -> Specifier ExtDecList SEMI
	// ExtDef -> Specifier SEMI
	// ExtDef -> Specifier FunDec CompSt
	// eg: int a, b, c;
	// eg: struct A{int a; int b;}; struct A;
	// eg: int a(){};
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS

	printf("ExtDef: %s\n", node->child->name);
#endif
	TypePtr spec = Specifier(node->child);

	if (!strcmp(node->child->sibling->name, "ExtDecList"))
	{
		ExtDecList(node->child->sibling, spec);
	}
	else if (!strcmp(node->child->sibling->name, "FunDec"))
	{
		// ExtDef -> Specifier FunDec CompSt
		FunDec(node->child->sibling, spec);
		CompSt(node->child->sibling->sibling, spec);
	}

	if (spec != NULL)
	{
		freeType(spec);
	}
}

TypePtr Specifier(NodePtr node)
{
	// Specifier -> TYPE
	// Specifier -> StructSpecifier
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Specifier: %s\n", node->child->name);
#endif
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
	// Specifier -> StructSpecifier
	else
	{
		return StructSpecifier(node->child);
	}
	return NULL;
}

void ExtDecList(NodePtr node, TypePtr type)
{
	// ExtDecList -> VarDec
	// ExtDecList -> VarDec COMMA ExtDecList
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("ExtDecList: %s\n", node->child->name);
#endif
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

ItemPtr VarDec(NodePtr node, TypePtr spec)
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
	// VarDec -> ID
	if (!strcmp(node->child->name, "ID"))
	{
#ifdef DEBUG_SEMANTIC_ANALYSIS
		printf("VarDec -> ID\n");
#endif
		item->fieldList->type = spec;
	}
	// VarDec -> VarDec LB INT RB
	else
	{
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
		freeItem(item);
	}
	else
	{
		insertTableItem(RootTable, item);
		return item;
	}

	return NULL;
}

void Def(NodePtr node, ItemPtr stcItem)
{
	// Def -> Specifier DecList SEMI
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Def: %s\n", node->child->name);
#endif

	TypePtr spec = Specifier(node->child);

	DecList(node->child->sibling, spec, stcItem);

	if (spec != NULL)
	{
		freeType(spec);
	}
}

void DecList(NodePtr node, TypePtr spec, ItemPtr stcItem)

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
		Dec(decNode->child, spec, stcItem);
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

void Dec(NodePtr node, TypePtr spec, ItemPtr stcItem)
{
	// Dec -> VarDec
	// Dec -> VarDec ASSIGNOP Exp
	assert(node != NULL);
#ifdef DEBUG_SEMANTIC_ANALYSIS
	printf("Dec: %s\n", node->child->name);
#endif

	NodePtr decNode = node;
	// Dec -> VarDec
	// 此时不涉及赋值，检查类型
	if (decNode->child->sibling == NULL)
	{
		// 如果此时不用处理结构体
		if (stcItem == NULL)
		{
			VarDec(decNode->child, spec);
		}
		// 处理结构体域
		else
		{
			// Dec -> VarDec
			// eg: struct A a;
			// eg: struct A a[10];
			ItemPtr varDecItem = VarDec(decNode->child, spec);
			FieldListPtr VarDecField = varDecItem->fieldList;
			FieldListPtr stcFiled = stcItem->fieldList->type->u.structure.field;
			FieldListPtr curField = NULL;
			while (stcFiled != NULL)
			{
				if (!strcmp(stcFiled->name, VarDecField->name))
				{
					reportError(RE_DEFINED_SCOPE, node->line, "Redefined field");
				}
				else
				{
					curField = stcFiled;
					stcFiled = stcFiled->nextField;
				}
			}
			if (curField == NULL)
			{
				stcFiled->nextField->type->u.structure.field = varDecItem->fieldList;
			}
			else
			{
				curField->nextField = varDecItem->fieldList;
			}
			freeItem(varDecItem);
		}
	}
	// Dec -> VarDec ASSIGNOP Exp
	// 处理赋值
	else
	{
		// 结构体不允许赋值
		if (stcItem != NULL)
		{
			reportError(RE_DEFINED_SCOPE, node->line, "Illegal assignment");
		}
		else
		{
			ItemPtr varDecItem = VarDec(decNode->child, spec);
			TypePtr expType = Exp(decNode->child->sibling->sibling);

			if (!compareType(varDecItem->fieldList->type, expType))
			{
				reportError(MISMATCHED_ASSIGNMENT, node->line, "Type mismatched for assignment");
			}

			// 对数组赋值
			if (varDecItem->fieldList->type->kind == ARRAY_KIND && (varDecItem->fieldList->type != NULL))
			{
				reportError(MISMATCHED_ASSIGNMENT, node->line, "Illegal assignment");
			}
			freeItem(varDecItem);
		}
	}
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
	else if (!strcmp(expNode->name, "ID") && expNode->sibling == NULL)
	{
		ItemPtr item = findItemByName(RootTable, expNode->value);
		if (item == NULL || isStructItem(item))
		{
			reportError(NOT_DEFINED_VARIABLE, node->line, "Undefined variable");
			return NULL;
		}
		else
		{
			return item->fieldList->type;
		}
	}
	// Exp -> ID LP RP
	// Exp -> ID LP Args RP
	else if (!strcmp(expNode->name, "ID") && !strcmp(expNode->sibling->name, "LP"))
	{
		ItemPtr funcItem = findItemByName(RootTable, expNode->value);
		if (funcItem == NULL)
		{
			reportError(NOT_DEFINED_FUNCTION, node->line, "Undefined function");
			return NULL;
		}
		else if (!isFuncItem(funcItem))
		{
			reportError(NOT_FUNCTION, node->line, "Not a function");
		}

		if (!strcmp(expNode->sibling->sibling->name, "Args"))
		{
			Args(expNode->sibling->sibling, funcItem);
			return funcItem->fieldList->type->u.function.retType;
		}
		else
		{
			if (funcItem->fieldList->type->u.function.argc != 0)
			{
				reportError(MISMATCHED_PARAMETER, node->line, "Too few arguments to function");
			}
			return funcItem->fieldList->type->u.function.retType;
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
			return leftType->u.array.elementType;
		}
		//
		else if (!strcmp(expNode->sibling->name, "DOT"))
		{
			// Exp -> Exp DOT ID
			// eg: a.b
			TypePtr leftType = Exp(expNode);
			if (leftType == NULL)
			{
				return NULL;
			}
			if (leftType->kind != STRUCTURE_KIND)
			{
				reportError(NOT_STRUCT, node->line, "Not a struct variable");
				return NULL;
			}
			NodePtr idNode = expNode->sibling->sibling;
			FieldListPtr curField = leftType->u.structure.field; 
			while (curField != NULL) {
				if (!strcmp(curField->name, idNode->value)) {
					return curField->type;
				}
				curField = curField->nextField;
			}
			if (curField == NULL)
			{
				reportError(NOT_DEFINED_SCOPE, node->line, "Undefined field");
				return NULL;
			}
			
			return NULL;
			
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
				else if (!strcmp(expNode->child->name, "ID") || !strcmp(expNode->child->sibling->name, "LB") || !strcmp(expNode->child->sibling->name, "DOT"))
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
				if (leftType->kind != BASIC_KIND || rightType->kind != BASIC_KIND)
				{
					reportError(MISMATCHED_OPERAND, node->line, "Type mismatched for operands.");
				}
				if (!compareType(leftType, rightType))
				{
					reportError(MISMATCHED_OPERAND, node->line, "Type mismatched for operands.");
				}
			}
			return leftType;
		}
		
	}
	else if (!strcmp(expNode->name, "MINUS") || !strcmp(expNode->name, "NOT"))
	{
		// Exp -> MINUS Exp
		// Exp -> NOT Exp
		TypePtr expType = Exp(expNode->sibling);
		if (expType == NULL || expType->kind != BASIC_KIND)
		{
			reportError(MISMATCHED_OPERAND, node->line, "Type mismatched for operands.");
			return NULL;
		}
		return expType;
	}
	else if (!strcmp(expNode->name, "LP"))
	{
		// Exp -> LP Exp RP
		return Exp(expNode->sibling);
	}
	return NULL;
}