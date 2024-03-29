
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tables.h"
#include "types.h"

#define CHILDREN_LIMIT 200 // Don't try this at home, kids... :P

struct node {
    NodeKind kind;
    int data; /* Se kind é variável ou função, data representa a posição na respectiva tabela. Se é número, representa o número. */
    int count;
    AST* child[CHILDREN_LIMIT];
};

AST* new_node(NodeKind kind, int data) {
    AST* node = malloc(sizeof * node);
    node->kind = kind;
    node->data = data;
    node->count = 0;
    for (int i = 0; i < CHILDREN_LIMIT; i++) {
        node->child[i] = NULL;
    }
    return node;
}

void add_child(AST *parent, AST *child) {
    if(parent == NULL){
        printf("Pai nulo. Algo está errado.\n");
        return;
    }
    if (parent->count == CHILDREN_LIMIT) {
        fprintf(stderr, "Cannot add another child!\n");
        exit(1);
    }
    parent->child[parent->count] = child;
    parent->count++;
}

AST* get_child(AST *parent, int idx) {
    return parent->child[idx];
}

AST* new_subtree(NodeKind kind, int child_count, ...) {
    if (child_count > CHILDREN_LIMIT) {
        fprintf(stderr, "Too many children as arguments!\n");
        exit(1);
    }

    AST* node = new_node(kind, 0);
    va_list ap;
    va_start(ap, child_count);
    for (int i = 0; i < child_count; i++) {
        add_child(node, va_arg(ap, AST*));
    }
    va_end(ap);
    return node;
}

NodeKind get_kind(AST *node) {
    return node->kind;
}

int get_data(AST *node) {
    return node->data;
}

int get_child_count(AST *node) {
    return node->count;
}

void free_tree(AST *tree) {
    if (tree == NULL) return;
    for (int i = 0; i < tree->count; i++) {
        free_tree(tree->child[i]);
    }
    free(tree);
}

// Dot output.

int nr;

extern VarTable *vt;
extern FuncTable *ft;

char* kind2str(NodeKind kind) {
    switch(kind) {
        case ARG_LIST_NODE: return "arg_list";
        case ASSIGN_NODE:   return "=";
        case EQ_NODE:       return "==";
        case BLOCK_NODE:    return "block";
        case IF_NODE:       return "if";
        case INPUT_NODE:    return "input";
        case INT_VAL_NODE:  return "num";
        case LE_NODE:       return "<=";
        case LT_NODE:       return "<";
        case MINUS_NODE:    return "-";
        case NEQ_NODE:      return "!=";
        case OUTPUT_NODE:   return "output";
        case OVER_NODE:     return "/";
        case PLUS_NODE:     return "+";
        case STR_VAL_NODE:  return "string";
        case TIMES_NODE:    return "*";
        case VAR_DECL_NODE: return "var_decl";
        case VAR_LIST_NODE: return "var_list";
        case VAR_USE_NODE:  return "var_use";
        case WRITE_NODE:    return "write";
        case FUNCTION_CALL_NODE: return "fcall";
        case FUNCTION_DECL_NODE: return "func_decl";
        case FUNC_LIST_NODE:     return "func_list";
        case FUNCTION_NAME_NODE: return "func_name";
        case FUNCTION_HEADER_NODE: return "func_header";
        case FUNCTION_BODY_NODE: return "func_body";
        case GE_NODE:            return ">=";
        case GT_NODE:            return ">";
        case PARAM_LIST_NODE: return "param_list";
        case STATEMENT_LIST_NODE: return "stmt_list";
        case WHILE_NODE:    return "while";
        case RETURN_NODE:   return "return";
        default:            return "ERROR!!";
    }
}

int has_data(NodeKind kind) {
    switch(kind) {
        case INT_VAL_NODE:
        case STR_VAL_NODE:
        case VAR_DECL_NODE:
        case VAR_USE_NODE:
        case FUNCTION_NAME_NODE:
        case FUNCTION_CALL_NODE:
            return 1;
        default:
            return 0;
    }
}

int print_node_dot(AST *node) {
    int my_nr = nr++;

    int debug = 0;

    fprintf(stderr, "node%d[label=\"", my_nr);
    fprintf(stderr, "%s", kind2str(node->kind));

    if (has_data(node->kind)){
        fprintf(stderr, ",%d", node->data);
    }

    if(debug){
        if(node->kind == VAR_USE_NODE || node->kind == VAR_DECL_NODE){
            fprintf(stderr, "(%s, scope = %d)", get_name(vt, node->data), get_scope(vt, node->data));
        }
        else if(node->kind == FUNCTION_CALL_NODE || node->kind == FUNCTION_NAME_NODE){
            fprintf(stderr, "(%s)", get_func_name(ft, node->data));
        }
    }

    fprintf(stderr, "\"];\n");

    for (int i = 0; i < node->count; i++) {
        int child_nr = print_node_dot(node->child[i]);
        fprintf(stderr, "node%d -> node%d;\n", my_nr, child_nr);
    }
    return my_nr;
}

void print_dot(AST *tree) {
    nr = 0;
    fprintf(stderr, "digraph {\ngraph [ordering=\"out\"];\n");
    print_node_dot(tree);
    fprintf(stderr, "}\n");
}
