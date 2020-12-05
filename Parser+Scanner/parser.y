%output "parser.c"
%defines "parser.h"
%define parse.error verbose
%define parse.lac full

%{
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "tables.h"
#include "ast.h"
#include "parser.h"

int yylex();
int yylex_destroy(void);
void yyerror(const char *s);

AST* check_var(char* name);
AST* new_var(char* name, int size);

AST* check_func(char* name);
AST* new_func(char* name);

extern char *yytext;
extern int yylineno;

StrTable *st;
VarTable *vt;
FuncTable *ft;

Type func_type;

int scope = 0; /* contador de escopo para adição de variáveis na vt. */
int arity = 0; /* contador de aridade para adição de funções na ft. É resetado quando uma função é adicionada. */
int arguments = 0; /* contador de argumentos passados para uma função. É usado para checar se bate com a aridade. Também resetado após cada chamada de função. */

char* id = NULL;
char* prev_id; /* O acesso de uma posição de vetor através de variável requer dois ids, por isso esse ponteiro auxiliar guarda o id anterior. */
int num;
char* func_id;

AST* root = NULL;

%}

%define api.value.type {AST*}

%token ELSE IF INPUT INT OUTPUT RETURN VOID WHILE WRITE
%token SEMI COMMA LPAREN RPAREN LBRACK RBRACK LBRACE RBRACE
%token ASSIGN
%token LT LE GT GE EQ NEQ

%token ID
%token NUM

%token STRING

%left PLUS MINUS
%left TIMES OVER

%start program

%%

program:
  func_decl_list { root = $1; }
;

func_decl_list:
  func_decl                { $$ = new_subtree(FUNC_LIST_NODE, 1, $1); scope++; }
| func_decl_list func_decl { add_child($1, $2); $$ = $1; scope++; }
;

func_decl:
  func_header func_body   { $$ = new_subtree(FUNCTION_DECL_NODE, 2, $1, $2); }
;

func_header:
  ret_type ID { func_id = id; } LPAREN params RPAREN { $$ = new_subtree(FUNCTION_HEADER_NODE, 2, new_func(func_id), $5); }
;

func_body:
  LBRACE opt_var_decl opt_stmt_list RBRACE { $$ = new_subtree(FUNCTION_BODY_NODE, 2, $2, $3); }
;

opt_var_decl:
  %empty          { $$ = new_subtree(VAR_LIST_NODE, 0); }
| var_decl_list   { $$ = $1; }
;

opt_stmt_list:
  %empty          { $$ = new_subtree(BLOCK_NODE, 0); }
| stmt_list       { $$ = $1; }
;

ret_type:
  INT   { func_type = INT_TYPE; }
| VOID  { func_type = VOID_TYPE; }
;

params:
  VOID            { $$ = new_subtree(PARAM_LIST_NODE, 0); }
| param_list      { $$ = $1; }
;

param_list:
  param                   { $$ = new_subtree(PARAM_LIST_NODE, 1, $1); }
| param_list COMMA param  { add_child($1, $3); $$ = $1; }
;

param:
  INT ID { $$ = new_var(id, 0); arity++; }
| INT ID LBRACK RBRACK { $$ = new_var(id, -1); arity++; }
;

var_decl_list:
  var_decl                  { $$ = new_subtree(VAR_LIST_NODE, 1, $1); }
| var_decl_list var_decl    { add_child($1, $2); $$ = $1; }
;

var_decl:
  INT ID { $2 = new_var(id, 0); } SEMI { $$ = $2; }
| INT ID LBRACK NUM RBRACK { $2 = new_var(id, num); } SEMI { $$ = $2; }
;

stmt_list:
  stmt            { $$ = new_subtree(BLOCK_NODE, 1, $1); }
| stmt_list stmt  { add_child($1, $2); $$ = $1; }
;

stmt:
  assign_stmt     { $$ = $1; }
| if_stmt         { $$ = $1; }
| while_stmt      { $$ = $1; }
| return_stmt     { $$ = $1; }
| func_call SEMI  { $$ = $1; }
;

assign_stmt:
  lval ASSIGN arith_expr SEMI { $$ = new_subtree(ASSIGN_NODE, 2, $1, $3); }
;

lval:
  ID { $$ = check_var(id); }
| ID LBRACK NUM RBRACK { $$ = check_var(id); }
| ID LBRACK ID { $1 = check_var(prev_id); $2 = check_var(id); add_child($1, $2); $$ = $1; } RBRACK
;

if_stmt:
  IF LPAREN bool_expr RPAREN block            { $$ = new_subtree(IF_NODE, 2, $3, $5); }
