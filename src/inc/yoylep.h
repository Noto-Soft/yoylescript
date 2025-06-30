#pragma once

#include "yoylel.h"
#include "yoyleast.h"

typedef struct {
    lexer_t* lexer;
    token_t current;
} parser_t;

void parser_init(parser_t* parser, lexer_t* lexer);
astnode_t* parse_statement(parser_t* parser);
astnode_t* parse_program(parser_t* parser);
astnode_t* parse_expression(parser_t* parser);
astnode_t* parse_primary(parser_t* parser);
astnode_t* parse_unary(parser_t* parser);
astnode_t* parse_binary(parser_t* parser, int min_prec);
