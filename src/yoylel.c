#include "inc/yoylel.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

char *strndup(const char *s, size_t n) {
    char *result = malloc(n + 1);
    if (!result) return NULL;
    memcpy(result, s, n);
    result[n] = '\0';
    return result;
}



void lexer_init(lexer_t* lexer, const char* src) {
    lexer->src = src;
    lexer->pos = 0;
    lexer->line = 1;
}

static char peek(lexer_t* lexer) {
    return lexer->src[lexer->pos];
}

static char advance(lexer_t* lexer) {
    return lexer->src[lexer->pos++];
}

static void skip_whitespace(lexer_t* lexer) {
    while (1) {
        char c = peek(lexer);

        if (isspace(c)) {
            if (c == '\n') lexer->line++;
            advance(lexer);
        }
        else if (c == '/' && lexer->src[lexer->pos + 1] == '/') {
            while (peek(lexer) && peek(lexer) != '\n') {
                advance(lexer);
            }
        }
        else {
            break;
        }
    }
}

token_t lexer_next(lexer_t* lexer) {
    skip_whitespace(lexer);

    token_t tok = {0};
    char c = peek(lexer);
    if (c == '\0') {
        tok.type = TOKEN_EOF;
        return tok;
    }

    if (isdigit(c)) {
        tok.type = TOKEN_INT;
        tok.intValue = 0;
        size_t start = lexer->pos;
        while (isdigit(peek(lexer))) {
            tok.intValue = tok.intValue * 10 + (advance(lexer) - '0');
        }
        size_t len = lexer->pos - start;
        tok.lexeme = strndup(lexer->src + start, len);
        return tok;
    }

    if (isalpha(c) || c == '_') {
        size_t start = lexer->pos;
        while (isalnum(peek(lexer)) || peek(lexer) == '_') advance(lexer);
        size_t len = lexer->pos - start;
        tok.lexeme = strndup(lexer->src + start, len);

        bool found = false;
        for (size_t i = 0; i < KEYWORD_COUNT; ++i) {
            if (strcmp(tok.lexeme, keywordmap[i].keyword) == 0) {
                tok.type = keywordmap[i].type;
                found = true;
                break;
            }
        }
        if (!found) {
            tok.type = TOKEN_IDENTIFIER;
        }

        return tok;
    }

    if (c == '"') {
        advance(lexer); 
        size_t start = lexer->pos;
        while (peek(lexer) && peek(lexer) != '"') advance(lexer);
        size_t len = lexer->pos - start;
        tok.strValue = strndup(lexer->src + start, len);
        tok.lexeme = strndup(lexer->src + start - 1, len + 2); 
        tok.type = TOKEN_STRING;
        if (peek(lexer) == '"') advance(lexer); 
        return tok;
    }

    switch (advance(lexer)) {
        DUALCHAR_CASE(tok, '=', '=', "=", TOKEN_ASSIGN, "e", TOKEN_BINARY_OPERATOR)
        DUALCHAR_CASE(tok, '!', '=', "!", TOKEN_UNARY_OPERATOR, "n", TOKEN_BINARY_OPERATOR)
        
        CHAR_CASE(tok, ';', ";", TOKEN_SEMICOLON)
        CHAR_CASE(tok, '(', "(", TOKEN_LPAREN)
        CHAR_CASE(tok, ')', ")", TOKEN_RPAREN)
        CHAR_CASE(tok, '{', "{", TOKEN_LBRACE)
        CHAR_CASE(tok, '}', "}", TOKEN_RBRACE)
        CHAR_CASE(tok, ',', ",", TOKEN_COMMA)
        CHAR_CASE(tok, '~', "~", TOKEN_UNARY_OPERATOR)

        CHAR_CASE(tok, '+', "+", TOKEN_BINARY_OPERATOR)
        CHAR_CASE(tok, '-', "-", TOKEN_BINARY_OPERATOR)
        CHAR_CASE(tok, '*', "*", TOKEN_BINARY_OPERATOR)
        CHAR_CASE(tok, '/', "/", TOKEN_BINARY_OPERATOR)
        CHAR_CASE(tok, '&', "&", TOKEN_BINARY_OPERATOR)
        CHAR_CASE(tok, '|', "|", TOKEN_BINARY_OPERATOR)

        DUALCHAR_CASE(tok, '>', '=', "g", TOKEN_BINARY_OPERATOR, "G", TOKEN_BINARY_OPERATOR)  
        DUALCHAR_CASE(tok, '<', '=', "l", TOKEN_BINARY_OPERATOR, "L", TOKEN_BINARY_OPERATOR)  
    }

    tok.type = TOKEN_ERROR;
    tok.lexeme = strdup("Unexpected character");
    return tok;
}

token_t lexer_expect(lexer_t* lexer, TokenType expected) {
    token_t tok = lexer_next(lexer);
    if (tok.type != expected) {
        tok.type = TOKEN_ERROR;
    }
    return tok;
}
