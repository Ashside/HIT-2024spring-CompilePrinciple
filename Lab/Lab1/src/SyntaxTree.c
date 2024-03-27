
#include "SyntaxTree.h"
#include <stdlib.h>
#include <stdio.h>
AstNode * createNode(int symbol, int lineNum, char *tokenType, int isLeaf){
    AstNode *node = (AstNode*)malloc(sizeof(AstNode));
    if(node == NULL) {
        return NULL; // Failed to allocate memory
    }

    node->symbol = symbol;
    node->lineNum = lineNum;
    node->tokenType = tokenType;
    node->isLeaf = isLeaf;
    node->children = NULL;
    node->brother = NULL;

    return node;
}

AstNode *initRootNode(){
    

}

void addChild(AstNode *parent, AstNode *child){

}

void addBrother(AstNode *node, AstNode *brother){

}
void freeTree(AstNode *root){

}
void printTree(AstNode *root, int level) {

}

void lexError(int lineNum, char *msg, char *token){

}
void synError(int lineNum, char *msg){

}

void yyerror (char const *s) {
   fprintf(stderr, "%s\n", s);
 }