
#ifndef TABLES_H
#define TABLES_H

#include "types.h"

// Strings Table
// ----------------------------------------------------------------------------

// Opaque structure.
// For simplicity, the table is implemented as a sequential list.
struct str_table;
typedef struct str_table StrTable;

// Creates an empty strings table.
StrTable* create_str_table();

// Adds the given string to the table without repetitions.
// String 's' is copied internally.
// Returns the index of the string in the table.
int add_string(StrTable* st, char* s);

// Returns a pointer to the string stored at index 'i'.
char* get_string(StrTable* st, int i);

// Prints the given table to stdout.
void print_str_table(StrTable* st);

// Clears the allocated structure.
void free_str_table(StrTable* st);


// Variables Table
// ----------------------------------------------------------------------------

// Opaque structure.
// For simplicity, the table is implemented as a sequential list.
// This table only stores the variable name and type, and its declaration line.
struct var_table;
typedef struct var_table VarTable;

// Creates an empty variables table.
VarTable* create_var_table();

// Adds a fresh var to the table.
// No check is made by this function, so make sure to call 'lookup_var' first.
// Returns the index where the variable was inserted.
int add_var(VarTable* vt, char* s, int line, int scope, int size);

// Returns the index where the given variable is stored or -1 otherwise.
int lookup_var(VarTable* vt, char* s, int* scope);

// Returns the variable name stored at the given index.
// No check is made by this function, so make sure that the index is valid first.
char* get_name(VarTable* vt, int i);

// Returns the declaration line of the variable stored at the given index.
// No check is made by this function, so make sure that the index is valid first.
int get_line(VarTable* vt, int i);

// Returns the variable size stored at the given index.
// 0 para inteiro, -1 se referência a vetor em parâmetro de função ou tamanho do vetor se vetor.
int get_size(VarTable* vt, int i);

// Prints the given table to stdout.
void print_var_table(VarTable* vt);

// Clears the allocated structure.
void free_var_table(VarTable* vt);

// Functions Table
// ----------------------------------------------------------------------------

struct func_table;
typedef struct func_table FuncTable;

// Creates an empty functions table.
FuncTable* create_func_table();

// Adds a fresh func to the table.
// No check is made by this function, so make sure to call 'lookup_func' first.
// Returns the index where the function was inserted.
int add_func(FuncTable* ft, char* s, int line, int arity, Type type);

// Returns the index where the given function is stored or -1 otherwise.
int lookup_func(FuncTable* ft, char* s);

// Returns the function name stored at the given index.
// No check is made by this function, so make sure that the index is valid first.
char* get_func_name(FuncTable* ft, int i);

// Returns the declaration line of the function stored at the given index.
// No check is made by this function, so make sure that the index is valid first.
int get_func_line(FuncTable* ft, int i);

// Returns the arity stored at the given index.
// Retorna número de argumentos da função na posição i da tabela.
int get_func_arity(FuncTable* ft, int i);

int get_func_type(FuncTable* ft, int i);

// Prints the given table to stdout.
void print_func_table(FuncTable* ft);

// Clears the allocated structure.
void free_func_table(FuncTable* ft);


#endif // TABLES_H

