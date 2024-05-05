#include "Semantic.h"
#include "SemanTypeEnum.h"
TablePtr table;

unsigned int getHashCode(char* name) {
    unsigned int val = 0, i;
    for (; *name; ++name) {
        val = (val << 2) + *name;
        if ((i = val & ~HASH_TABLE_SIZE))
            val = (val ^ (i >> 12)) & HASH_TABLE_SIZE;
    }
    return val;
}

void reportError(ErrorType type, int line, char* msg) {
    printf("Error type %d at Line %d: %s\n", type, line, msg);
}
// Type functions
TypePtr newType(Kind kind,int argNum ,...)
{
    TypePtr p = (TypePtr)malloc(sizeof(Type));
    assert(p != NULL);
    p->kind = kind;
    va_list vaList;
    assert(kind == BASIC || kind == ARRAY || kind == STRUCTURE ||
           kind == FUNCTION);
    switch (kind)
    {
    case BASIC:
        va_start(vaList, argNum);
        p->u.basic = va_arg(vaList, BasicType);
        break;
    case ARRAY:
        va_start(vaList, argNum);
        p->u.array.elem = va_arg(vaList, TypePtr);
        p->u.array.size = va_arg(vaList, int);
        break;
    case STRUCTURE:
        va_start(vaList, argNum);
        p->u.structure.structName = va_arg(vaList, char *);
        p->u.structure.field = va_arg(vaList, FieldListPtr);
        break;
    case FUNCTION:
        va_start(vaList, argNum);
        p->u.function.argc = va_arg(vaList, int);
        p->u.function.argv = va_arg(vaList, FieldListPtr);
        p->u.function.returnType = va_arg(vaList, TypePtr);
        break;
    }
    va_end(vaList);
    return p;
}

TypePtr copyType(TypePtr src)
{
    if (src == NULL)
        return NULL;
    TypePtr p = (TypePtr)malloc(sizeof(Type));
    assert(p != NULL);
    p->kind = src->kind;
    assert(p->kind == BASIC || p->kind == ARRAY || p->kind == STRUCTURE ||
           p->kind == FUNCTION);
    switch (p->kind)
    {
    case BASIC:
        p->u.basic = src->u.basic;
        break;
    case ARRAY:
        p->u.array.elem = copyType(src->u.array.elem);
        p->u.array.size = src->u.array.size;
        break;
    case STRUCTURE:
        p->u.structure.structName = newString(src->u.structure.structName);
        p->u.structure.field = copyFieldList(src->u.structure.field);
        break;
    case FUNCTION:
        p->u.function.argc = src->u.function.argc;
        p->u.function.argv = copyFieldList(src->u.function.argv);
        p->u.function.returnType = copyType(src->u.function.returnType);
        break;
    }

    return p;
}

void deleteType(TypePtr type)
{
    assert(type != NULL);
    if (!(type->kind == BASIC || type->kind == ARRAY ||
          type->kind == STRUCTURE || type->kind == FUNCTION))
    {
        return;
    }
    FieldListPtr temp = NULL;
    switch (type->kind)
    {
    case BASIC:
        break;
    case ARRAY:
        deleteType(type->u.array.elem);
        type->u.array.elem = NULL;
        break;
    case STRUCTURE:
        if (type->u.structure.structName)
            free(type->u.structure.structName);
        type->u.structure.structName = NULL;

        temp = type->u.structure.field;
        while (temp)
        {
            FieldListPtr tDelete = temp;
            temp = temp->tail;
            deleteFieldList(tDelete);
        }
        type->u.structure.field = NULL;
        break;
    case FUNCTION:
        deleteType(type->u.function.returnType);
        type->u.function.returnType = NULL;
        temp = type->u.function.argv;
        while (temp)
        {
            FieldListPtr tDelete = temp;
            temp = temp->tail;
            deleteFieldList(tDelete);
        }
        type->u.function.argv = NULL;
        break;
    }
    free(type);
}

boolean checkType(TypePtr type1, TypePtr type2)
{
    if (type1 == NULL || type2 == NULL)
        return TRUE;
    if (type1->kind == FUNCTION || type2->kind == FUNCTION)
        return FALSE;
    if (type1->kind != type2->kind)
        return FALSE;
    else
    {
        assert(type1->kind == BASIC || type1->kind == ARRAY ||
               type1->kind == STRUCTURE);
        switch (type1->kind)
        {
        case BASIC:
            return type1->u.basic == type2->u.basic;
        case ARRAY:
            return checkType(type1->u.array.elem, type2->u.array.elem);
        case STRUCTURE:
            return !strcmp(type1->u.structure.structName,
                           type2->u.structure.structName);
        default:
            return FALSE;
        }
    }
}

