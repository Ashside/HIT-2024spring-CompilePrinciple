
#include "SyntaxTree.h"
#include <stdlib.h>
#include <stdio.h>
AstNode *createNode(int token, int lineNum, int isLeaf,char * val)
{
    AstNode *node = (AstNode *)malloc(sizeof(AstNode));
    if (node == NULL)
    {
        return NULL; // Failed to allocate memory
    }

    node->token = token;
    node->lineNum = lineNum;
    node->isLeaf = isLeaf;
    node->children = NULL;
    node->brother = NULL;

    return node;
}

AstNode *initRootNode()
{
}

void addChild(AstNode *parent, AstNode *child)
{
}

void addBrother(AstNode *node, AstNode *brother)
{
}
void freeTree(AstNode *root)
{
}
void printTree(AstNode *root, int level)
{
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