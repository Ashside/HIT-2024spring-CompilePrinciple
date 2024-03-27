%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "SyntaxTree.h"
#include "Parse.tab.h"

int yycolumn = 1;
extern void yyerror(const char *msg, ...);
%}

%option yylineno


INTOCT [+-]?0[0-7]+ 
INTHEX [+-]?0[xX][0-9a-fA-F]+
INTDEC [+-]?(0|[1-9][0-9]*)
INT {INTDEC}|{INTOCT}|{INTHEX}

BASENUM [0-9]+\.[0-9]*|[0-9]*\.[0-9]* 
FLOAT ({INTDEC}\.[0-9]+)|(([+-]?{BASENUM})[Ee][+-]?[0-9]+)


ID [a-zA-Z_][a-zA-Z0-9_]* 

SEMI ;
COMMA ,
ASSIGNOP =
RELOP ==|!=|<=|<|>=|> 
PLUS \+
MINUS \-
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !

TYPE int|float

LP \(
RP \)
LB \[
RB \]
LC \{
RC \}


STRUCT struct
RETURN return
IF if
ELSE else
WHILE while

SPACE [ \t\n\r]+


ID_ERROR [0-9][a-zA-Z0-9_]* 
INTOCT_ERROR [+-]?0[0-7]*[89][0-9]* 
INTHEX_ERROR [+-]?0[xX][0-9a-fA-F]*[g-zG-Z]*[0-9a-zA-Z]* 
INTDEC_ERROR [+-]?[1-9][0-9]*[a-zA-Z]* 
FLOAT_ERROR ({INTDEC}\.[0-9]*[a-zA-Z]+)|(([+-]?[0-9]*\.[0-9]*|[0-9]*\.[0-9]*)[Ee][+-]?[0-9]*[a-zA-Z]+) 
INT_ERROR {INTDEC_ERROR}|{INTOCT_ERROR}|{INTHEX_ERROR} 

ERROR .

%%
{SPACE} { }
"//" {
            while (input() != '\n') ;
            
}
"/*" {
            int depth = 1;
            int c;

            while ((c = input()) != EOF) {
                if (c == '*') {
                    if ((c = input()) == '/') {
                        if (--depth == 0)
                            break;
                    }
                } else if (c == '/') {
                    if ((c = input()) == '*') {
                        depth++;
                    }
                }
            }

            if (c == EOF) {
                fprintf(stderr, "Error: Unterminated comment\n");
            }
        }

%%
