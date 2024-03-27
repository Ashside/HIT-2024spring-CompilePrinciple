#include <stdio.h>
#include <stdlib.h>

extern FILE *yyin;
extern int yylex(void);

int main(int argc, char **argv)
{
    printf("Hello, World!\n");
    if (argc > 1)
    {
        if (!(yyin = fopen(argv[1], "r")))
        {
            perror(argv[1]);
            return 1;
        }
    }
    while (yylex() != 0);
    return 0;
}