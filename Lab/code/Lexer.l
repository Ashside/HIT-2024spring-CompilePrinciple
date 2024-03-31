%{
#include "SyntaxTree.h"
#include "Parse.tab.h"

extern int LexError;

int lines = 0;
int yycolumn = 1;

#define YY_USER_ACTION \
    yylloc.first_line=yylloc.last_line=yylineno; \
    yylloc.first_column=yycolumn; \
    yylloc.last_column=yycolumn+yyleng-1; \
    yycolumn+=yyleng;

%}

%option yylineno


SPACE [ \t]
ENTER [\n]

COMMENT_BLK_START [/][*]
COMMENT_BLK_END [*][/]
COMMENT_NOT_STAR [^*]
COMMENT_BLK {COMMENT_BLK_START}{COMMENT_NOT_STAR}*[*]+([^*/][^*]*[*]+)*[/]


/**/

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


ID_ERROR [0-9][a-zA-Z0-9_]* 
INTOCT_ERROR [+-]?0[0-7]*[89][0-9]* 
INTHEX_ERROR [+-]?0[xX][0-9a-fA-F]*[g-zG-Z]*[0-9a-zA-Z]* 
INTDEC_ERROR [+-]?[1-9][0-9]*[a-zA-Z]* 
FLOAT_ERROR ({INTDEC}\.[0-9]*[a-zA-Z]+)|(([+-]?[0-9]*\.[0-9]*|[0-9]*\.[0-9]*)[Ee][+-]?[0-9]*[a-zA-Z]+) 
INT_ERROR {INTDEC_ERROR}|{INTOCT_ERROR}|{INTHEX_ERROR} 

ERROR .
%%
{SPACE} { }

{ENTER} {yycolumn=1;}

"//" {
            while (input() != '\n') ;
            
}

{COMMENT_BLK} {/*give up solving nested comment*/ }
{COMMENT_BLK_START} {printf("Error type B at Line %d: Unclosed comment.\n", yylineno);}
{COMMENT_BLK_END} {printf("Error type B at Line %d: Unexpected comment end.\n", yylineno);}


{IF} {
            //printf("IF\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"IF",yytext);
            return IF;
}
{ELSE} {
            //printf("ELSE\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"ELSE",yytext);
            return ELSE;
}
{WHILE} {
            //printf("WHILE\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"WHILE",yytext);
            return WHILE;
}



{TYPE} {
            //printf("TYPE\n");
            yylval.node = newTokenNode(TOKEN_TYPE,yylineno,"TYPE",yytext);
            return TYPE;
}
{STRUCT} {
            //printf("STRUCT\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"STRUCT",yytext);
            return STRUCT;
}
{RETURN} {
            //printf("RETURN\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"RETURN",yytext);
            return RETURN;
}



{RELOP} {
            ////printf("RELOP\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"RELOP",yytext);
            return RELOP;
}
{PLUS} {
            //printf("PLUS\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"PLUS",yytext);
            return PLUS;
}
{MINUS} {
            //printf("MINUS\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"MINUS",yytext);
            return MINUS;
}
{STAR} {
            //printf("STAR\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"STAR",yytext);
            return STAR;
}
{DIV} {
            //printf("DIV\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"DIV",yytext);
            return DIV;
}
{AND} {
            //printf("AND\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"AND",yytext);
            return AND;
}
{OR} {
            //printf("OR\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"OR",yytext);
            return OR;
}
{NOT} {
            //printf("NOT\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"NOT",yytext);
            return NOT;
}



{DOT} {
            //printf("DOT\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"DOT",yytext);
            return DOT;
}
{SEMI} {
            //printf("SEMI\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"SEMI",yytext);
            return SEMI;
}
{COMMA} {
            //printf("COMMA\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"COMMA",yytext);
            return COMMA;
}
{ASSIGNOP} {
            //printf("ASSIGNOP\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"ASSIGNOP",yytext);
            return ASSIGNOP;
}



{LP} {
            //printf("LP\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"LP",yytext);
            return LP;
}
{RP} {
            //printf("RP\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"RP",yytext);
            return RP;
}
{LB} {
            //printf("LB\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"LB",yytext);
            return LB;
}
{RB} {
            //printf("RB\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"RB",yytext);
            return RB;
}
{LC} {
            //printf("LC\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"LC",yytext);
            return LC;
}
{RC} {
            //printf("RC\n");
            yylval.node = newTokenNode(TOKEN_SYMBOL,yylineno,"RC",yytext);
            return RC;
}


{ID} {
            //printf("ID\n");
            yylval.node = newTokenNode(TOKEN_ID,yylineno,"ID",yytext);
            return ID;
}
{INT} {
            //printf("INT\n");
            yylval.node = newTokenNode(TOKEN_INT,yylineno,"INT",yytext);
            return INT;
}
{FLOAT} {
            //printf("FLOAT\n");
            yylval.node = newTokenNode(TOKEN_FLOAT,yylineno,"FLOAT",yytext);
            return FLOAT;
}

{INTOCT_ERROR} {
            LexError = TRUE;
            printf("Error type A at Line %d: Invalid OCT integer \"%s\".\n", yylineno, yytext);
}
{INTHEX_ERROR} {
            LexError = TRUE;
            printf("Error type A at Line %d: Invalid HEX integer \"%s\".\n", yylineno, yytext);
}
{INTDEC_ERROR} {
            LexError = TRUE;
            printf("Error type A at Line %d: Invalid DEC integer \"%s\".\n", yylineno, yytext);
}
{INT_ERROR} {
            LexError = TRUE;
            printf("Error type A at Line %d: Invalid integer \"%s\".\n", yylineno, yytext);
            //return ERROR;
}
{FLOAT_ERROR} {
            LexError = TRUE;
            printf("Error type A at Line %d: Invalid float \"%s\".\n", yylineno, yytext);
            //return ERROR;
}
{ID_ERROR} {
            LexError = TRUE;
            printf("Error type A at Line %d: Invalid identifier \"%s\".\n", yylineno, yytext);
            //return ERROR;
}
{ERROR} {
            LexError = TRUE;
            printf("Error type A at Line %d: Invalid character \"%s\".\n", yylineno, yytext);
            //return ERROR;
}
%%