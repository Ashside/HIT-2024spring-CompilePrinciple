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
		if (i = val & ~0x3fff)
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
void freeSymbolTable()
{
	// TODO: freeSymbolTable
}

void initSymbolTable()
{
	// TODO: initSymbolTable
}
void startSemantic(NodePtr node)
{
	assert(node != NULL); // 保证node不为空

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

	startSemantic(node->child);
	startSemantic(node->sibling);
}
// 类型相关函数
TypePtr createType(Kind kind, ...)
{
	TypePtr type = (TypePtr)malloc(sizeof(Type));

	type->kind = kind;
	va_list argList;
	switch (kind)
	{
	case BASIC_KIND:
		va_start(argList, 1);
		type->u.basic = va_arg(argList, BasicTypeEnum);
		break;
	case ARRAY_KIND:
		va_start(argList, 2);
		type->u.array.elementType = va_arg(argList, TypePtr);
		type->u.array.size = va_arg(argList, int);
	case STRUCTURE_KIND:
		va_start(argList, 2);
		type->u.structure.structName = va_arg(argList, char *);
		type->u.structure.field = va_arg(argList, FieldListPtr);
		break;
	case FUNCTION_KIND:
		va_start(argList, 3);
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
	else
	{
		assert(type1->kind == type2->kind);
		// assert(type1->kind == BASIC || type1->kind == ARRAY || type1->kind == STRUCTURE)
		switch (type1->kind)
		{
		case BASIC_KIND:
			return type1->u.basic == type2->u.basic;
		case ARRAY_KIND:
			return compareType(type1->u.array.elementType, type2->u.array.elementType);
		case STRUCTURE_KIND:
			return !strcmp(type1->u.structure.structName, type2->u.structure.structName);
		default:
			fprintf(stderr, "Traceback: compareType().\nInvalid type kind\n");
			break;
		}
	}
}
void printType(TypePtr type)
{
	if (type == NULL)
	{
		printf("NULL Type\n");
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
	assert(type != NULL);
	assert(type->kind == BASIC_KIND || type->kind == ARRAY_KIND || type->kind == STRUCTURE_KIND || type->kind == FUNCTION_KIND);
	if (type == NULL)
	{
		return;
	}
	FieldListPtr temp = NULL;
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
			type->u.structure.structName = NULL;
		}
		temp = type->u.structure.field;

		while (temp != NULL)
		{
			FieldListPtr next = temp->nextField;
			freeFieldList(temp);
			temp = next;
		}
		type->u.structure.field = NULL;
		break;
	case FUNCTION_KIND:
		freeType(type->u.function.retType);
		type->u.function.retType = NULL;
		temp = type->u.function.argv;
		while (temp != NULL)
		{
			FieldListPtr next = temp->nextField;
			freeFieldList(temp);
			temp = next;
		}
		type->u.function.argv = NULL;
		break;
	default:
		fprintf(stderr, "Traceback: freeType().\nInvalid type kind\n");
		break;
	}
	free(type);
}

FieldListPtr createFieldList(char *name, TypePtr type)
{

	FieldListPtr fieldList = (FieldListPtr)malloc(sizeof(FieldList));

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
			printf("Type: \n");
			printType(fieldList->type);
			fieldList = fieldList->nextField;
		}
		// printFieldList(fieldList->nextField);
	}
}

void freeFieldList(FieldListPtr fieldList)
{
	assert(fieldList != NULL);
	free(fieldList->name);
	while (fieldList != NULL)
	{
		freeFieldList(fieldList->nextField);
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
	item->nextSymbol = NULL;
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
		printf("FieldList: \n");
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
		ItemPtr temp = hashTable->table[i];
		while (temp != NULL)
		{
			ItemPtr next = temp->nextHashItem;
			freeItem(temp);
			temp = next;
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
	assert(table != NULL);

	freeHash(table->hashTable);
	table->hashTable = NULL;
	free(table);
}

ItemPtr searchTable(TablePtr table, char *name)
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
	ItemPtr itemInTable = searchTable(table, item->fieldList->name);
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
	// Specifier -> TYPE
	// Specifier -> StructSpecifier
	assert(node != NULL);

	if (!strcmp(node->child->name, "TYPE"))
	{
		if (!strcmp(node->child->value, "int"))
		{
			return createType(BASIC_KIND, INT_TYPE);
		}
		else if (!strcmp(node->child->value, "float"))
		{
			return createType(BASIC_KIND, FLOAT_TYPE);
		}
	}
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

	NodePtr idNode = node;

	// 找到最后一个ID节点
	while (idNode->child != NULL)
	{
		idNode = idNode->child;
	}

	ItemPtr item = createItem(createFieldList(idNode->value, NULL));

	// 判断类型
	if (!strcmp(idNode->child->name, "ID"))
	{
		// VarDec -> ID
		item->fieldList->type = spec;
	}
	else
	{
		// VarDec -> VarDec LB INT RB
		NodePtr varDecNode = idNode->child;
		TypePtr tempType = spec;
		// 找到最后一个VarDec节点
		while (varDecNode->sibling != NULL)
		{
			item->fieldList->type = createType(ARRAY_KIND, tempType, atoi(varDecNode->sibling->sibling->value));
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

	return item;
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

	NodePtr decNode = node;
	while (decNode != NULL)
	{
		Dec(decNode->child, spec);
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

	ItemPtr item = VarDec(node->child, spec);
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
		return createType(BASIC_KIND, INT_TYPE);
	}
	else if (!strcmp(expNode->name, "FLOAT"))
	{
		return createType(BASIC_KIND, FLOAT_TYPE);
	}
	else if (!strcmp(expNode->name, "ID"))
	{
		ItemPtr item = searchTable(RootTable, expNode->value);
		if (item == NULL)
		{
			reportError(NOT_DEFINED_VARIABLE, node->line, "Not defined variable");
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
		if (!strcmp(expNode->sibling->name,"LB")){
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
		else if (!strcmp(expNode->sibling->name,"DOT")){
			// Exp -> Exp DOT ID
			TypePtr type = Exp(expNode);
			if (type == NULL || type->kind != STRUCTURE_KIND)
			{
				reportError(NOT_STRUCT, node->line, "Not struct variable");
				return NULL;
			}
			FieldListPtr field = type->u.structure.field;
			while (field != NULL)
			{
				if (!strcmp(field->name, expNode->sibling->sibling->value))
				{
					return field->type;
				}
				field = field->nextField;
			}
			reportError(NOT_DEFINED_SCOPE, node->line, "Not defined scope");
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
			if (!compareType(leftType, rightType))
			{
				reportError(MISMATCHED_OPERAND, node->line, "Mismatched operand");
				return NULL;
			}
			return leftType;
		}
	}
}