// FieldList functions
FieldListPtr newFieldList(char *newName, TypePtr newType)
{
    FieldListPtr p = (FieldListPtr)malloc(sizeof(FieldList));
    assert(p != NULL);
    p->name = newString(newName);
    p->type = newType;
    p->tail = NULL;
    return p;
}

FieldListPtr copyFieldList(FieldListPtr src)
{
    assert(src != NULL);
    FieldListPtr head = NULL, cur = NULL;
    FieldListPtr temp = src;

    while (temp)
    {
        if (!head)
        {
            head = newFieldList(temp->name, copyType(temp->type));
            cur = head;
            temp = temp->tail;
        }
        else
        {
            cur->tail = newFieldList(temp->name, copyType(temp->type));
            cur = cur->tail;
            temp = temp->tail;
        }
    }
    return head;
}

void deleteFieldList(FieldListPtr fieldList)
{
    assert(fieldList != NULL);
    if (fieldList->name)
    {
        free(fieldList->name);
        fieldList->name = NULL;
    }
    if (fieldList->type)
        deleteType(fieldList->type);
    fieldList->type = NULL;
    free(fieldList);
}

void setFieldListName(FieldListPtr p, char *newName)
{
    assert(p != NULL && newName != NULL);
    if (p->name != NULL)
    {
        free(p->name);
    }
    // int length = strlen(newName) + 1;
    // p->name = (char*)malloc(sizeof(char) * length);
    // strncpy(p->name, newName, length);
    p->name = newString(newName);
}


// tableItem functions
ItemPtr newItem(int symbolDepth, FieldListPtr pfield)
{
    ItemPtr p = (ItemPtr)malloc(sizeof(TableItem));
    assert(p != NULL);
    p->symbolDepth = symbolDepth;
    p->field = pfield;
    p->nextHash = NULL;
    p->nextSymbol = NULL;
    return p;
}

void deleteItem(ItemPtr item)
{
    assert(item != NULL);
    if (item->field != NULL)
        deleteFieldList(item->field);
    free(item);
}

// Hash functions
HashPtr newHash()
{
    HashPtr p = (HashPtr)malloc(sizeof(HashTable));
    assert(p != NULL);
    p->hashArray = (ItemPtr *)malloc(sizeof(ItemPtr) * HASH_TABLE_SIZE);
    assert(p->hashArray != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        p->hashArray[i] = NULL;
    }
    return p;
}

void deleteHash(HashPtr hash)
{
    assert(hash != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        ItemPtr temp = hash->hashArray[i];
        while (temp)
        {
            ItemPtr tdelete = temp;
            temp = temp->nextHash;
            deleteItem(tdelete);
        }
        hash->hashArray[i] = NULL;
    }
    free(hash->hashArray);
    hash->hashArray = NULL;
    free(hash);
}

ItemPtr getHashHead(HashPtr hash, int index)
{
    assert(hash != NULL);
    return hash->hashArray[index];
}

void setHashHead(HashPtr hash, int index, ItemPtr newVal)
{
    assert(hash != NULL);
    hash->hashArray[index] = newVal;
}
// Table functions

TablePtr initTable()
{
    TablePtr table = (TablePtr)malloc(sizeof(Table));
    assert(table != NULL);
    table->hash = newHash();
    table->stack = newStack();
    table->unNamedStructNum = 0;
    return table;
};

void deleteTable(TablePtr table)
{
    deleteHash(table->hash);
    table->hash = NULL;
    deleteStack(table->stack);
    table->stack = NULL;
    free(table);
};

ItemPtr searchTableItem(TablePtr table, char *name)
{
    unsigned hashCode = getHashCode(name);
    ItemPtr temp = getHashHead(table->hash, hashCode);
    if (temp == NULL)
        return NULL;
    while (temp)
    {
        if (!strcmp(temp->field->name, name))
            return temp;
        temp = temp->nextHash;
    }
    return NULL;
}

// Return false -> no confliction, true -> has confliction
boolean checkTableItemConflict(TablePtr table, ItemPtr item)
{
    ItemPtr temp = searchTableItem(table, item->field->name);
    if (temp == NULL)
        return FALSE;
    while (temp)
    {
        if (!strcmp(temp->field->name, item->field->name))
        {
            if (temp->field->type->kind == STRUCTURE ||
                item->field->type->kind == STRUCTURE)
                return TRUE;
            if (temp->symbolDepth == table->stack->curStackDepth)
                return TRUE;
        }
        temp = temp->nextHash;
    }
    return FALSE;
}

void addTableItem(TablePtr table, ItemPtr item)
{
    assert(table != NULL && item != NULL);
    unsigned hashCode = getHashCode(item->field->name);
    HashPtr hash = table->hash;
    StackPtr stack = table->stack;
    item->nextSymbol = getCurDepthStackHead(stack);
    setCurDepthStackHead(stack, item);

    item->nextHash = getHashHead(hash, hashCode);
    setHashHead(hash, hashCode, item);
}

