#include "SyntaxTree.h"
#include "Parse.tab.h"
#include "Semantic.h"
#include "InterCode.h"

#ifdef DEBUG_LAB2
#define DEBUG_SEMANTIC_ANALYSIS
#define LAB2_ACTIVE
#endif

#ifdef DEBUG_LAB3
#define DEBUG_INTER_CODE_GENERATION
#define LAB3_ACTIVE
#endif
#define LAB2_ACTIVE

extern NodePtr RootNode;
extern int yylineno;
extern int yyparse();
extern void yyrestart(FILE *);

int LexError = FALSE;
int SynError = FALSE;
int IrError = FALSE;

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        yyparse();
        return 1;
    }

    FILE *File2Read = fopen(argv[1], "r");
    if (!File2Read)
    {
        perror(argv[1]);
        return 1;
    }
#ifdef LAB3_ACTIVE
    FILE *File2Write = fopen(argv[2], "wt+");
    if (!File2Write)
    {
        perror(argv[2]);
        return 1;
    }
#endif

    yyrestart(File2Read);
    yyparse();

    if (!LexError && !SynError)
    {
#ifdef LAB1_ACTIVE

        printTree(RootNode, 0);

#endif

#ifdef LAB2_ACTIVE
        table = initTable();
        preTree(RootNode);
        deleteTable(table);
#endif

#ifdef LAB3_ACTIVE
        table = initTable();
        traverseTree(RootNode);
        interCodeList = newInterCodeList();
        genInterCodes(RootNode);
        if (!IrError)
        {
            printInterCode(File2Write, interCodeList);
        }
        deleteTable(table);
#endif
    }

    freeTree(RootNode);

    return 0;
}
