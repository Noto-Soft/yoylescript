#pragma once

#include "yoyletoken.h"
#include "stddef.h"

#define CHAR_CASE(tok, ch, lexeme_str, token_type) \
    case ch: \
        tok.type = token_type; \
        tok.lexeme = strdup(lexeme_str); \
        return tok;

#define DUALCHAR_CASE(tok, first_ch, second_ch, single_lexeme, single_type, dual_lexeme, dual_type) \
    case first_ch: \
        if (peek(lexer) == second_ch) { \
            advance(lexer); \
            tok.type = dual_type; \
            tok.lexeme = strdup(dual_lexeme); \
            return tok; \
        } else { \
            tok.type = single_type; \
            tok.lexeme = strdup(single_lexeme); \
            return tok; \
        }

typedef struct {
    const char* src;
    size_t pos;
    size_t line;
} lexer_t;

void lexer_init(lexer_t* lexer, const char* src);
token_t lexer_next(lexer_t* lexer);
token_t lexer_expect(lexer_t* lexer, TokenType expected);
void free_token(token_t tok);
