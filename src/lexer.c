#include "lexer.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static char *copy_text(const char *start, size_t length) {
    char *text = malloc(length + 1);

    if (text == NULL) {
        return NULL;
    }

    memcpy(text, start, length);
    text[length] = '\0';
    return text;
}

static int is_keyword(const char *text) {
    return strcmp(text, "int") == 0 ||
           strcmp(text, "return") == 0 ||
           strcmp(text, "void") == 0;
}

static int is_identifier_start(unsigned char c) {
    return isalpha(c) || c == '_';
}

static int is_identifier_part(unsigned char c) {
    return isalnum(c) || c == '_';
}

static int is_symbol(char c) {
    return strchr("(){};,", c) != NULL;
}

void lexer_free_tokens(Token *tokens, size_t count) {
    if (tokens == NULL) {
        return;
    }

    for (size_t i = 0; i < count; i++) {
        free(tokens[i].text);
    }

    free(tokens);
}

Token *lexer_tokenize(const char *source, size_t *count) {
    if (count == NULL) {
        return NULL;
    }

    *count = 0;

    if (source == NULL) {
        return NULL;
    }

    size_t capacity = 16;
    Token *tokens = malloc(capacity * sizeof(Token));

    if (tokens == NULL) {
        return NULL;
    }

    size_t position = 0;
    size_t line = 1;
    size_t column = 1;

    while (source[position] != '\0') {
        unsigned char c = (unsigned char)source[position];

        if (isspace(c)) {
            if (c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }

            position++;
            continue;
        }

        size_t start = position;
        size_t token_line = line;
        size_t token_column = column;
        TokenType type;

        if (is_identifier_start(c)) {
            position++;
            column++;

            while (is_identifier_part(
                       (unsigned char)source[position])) {
                position++;
                column++;
            }

            char *word = copy_text(source + start, position - start);

            if (word == NULL) {
                lexer_free_tokens(tokens, *count);
                *count = 0;
                return NULL;
            }

            type = is_keyword(word) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;

            if (*count == capacity) {
                size_t new_capacity = capacity * 2;
                Token *grown = realloc(
                    tokens, new_capacity * sizeof(Token));

                if (grown == NULL) {
                    free(word);
                    lexer_free_tokens(tokens, *count);
                    *count = 0;
                    return NULL;
                }

                tokens = grown;
                capacity = new_capacity;
            }

            tokens[*count] = (Token){
                .type = type,
                .text = word,
                .line = token_line,
                .column = token_column
            };
            (*count)++;
            continue;
        }

        if (isdigit(c)) {
            position++;
            column++;

            while (isdigit((unsigned char)source[position])) {
                position++;
                column++;
            }

            char *number = copy_text(source + start, position - start);

            if (number == NULL) {
                lexer_free_tokens(tokens, *count);
                *count = 0;
                return NULL;
            }

            if (*count == capacity) {
                size_t new_capacity = capacity * 2;
                Token *grown = realloc(
                    tokens, new_capacity * sizeof(Token));

                if (grown == NULL) {
                    free(number);
                    lexer_free_tokens(tokens, *count);
                    *count = 0;
                    return NULL;
                }

                tokens = grown;
                capacity = new_capacity;
            }

            tokens[*count] = (Token){
                .type = TOKEN_NUMBER,
                .text = number,
                .line = token_line,
                .column = token_column
            };
            (*count)++;
            continue;
        }

        if (is_symbol((char)c)) {
            char *symbol = copy_text(source + position, 1);

            if (symbol == NULL) {
                lexer_free_tokens(tokens, *count);
                *count = 0;
                return NULL;
            }

            position++;
            column++;

            if (*count == capacity) {
                size_t new_capacity = capacity * 2;
                Token *grown = realloc(
                    tokens, new_capacity * sizeof(Token));

                if (grown == NULL) {
                    free(symbol);
                    lexer_free_tokens(tokens, *count);
                    *count = 0;
                    return NULL;
                }

                tokens = grown;
                capacity = new_capacity;
            }

            tokens[*count] = (Token){
                .type = TOKEN_SYMBOL,
                .text = symbol,
                .line = token_line,
                .column = token_column
            };
            (*count)++;
            continue;
        }

        lexer_free_tokens(tokens, *count);
        *count = 0;
        return NULL;
    }

    if (*count == capacity) {
        Token *grown = realloc(tokens, (capacity + 1) * sizeof(Token));

        if (grown == NULL) {
            lexer_free_tokens(tokens, *count);
            *count = 0;
            return NULL;
        }

        tokens = grown;
    }

    tokens[*count] = (Token){
        .type = TOKEN_EOF,
        .text = copy_text("", 0),
        .line = line,
        .column = column
    };

    if (tokens[*count].text == NULL) {
        lexer_free_tokens(tokens, *count);
        *count = 0;
        return NULL;
    }

    (*count)++;
    return tokens;
}
