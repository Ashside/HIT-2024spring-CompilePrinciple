%{
    //#include "lex.yy.c"
    #include "stdio.h"
    #include "stddef.h"
    #include "SyntaxTree.h"
    #include "Parse.tab.h"
    extern int yylex(void); 
    extern int yyparse(void); 
    extern void yyerror(const char *s);
    int yydebug = 1;
    AstNode* root = NULL;

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
%token <parseNode> PLUS
%token <parseNode> MINUS
%token <parseNode> STAR
%token <parseNode> DIV
%token <parseNode> AND
%token <parseNode> OR
%token <parseNode> DOT
%token <parseNode> NOT
%token <parseNode> RELOP
%token <parseNode> LP
%token <parseNode> RP
%token <parseNode> LB
%token <parseNode> RB
%token <parseNode> LC
%token <parseNode> RC
%token <parseNode> TYPE
%token <parseNode> STRUCT
%token <parseNode> RETURN
%token <parseNode> IF
%token <parseNode> ELSE
%token <parseNode> WHILE
%token <parseNode> INT_ERROR
%token <parseNode> FLOAT_ERROR
%token <parseNode> ID_ERROR
%token <parseNode> ERROR

%token <parseNode> NOTATOKEN

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
%nonassoc ELSE ERROR                             
%%
// HIGH LEVEL GRAMMAR
Program:
    ExtDefList  {printf("Hello");$$ = createNode("PROGRAM", yylineno,0," "); addChild($$,1,$1); root = $$;}                 
    ;

ExtDefList:
    ExtDef ExtDefList {$$ = createNode("EXTDEFLIST", yylineno,0," "); addChild($$,2,$1,$2);}       
    | /* empty */ {$$ = createNode("EXTDEFLIST", -1 ,0," ");}                  
    ;

ExtDef:
    Specifier ExtDecList SEMI {$$ = createNode("EXTDEF", yylineno,0," "); addChild($$,3,$1,$2,$3);}      
    |Specifier SEMI           {$$ = createNode("EXTDEF", yylineno,0," "); addChild($$,2,$1,$2);}     
    |Specifier FunDec CompStm {$$ = createNode("EXTDEF", yylineno,0," "); addChild($$,3,$1,$2,$3);}      
    |error SEMI               {}      
    ;

ExtDecList:
    VarDec                    {$$ = createNode("EXTDECLIST", yylineno,0," "); addChild($$,1,$1);}      
    |VarDec COMMA ExtDecList  {$$ = createNode("EXTDECLIST", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    ;

//SPECIFIER
Specifier:
    TYPE                        {$$ = createNode("SPECIFIER", yylineno,0," "); addChild($$,1,$1);}
    |StructSpecifier            {$$ = createNode("SPECIFIER", yylineno,0," "); addChild($$,1,$1);}
    ;

StructSpecifier:
    STRUCT OptTag LC DefList RC {$$ = createNode("STRUCTSPECIFIER", yylineno,0," "); addChild($$,5,$1,$2,$3,$4,$5);}
    |STRUCT Tag                 {$$ = createNode("STRUCTSPECIFIER", yylineno,0," "); addChild($$,2,$1,$2);}
    ;

OptTag:
    ID                          {$$ = createNode("OPTTAG", yylineno,0," "); addChild($$,1,$1);}
    |/* empty */                {$$ = createNode("OPTTAG", -1 ,0," ");}
    ;

Tag:
    ID                          {$$ = createNode("TAG", yylineno,0," "); addChild($$,1,$1);}
    ;

//DECLARATION
VarDec:
    ID                          {$$ = createNode("VARDEC", yylineno,0," "); addChild($$,1,$1);}
    |VarDec LB INT RB           {$$ = createNode("VARDEC", yylineno,0," "); addChild($$,4,$1,$2,$3,$4);}
    ;

FunDec:
    ID LP VarList RP            {$$ = createNode("FUNDEC", yylineno,0," "); addChild($$,4,$1,$2,$3,$4);}
    |ID LP RP                   {$$ = createNode("FUNDEC", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    ;

VarList:
    ParamDec COMMA VarList      {$$ = createNode("VARLIST", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |ParamDec                   {$$ = createNode("VARLIST", yylineno,0," "); addChild($$,1,$1);}
    ;

ParamDec:
    Specifier VarDec            {$$ = createNode("PARAMDEC", yylineno,0," "); addChild($$,2,$1,$2);}
    ;

//STATEMENT
CompStm:
    LC DefList StmtList RC      {$$ = createNode("COMPSTM", yylineno,0," "); addChild($$,4,$1,$2,$3,$4);}
    ;

StmtList:
    Stmt StmtList               {$$ = createNode("STMTLIST", yylineno,0," "); addChild($$,2,$1,$2);}
    |/* empty */                {$$ = createNode("STMTLIST", -1 ,0," ");}
    ;

Stmt:
    Exp SEMI                    {$$ = createNode("STMT", yylineno,0," "); addChild($$,2,$1,$2);}
    |CompStm                    {$$ = createNode("STMT", yylineno,0," "); addChild($$,1,$1);}
    |RETURN Exp SEMI            {$$ = createNode("STMT", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |IF LP Exp RP Stmt          {$$ = createNode("STMT", yylineno,0," "); addChild($$,4,$1,$2,$3,$4);}
    |IF LP Exp RP Stmt ELSE Stmt    {$$ = createNode("STMT", yylineno,0," "); addChild($$,6,$1,$2,$3,$4,$5,$6);}
    |WHILE LP Exp RP Stmt       {$$ = createNode("STMT", yylineno,0," "); addChild($$,5,$1,$2,$3,$4,$5);}
    |error SEMI                 {}
    ;

//LOCAL DEFINITION
DefList:
    Def DefList                 {$$ = createNode("DEFLIST", yylineno,0," "); addChild($$,2,$1,$2);}
    |/* empty */                {$$ = createNode("DEFLIST", -1 ,0," ");}
    ;

Def:
    Specifier DecList SEMI      {$$ = createNode("DEF", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    ;

DecList:
    Dec                         {$$ = createNode("DECLIST", yylineno,0," "); addChild($$,1,$1);}
    |Dec COMMA DecList          {$$ = createNode("DECLIST", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    ;

Dec:
    VarDec                      {$$ = createNode("DEC", yylineno,0," "); addChild($$,1,$1);}
    |VarDec ASSIGNOP Exp        {$$ = createNode("DEC", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    ;

//EXPRESSION

Exp:
    Exp ASSIGNOP Exp            {$$ = createNode("EXP", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |Exp AND Exp                {$$ = createNode("EXP", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |Exp OR Exp                 {$$ = createNode("EXP", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |Exp RELOP Exp            {$$ = createNode("EXP", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |Exp PLUS Exp            {$$ = createNode("EXP", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |Exp MINUS Exp          {$$ = createNode("EXP", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |Exp STAR Exp           {$$ = createNode("EXP", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |Exp DIV Exp            {$$ = createNode("EXP", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |LP Exp RP              {$$ = createNode("EXP", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |MINUS Exp              {$$ = createNode("EXP", yylineno,0," "); addChild($$,2,$1,$2);}
    |NOT Exp                {$$ = createNode("EXP", yylineno,0," "); addChild($$,2,$1,$2);}
    |ID LP Args RP          {$$ = createNode("EXP", yylineno,0," "); addChild($$,4,$1,$2,$3,$4);}
    |ID LP RP               {$$ = createNode("EXP", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |Exp LB Exp RB          {$$ = createNode("EXP", yylineno,0," "); addChild($$,4,$1,$2,$3,$4);}
    |Exp DOT ID             {$$ = createNode("EXP", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |ID                     {$$ = createNode("EXP", yylineno,0," "); addChild($$,1,$1);}
    |INT                    {$$ = createNode("EXP", yylineno,0," "); addChild($$,1,$1);}
    |FLOAT                  {$$ = createNode("EXP", yylineno,0," "); addChild($$,1,$1);}
    ;

Args:
    Exp COMMA Args          {$$ = createNode("ARGS", yylineno,0," "); addChild($$,3,$1,$2,$3);}
    |Exp                    {$$ = createNode("ARGS", yylineno,0," "); addChild($$,1,$1);}
    ;

%%