| IF LPAREN bool_expr RPAREN block ELSE block { $$ = new_subtree(IF_NODE, 3, $3, $5, $7); }
;

block:
  LBRACE opt_stmt_list RBRACE                  { $$ = $2; }
;

while_stmt:
  WHILE LPAREN bool_expr RPAREN block          { $$ = new_subtree(WHILE_NODE, 2, $3, $5); }
;

return_stmt:
  RETURN SEMI                                  { $$ = new_subtree(RETURN_NODE, 0); }
| RETURN arith_expr SEMI                       { $$ = new_subtree(RETURN_NODE, 1, $2); }
;

func_call:
  output_call                     { $$ = $1; }
| write_call                      { $$ = $1; }
| user_func_call                  { $$ = $1; }
;

input_call:
  INPUT LPAREN RPAREN             { $$ = new_subtree(INPUT_NODE, 0); }
;

output_call:
  OUTPUT LPAREN arith_expr RPAREN { $$ = new_subtree(OUTPUT_NODE, 1, $3); }
;

write_call:
  WRITE LPAREN STRING RPAREN      { $$ = new_subtree(WRITE_NODE, 1, $3); }
;

user_func_call:
  ID {func_id = id; } LPAREN opt_arg_list RPAREN { $1 = check_func(func_id); add_child($1, $4); $$ = $1; arguments = 0; }
;

opt_arg_list:
  %empty          { $$ = new_subtree(ARG_LIST_NODE, 0); }
| arg_list        { $$ = $1; }
;

arg_list:
  arith_expr                { $$ = new_subtree(ARG_LIST_NODE, 1, $1); arguments++; }
| arg_list COMMA arith_expr { add_child($1, $3); $$ = $1; arguments++; } 
;

bool_expr:
  arith_expr LT arith_expr  { $$ = new_subtree(LT_NODE, 2, $1, $3); }
| arith_expr LE arith_expr  { $$ = new_subtree(LE_NODE, 2, $1, $3); }
| arith_expr GT arith_expr  { $$ = new_subtree(GT_NODE, 2, $1, $3); }
| arith_expr GE arith_expr  { $$ = new_subtree(GE_NODE, 2, $1, $3); }
| arith_expr EQ arith_expr  { $$ = new_subtree(EQ_NODE, 2, $1, $3); }
| arith_expr NEQ arith_expr { $$ = new_subtree(NEQ_NODE, 2, $1, $3); }
;

arith_expr:
  arith_expr PLUS arith_expr    { $$ = new_subtree(PLUS_NODE, 2, $1, $3); }
| arith_expr MINUS arith_expr   { $$ = new_subtree(MINUS_NODE, 2, $1, $3); }
| arith_expr TIMES arith_expr   { $$ = new_subtree(TIMES_NODE, 2, $1, $3); }
| arith_expr OVER arith_expr    { $$ = new_subtree(OVER_NODE, 2, $1, $3); }
| LPAREN arith_expr RPAREN      { $$ = $2; }
| lval                          { $$ = $1; }
| input_call                    { $$ = $1; }
| user_func_call                { $$ = $1; }
| NUM                           { $$ = $1; }
;

%%

AST* check_var(char* name) {
    int idx = lookup_var(vt, name, scope);
    if (idx == -1) {
        printf("SEMANTIC ERROR (%d): variable '%s' was not declared.\n",
                yylineno, name);
        exit(EXIT_FAILURE);
    }
    return new_node(VAR_USE_NODE, idx);
}

AST* new_var(char* name, int size) {
    int idx = lookup_var(vt, name, scope);
    if (idx != -1) {
        printf("SEMANTIC ERROR (%d): variable '%s' already declared at line %d.\n",
                yylineno, name, get_line(vt, idx));
        exit(EXIT_FAILURE);
    }
    idx = add_var(vt, name, yylineno, scope, size);
    return new_node(VAR_DECL_NODE, idx);
}

AST* check_func(char* name) {
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
  return new_node(FUNCTION_CALL_NODE, idx);
}

AST* new_func(char* name) {
    int idx = lookup_func(ft, name);
    if (idx != -1) {
        printf("SEMANTIC ERROR (%d): function '%s' already declared at line %d.\n",
                yylineno, name, get_func_line(ft, idx));
        exit(EXIT_FAILURE);
    }
    idx = add_func(ft, name, yylineno, arity, func_type);
    arity = 0;
    return new_node(FUNCTION_NAME_NODE, idx);
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
    
    print_dot(root);

    free_str_table(st);
    free_var_table(vt);
    free_func_table(ft);
    free_tree(root);
    yylex_destroy();    // To avoid memory leaks within flex...

    return 0;
}
