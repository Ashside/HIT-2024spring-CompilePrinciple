#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H
struct AstNode_s
{
    int symbol;
    int lineNum;
    char *tokenType;
    int isLeaf;

    struct AstNode_s *children, *brother;
    union
    {
        int intVal;
        float floatVal;
        char *ID;
    };
};
typedef struct AstNode_s AstNode;
AstNode *createNode(int symbol, int lineNum, char *tokenType, int isLeaf);
AstNode *initRootNode();

void addChild(AstNode *parent, AstNode *child);
void addBrother(AstNode *node, AstNode *brother);
void printTree(AstNode *root, int depth);
void freeTree(AstNode *root);
void printTree(AstNode *root, int depth);

void lexError(int lineNum, char *msg, char *token);
#endif // SYNTAXTREE_H
