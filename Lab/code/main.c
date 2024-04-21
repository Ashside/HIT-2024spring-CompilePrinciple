#include "SyntaxTree.h"
#include "Parse.tab.h"
#include "Semantic.h"
#define LAB1_ACTIVE
// #define LAB2_ACTIVE

extern NodePtr RootNode;
extern TablePtr RootTable;
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
        #ifdef LAB1_ACTIVE
            printTree(RootNode, 0);
        #endif

        #ifdef LAB2_ACTIVE
            //Program(RootNode);
            
        #endif
        

    }
    freeTree(RootNode);

    
    #ifdef LAB2_ACTIVE
        freeTable();
    #endif


    return 0;
}
