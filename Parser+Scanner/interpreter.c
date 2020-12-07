
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interpreter.h"
#include "tables.h"

// ----------------------------------------------------------------------------

extern StrTable *st;
extern VarTable *vt;
extern FuncTable *ft;

// ----------------------------------------------------------------------------

// Data stack -----------------------------------------------------------------

#define STACK_SIZE 100

int stack[STACK_SIZE];
int sp; // stack pointer

// All these ops should have a boundary check, buuuut... X_X

void push(int x) {
    stack[++sp] = x;
}

int pop() {
    return stack[sp--];
}

void init_stack() {
    for (int i = 0; i < STACK_SIZE; i++) {
        stack[i] = 0;
    }
    sp = -1;
}

void print_stack() {
    printf("*** STACK: ");
    for (int i = 0; i <= sp; i++) {
        printf("%d ", stack[i]);
    }
    printf("\n");
}

// ----------------------------------------------------------------------------

// Variables memory -----------------------------------------------------------

#define MEM_SIZE 100

int mem[MEM_SIZE];

void store(int addr, int val) {
    mem[addr] = val;
}

int load(int addr) {
    return mem[addr];
}

void init_mem() {
    for (int addr = 0; addr < MEM_SIZE; addr++) {
        mem[addr] = 0;
    }
}

// ----------------------------------------------------------------------------

// #define TRACE
#ifdef TRACE
#define trace(msg) printf("TRACE: %s\n", msg)
#else
#define trace(msg)
#endif

#define MAX_STR_SIZE 128
//static char str_buf[MAX_STR_SIZE];
#define clear_str_buf() str_buf[0] = '\0'

void rec_run_ast(AST* ast);

void read_int(int var_idx) {
    int x;
    scanf("%d", &x);
    store(var_idx, x);
}

void write_int() {
    printf("%d\n", pop());
}

#define run_bin_op()                \
    AST* lexpr = get_child(ast, 0); \
    AST* rexpr = get_child(ast, 1); \
    rec_run_ast(lexpr);             \
    rec_run_ast(rexpr)

void plus_int(AST* ast) {
    run_bin_op();
    int r = pop();
    int l = pop();
    push(l + r);
}

void plus_bool(AST* ast) {
    run_bin_op();
    int r = pop();
    int l = pop();
    push(l || r); // Can't add l and r here because of overflow (>1).
}

void run_other_arith(AST* ast, int (*int_op)(int,int)) {
    run_bin_op();
    int r = pop();
    int l = pop();
    push(int_op(l,r));
}

int int_minus(int l, int r) {
    return l - r;
}

int int_over(int l, int r) {
    return l / r;
}

int int_times(int l, int r) {
    return l * r;
}

void run_cmp(AST* ast, int (*int_cmp)(int,int)) {
    run_bin_op();
    int r = pop();
    int l = pop();
    push(int_cmp(l, r));
}

int int_eq(int l, int r) {
    return l == r;
}

int int_neq(int l, int r){
    return l != r;
}

int int_lt(int l, int r) {
    return l < r;
}

int int_le(int l, int r){
    return l <= r;
}

int int_ge(int l, int r){
    return l >= r;
}

int int_gt(int l, int r){
    return l > r;
}

// ----------------------------------------------------------------------------

void run_assign(AST* ast) {
    trace("assign");
    AST* rexpr = get_child(ast, 1);
    rec_run_ast(rexpr);
    int var_idx = get_data(get_child(ast, 0));
    store(var_idx, pop());
}

void run_eq(AST* ast) {
    trace("eq");
    run_cmp(ast, int_eq);
}

void run_neq(AST* ast){
    trace("neq");
    run_cmp(ast, int_neq);
}

void run_block(AST* ast) {
    trace("block");
    int size = get_child_count(ast);
    for (int i = 0; i < size; i++) {
        rec_run_ast(get_child(ast, i));
    }
}

void run_bool_val(AST* ast) {
    trace("bool_val");
    push(get_data(ast));
}

void run_if(AST* ast) {
    trace("if");
    rec_run_ast(get_child(ast, 0));
    int test = pop();
    if (test == 1) {
        rec_run_ast(get_child(ast, 1));
    } else if (test == 0 && get_child_count(ast) == 3) {
        rec_run_ast(get_child(ast, 2));
    }
}

void run_int_val(AST* ast) {
    trace("int_val");
    push(get_data(ast));
}

void run_lt(AST* ast) {
    trace("lt");
    run_cmp(ast, int_lt);
}

void run_le(AST* ast){
    trace("le");
    run_cmp(ast, int_le);
}

void run_ge(AST* ast){
    trace("ge");
    run_cmp(ast, int_ge);
}

void run_gt(AST* ast){
    trace("gt");
    run_cmp(ast, int_gt);
}

void run_minus(AST* ast) {
    trace("minus");
    run_other_arith(ast, int_minus);
}

void run_over(AST* ast) {
    trace("over");
    run_other_arith(ast, int_over);
}

void run_plus(AST* ast) {
    trace("plus");
    plus_int(ast);
}

void run_program(AST* ast) {
    trace("program");
    rec_run_ast(get_child(ast, 0)); // run var_list
    rec_run_ast(get_child(ast, 1)); // run block
}

void run_input(AST* ast) {
    trace("input");
    int n;
    scanf("%d", &n);
    push(n);
}

void run_while(AST* ast) {
    trace("while");
    rec_run_ast(get_child(ast, 0)); // Run test.
    int loop = pop();
    while (loop) {
        rec_run_ast(get_child(ast, 1)); // Run block.
        rec_run_ast(get_child(ast, 0)); // Run test.
        loop = pop();
    }
}

