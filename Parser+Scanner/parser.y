%output "parser.c"
%defines "parser.h"
%define parse.error verbose
%define parse.lac full

%{
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "tables.h"
#include "parser.h"

int yylex();
int yylex_destroy(void);
void yyerror(const char *s);

void check_var(char* name);
void new_var(char* name, int size);

void check_func(char* name);
void new_func(char* name);

extern char *yytext;
extern int yylineno;

StrTable *st;
VarTable *vt;
FuncTable *ft;

Type last_decl_type;

int scope = 0; /* contador de escopo para adição de variáveis na vt. */
int arity = 0; /* contador de aridade para adição de funções na ft. É resetado quando uma função é adicionada. */
int arguments = 0; /* contador de argumentos passados para uma função. É usado para checar se bate com a aridade. Também resetado após cada chamada de função. */

%}

%token ELSE IF INPUT INT OUTPUT RETURN VOID WHILE WRITE
%token SEMI COMMA LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE
%token ASSIGN
%token LT LE GT GE EQ NEQ

%union {
    char *str;
    int number;
}

%token <str> ID
%token <number> NUM

%token STRING

%left PLUS MINUS
%left TIMES OVER

%start program

%%

program:
  func_decl_list
;

func_decl_list:
  func_decl_list func_decl
| func_decl { scope++; }
;

func_decl:
  func_header func_body
;

func_header:
  ret_type ID LPAREN params RPAREN { new_func($2); }
;

func_body:
  LBRACE opt_var_decl opt_stmt_list RBRACE
;

opt_var_decl:
  %empty
| var_decl_list
;

opt_stmt_list:
  %empty
| stmt_list
;

ret_type:
  INT
| VOID
;

params:
  VOID
| param_list
;

param_list:
  param_list COMMA param
| param
;

param:
  INT ID { new_var($2, 0); arity++; }
| INT ID LBRACK RBRACK { new_var($2, -1); arity++; }
;

var_decl_list:
  var_decl_list var_decl
| var_decl
;

var_decl:
  INT ID { new_var($2, 0); } SEMI
| INT ID LBRACK NUM RBRACK { new_var($2, $4); } SEMI
;

stmt_list:
  stmt_list stmt
| stmt
;

stmt:
  assign_stmt
| if_stmt
| while_stmt
| return_stmt
| func_call SEMI
;

assign_stmt:
  lval ASSIGN arith_expr SEMI
;

lval:
  ID { check_var($1); }
| ID LBRACK NUM RBRACK
| ID LBRACK ID { check_var($3); } RBRACK
;

if_stmt:
  IF LPAREN bool_expr RPAREN block
| IF LPAREN bool_expr RPAREN block ELSE block
;

block:
  LBRACE opt_stmt_list RBRACE
;

while_stmt:
  WHILE LPAREN bool_expr RPAREN block
;

return_stmt:
  RETURN SEMI
| RETURN arith_expr SEMI
;

func_call:
  output_call
| write_call
| user_func_call
;

input_call:
  INPUT LPAREN RPAREN
;

output_call:
  OUTPUT LPAREN arith_expr RPAREN
;

write_call:
  WRITE LPAREN STRING RPAREN
;

user_func_call:
  ID LPAREN opt_arg_list RPAREN { check_func($1); arguments = 0; }
;

opt_arg_list:
  %empty
| arg_list
;

arg_list:
  arg_list COMMA arith_expr { arguments++; }
| arith_expr                { arguments++; }
;

bool_expr:
  arith_expr LT arith_expr
| arith_expr LE arith_expr
| arith_expr GT arith_expr
| arith_expr GE arith_expr
| arith_expr EQ arith_expr
| arith_expr NEQ arith_expr
;

arith_expr:
  arith_expr PLUS arith_expr
| arith_expr MINUS arith_expr
| arith_expr TIMES arith_expr
| arith_expr OVER arith_expr
| LPAREN arith_expr RPAREN
| lval
| input_call
| user_func_call
| NUM
;

%%

void check_var(char* name) {
    int var_scope = -1;
    int idx = lookup_var(vt, name, &var_scope);
    if (idx == -1) {
        printf("SEMANTIC ERROR (%d): variable '%s' was not declared.\n",
                yylineno, name);
        exit(EXIT_FAILURE);
    }
}

void new_var(char* name, int size) {
    int var_scope = -1;
    int idx = lookup_var(vt, name, &var_scope);
    if (idx != -1 && var_scope == scope) {
        printf("SEMANTIC ERROR (%d): variable '%s' already declared at line %d.\n",
                yylineno, name, get_line(vt, idx));
        exit(EXIT_FAILURE);
    }
    add_var(vt, name, yylineno, scope, size);
}

void check_func(char* name) {
  int idx = lookup_func(ft, name);
  if (idx == -1) {
    printf("SEMANTIC ERROR (%d): function '%s' was not declared.\n", yylineno, name);
    exit(EXIT_FAILURE);
  }
  else{
    int expected_arity = get_func_arity(ft, idx);
    if(arguments != expected_arity){
        printf("SEMANTIC ERROR (%d): function '%s' was called with %d arguments but declared with %d parameters.\n", yylineno, name, arguments, expected_arity);
        exit(EXIT_FAILURE);
      }
    }
}

void new_func(char* name) {
    int idx = lookup_func(ft, name);
    if (idx != -1) {
        printf("SEMANTIC ERROR (%d): function '%s' already declared at line %d.\n",
                yylineno, name, get_func_line(ft, idx));
        exit(EXIT_FAILURE);
    }
    add_func(ft, name, yylineno, arity);
    arity = 0;
}

// Error handling.
void yyerror (char const *s) {
    printf("PARSE ERROR (%d): %s\n", yylineno, s);
    exit(EXIT_FAILURE);
}

// Main.
int main() {
    st = create_str_table();
    vt = create_var_table();
    ft = create_func_table();

    yyparse();
    printf("PARSE SUCCESSFUL!\n");

    printf("\n\n");
    print_str_table(st); printf("\n\n");
    print_var_table(vt); printf("\n\n");
    print_func_table(ft); printf("\n\n");
    

    free_str_table(st);
    free_var_table(vt);
    free_func_table(ft);
    yylex_destroy();    // To avoid memory leaks within flex...

    return 0;
}
