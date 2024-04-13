
#include "SyntaxTree.h"

//#define DEBUG_NEW_NODES
NodePtr newTokenNode(TokenTypeEnum type, int line, char *name, char *value)
{
	#ifdef DEBUG_NEW_NODES
		printf("Starting new token node: %s (%d) %s\n", name, line, value);
	#endif
	// Token node is a leaf node

	NodePtr node = (NodePtr)malloc(sizeof(Node));
	assert(node != NULL);

	node->type = type;
	node->line = line;

	int nameLen = strlen(name);
	node->name = (char *)malloc((nameLen + 1) * sizeof(char));

	int valueLen = strlen(value);
	node->value = (char *)malloc((valueLen + 1) * sizeof(char));

	assert(node->name != NULL);
	assert(node->value != NULL);

	//TODO: 检查是否有内存泄漏
	//TODO: 野指针导致段错误
	strncpy(node->name, name, nameLen + 1);
	strncpy(node->value, value, valueLen + 1);

	node->child = NULL;
	node->sibling = NULL;

	#ifdef DEBUG_NEW_NODES
		printf("Finished new token node: %s (%d) %s\n", name, line, value);
	#endif

	return node;
}
NodePtr newParseNode(TokenTypeEnum type, int line, char *name, int numChildren, ...)
{
	// Parse node is an internal node
	#ifdef DEBUG_NEW_NODES
		printf("Starting new parse node: %s (%d)\n", name, line);
	#endif

	NodePtr node = (NodePtr)malloc(sizeof(Node));
	assert(node != NULL);

	node->type = type;
	node->line = line;

	int nameLen = strlen(name);
	node->name = (char *)malloc((nameLen + 1) * sizeof(char));
	assert(node->name != NULL);
	strncpy(node->name, name, nameLen + 1);

	/*
	 *	no need to allocate memory for value
	 */

	node->child = NULL;
	node->sibling = NULL;

	va_list children;
	va_start(children, numChildren);

	NodePtr child = va_arg(children, NodePtr);
	node->child = child;

	NodePtr prevChild = child;
	for (int i = 1; i < numChildren; i++)
	{
		child = va_arg(children, NodePtr);
		if (child != NULL)
		{
			prevChild->sibling = child;
			prevChild = child;
		}
	}

	va_end(children);

	#ifdef DEBUG_NEW_NODES
		printf("Finished new parse node: %s (%d)\n", name, line);
	#endif

	return node;
}
void addChild(NodePtr parent, NodePtr child)
{
	parent->child = child;
}
void addSibling(NodePtr node, NodePtr sibling)
{
	node->sibling = sibling;
}
void freeNode(NodePtr node)
{
	free(node->name);
	free(node->value);
	free(node);
}
void freeTree(NodePtr node)
{
	if (node == NULL)
	{
		return;
	}
	freeTree(node->child);
	freeTree(node->sibling);
	freeNode(node);
}
void printTree(NodePtr node, int depth)
{
	#ifdef DEBUG_PRINT_TREE
		printf("Printing tree\n");
	#endif

	if (node == NULL)
	{
		return;
	}
	for (int i = 0; i < depth; i++)
	{
		printf("  "); // (2 spaces) * depth
	}
	printf("%s", node->name);

	switch (node->type)
	{
	case TOKEN_NOTTOKEN:
		printf(" (%d)", node->line);
		break;
	case TOKEN_TYPE:
	case TOKEN_ID:
	case TOKEN_INT:
		printf(": %s", node->value);
		break;
	case TOKEN_FLOAT:
		printf(": %lf", atof(node->value));
		break;
	default:
		break;
	}
	printf("\n");
	printTree(node->child, depth + 1);
	printTree(node->sibling, depth);

	#ifdef DEBUG_PRINT_TREE
		printf("Finished printing tree\n");
	#endif
}