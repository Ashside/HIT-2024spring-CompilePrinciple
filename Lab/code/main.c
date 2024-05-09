#define LAB3_ACTIVE
#include "SyntaxTree.h"
#include "Parse.tab.h"
#include "Semantic.h"
#include "InterCode.h"

extern NodePtr RootNode;
extern int yylineno;
extern int yyparse();
extern void yyrestart(FILE *);

int LexError = FALSE;
int SynError = FALSE;
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
    FILE *File2Write = stdout;
    if (argc > 2)
    {
        FILE *File2Write = fopen(argv[2], "wt+");
        if (!File2Write)
        {
            perror(argv[2]);
            return 1;
        }
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
        table = newTable();
        preTree(RootNode);
        deleteTable(table);
#endif

#ifdef LAB3_ACTIVE
        //printf("Generating intermediate code...\n");
        table = newTable();
        preTree(RootNode);
        interCodeList = newInterCodeList();
        travelTranslate(RootNode);
#ifdef DEBUG_INTER_CODE_GENERATION
        printf("Intermediate code generated:\n");
        printInterCodeList(File2Write, interCodeList);

#else // DEBUG_INTER_CODE_GENERATION
        printInterCodeList(File2Write, interCodeList);
#endif // DEBUG_INTER_CODE_GENERATION
        deleteTable(table);
#endif // LAB3_ACTIVE
    }

    freeTree(RootNode);

    return 0;
}
