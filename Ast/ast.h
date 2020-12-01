#ifndef AST_H
#define AST_H

#include "types.h"

typedef enum {
    ARG_LIST_NODE,
    ASSIGN_NODE,
    BLOCK_NODE,
    CVAR_NODE,
    EQ_NODE,
    FUNC_BODY_NODE,
    FUNC_CALL_NODE,
    FUNC_DCL_NODE,
    FUNC_HEADER_NODE,
    FUNC_LIST_NODE,
    GE_NODE,
    GT_NODE,
    ID_NODE,
    IF_NODE,
    INPUT_NODE,
    INT_NODE,
    LE_NODE,
    LT_NODE,
    MINUS_NODE,
    NEQ_NODE,
    NUM_NODE,
    OUTPUT_NODE,
    OVER_NODE,
    PARAM_LIST_NODE,
    PLUS_NODE,
    RETURN_NODE,
    STRING_NODE,
    SVAR_NODE,
    TIMES_NODE,
    VAR_LIST_NODE,
    VOID_NODE,
    WHILE_NODE,
    WRITE_NODE
} NodeKind;

struct node; // Opaque structure to ensure encapsulation.

typedef struct node AST;

AST* new_node(NodeKind kind, int data, Type type);

void add_child(AST *parent, AST *child);
AST* get_child(AST *parent, int idx);

AST* new_subtree(NodeKind kind, Type type, int child_count, ...);

NodeKind get_kind(AST *node);
char* kind2str(NodeKind kind);

int get_data(AST *node);
void set_data(AST *node, int data);

Type get_node_type(AST *node);
int get_child_count(AST *node);

void print_tree(AST *ast);
void print_dot(AST *ast);

void free_tree(AST *ast);

#endif
