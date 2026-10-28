#include "parser.h"

#include <stdio.h>
#include <string.h>

static const Token *tokens;
static size_t token_count;
static size_t position;

static const Token *current_token(void) {
    if (position < token_count) {
        return &tokens[position];
    }

    return NULL;
}

static int token_is(TokenType type, const char *text) {
    const Token *token = current_token();

    if (token == NULL || token->type != type) {
        return 0;
    }

    return text == NULL || strcmp(token->text, text) == 0;
}

static int expect(TokenType type, const char *text) {
    const Token *token = current_token();

    if (token == NULL || token->type != type) {
        fprintf(stderr, "Parser error: unexpected token\n");
        return 0;
    }

    if (text != NULL && !token_is(type, text)) {
        fprintf(stderr, "Parser error: expected '%s', got '%s'\n",
                text, token->text);
        return 0;
    }

    position++;
    return 1;
}

static int parse_statement(void) {
    if (!expect(TOKEN_KEYWORD, "return")) {
        return 0;
    }

    if (!expect(TOKEN_NUMBER, NULL)) {
        fprintf(stderr, "Parser error: expected an integer after return\n");
        return 0;
    }

    return expect(TOKEN_SYMBOL, ";");
}

static int parse_block(void) {
    if (!expect(TOKEN_SYMBOL, "{")) {
        return 0;
    }

    while (!token_is(TOKEN_SYMBOL, "}")) {
        if (current_token() == NULL ||
            current_token()->type == TOKEN_EOF) {
            fprintf(stderr, "Parser error: missing closing brace\n");
            return 0;
        }

        if (!parse_statement()) {
            return 0;
        }
    }

    return expect(TOKEN_SYMBOL, "}");
}

static int parse_function(void) {
    if (!expect(TOKEN_KEYWORD, "int")) {
        return 0;
    }

    if (!expect(TOKEN_IDENTIFIER, NULL)) {
        fprintf(stderr, "Parser error: expected a function name\n");
        return 0;
    }

    if (!expect(TOKEN_SYMBOL, "(")) {
        return 0;
    }

    if (!expect(TOKEN_KEYWORD, "void")) {
        fprintf(stderr, "Parser error: expected 'void' in function parameters\n");
        return 0;
    }

    if (!expect(TOKEN_SYMBOL, ")")) {
        return 0;
    }

    return parse_block();
}

int parser_parse(const Token *input_tokens, size_t count) {
    if (input_tokens == NULL || count == 0) {
        fprintf(stderr, "Parser error: no tokens supplied\n");
        return 0;
    }

    tokens = input_tokens;
    token_count = count;
    position = 0;

    if (!parse_function()) {
        return 0;
    }

    if (!expect(TOKEN_EOF, NULL)) {
        fprintf(stderr, "Parser error: unexpected content after function\n");
        return 0;
    }

    return 1;
}

