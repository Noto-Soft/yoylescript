#include "inc/yoyletoken.h"

#include <stdlib.h>

keywordmap_t keywordmap[] = {
    {"function", TOKEN_FUNC},
    {"if",       TOKEN_IF},
    {"else",     TOKEN_ELSE},
    {"while",    TOKEN_WHILE},
    {"nil",      TOKEN_NIL},
    {"return",   TOKEN_RETURN},
    {"true",     TOKEN_TRUE},
    {"false",    TOKEN_FALSE},
};

const int keywordmap_count = sizeof(keywordmap) / sizeof(keywordmap[0]);

void free_token(token_t tok) {
    if (tok.lexeme) free(tok.lexeme);
    if (tok.type == TOKEN_STRING && tok.strValue) free(tok.strValue);
}