void deleteTableItem(TablePtr table, ItemPtr item)
{
    assert(table != NULL && item != NULL);
    unsigned hashCode = getHashCode(item->field->name);
    if (item == getHashHead(table->hash, hashCode))
        setHashHead(table->hash, hashCode, item->nextHash);
    else
    {
        ItemPtr cur = getHashHead(table->hash, hashCode);
        ItemPtr last = cur;
        while (cur != item)
        {
            last = cur;
            cur = cur->nextHash;
        }
        last->nextHash = cur->nextHash;
    }
    deleteItem(item);
}

boolean isStructDef(ItemPtr src)
{
    if (src == NULL)
        return FALSE;
    if (src->field->type->kind != STRUCTURE)
        return FALSE;
    if (src->field->type->u.structure.structName)
        return FALSE;
    return TRUE;
}


void clearCurDepthStackList(TablePtr table)
{
    assert(table != NULL);
    StackPtr stack = table->stack;
    ItemPtr temp = getCurDepthStackHead(stack);
    while (temp)
    {
        ItemPtr tDelete = temp;
        temp = temp->nextSymbol;
        deleteTableItem(table, tDelete);
    }
    setCurDepthStackHead(stack, NULL);
    minusStackDepth(stack);
}


// Stack functions
StackPtr newStack()
{
    StackPtr p = (StackPtr)malloc(sizeof(Stack));
    assert(p != NULL);
    p->stackArray = (ItemPtr *)malloc(sizeof(ItemPtr) * HASH_TABLE_SIZE);
    assert(p->stackArray != NULL);
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        p->stackArray[i] = NULL;
    }
    p->curStackDepth = 0;
    return p;
}

void deleteStack(StackPtr stack)
{
    assert(stack != NULL);
    free(stack->stackArray);
    stack->stackArray = NULL;
    stack->curStackDepth = 0;
    free(stack);
}

void addStackDepth(StackPtr stack)
{
    assert(stack != NULL);
    stack->curStackDepth++;
}

void minusStackDepth(StackPtr stack)
{
    assert(stack != NULL);
    stack->curStackDepth--;
}

ItemPtr getCurDepthStackHead(StackPtr stack)
{
    assert(stack != NULL);
    return stack->stackArray[stack->curStackDepth];
    // return p == NULL ? NULL : p->stackArray[p->curStackDepth];
}

void setCurDepthStackHead(StackPtr stack, ItemPtr newVal)
{
    assert(stack != NULL);
    stack->stackArray[stack->curStackDepth] = newVal;
}

// Global function
void preTree(NodePtr node)
{
    if (node == NULL)
        return;

    if (!strcmp(node->name, "ExtDef")){
        ExtDef(node);
    }
        

    preTree(node->child);
    preTree(node->sibling);
}

// Generate symbol table functions
void ExtDef(NodePtr node)
{
    assert(node != NULL);
    // ExtDef -> Specifier ExtDecList SEMI
    //         | Specifier SEMI
    //         | Specifier FunDec CompSt
    TypePtr specifierType = Specifier(node->child);
    char *secondName = node->child->sibling->name;

    // printType(specifierType);
    // ExtDef -> Specifier ExtDecList SEMI
    if (!strcmp(secondName, "ExtDecList"))
    {
        // TODO: process first situation
        ExtDecList(node->child->sibling, specifierType);
    }
    // ExtDef -> Specifier FunDec CompSt
    else if (!strcmp(secondName, "FunDec"))
    {
        // TODO: process third situation
        FunDec(node->child->sibling, specifierType);
        CompSt(node->child->sibling->sibling, specifierType);
    }
    if (specifierType)
        deleteType(specifierType);
    // printTable(table);
    // Specifier SEMI
    // this situation has no meaning
    // or is struct define(have been processe inSpecifier())
}

void ExtDecList(NodePtr node, TypePtr specifier)
{
    assert(node != NULL);
    // ExtDecList -> VarDec
    //             | VarDec COMMA ExtDecList
    NodePtr temp = node;
    while (temp)
    {
        ItemPtr item = VarDec(temp->child, specifier);
        if (checkTableItemConflict(table, item))
        {
            char msg[100] = {0};
            sprintf(msg, "Redefined variable \"%s\".", item->field->name);
            reportError(REDEF_VAR, temp->lineNo, msg);
            deleteItem(item);
        }
        else
        {
            addTableItem(table, item);
        }
        if (temp->child->sibling)
        {
            temp = temp->sibling->sibling->child;
        }
        else
        {
            break;
        }
    }
}

TypePtr Specifier(NodePtr node)
{
    assert(node != NULL);
    // Specifier -> TYPE
    //            | StructSpecifier

    NodePtr t = node->child;
    // Specifier -> TYPE
    if (!strcmp(t->name, "TYPE"))
    {
        if (!strcmp(t->value, "float"))
        {
            return newType(BASIC, 1,FLOAT_TYPE);
        }
        else
        {
            return newType(BASIC,1, INT_TYPE);
        }
    }
    // Specifier -> StructSpecifier
    else
    {
        return StructSpecifier(t);
    }
}

