#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TokenTypeEnum.h"

#define TRUE 1
#define FALSE 0

typedef struct _Node
{
	TokenTypeEnum type;
	int line;
	char *value; // 字面值，例如：int、a、1等
	char *name;	 // 终结符或非终结符的名字，例如：ID、Program、ExtDefList等
	struct _Node *child;
	struct _Node *sibling;
} Node;

typedef Node* NodePtr;

NodePtr newTokenNode(TokenTypeEnum type, int line, char *name, char *value);
NodePtr newParseNode(TokenTypeEnum type, int line, char *name, int numChildren, ...);
/*addChild 弃用*/
void addChild(NodePtr parent, NodePtr child);
/*addSibling 弃用*/
void addSibling(NodePtr node, NodePtr sibling);
void freeNode(NodePtr node);
void freeTree(NodePtr node);
void printTree(NodePtr node, int depth);

#endif // SYNTAX_TREE_H
