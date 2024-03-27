%{
    //#include "lex.yy.c"
    #include "SyntaxTree.h"
    int yyerror(char* msg);
%}

%union {
    AstNode* parseNode;
}

%token <parseNode> INT
%token <parseNode> FLOAT
%token <parseNode> ID
%token <parseNode> SEMI
%token <parseNode> COMMA
%token <parseNode> ASSIGNOP
%token <parseNode> RELOP
%token <parseNode> PLUS
%token <parseNode> MINUS
%token <parseNode> STAR
%token <parseNode> DIV
%token <parseNode> AND
%token <parseNode> OR
%token <parseNode> DOT
%token <parseNode> NOT
%token <parseNode> TYPE
%token <parseNode> LP
%token <parseNode> RP
%token <parseNode> LB
%token <parseNode> RB
%token <parseNode> LC
%token <parseNode> RC
%token <parseNode> STRUCT
%token <parseNode> RETURN
%token <parseNode> IF
%token <parseNode> ELSE
%token <parseNode> WHILE

%type <parseNode> Program ExtDefList ExtDef ExtDecList
%type <parseNode> Specifier StructSpecifier OptTag Tag
%type <parseNode> VarDec FunDec VarList ParamDec
%type <parseNode> CompStm StmtList Stmt
%type <parseNode> DefList Def DecList Dec
%type <parseNode> Exp Args

// Define the precedence and associativity of the operators
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT                      /* TODO: 取负操作呢？ */
%left DOT LP RP LB RB           /* TODO: 似乎{}也是左结合的 */
%nonassoc ELSE                              
%%
// HIGH LEVEL GRAMMAR
Program:
    ExtDefList                      
    ;

ExtDefList:
    ExtDef ExtDefList               
    | /* empty */                   
    ;

ExtDef:
    Specifier ExtDecList SEMI       
    |Specifier SEMI                 
    |Specifier FunDec CompStm       
    |error SEMI                     
    ;

ExtDecList:
    VarDec                          
    |VarDec COMMA ExtDecList
        ;

//SPECIFIER
Specifier:
    TYPE
    |StructSpecifier
    ;

StructSpecifier:
    STRUCT OptTag LC DefList RC
    |STRUCT Tag
    ;

OptTag:
    ID
    |/* empty */
    ;

Tag:
    ID
    ;

//DECLARATION
VarDec:
    ID
    |VarDec LB INT RB
    ;

FunDec:
    ID LP VarList RP
    |ID LP RP
    ;

VarList:
    ParamDec COMMA VarList
    |ParamDec
    ;

ParamDec:
    Specifier VarDec
    ;

//STATEMENT
CompStm:
    LC DefList StmtList RC
    ;

StmtList:
    Stmt StmtList
    |/* empty */
    ;

Stmt:
    Exp SEMI
    |CompStm
    |RETURN Exp SEMI
    |IF LP Exp RP Stmt
    |IF LP Exp RP Stmt ELSE Stmt
    |WHILE LP Exp RP Stmt
    |error SEMI
    ;

//LOCAL DEFINITION
DefList:
    Def DefList
    |/* empty */
    ;

Def:
    Specifier DecList SEMI
    ;

DecList:
    Dec
    |Dec COMMA DecList
    ;

Dec:
    VarDec
    |VarDec ASSIGNOP Exp
    ;

//EXPRESSION

Exp:
    Exp ASSIGNOP Exp
    |Exp AND Exp
    |Exp OR Exp
    |Exp RELOP Exp
    |Exp PLUS Exp
    |Exp MINUS Exp
    |Exp STAR Exp
    |Exp DIV Exp
    |LP Exp RP
    |MINUS Exp
    |NOT Exp
    |ID LP Args RP
    |ID LP RP
    |Exp LB Exp RB
    |Exp DOT ID
    |ID
    |INT
    |FLOAT
    ;

Args:
    Exp COMMA Args
    |Exp
    ;

%%