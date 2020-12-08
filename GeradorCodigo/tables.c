
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tables.h"
#include "ast.h"

static int address_counter = 0; // É responsável por determinar a posição em memória de uma variável no momento da adição na tabela.

// Strings Table
// ----------------------------------------------------------------------------

#define STRING_MAX_SIZE 128
#define STRINGS_TABLE_MAX_SIZE 100

struct str_table {
    char t[STRINGS_TABLE_MAX_SIZE][STRING_MAX_SIZE];
    int size;
};

StrTable* create_str_table() {
    StrTable *st = malloc(sizeof * st);
    st->size = 0;
    return st;
}

int add_string(StrTable* st, char* s) {
    for (int i = 0; i < st->size; i++) {
        if (strcmp(st->t[i], s) == 0) {
            return i;
        }
    }
    strcpy(st->t[st->size], s);
    int idx_added = st->size;
    st->size++;
    return idx_added;
}

char* get_string(StrTable* st, int i) {
    return st->t[i];
}

void print_str_table(StrTable* st) {
    printf("Literals table:\n");
    for (int i = 0; i < st->size; i++) {
        printf("Entry %d -- %s\n", i, get_string(st, i));
    }
}

void free_str_table(StrTable* st) {
    free(st);
}

// Variables Table
// ----------------------------------------------------------------------------

#define VARIABLE_MAX_SIZE 128
#define VARIABLES_TABLE_MAX_SIZE 100

typedef struct {
  char name[VARIABLE_MAX_SIZE];
  int line;
  int scope;
  int size;
  int addr;
} Entry;

struct var_table {
    Entry t[VARIABLES_TABLE_MAX_SIZE];
    int size;
};

VarTable* create_var_table() {
    VarTable *vt = malloc(sizeof * vt);
    vt->size = 0;
    return vt;
}

int lookup_var(VarTable* vt, char* s, int scope) {
    for (int i = 0; i < vt->size; i++) {
        if (strcmp(vt->t[i].name, s) == 0 && scope == vt->t[i].scope)  {
            return i;
        }
    }
    return -1;
}

int add_var(VarTable* vt, char* s, int line, int scope, int size) {
    strcpy(vt->t[vt->size].name, s);
    vt->t[vt->size].line = line;
    vt->t[vt->size].scope = scope;
    vt->t[vt->size].size = size;
    
    if(size != -1){
        vt->t[vt->size].addr = address_counter;
        if(size == 0){ // é uma variável simples.
            address_counter++;
        }
        else{ // é um vetor.
            address_counter += size;
        }
    }
    else{ // é uma referência para vetor, portanto não vai para memória.
        vt->t[vt->size].addr = -1;
    }
    
    int idx_added = vt->size;
    vt->size++;
    return idx_added;
}

char* get_name(VarTable* vt, int i) {
    return vt->t[i].name;
}

int get_line(VarTable* vt, int i) {
    return vt->t[i].line;
}

int get_size(VarTable* vt, int i) {
    return vt->t[i].size;
}

int get_scope(VarTable* vt, int i){
    return vt->t[i].scope;
}

int get_address(VarTable* vt, int i){
    return vt->t[i].addr;
}

void set_address(VarTable* vt, int i, int addr){
    vt->t[i].addr = addr;
}

void print_var_table(VarTable* vt) {
    printf("Variables table:\n");
    for (int i = 0; i < vt->size; i++) {
         printf("Entry %d -- name: %s, line: %d, scope: %d, size: %d, address: %d\n", i,
                get_name(vt, i), get_line(vt, i), get_scope(vt, i), get_size(vt, i), get_address(vt, i));
    }
}

void free_var_table(VarTable* vt) {
    free(vt);
}

// Functions Table
// ----------------------------------------------------------------------------

typedef struct {
  char name[VARIABLE_MAX_SIZE];
  int line;
  int arity; /* o número de parâmetros da função. */
  Type type;
  AST* node;
} FuncEntry;

struct func_table {
    FuncEntry t[VARIABLES_TABLE_MAX_SIZE];
    int size;
};

FuncTable* create_func_table(){
    FuncTable *ft = malloc(sizeof * ft);
    ft->size = 0;
    return ft;    
}

int add_func(FuncTable* ft, char* s, int line, int arity, Type type){
    strcpy(ft->t[ft->size].name, s);
    ft->t[ft->size].line = line;
    ft->t[ft->size].arity = arity;
    ft->t[ft->size].type = type;
    ft->t[ft->size].node = NULL;
    int idx_added = ft->size;
    ft->size++;
    return idx_added;
}

int lookup_func(FuncTable* ft, char* s){
    for (int i = 0; i < ft->size; i++) {
        if (strcmp(ft->t[i].name, s) == 0)  {
            return i;
        }
    }
    return -1;
}

char* get_func_name(FuncTable* ft, int i){
    return ft->t[i].name;
}

int get_func_line(FuncTable* ft, int i){
    return ft->t[i].line;
}

int get_func_arity(FuncTable* ft, int i){
    return ft->t[i].arity;
}

int get_func_type(FuncTable* ft, int i){
    return ft->t[i].type;
}

void add_func_node(FuncTable* ft, int i, AST* node){
    ft->t[i].node = node;
}

AST* get_func_node(FuncTable* ft, int i){
    return ft->t[i].node;
}

void print_func_table(FuncTable* ft){
    printf("Functions table:\n");
    for (int i = 0; i < ft->size; i++) {
         printf("Entry %d -- type: %s, name: %s, line: %d, arity: %d\n", i, get_text(get_func_type(ft, i)), 
                get_func_name(ft, i), get_func_line(ft, i), get_func_arity(ft, i));
    }
}

void free_func_table(FuncTable* ft){
    free(ft);
}
