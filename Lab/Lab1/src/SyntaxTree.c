
#include "SyntaxTree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
AstNode *createNode(char *token, int lineNum, int isTerminal, char *val)
{
    AstNode *node = (AstNode *)malloc(sizeof(AstNode));
    if (node == NULL)
    {
        return NULL; // Failed to allocate memory
    }

    node->token = token;
    node->lineNum = lineNum;
    node->isTerminal = isTerminal;
    node->val = val;
    node->children = (AstNode *)malloc(sizeof(AstNode));
    node->brother = (AstNode *)malloc(sizeof(AstNode));

    if (isTerminal)
    {
        if (!strcmp(token, "ID") || !strcmp(token, "TYPE"))
        {
            node->ID = val;
        }
        else if (!strcmp(token, "INT"))
        {
            node->intVal = atoi(val);
        }
        else if (!strcmp(token, "FLOAT"))
        {
            node->floatVal = atof(val);
        }
    }

    return node;
}

void addChild(AstNode *parent, int nums, ...)
{
    va_list valist;
    va_start(valist, nums);
    AstNode *child = va_arg(valist, AstNode *);
    parent->children = child;
    AstNode *lastChild = child;
    for (int i = 1; i < nums; i++)
    {
        child = va_arg(valist, AstNode *);
        lastChild->brother = child;
        lastChild = child;
    }
}

void freeTree(AstNode *root)
{
    if (root->children != NULL)
    {
        freeTree(root->children);
    }
    if (root->brother != NULL)
    {
        freeTree(root->brother);
    }
    free(root);
}
void printTree(AstNode *root, int level)
{
    if (root != NULL)
    {
        printf("%*c", level * 2, ' ');
        if (!strcmp(root->token, "ID") || !strcmp(root->token, "TYPE"))
        {
            printf(": %s", root->ID);
        }
        else if (!strcmp(root->token, "INT"))
        {
            printf(": %d", root->intVal);
        }
        else if (!strcmp(root->token, "FLOAT"))
        {
            printf(": %f", root->floatVal);
        }
        else if (!root->isTerminal)
        {
            printf(" (%d)", root->lineNum);
        }
        printf("\n");
        printTree(root->children, level + 1);
        printTree(root->brother, level);
    }
}

void lexError(int lineNum, char *msg, char *token)
{
}
void synError(int lineNum, char *msg)
{
}

void yyerror(const char *msg, int lineno, char type, const char *tokenText)
{
    printf("Error type %c at Line %d: %s \"%s\"\n", type, lineno, msg, tokenText);
}