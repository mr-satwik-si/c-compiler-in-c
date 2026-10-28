
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

static ASTNode *parse_expression(void);

static ASTNode *parse_primary(void) {
    const Token *token = current_token();

    if (token == NULL) {
        fprintf(stderr, "Parser error: expected an expression\n");
        return NULL;
    }

    if (token->type == TOKEN_NUMBER) {
        char *end = NULL;
        long value = strtol(token->text, &end, 10);

        if (end == token->text || *end != '\0') {
            fprintf(stderr, "Parser error: invalid integer literal\n");
            return NULL;
        }

        position++;

        ASTNode *node = ast_create_number(value);

        if (node == NULL) {
            fprintf(stderr, "Parser error: out of memory\n");
        }

        return node;
    }

    if (token->type == TOKEN_IDENTIFIER) {
        ASTNode *node = ast_create_identifier(token->text);

        if (node == NULL) {
            fprintf(stderr, "Parser error: out of memory\n");
            return NULL;
        }

        position++;
        return node;
    }


    if (token_is(TOKEN_SYMBOL, "(")) {
        position++;

        ASTNode *expression = parse_expression();

        if (expression == NULL) {
            return NULL;
        }

        if (!expect(TOKEN_SYMBOL, ")")) {
            ast_free(expression);
            return NULL;
        }

        return expression;
    }

    fprintf(stderr,
            "Parser error: expected a number, identifier, or parenthesized expression\n");
    return NULL;
}

static ASTNode *parse_multiplicative(void) {
    ASTNode *left = parse_primary();

    if (left == NULL) {
        return NULL;
    }

    while (token_is(TOKEN_SYMBOL, "*") ||
           token_is(TOKEN_SYMBOL, "/")) {
        char operator = current_token()->text[0];
        position++;

        ASTNode *right = parse_primary();

        if (right == NULL) {
            ast_free(left);
            return NULL;
        }

        ASTNode *binary = ast_create_binary(operator, left, right);

        if (binary == NULL) {
            ast_free(left);
            ast_free(right);
            fprintf(stderr, "Parser error: out of memory\n");
            return NULL;
        }

        left = binary;
    }

    return left;
}

static ASTNode *parse_expression(void) {
    ASTNode *left = parse_multiplicative();

    if (left == NULL) {
        return NULL;
    }

    while (token_is(TOKEN_SYMBOL, "+") ||
           token_is(TOKEN_SYMBOL, "-")) {
        char operator = current_token()->text[0];
        position++;

        ASTNode *right = parse_multiplicative();

        if (right == NULL) {
            ast_free(left);
            return NULL;
        }

        ASTNode *binary = ast_create_binary(operator, left, right);

        if (binary == NULL) {
            ast_free(left);
            ast_free(right);
            fprintf(stderr, "Parser error: out of memory\n");
            return NULL;
        }

        left = binary;
    }

    return left;
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

    ASTNode *value = parse_expression();

    if (value == NULL) {
        free(name);
        return NULL;
    }

    if (!expect(TOKEN_SYMBOL, ";") ||
        !expect(TOKEN_SYMBOL, "}")) {
        ast_free(value);
        free(name);
        return NULL;
    }

    ASTNode *return_node = ast_create_return(value);

    if (return_node == NULL) {
        ast_free(value);
        free(name);
        fprintf(stderr, "Parser error: out of memory\n");
        return NULL;
    }

    ASTNode *function_node = ast_create_function(name, return_node);
    free(name);

    if (function_node == NULL) {
        ast_free(return_node);
        fprintf(stderr, "Parser error: out of memory\n");
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
