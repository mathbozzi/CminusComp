
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "tables.h"
#include "types.h"

#define CHILDREN_LIMIT 20 

struct node {
    NodeKind kind;
    int data;
    int count;
    Type type;
    AST* child[CHILDREN_LIMIT];
};

AST* new_node(NodeKind kind, int data, Type type) {
    AST* node = malloc(sizeof * node);
    node->kind = kind;
    node->data = data;
    node->type = type;
    node->count = 0;
    for (int i = 0; i < CHILDREN_LIMIT; i++) {
        node->child[i] = NULL;
    }
    return node;
}

void add_child(AST *parent, AST *child) {
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

AST* new_subtree(NodeKind kind, Type type, int child_count, ...) {
    if (child_count > CHILDREN_LIMIT) {
        fprintf(stderr, "Too many children as arguments!\n");
        exit(1);
    }

    AST* node = new_node(kind, 0, type);
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

void set_data(AST *node, int data) {
    node->data = data;
}

Type get_node_type(AST *node) {
    return node->type;
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

char* kind2str(NodeKind kind) {
    switch(kind) {
        case ARG_LIST_NODE: return "arg_list";
        case ASSIGN_NODE: return "=";
        case BLOCK_NODE: return "block";
        case CVAR_NODE: return "cvar";
        case EQ_NODE: return "==";
        case FUNC_BODY_NODE: return "func_body";
        case FUNC_CALL_NODE: return "fcall";
        case FUNC_DCL_NODE: return "func_decl";
        case FUNC_HEADER_NODE: return "func_header";
        case FUNC_LIST_NODE: return "func_list";
        case GE_NODE: return ">=";
        case GT_NODE: return ">";
        case ID_NODE: return "id";
        case IF_NODE: return "if";
        case INPUT_NODE: return "input";
        case INT_NODE: return "int";
        case LE_NODE: return "<=";
        case LT_NODE: return "<";
        case MINUS_NODE: return "-";
        case NEQ_NODE: return "!=";
        case NUM_NODE: return "num";
        case OUTPUT_NODE: return "output";
        case OVER_NODE: return "/";
        case PARAM_LIST_NODE: return "param_list";
        case PLUS_NODE: return "+";
        case RETURN_NODE: return "return";
        case STRING_NODE: return "string";
        case SVAR_NODE: return "svar";
        case TIMES_NODE: return "*";
        case VAR_LIST_NODE: return "var_list";
        case VOID_NODE: return "void";
        case WHILE_NODE: return "while";
        case WRITE_NODE: return "write";
        default: return "ERROR";
    }
}

int has_data(NodeKind kind) {
    switch(kind) {
        case ID_NODE:
        case SVAR_NODE:
        case CVAR_NODE:
        case NUM_NODE:
        case STRING_NODE:
        case FUNC_CALL_NODE:
            return 1;
        default:
            return 0;
    }
}

int print_node_dot(AST *node) {
    int my_nr = nr++;

    fprintf(stderr, "node%d[label=\"", my_nr);
    if (node->type != NO_TYPE) {
        fprintf(stderr, "(%s) ", get_text(node->type));
    }
    fprintf(stderr, "%s", kind2str(node->kind));
    
    if (has_data(node->kind)) {
        printf(",%d",node->data);
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
