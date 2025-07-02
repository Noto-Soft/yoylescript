#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>

#include "inc/yoyleast.h"
#include "inc/yoylestate.h"
#include "inc/yoylei.h"
#include "inc/yoyleio.h"
#include "inc/yoyleerr.h"
#include "inc/yoylel.h"
#include "inc/yoylep.h"

void print_c(yoylestate_t* state) {
    astnode_t* arg = pop_stack(&state->arg_stack);
    astnode_t* newline = pop_stack(&state->arg_stack);
    bool printlf = true;
    if (newline != NULL && newline->type != NODE_NIL) {
        printlf = eval(state, newline)->intValue != 0;
    }
    print_astnode(eval(state, arg), printlf);
}

char* read_input() {
    size_t capacity = 128;
    size_t length = 0;
    char *input = malloc(capacity);
    if (!input) return NULL;

    int c;
    while ((c = fgetc(stdin)) != EOF) {
        if (length + 1 >= capacity) {
            capacity *= 2;
            char *new_input = realloc(input, capacity);
            if (!new_input) {
                free(input);
                return NULL;
            }
            input = new_input;
        }

        if (c == '\n') {
            break;
        }

        input[length++] = (char)c;
    }

    if (length == 0 && c == EOF) {
        free(input);
        return NULL;
    }

    input[length] = '\0';
    return input;
}

void input_c(yoylestate_t* state) {
    astnode_t* message = pop_stack(&state->arg_stack);
    if (message != NULL && message->type != NODE_NIL) {
        astnode_t* newline = pop_stack(&state->arg_stack);
        bool printlf = true;
        if (newline != NULL && newline->type != NODE_NIL) {
            printlf = eval(state, newline)->intValue != 0;
        }
        print_astnode(eval(state, message), printlf);
    }
    char* in = read_input();
    astnode_t* input = new_literal_string(in);
    free(in);
    push_stack(&state->arg_stack, input);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fatal("No filename");
    }

    char* file = read_file(argv[1]);

    yoylestate_t state = {NULL, NULL};

    symbol_table_set(&state.symbol_table, "print", new_cfunc(print_c));
    symbol_table_set(&state.symbol_table, "input", new_cfunc(input_c));
    
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