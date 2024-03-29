#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H
extern int yylineno;
struct AstNode_s
{
    char* token;
    int lineNum;
    int isTerminal;
    char* val;
    struct AstNode_s *children, *brother;
    union
    {
        int intVal;
        float floatVal;
        char *ID;
    };
};
typedef struct AstNode_s AstNode;



AstNode *createNode(char* token, int lineNum,int isTerminal,char* val);
//AstNode *initRootNode();

void addChild(AstNode *parent,int nums,...);
//void addBrother(AstNode *node, AstNode *brother);
void printTree(AstNode *root, int depth);
void freeTree(AstNode *root);

#endif // SYNTAXTREE_H
