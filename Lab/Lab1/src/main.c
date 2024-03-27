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
    while (1){
        printf("Token: %d\n", yylex());
        if (yylex() == 0)
        {
            break;
        }
        
    }
    return 0;
}