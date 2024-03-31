#include "SyntaxTree.h"
#include "Parse.tab.h"

extern NodePtr RootNode;

extern int yylineno;
extern int yyparse();
extern void yyrestart(FILE*);

int LexError = FALSE;
int SynError = FALSE;

int main(int argc, char** argv) {
	if (argc <= 1) {
        yyparse();
        return 1;
    }

    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }

    yyrestart(f);
    yyparse();
    
    #ifdef DEBUG_NEW_NODES
        printf("Syntax analysis finished\n");
    #endif

    if (!LexError && !SynError) {
        printTree(RootNode, 0);
    }
    freeTree(RootNode);
    return 0;
}