TypePtr StructSpecifier(NodePtr node)
{
    assert(node != NULL);
    // StructSpecifier -> STRUCT OptTag LC DefList RC
    //                  | STRUCT Tag

    // OptTag -> ID | e
    // Tag -> ID
    TypePtr returnType = NULL;
    NodePtr t = node->child->sibling;
    // StructSpecifier->STRUCT OptTag LC DefList RC
    // printTreeInfo(t, 0);
    if (strcmp(t->name, "Tag"))
    {
        // addStructLayer(table);
        ItemPtr structItem =
            newItem(table->stack->curStackDepth,
                    newFieldList("", newType(STRUCTURE, 2,NULL, NULL)));
        if (!strcmp(t->name, "OptTag"))
        {
            setFieldListName(structItem->field, t->child->value);
            t = t->sibling;
        }
        // unnamed struct
        else
        {
            table->unNamedStructNum++;
            char structName[20] = {0};
            sprintf(structName, "%d", table->unNamedStructNum);
            // printf("unNamed struct's name is %s.\n", structName);
            setFieldListName(structItem->field, structName);
        }
        // 现在我们进入结构体了！注意，报错信息会有不同！
        //  addStackDepth(table->stack);
        if (!strcmp(t->sibling->name, "DefList"))
        {
            DefList(t->sibling, structItem);
        }

        if (checkTableItemConflict(table, structItem))
        {
            char msg[100] = {0};
            sprintf(msg, "Duplicated name \"%s\".", structItem->field->name);
            reportError(DUPLICATED_NAME, node->lineNo, msg);
            deleteItem(structItem);
        }
        else
        {
            returnType = newType(
                STRUCTURE, 2,newString(structItem->field->name),
                copyFieldList(structItem->field->type->u.structure.field));

            // printf("\nnew Type:\n");
            // printType(returnType);
            // printf("\n");

            if (!strcmp(node->child->sibling->name, "OptTag"))
            {
                addTableItem(table, structItem);
            }
            // OptTag -> e
            else
            {
                deleteItem(structItem);
            }
        }

        // 我们出了结构体
        //  minusStackDepth(table->stack);
        //  minusStructLayer(table);
    }

    // StructSpecifier->STRUCT Tag
    else
    {
        ItemPtr structItem = searchTableItem(table, t->child->value);
        if (structItem == NULL || !isStructDef(structItem))
        {
            char msg[100] = {0};
            sprintf(msg, "Undefined structure \"%s\".", t->child->value);
            reportError(UNDEF_STRUCT, node->lineNo, msg);
        }
        else
            returnType = newType(
                STRUCTURE, 2,newString(structItem->field->name),
                copyFieldList(structItem->field->type->u.structure.field));
    }
    // printType(returnType);
    return returnType;
}

ItemPtr VarDec(NodePtr node, TypePtr specifier)
{
    assert(node != NULL);
    // VarDec -> ID
    //         | VarDec LB INT RB
    NodePtr id = node;
    // get ID
    while (id->child)
        id = id->child;
    ItemPtr p = newItem(table->stack->curStackDepth, newFieldList(id->value, NULL));
    if (!strcmp(node->child->name, "ID"))
    {
        // printf("copy type tp %s.\n", node->child->val);
        p->field->type = copyType(specifier);
    }
    // VarDec -> VarDec LB INT RB
    else
    {
        NodePtr varDec = node->child;
        TypePtr temp = specifier;
        // printf("VarDec -> VarDec LB INT RB.\n");
        while (varDec->sibling)
        {
            // printTreeInfo(varDec, 0);
            // printf("number: %s\n", varDec->next->next->val);
            // printf("temp type: %d\n", temp->kind);
            p->field->type =
                newType(ARRAY, 2,copyType(temp), atoi(varDec->sibling->sibling->value));
            temp = p->field->type;
            varDec = varDec->child;
        }
    }
    // printf("-------test VarDec ------\n");
    // printType(specifier);
    // printFieldList(p->field);
    // printf("-------test End ------\n");
    return p;
}


void FunDec(NodePtr node, TypePtr returnType)
{
    assert(node != NULL);
    // FunDec -> ID LP VarList RP
    //         | ID LP RP
    ItemPtr p =
        newItem(table->stack->curStackDepth,
                newFieldList(node->child->value,
                             newType(FUNCTION, 3,0, NULL, copyType(returnType))));

    // FunDec -> ID LP VarList RP
    if (!strcmp(node->child->sibling->sibling->name, "VarList"))
    {
        VarList(node->child->sibling->sibling, p);
    }

    // FunDec -> ID LP RP don't need process

    // check redefine
    if (checkTableItemConflict(table, p))
    {
        char msg[100] = {0};
        sprintf(msg, "Redefined function \"%s\".", p->field->name);
        reportError(REDEF_FUNC, node->lineNo, msg);
        deleteItem(p);
        p = NULL;
    }
    else
    {
        addTableItem(table, p);
    }
}

