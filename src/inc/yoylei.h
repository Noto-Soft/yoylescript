#pragma once

#include "yoyleast.h"
#include "yoylestate.h"

#define free_ast(node) do { _free_ast(node); node = NULL; } while(0)
#define free_symbol_table(table) do { _free_symbol_table(table); table = NULL; } while(0)
#define free_stack(stack) do { _free_stack(stack); stack = NULL; } while(0)

typedef struct varentry_t {
    char* name;
    astnode_t* value;
    struct varentry_t* next;
} varentry_t;

typedef struct yoylestack_t {
    astnode_t* value;
    struct yoylestack_t* next;
} yoylestack_t;

astnode_t* eval(yoylestate_t* state, astnode_t* node);
void print_astnode(astnode_t* node);
void _free_ast(astnode_t* rootNode);

astnode_t* symbol_table_lookup(varentry_t** symbol_table, const char* name);
void symbol_table_set(varentry_t** symbol_table, const char* name, astnode_t* value);
void _free_symbol_table(varentry_t* entry);

void push_stack(yoylestack_t** stack, astnode_t* value);
astnode_t* pop_stack(yoylestack_t** stack);
void _free_stack(yoylestack_t* stack);