void run_str_val(AST* ast) {
    trace("str_val");
    push(get_data(ast));
}

void run_times(AST* ast) {
    trace("times");
    run_other_arith(ast, int_times);
}

void run_var_decl(AST* ast) {
    trace("var_decl");
    // Nothing to do, memory was already cleared upon initialization.
}

void run_var_list(AST* ast) {
    trace("var_list");
    // Nothing to do, memory was already cleared upon initialization.
}

void run_var_use(AST* ast) {
    trace("var_use");
    int var_idx = get_data(ast);
    push(load(var_idx));
}

void print_string(char* s){
    int i = 0;
    int j = 0;

    int len = strlen(s);
    char output[len + 1];

    while(s[i + 1] != '\0'){
        if(s[i] == '\"'){
            i++;
        }
        else if(s[i] == '\\' && s[i + 1] == 'n'){
            output[j] = '\n';
            j++;
            i += 2;
        }
        else{
            output[j] = s[i];
            j++;
            i++;
        }
    }

    output[j] = '\0';

    printf("%s", output);
}

void run_write(AST* ast) {
    trace("write");
    AST* str_node = get_child(ast, 0);
    int str_id = get_data(str_node);
    char* s = get_string(st, str_id);
    print_string(s);
}

void run_func_list(AST* ast){
    trace("func_list");   
    AST* main_decl_node = NULL;

    int size = get_child_count(ast);
    for (int i = 0; i < size; i++) {
        AST* func_decl_node = get_child(ast, i);
        AST* func_header_node = get_child(func_decl_node, 0);
        AST* func_name_node = get_child(func_header_node, 0);
        int func_id = get_data(func_name_node);
        add_func_node(ft, func_id, func_decl_node);
        
        char* name = get_func_name(ft, func_id);
        
        if(strcmp(name, "main") == 0){
            main_decl_node = func_decl_node;
        }
    }

    if(main_decl_node != NULL){
        rec_run_ast(main_decl_node);
    }
    else{
        printf("Algo está errado. Main não encontrada.\n");
    }
}

void run_func_decl(AST* ast){
    AST* func_header = get_child(ast, 0);
    AST* func_body = get_child(ast, 1);
    rec_run_ast(func_header);
    rec_run_ast(func_body);
}

void run_func_header(AST* ast){
}

void run_func_body(AST* ast){
    AST* var_list = get_child(ast, 0);
    rec_run_ast(var_list);

    AST* block = get_child(ast, 1);
    rec_run_ast(block);
}

void run_output(AST* ast){
    AST* expr = get_child(ast, 0);
    rec_run_ast(expr);
    printf("%d\n", pop());
}

void rec_run_ast(AST* ast) {
    switch(get_kind(ast)) {
        case ASSIGN_NODE:           run_assign(ast);        break;
        case BLOCK_NODE:            run_block(ast);         break;
        case INPUT_NODE:            run_input(ast);          break;
        case INT_VAL_NODE:          run_int_val(ast);       break;
        case FUNC_LIST_NODE:        run_func_list(ast);     break;
        case FUNCTION_DECL_NODE:    run_func_decl(ast);     break;
        case FUNCTION_HEADER_NODE:  run_func_header(ast);   break;
        case FUNCTION_BODY_NODE:    run_func_body(ast);     break;
        case VAR_LIST_NODE:         run_var_list(ast);      break;
        case VAR_USE_NODE:          run_var_use(ast);       break;
        
        case WRITE_NODE:            run_write(ast);         break;
        case OUTPUT_NODE:           run_output(ast);        break;
        
        /* Arithmetic. */
        case PLUS_NODE:             run_plus(ast);          break;
        case MINUS_NODE:            run_minus(ast);         break;
        case TIMES_NODE:            run_times(ast);         break;
        case OVER_NODE:             run_over(ast);          break;

        /* Conditionals: */
        case IF_NODE:               run_if(ast);            break;

        case EQ_NODE:               run_eq(ast);            break;
        case NEQ_NODE:              run_neq(ast);           break;
        
        case LE_NODE:               run_le(ast);            break;
        case LT_NODE:               run_lt(ast);            break;

        case GE_NODE:               run_ge(ast);            break;
        case GT_NODE:               run_gt(ast);            break;
        

        /* Loop: */
        case WHILE_NODE:            run_while(ast);         break;

        /*
        case ASSIGN_NODE:   run_assign(ast);    break;--
        case EQ_NODE:       run_eq(ast);        break;
        case BLOCK_NODE:    run_block(ast);     break;--
        case BOOL_VAL_NODE: run_bool_val(ast);  break;
        case PROGRAM_NODE:  run_program(ast);   break;
        
        case STR_VAL_NODE:  run_str_val(ast);   break;
        case TIMES_NODE:    run_times(ast);     break;--
        case VAR_DECL_NODE: run_var_decl(ast);  break;
        case VAR_LIST_NODE: run_var_list(ast);  break;--
        case VAR_USE_NODE:  run_var_use(ast);   break;--
        case WRITE_NODE:    run_write(ast);     break;--
        */

        default:
            fprintf(stderr, "Invalid kind: %s!\n", kind2str(get_kind(ast)));
            exit(EXIT_FAILURE);
    }
}

// ----------------------------------------------------------------------------

void run_ast(AST* ast) {
    init_stack();
    init_mem();
    rec_run_ast(ast);
}
