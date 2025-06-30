#pragma once

typedef enum {
    TOKEN_EOF,
    TOKEN_INT,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    TOKEN_ASSIGN,
    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_FUNC,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_ERROR,
    TOKEN_COMMA,
    TOKEN_UNARY_OPERATOR,
    TOKEN_BINARY_OPERATOR,
} TokenType;

typedef struct {
    TokenType type;
    char* lexeme;
    union {
        int intValue;
        char* strValue;
    };
} token_t;

void free_token(token_t tok);
