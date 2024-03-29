%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "SyntaxTree.h"

#include "Parse.tab.h"
extern YYSTYPE yylval;

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
            //printf("INT\n");
            yylval.parseNode=createNode("INT",yylineno,1,yytext);
            printf("INT\n");
            return INT;
}
{FLOAT} {
            yylval.parseNode=createNode("FLOAT",yylineno,1,yytext);
            printf("FLOAT\n");
            return FLOAT;
}
{SEMI} {
            yylval.parseNode=createNode("SEMI",yylineno,1,yytext);
            printf("SEMI\n");
            return SEMI;
}
{COMMA} {
            yylval.parseNode=createNode("COMMA",yylineno,1,yytext);
            printf("COMMA\n");
            return COMMA;
}
{ASSIGNOP} {
            yylval.parseNode=createNode("ASSIGNOP",yylineno,1,yytext);
            printf("ASSIGNOP\n");
            return ASSIGNOP;
}
{RELOP} {
            yylval.parseNode=createNode("RELOP",yylineno,1,yytext);
            printf("RELOP\n");
            return RELOP;
}
{PLUS} {
            yylval.parseNode=createNode("PLUS",yylineno,1,yytext);
            printf("PLUS\n");
            return PLUS;
}
{MINUS} {
            yylval.parseNode=createNode("MINUS",yylineno,1,yytext);
            printf("MINUS\n");
            return MINUS;
}
{STAR} {
            yylval.parseNode=createNode("STAR",yylineno,1,yytext);
            printf("STAR\n");
            return STAR;
}
{DIV} {
            yylval.parseNode=createNode("DIV",yylineno,1,yytext);
            printf("DIV\n");
            return DIV;
}
{AND} {
            yylval.parseNode=createNode("AND",yylineno,1,yytext);
            printf("AND\n");
            return AND;
}
{OR} {
            yylval.parseNode=createNode("OR",yylineno,1,yytext);
            printf("OR\n");
            return OR;
}
{DOT} {
            yylval.parseNode=createNode("DOT",yylineno,1,yytext);
            printf("DOT\n");
            return DOT;
}
{NOT} {
            yylval.parseNode=createNode("NOT",yylineno,1,yytext);
            printf("NOT\n");
            return NOT;
}
{TYPE} {
            yylval.parseNode=createNode("TYPE",yylineno,1,yytext);
            printf("TYPE\n");
            return TYPE;
}
{LP} {
            yylval.parseNode=createNode("LP",yylineno,1,yytext);
            printf("LP\n");
            return LP;
}
{RP} {
            yylval.parseNode=createNode("RP",yylineno,1,yytext);
            printf("RP\n");
            return RP;
}
{LB} {
            yylval.parseNode=createNode("LB",yylineno,1,yytext);
            printf("LB\n");
            return LB;
}
{RB} {
            yylval.parseNode=createNode("RB",yylineno,1,yytext);
            printf("RB\n");
            return RB;
}
{LC} {
            yylval.parseNode=createNode("LC",yylineno,1,yytext);
            printf("LC\n");
            return LC;
}
{RC} {
            yylval.parseNode=createNode("RC",yylineno,1,yytext);
            printf("RC\n");
            return RC;
}
{STRUCT} {
            yylval.parseNode=createNode("STRUCT",yylineno,1,yytext);
            printf("STRUCT\n");
            return STRUCT;
}
{RETURN} {
            yylval.parseNode=createNode("RETURN",yylineno,1,yytext);
            printf("RETURN\n");
            return RETURN;
}
{IF} {
            yylval.parseNode=createNode("IF",yylineno,1,yytext);
            printf("IF\n");
            return IF;
}
{ELSE} {
            yylval.parseNode=createNode("ELSE",yylineno,1,yytext);
            printf("ELSE\n");
            return ELSE;
}
{WHILE} {
            yylval.parseNode=createNode("WHILE",yylineno,1,yytext);
            printf("WHILE\n");
            return WHILE;
}
{ID} {
            yylval.parseNode=createNode("ID",yylineno,1,yytext);
            printf("ID\n");
            return ID;
}
{INT_ERROR} {
            yyerror("Invalid INT", yylineno, 'A',yytext);
            return INT_ERROR;
}
{FLOAT_ERROR} {
            yyerror("Invalid FLOAT", yylineno, 'A',yytext);
            return FLOAT_ERROR;
}

{ID_ERROR} {
            yyerror("Invalid ID", yylineno, 'A',yytext);
            return ID_ERROR;
}

{ERROR} {
            yyerror("Invalid character", yylineno, 'A',yytext);
            return ERROR;
}
%%