void VarList(NodePtr node, ItemPtr func)
{
    assert(node != NULL);
    // VarList -> ParamDec COMMA VarList
    //          | ParamDec
    addStackDepth(table->stack);
    int argc = 0;
    NodePtr temp = node->child;
    FieldListPtr cur = NULL;

    // VarList -> ParamDec
    FieldListPtr paramDec = ParamDec(temp);
    func->field->type->u.function.argv = copyFieldList(paramDec);
    cur = func->field->type->u.function.argv;
    argc++;

    // VarList -> ParamDec COMMA VarList
    while (temp->sibling)
    {
        temp = temp->sibling->sibling->child;
        paramDec = ParamDec(temp);
        if (paramDec)
        {
            cur->tail = copyFieldList(paramDec);
            cur = cur->tail;
            argc++;
        }
    }

    func->field->type->u.function.argc = argc;

    minusStackDepth(table->stack);
}

FieldListPtr ParamDec(NodePtr node)
{
    assert(node != NULL);
    // ParamDec -> Specifier VarDec
    TypePtr specifierType = Specifier(node->child);
    ItemPtr p = VarDec(node->child->sibling, specifierType);
    if (specifierType)
        deleteType(specifierType);
    if (checkTableItemConflict(table, p))
    {
        char msg[100] = {0};
        sprintf(msg, "Redefined variable \"%s\".", p->field->name);
        reportError(REDEF_VAR, node->lineNo, msg);
        deleteItem(p);
        return NULL;
    }
    else
    {
        addTableItem(table, p);
        return p->field;
    }
}

void CompSt(NodePtr node, TypePtr returnType)
{
    assert(node != NULL);
    // CompSt -> LC DefList StmtList RC
    // printTreeInfo(node, 0);
    addStackDepth(table->stack);
    NodePtr temp = node->child->sibling;
    if (!strcmp(temp->name, "DefList"))
    {
        DefList(temp, NULL);
        temp = temp->sibling;
    }
    if (!strcmp(temp->name, "StmtList"))
    {
        StmtList(temp, returnType);
    }

    clearCurDepthStackList(table);
}

void StmtList(NodePtr node, TypePtr returnType)
{
    // assert(node != NULL);
    // StmtList -> Stmt StmtList
    //           | e
    // printTreeInfo(node, 0);
    while (node)
    {
        Stmt(node->child, returnType);
        node = node->child->sibling;
    }
}

void Stmt(NodePtr node, TypePtr returnType)
{
    assert(node != NULL);
    // Stmt -> Exp SEMI
    //       | CompSt
    //       | RETURN Exp SEMI
    //       | IF LP Exp RP Stmt
    //       | IF LP Exp RP Stmt ELSE Stmt
    //       | WHILE LP Exp RP Stmt
    // printTreeInfo(node, 0);

    TypePtr exTypePtr = NULL;
    // Stmt -> Exp SEMI
    if (!strcmp(node->child->name, "Exp"))
        exTypePtr = Exp(node->child);

    // Stmt -> CompSt
    else if (!strcmp(node->child->name, "CompSt"))
        CompSt(node->child, returnType);

    // Stmt -> RETURN Exp SEMI
    else if (!strcmp(node->child->name, "RETURN"))
    {
        exTypePtr = Exp(node->child->sibling);

        // check return type
        if (!checkType(returnType, exTypePtr))
            reportError(TYPE_MISMATCH_RETURN, node->lineNo,
                   "Type mismatched for return.");
    }

    // Stmt -> IF LP Exp RP Stmt
    else if (!strcmp(node->child->name, "IF"))
    {
        NodePtr stmt = node->child->sibling->sibling->sibling->sibling;
        exTypePtr = Exp(node->child->sibling->sibling);
        Stmt(stmt, returnType);
        // Stmt -> IF LP Exp RP Stmt ELSE Stmt
        if (stmt->sibling != NULL)
            Stmt(stmt->sibling->sibling, returnType);
    }

    // Stmt -> WHILE LP Exp RP Stmt
    else if (!strcmp(node->child->name, "WHILE"))
    {
        exTypePtr = Exp(node->child->sibling->sibling);
        Stmt(node->child->sibling->sibling->sibling->sibling, returnType);
    }

    if (exTypePtr)
        deleteType(exTypePtr);
}

void DefList(NodePtr node, ItemPtr structInfo)
{
    // assert(node != NULL);
    // DefList -> Def DefList
    //          | e
    while (node)
    {
        Def(node->child, structInfo);
        node = node->child->sibling;
    }
}

