#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
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

    if (!token_is(type, text)) {
        if (token != NULL) {
            fprintf(stderr, "Parser error: unexpected token '%s'\n",
                    token->text);
        } else {
            fprintf(stderr, "Parser error: unexpected end of input\n");
        }

        return 0;
    }

    position++;
    return 1;
}

static ASTNode *parse_function(void) {
    if (!expect(TOKEN_KEYWORD, "int")) {
        return NULL;
    }

    const Token *name_token = current_token();

    if (!expect(TOKEN_IDENTIFIER, NULL)) {
        fprintf(stderr, "Parser error: expected a function name\n");
        return NULL;
    }

    char *name = malloc(strlen(name_token->text) + 1);

    if (name == NULL) {
        fprintf(stderr, "Parser error: out of memory\n");
        return NULL;
    }

    strcpy(name, name_token->text);

    if (!expect(TOKEN_SYMBOL, "(") ||
        !expect(TOKEN_KEYWORD, "void") ||
        !expect(TOKEN_SYMBOL, ")") ||
        !expect(TOKEN_SYMBOL, "{") ||
        !expect(TOKEN_KEYWORD, "return")) {
        free(name);
        return NULL;
    }

    const Token *number_token = current_token();

    if (!expect(TOKEN_NUMBER, NULL)) {
        fprintf(stderr, "Parser error: expected an integer after return\n");
        free(name);
        return NULL;
    }

    char *end = NULL;
    long number = strtol(number_token->text, &end, 10);

    if (end == number_token->text || *end != '\0') {
        fprintf(stderr, "Parser error: invalid integer literal\n");
        free(name);
        return NULL;
    }

    if (!expect(TOKEN_SYMBOL, ";") ||
        !expect(TOKEN_SYMBOL, "}")) {
        free(name);
        return NULL;
    }

    ASTNode *number_node = ast_create_number(number);

    if (number_node == NULL) {
        free(name);
        return NULL;
    }

    ASTNode *return_node = ast_create_return(number_node);

    if (return_node == NULL) {
        ast_free(number_node);
        free(name);
        return NULL;
    }

    ASTNode *function_node = ast_create_function(name, return_node);
    free(name);

    if (function_node == NULL) {
        ast_free(return_node);
        return NULL;
    }

    return function_node;
}

ASTNode *parser_parse(const Token *input_tokens, size_t count) {
    if (input_tokens == NULL || count == 0) {
        fprintf(stderr, "Parser error: no tokens supplied\n");
        return NULL;
    }

    tokens = input_tokens;
    token_count = count;
    position = 0;

    ASTNode *function = parse_function();

    if (function == NULL) {
        return NULL;
    }

    if (!expect(TOKEN_EOF, NULL)) {
        fprintf(stderr, "Parser error: unexpected content after function\n");
        ast_free(function);
        return NULL;
    }

    return function;
}
