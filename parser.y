%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "node.h"

nodeType *opr(int oper, int nops, ...);
nodeType *id(int i);
nodeType *con(char* value);
void freeNode(nodeType *p);
char* ex(nodeType *p);
int yylex(void);
void yyerror(char *s);

char* sym[52]; /* symbol table */
%}

%union {
    char* iValue; /* EXPR value */
    char sIndex; /* symbol table index */
    nodeType *nPtr; /* node pointer */
};

%token PRINT EQ NEQ LE GE GT LT INC DEC
%token <iValue> STRING NUMBER
%token <sIndex> VAR

%right '='
%right '^' '~' INC DEC EQ NEQ LE GE GT LT
%left '+' '-'
%left '*' '/' '#'
%nonassoc '(' ')'

%type <nPtr> program stmt stmt_list assignment assignment_list print_stmt expression

%%

program:
    stmt_list
    ;

stmt_list:
    stmt
    | stmt_list stmt
    ;

stmt:
    assignment_list ';' { ex($1); freeNode($1); }
    | print_stmt ';'
    | expression ';' { ex($1); freeNode($1); }
    ;

print_stmt:
    PRINT expression { $$ = opr(PRINT, 1, $2); ex($$); freeNode($$); }
    ;

assignment_list:
    assignment
    | assignment_list ',' assignment { $$ = opr(',', 2, $1, $3); }
    ;

assignment:
    VAR '=' expression { $$ = opr('=', 2, id($1), $3); }
    ;

expression:
    STRING { $$ = con($1); }
    | VAR { $$ = id($1); }
	| NUMBER { $$ = con($1); }
	| expression '*' expression { $$ = opr('*', 2, $1, $3); }
    | expression '+' expression { $$ = opr('+', 2, $1, $3); }
    | expression '-' expression { $$ = opr('-', 2, $1, $3); }
    | expression LT expression { $$ = opr(LT, 2, $1, $3); }
    | expression GT expression { $$ = opr(GT, 2, $1, $3); }
    | expression LE expression { $$ = opr(LE, 2, $1, $3); }
    | expression GE expression { $$ = opr(GE, 2, $1, $3); }
    | expression NEQ expression { $$ = opr(NEQ, 2, $1, $3); }
    | expression EQ expression { $$ = opr(EQ, 2, $1, $3); }
    | expression '#' expression { $$ = opr('#', 2, $1, $3); }
    | expression '/' expression { $$ = opr('/', 2, $1, $3); }
    | INC expression { $$ = opr(INC, 1, $2); }   /* Pre-increment */
    | DEC expression { $$ = opr(DEC, 1, $2); }   /* Pre-decrement */
    | expression INC { $$ = opr('P', 1, $1); }   /* Post-increment */
    | expression DEC { $$ = opr('Q', 1, $1); }   /* Post-decrement */
    | '~' expression { $$ = opr('~', 1, $2); }   /* Unary length */
    | '^' expression { $$ = opr('^', 1, $2); }   /* Unary case toggle */
    | '(' expression ')' { $$ = $2; } // Handle parentheses
    ;

%%

nodeType *con(char* value) {
    nodeType *p;
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");
    p->con.val = strdup(value);
    p->type = typeCon;
    return p;
}

nodeType *id(int i) {
    nodeType *p;
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");
    p->type = typeId;
    p->id.i = i;
    return p;
}

nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    int i;
    if ((p = malloc(sizeof(nodeType) + (nops-1) * sizeof(nodeType *))) == NULL)
        yyerror("out of memory");
    p->type = typeOper;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOper) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }

    free(p);
}

void yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
}