void Def(NodePtr node, ItemPtr structInfo)
{
    assert(node != NULL);
    // Def -> Specifier DecList SEMI
    // TODO:调用接口
    TypePtr dectype = Specifier(node->child);
    // 你总会得到一个正确的type
    DecList(node->child->sibling, dectype, structInfo);
    if (dectype)
        deleteType(dectype);
}

void DecList(NodePtr node, TypePtr specifier, ItemPtr structInfo)
{
    assert(node != NULL);
    // DecList -> Dec
    //          | Dec COMMA DecList
    NodePtr temp = node;
    while (temp)
    {
        Dec(temp->child, specifier, structInfo);
        if (temp->child->sibling)
            temp = temp->child->sibling->sibling;
        else
            break;
    }
}

void Dec(NodePtr node, TypePtr specifier, ItemPtr structInfo)
{
    assert(node != NULL);
    // Dec -> VarDec
    //      | VarDec ASSIGNOP Exp

    // Dec -> VarDec
    if (node->child->sibling == NULL)
    {
        if (structInfo != NULL)
        {
            // 结构体内，将VarDec返回的Item中的filedList
            // Copy判断是否重定义，无错则到结构体链表尾 记得delete掉Item
            ItemPtr decitem = VarDec(node->child, specifier);
            FieldListPtr payload = decitem->field;
            FieldListPtr structField = structInfo->field->type->u.structure.field;
            FieldListPtr last = NULL;
            while (structField != NULL)
            {
                // then we have to check
                if (!strcmp(payload->name, structField->name))
                {
                    // 出现重定义，报错
                    char msg[100] = {0};
                    sprintf(msg, "Redefined field \"%s\".",
                            decitem->field->name);
                    reportError(REDEF_FEILD, node->lineNo, msg);
                    deleteItem(decitem);
                    return;
                }
                else
                {
                    last = structField;
                    structField = structField->tail;
                }
            }
            // 新建一个fieldlist,删除之前的item
            if (last == NULL)
            {
                // that is good
                structInfo->field->type->u.structure.field =
                    copyFieldList(decitem->field);
            }
            else
            {
                last->tail = copyFieldList(decitem->field);
            }
            deleteItem(decitem);
        }
        else
        {
            // 非结构体内，判断返回的item有无冲突，无冲突放入表中，有冲突报错delete
            ItemPtr decitem = VarDec(node->child, specifier);
            if (checkTableItemConflict(table, decitem))
            {
                // 出现冲突，报错
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->field->name);
                reportError(REDEF_VAR, node->lineNo, msg);
                deleteItem(decitem);
            }
            else
            {
                addTableItem(table, decitem);
            }
        }
    }
    // Dec -> VarDec ASSIGNOP Exp
    else
    {
        if (structInfo != NULL)
        {
            // 结构体内不能赋值，报错
            reportError(REDEF_FEILD, node->lineNo,
                   "Illegal initialize variable in struct.");
        }
        else
        {
            // 判断赋值类型是否相符
            // 如果成功，注册该符号
            ItemPtr decitem = VarDec(node->child, specifier);
            TypePtr extypePtrTypePtr = Exp(node->child->sibling->sibling);
            if (checkTableItemConflict(table, decitem))
            {
                // 出现冲突，报错
                char msg[100] = {0};
                sprintf(msg, "Redefined variable \"%s\".",
                        decitem->field->name);
                reportError(REDEF_VAR, node->lineNo, msg);
                deleteItem(decitem);
            }
            if (!checkType(decitem->field->type, extypePtrTypePtr))
            {
                // 类型不相符
                // 报错
                reportError(TYPE_MISMATCH_ASSIGN, node->lineNo,
                       "Type mismatchedfor assignment.");
                deleteItem(decitem);
            }
            if (decitem->field->type && decitem->field->type->kind == ARRAY)
            {
                // 报错，对非basic类型赋值
                reportError(TYPE_MISMATCH_ASSIGN, node->lineNo,
                       "Illegal initialize variable.");
                deleteItem(decitem);
            }
            else
            {
                addTableItem(table, decitem);
            }
            // exp不出意外应该返回一个无用的type，删除
            if (extypePtrTypePtr)
                deleteType(extypePtrTypePtr);
        }
    }
}

