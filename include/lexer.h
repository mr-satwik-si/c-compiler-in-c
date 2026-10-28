#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_SYMBOL,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char *text;
    size_t line;
    size_t column;
} Token;

Token *lexer_tokenize(const char *source, size_t *count);
void lexer_free_tokens(Token *tokens, size_t count);

#endif
