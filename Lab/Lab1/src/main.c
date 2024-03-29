#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
extern int yylex(void);
extern int yyparse(void);
extern void yyrestart(FILE *input_file);
extern void yyset_debug(int debug_flag);
int main(int argc, char **argv)
{
    if(argc<2)
    {
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    FILE* file = fopen(argv[1], "r");
    if(file == NULL)
    {
        fprintf(stderr, "Error: Cannot open file %s\n", argv[1]);
        return 1;
    }

    yyset_debug(0);
    yyrestart(file);
    yyparse();
}