TypePtr Exp(NodePtr node)
{
    assert(node != NULL);
    // Exp -> Exp ASSIGNOP Exp
    //      | Exp AND Exp
    //      | Exp OR Exp
    //      | Exp RELOP Exp
    //      | Exp PLUS Exp
    //      | Exp MINUS Exp
    //      | Exp STAR Exp
    //      | Exp DIV Exp
    //      | LP Exp RP
    //      | MINUS Exp
    //      | NOT Exp
    //      | ID LP Args RP
    //      | ID LP RP
    //      | Exp LB Exp RB
    //      | Exp DOT ID
    //      | ID
    //      | INT
    //      | FLOAT
    NodePtr t = node->child;
    // exp will only check if the cal is right
    //  printTable(table);
    // 二值运算
    if (!strcmp(t->name, "Exp"))
    {
        // 基本数学运算符
        if (strcmp(t->sibling->name, "LB") && strcmp(t->sibling->name, "DOT"))
        {
            TypePtr p1 = Exp(t);
            TypePtr p2 = Exp(t->sibling->sibling);
            TypePtr returnType = NULL;

            // Exp -> Exp ASSIGNOP Exp
            if (!strcmp(t->sibling->name, "ASSIGNOP"))
            {
                // 检查左值
                NodePtr tchild = t->child;

                if (!strcmp(tchild->name, "FLOAT") ||
                    !strcmp(tchild->name, "INT"))
                {
                    // 报错，左值
                    reportError(LEFT_VAR_ASSIGN, t->lineNo,
                           "The left-hand side of an assignment must be "
                           "avariable.");
                }
                else if (!strcmp(tchild->name, "ID") ||
                         !strcmp(tchild->sibling->name, "LB") ||
                         !strcmp(tchild->sibling->name, "DOT"))
                {
                    if (!checkType(p1, p2))
                    {
                        // 报错，类型不匹配
                        reportError(TYPE_MISMATCH_ASSIGN, t->lineNo,
                               "Type mismatched for assignment.");
                    }
                    else
                        returnType = copyType(p1);
                }
                else
                {
                    // 报错，左值
                    reportError(LEFT_VAR_ASSIGN, t->lineNo,
                           "The left-hand side of an assignment must be "
                           "avariable.");
                }
            }
            // Exp -> Exp AND Exp
            //      | Exp OR Exp
            //      | Exp RELOP Exp
            //      | Exp PLUS Exp
            //      | Exp MINUS Exp
            //      | Exp STAR Exp
            //      | Exp DIV Exp
            else
            {
                if (p1 && p2 && (p1->kind == ARRAY || p2->kind == ARRAY))
                {
                    // 报错，数组，结构体运算
                    reportError(TYPE_MISMATCH_OP, t->lineNo,
                           "Type mismatched for operands.");
                }
                else if (!checkType(p1, p2))
                {
                    // 报错，类型不匹配
                    reportError(TYPE_MISMATCH_OP, t->lineNo,
                           "Type mismatched for operands.");
                }
                else
                {
                    if (p1 && p2)
                    {
                        returnType = copyType(p1);
                    }
                }
            }

            if (p1)
                deleteType(p1);
            if (p2)
                deleteType(p2);
            return returnType;
        }
        // 数组和结构体访问
        else
        {
            // Exp -> Exp LB Exp RB
            if (!strcmp(t->sibling->name, "LB"))
            {
                // 数组
                TypePtr p1 = Exp(t);
                TypePtr p2 = Exp(t->sibling->sibling);
                TypePtr returnType = NULL;

                if (!p1)
                {
                    // 第一个exp为null，上层报错，这里不用再管
                }
                else if (p1 && p1->kind != ARRAY)
                {
                    // 报错，非数组使用[]运算符
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an array.", t->child->value);
                    reportError(NOT_A_ARRAY, t->lineNo, msg);
                }
                else if (!p2 || p2->kind != BASIC ||
                         p2->u.basic != INT_TYPE)
                {
                    // 报错，不用int索引[]
                    char msg[100] = {0};
                    sprintf(msg, "\"%s\" is not an integer.",
                            t->sibling->sibling->child->value);
                    reportError(NOT_A_INT, t->lineNo, msg);
                }
                else
                {
                    returnType = copyType(p1->u.array.elem);
                }
                if (p1)
                    deleteType(p1);
                if (p2)
                    deleteType(p2);
                return returnType;
            }
            // Exp -> Exp DOT ID
            else
            {
                TypePtr p1 = Exp(t);
                TypePtr returnType = NULL;
                if (!p1 || p1->kind != STRUCTURE ||
                    !p1->u.structure.structName)
                {
                    // 报错，对非结构体使用.运算符
                    reportError(ILLEGAL_USE_DOT, t->lineNo, "Illegal use of \".\".");
                    if (p1)
                        deleteType(p1);
                }
                else
                {
                    NodePtr ref_id = t->sibling->sibling;
                    FieldListPtr structfield = p1->u.structure.field;
                    while (structfield != NULL)
                    {
                        if (!strcmp(structfield->name, ref_id->value))
                        {
                            break;
                        }
                        structfield = structfield->tail;
                    }
                    if (structfield == NULL)
                    {
                        // 报错，没有可以匹配的域名
                        printf("Error type %d at Line %d: %s.\n", 14, t->lineNo,
                               "NONEXISTFIELD");
                        ;
                    }
                    else
                    {
                        returnType = copyType(structfield->type);
                    }
                }
                if (p1)
                    deleteType(p1);
                return returnType;
            }
        }
    }
    // 单目运算符
    //  Exp -> MINUS Exp
    //       | NOT Exp
    else if (!strcmp(t->name, "MINUS") || !strcmp(t->name, "NOT"))
    {
        TypePtr p1 = Exp(t->sibling);
        TypePtr returnType = NULL;
        if (!p1 || p1->kind != BASIC)
        {
            // 报错，数组，结构体运算
            printf("Error type %d at Line %d: %s.\n", 7, t->lineNo,
                   "TYPE_MISMATCH_OP");
        }
        else
        {
            returnType = copyType(p1);
        }
        if (p1)
            deleteType(p1);
        return returnType;
    }
    else if (!strcmp(t->name, "LP"))
    {
        return Exp(t->sibling);
    }
    // Exp -> ID LP Args RP
    //		| ID LP RP
    else if (!strcmp(t->name, "ID") && t->sibling)
    {
        ItemPtr funcInfo = searchTableItem(table, t->value);

        // function not find
        if (funcInfo == NULL)
        {
            char msg[100] = {0};
            sprintf(msg, "Undefined function \"%s\".", t->value);
            reportError(UNDEF_FUNC, node->lineNo, msg);
            return NULL;
        }
        else if (funcInfo->field->type->kind != FUNCTION)
        {
            char msg[100] = {0};
            sprintf(msg, "\"%s\" is not a function.", t->value);
            reportError(NOT_A_FUNC, node->lineNo, msg);
            return NULL;
        }
        // Exp -> ID LP Args RP
        else if (!strcmp(t->sibling->sibling->name, "Args"))
        {
            Args(t->sibling->sibling, funcInfo);
            return copyType(funcInfo->field->type->u.function.returnType);
        }
        // Exp -> ID LP RP
        else
        {
            if (funcInfo->field->type->u.function.argc != 0)
            {
                char msg[100] = {0};
                sprintf(msg,
                        "too few arguments to function \"%s\", except %d args.",
                        funcInfo->field->name,
                        funcInfo->field->type->u.function.argc);
                reportError(FUNC_AGRC_MISMATCH, node->lineNo, msg);
            }
            return copyType(funcInfo->field->type->u.function.returnType);
        }
    }
    // Exp -> ID
    else if (!strcmp(t->name, "ID"))
    {
        ItemPtr tp = searchTableItem(table, t->value);
        if (tp == NULL || isStructDef(tp))
        {
            char msg[100] = {0};
            sprintf(msg, "Undefined variable \"%s\".", t->value);
            reportError(UNDEF_VAR, t->lineNo, msg);
            return NULL;
        }
        else
        {
            // good
            return copyType(tp->field->type);
        }
    }
    else
    {
        // Exp -> FLOAT
        if (!strcmp(t->name, "FLOAT"))
        {
            return newType(BASIC,1, FLOAT_TYPE);
        }
        // Exp -> INT
        else
        {
            return newType(BASIC, 1,INT_TYPE);
        }
    }
}

