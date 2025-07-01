#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "inc/yoyleast.h"
#include "inc/yoylestate.h"
#include "inc/yoylei.h"
#include "inc/yoyleio.h"
#include "inc/yoyleerr.h"
#include "inc/yoylel.h"
#include "inc/yoylep.h"

void print_c(yoylestate_t* state) {
    astnode_t* arg = pop_stack(&state->arg_stack);
    print_astnode(eval(state, arg));
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fatal("No filename");
    }

    char* file = read_file(argv[1]);

    yoylestate_t state = {NULL, NULL};

    symbol_table_set(&state.symbol_table, "print", new_cfunc(print_c));

    
    astnode_t* rootNode;
    lexer_t lexer;
    parser_t parser;
    lexer_init(&lexer, file);
    parser_init(&parser, &lexer);
    rootNode = parse_program(&parser);
    

    eval(&state, rootNode);
    free_ast(rootNode);
    free_symbol_table(state.symbol_table);
    free_stack(state.arg_stack);

    free(file);

    return 0;
}