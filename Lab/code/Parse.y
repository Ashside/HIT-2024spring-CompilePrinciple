%{
    #include<stdio.h>
    #include"SyntaxTree.h"
    #include"lex.yy.c"

    //TODO: add to elimate warnings
    extern int yylex();
    extern void yyerror(const char* msg);

    // report syntax error
    extern int SynError;

    // root of ast declared here
    NodePtr RootNode;

    int yydebug = 1;
%}
%locations
%define parse.error verbose
// yylval union
%union{
    NodePtr node; 
}

// terminals
%token <node> INT FLOAT ID
%token <node> IF ELSE WHILE TYPE STRUCT RETURN
%token <node> RELOP
%token <node> PLUS MINUS STAR DIV AND OR NOT
%token <node> DOT SEMI COMMA ASSIGNOP
%token <node> LP RP LB RB LC RC



// non-terminals

%type <node> Program ExtDefList ExtDef ExtDecList   //  High-level Definitions
%type <node> Specifier StructSpecifier OptTag Tag   //  Specifiers
%type <node> VarDec FunDec VarList ParamDec         //  Declarators
%type <node> CompSt StmtList Stmt                   //  Statements
%type <node> DefList Def Dec DecList                //  Local Definitions
%type <node> Exp Args                               //  Expressions

// precedence and associativity

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left DOT
%left LB RB
%left LP RP
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
// High-level Definitions
Program: ExtDefList {$$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Program", 1, $1);  RootNode = $$;}
; 
ExtDefList: ExtDef ExtDefList { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "ExtDefList", 2, $1, $2); }
| { $$ = NULL; } 
;

ExtDef: Specifier ExtDecList SEMI { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "ExtDef", 3, $1, $2, $3); }
| Specifier SEMI { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "ExtDef", 2, $1, $2); }
| Specifier FunDec CompSt { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "ExtDef", 3, $1, $2, $3); }
;

ExtDecList: VarDec { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "ExtDecList", 1, $1); }
| VarDec COMMA ExtDecList { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "ExtDecList", 3, $1, $2, $3); }
;

// Specifiers
Specifier: TYPE { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Specifier", 1, $1); }
| StructSpecifier { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Specifier", 1, $1); }
;

StructSpecifier: STRUCT OptTag LC DefList RC { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "StructSpecifier", 5, $1, $2, $3, $4, $5); }
| STRUCT Tag { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "StructSpecifier", 2, $1, $2); }
;

OptTag: ID { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "OptTag", 1, $1); }
| { $$ = NULL; }
;

Tag: ID { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Tag", 1, $1); }
;

// Declarators
VarDec: ID { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "VarDec", 1, $1); }
| VarDec LB INT RB { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "VarDec", 4, $1, $2, $3, $4); }
| error RB { SynError = TRUE; yyerrok;}
;

FunDec: ID LP VarList RP { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "FunDec", 4, $1, $2, $3, $4); }
| ID LP RP { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "FunDec", 3, $1, $2, $3); }
| error RP { SynError = TRUE; yyerrok;}
;

VarList: ParamDec COMMA VarList { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "VarList", 3, $1, $2, $3); }
| ParamDec { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "VarList", 1, $1); }

ParamDec: Specifier VarDec { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "ParamDec", 2, $1, $2); }
;

// Statements
CompSt: LC DefList StmtList RC { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "CompSt", 4, $1, $2, $3, $4); }
| error RC { SynError = TRUE; yyerrok;}
;

StmtList: Stmt StmtList { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "StmtList", 2, $1, $2); }
| { $$ = NULL; }
;

Stmt: Exp SEMI { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Stmt", 2, $1, $2); }
| CompSt { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Stmt", 1, $1); }
| RETURN Exp SEMI { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Stmt", 3, $1, $2, $3); }   
| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Stmt", 5, $1, $2, $3, $4, $5); }
| IF LP Exp RP Stmt ELSE Stmt { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Stmt", 7, $1, $2, $3, $4, $5, $6, $7); }
| WHILE LP Exp RP Stmt { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Stmt", 5, $1, $2, $3, $4, $5); }
| error SEMI { SynError = TRUE; yyerrok;}
;

// Local Definitions
DefList: Def DefList { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "DefList", 2, $1, $2); }
| { $$ = NULL; }
;

Def: Specifier DecList SEMI { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Def", 3, $1, $2, $3); }
;

DecList: Dec { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "DecList", 1, $1); }
| Dec COMMA DecList { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "DecList", 3, $1, $2, $3); }
;

Dec: VarDec { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Dec", 1, $1); }
| VarDec ASSIGNOP Exp { $$ = newParseNode(TOKEN_NOTTOKEN,@$.first_line, "Dec", 3, $1, $2, $3); }
;

// Expressions
Exp: Exp ASSIGNOP Exp { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 3, $1, $2, $3); }
| Exp AND Exp { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 3, $1, $2, $3); }
| Exp OR Exp { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 3, $1, $2, $3); }
| Exp RELOP Exp { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 3, $1, $2, $3); }
| Exp PLUS Exp { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 3, $1, $2, $3); }
| Exp MINUS Exp { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 3, $1, $2, $3); }
| Exp STAR Exp { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 3, $1, $2, $3); }
| Exp DIV Exp { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 3, $1, $2, $3); }
| LP Exp RP { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 3, $1, $2, $3); }
| MINUS Exp { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 2, $1, $2); }
| NOT Exp { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 2, $1, $2); }
| ID LP Args RP { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 4, $1, $2, $3, $4); }
| ID LP RP { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 3, $1, $2, $3); }
| Exp LB Exp RB { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 4, $1, $2, $3, $4); }
| Exp DOT ID { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 3, $1, $2, $3); }
| ID { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 1, $1); }
| INT { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 1, $1); }
| FLOAT { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Exp", 1, $1); }
| error RP{ SynError = TRUE; yyerrok;}
;

Args : Exp COMMA Args { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Args", 3, $1, $2, $3); }
| Exp { $$ = newParseNode(TOKEN_NOTTOKEN, @$.first_line, "Args", 1, $1); }
;

%%

void yyerror(const char* msg){
    printf("Error type B at Line %d Column %d: %s.\n", yylineno,yycolumn, msg);
}
