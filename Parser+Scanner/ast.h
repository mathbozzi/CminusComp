#ifndef AST_H
#define AST_H

#include "types.h"

typedef enum {
    ARG_LIST_NODE,
    ASSIGN_NODE,
    EQ_NODE,
    BLOCK_NODE,
    IF_NODE,
    INPUT_NODE,
    INT_VAL_NODE,
    LE_NODE,
    LT_NODE,
    MINUS_NODE,
    NEQ_NODE,
    OUTPUT_NODE,
    OVER_NODE,
    PLUS_NODE,
    STR_VAL_NODE,
    TIMES_NODE,
    VAR_DECL_NODE,
    VAR_LIST_NODE,
    VAR_USE_NODE,
    WRITE_NODE,
    FUNCTION_CALL_NODE,
    FUNCTION_DECL_NODE,
    FUNC_LIST_NODE,
    FUNCTION_NAME_NODE,
    FUNCTION_HEADER_NODE,
    FUNCTION_BODY_NODE,
    GE_NODE,
    GT_NODE,
    PARAM_LIST_NODE,
    STATEMENT_LIST_NODE,
    WHILE_NODE,
    RETURN_NODE,
} NodeKind;

struct node; // Opaque structure to ensure encapsulation.

typedef struct node AST;

AST* new_node(NodeKind kind, int data);

void add_child(AST *parent, AST *child);
AST* get_child(AST *parent, int idx);

AST* new_subtree(NodeKind kind, int child_count, ...);

NodeKind get_kind(AST *node);
char* kind2str(NodeKind kind);

int get_data(AST *node);
int get_child_count(AST *node);

void print_tree(AST *ast);
void print_dot(AST *ast);

void free_tree(AST *ast);

#endif
