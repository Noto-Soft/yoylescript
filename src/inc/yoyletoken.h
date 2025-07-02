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
    TOKEN_NIL,
    TOKEN_RETURN,
    TOKEN_TRUE,
    TOKEN_FALSE,
} TokenType;

typedef struct {
    const char* keyword;
    TokenType type;
} keywordmap_t;

extern keywordmap_t keywordmap[];
extern const int keywordmap_count;

#define KEYWORD_COUNT keywordmap_count

typedef struct {
    TokenType type;
    char* lexeme;
    union {
        int intValue;
        char* strValue;
    };
} token_t;

void free_token(token_t tok);