void Args(NodePtr node, ItemPtr funcInfo)
{
    assert(node != NULL);
    // Args -> Exp COMMA Args
    //       | Exp
    // printTreeInfo(node, 0);
    NodePtr temp = node;
    FieldListPtr arg = funcInfo->field->type->u.function.argv;
    // printf("-----function atgs-------\n");
    // printFieldList(arg);
    // printf("---------end-------------\n");
    while (temp)
    {
        if (arg == NULL)
        {
            char msg[100] = {0};
            sprintf(
                msg, "too many arguments to function \"%s\", except %d args.",
                funcInfo->field->name, funcInfo->field->type->u.function.argc);
            reportError(FUNC_AGRC_MISMATCH, node->lineNo, msg);
            break;
        }
        TypePtr realType = Exp(temp->child);
        // printf("=======arg type=========\n");
        // printType(realType);
        // printf("===========end==========\n");
        if (!checkType(realType, arg->type))
        {
            char msg[100] = {0};
            sprintf(msg, "Function \"%s\" is not applicable for arguments.",
                    funcInfo->field->name);
            reportError(FUNC_AGRC_MISMATCH, node->lineNo, msg);
            if (realType)
                deleteType(realType);
            return;
        }
        if (realType)
            deleteType(realType);

        arg = arg->tail;
        if (temp->child->sibling)
        {
            temp = temp->child->sibling->sibling;
        }
        else
        {
            break;
        }
    }
    if (arg != NULL)
    {
        char msg[100] = {0};

        sprintf(msg, "too few arguments to function \"%s\", except %d args.",
                funcInfo->field->name, funcInfo->field->type->u.function.argc);
        reportError(FUNC_AGRC_MISMATCH, node->lineNo, msg);
    }
}
