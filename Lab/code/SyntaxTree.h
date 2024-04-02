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
	char *value; // for token, NULL for non-terminal. Return value for lex
	char *name;	 // for variable
	struct _Node *child;
	struct _Node *sibling;
} Node;

typedef Node* NodePtr;

NodePtr newTokenNode(TokenTypeEnum type, int line, char *name, char *value);
NodePtr newParseNode(TokenTypeEnum type, int line, char *name, int numChildren, ...);
/*addChild Deprecated*/
void addChild(NodePtr parent, NodePtr child);
/*addSibling Deprecated*/
void addSibling(NodePtr node, NodePtr sibling);
void freeNode(NodePtr node);
void freeTree(NodePtr node);
void printTree(NodePtr node, int depth);

#endif // SYNTAX_TREE_H
