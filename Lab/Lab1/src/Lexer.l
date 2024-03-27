%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "SyntaxTree.h"

#include "Parse.tab.h"

int yycolumn = 1;
extern void yyerror(const char *msg, int lineno,char type,const char *tokenText);
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
        int c;
        int startLine = yylineno;
        while ((c = input()) != EOF) {
            if (c == '*') {
                if ((c = input()) == '/') {
                    break;
                }
                else {
                    unput(c);
                }
            }
            if (c == EOF) {
                yyerror("Unmatched comment", startLine, 'B',yytext);
                break;
            }
        }
            
}

{INT} {
            printf("INT\n");
            return INT;
}
{FLOAT} {
            printf("FLOAT\n");
            return FLOAT;
}
{SEMI} {
            printf("SEMI\n");
            return SEMI;
}
{COMMA} {
            printf("COMMA\n");
            return COMMA;
}
{ASSIGNOP} {
            printf("ASSIGNOP\n");
            return ASSIGNOP;
}
{RELOP} {
            printf("RELOP\n");
            return RELOP;
}
{PLUS} {
            printf("PLUS\n");
            return PLUS;
}
{MINUS} {
            printf("MINUS\n");
            return MINUS;
}
{STAR} {
            printf("STAR\n");
            return STAR;
}
{DIV} {
            printf("DIV\n");
            return DIV;
}
{AND} {
            printf("AND\n");
            return AND;
}
{OR} {
            printf("OR\n");
            return OR;
}
{DOT} {
            printf("DOT\n");
            return DOT;
}
{NOT} {
            printf("NOT\n");
            return NOT;
}
{TYPE} {
            printf("TYPE: %s\n", yytext);
            return TYPE;
}
{LP} {
            printf("LP\n");
            return LP;
}
{RP} {
            printf("RP\n");
            return RP;
}
{LB} {
            printf("LB\n");
            return LB;
}
{RB} {
            printf("RB\n");
            return RB;
}
{LC} {
            printf("LC\n");
            return LC;
}
{RC} {
            printf("RC\n");
            return RC;
}
{STRUCT} {
            printf("STRUCT\n");
            return STRUCT;
}
{RETURN} {
            printf("RETURN\n");
            return RETURN;
}
{IF} {
            printf("IF\n");
            return IF;
}
{ELSE} {
            printf("ELSE\n");
            return ELSE;
}
{WHILE} {
            printf("WHILE\n");
            return WHILE;
}
{ID} {
            printf("ID: %s\n", yytext);
            return ID;
}
{INT_ERROR} {
            yyerror("Invalid INT", yylineno, 'A',yytext);
            return ERROR;
}
{FLOAT_ERROR} {
            yyerror("Invalid FLOAT", yylineno, 'A',yytext);
            return ERROR;
}

{ID_ERROR} {
            yyerror("Invalid ID", yylineno, 'A',yytext);
            return ERROR;
}

{ERROR} {
            yyerror("Invalid character", yylineno, 'A',yytext);
            return ERROR;
}
%%