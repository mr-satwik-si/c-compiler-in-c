#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

static const Token *tokens;
static size_t token_count;
static size_t position;
static int parser_failed;

static const Token *current_token(void) {
    if (position >= token_count) {
        return NULL;
    }

    return &tokens[position];
}

static int token_is(TokenType type, const char *text) {
    const Token *token = current_token();

    if (token == NULL || token->type != type) {
        return 0;
    }

    if (text == NULL) {
        return 1;
    }

    return strcmp(token->text, text) == 0;
}

static void parser_error(const char *message) {
    if (!parser_failed) {
        fprintf(stderr, "Parser error: %s\n", message);
    }

    parser_failed = 1;
}

static int expect(TokenType type, const char *text) {
    if (!token_is(type, text)) {
        parser_error("unexpected token");
        return 0;
    }

    position++;
    return 1;
}

static ASTNode *parse_expression(void);

static ASTNode *parse_primary(void) {
    const Token *token = current_token();

    if (token == NULL) {
        parser_error("unexpected end of input");
        return NULL;
    }

    if (token->type == TOKEN_NUMBER) {
        char *end = NULL;
        long value = strtol(token->text, &end, 10);

        if (end == token->text || *end != '\0') {
            parser_error("invalid number");
            return NULL;
        }

        position++;
        return ast_create_number(value);
    }

    if (token->type == TOKEN_IDENTIFIER) {
        ASTNode *node = ast_create_identifier(token->text);

        if (node == NULL) {
            parser_error("failed to create identifier node");
            return NULL;
        }

        position++;
        return node;
    }

    if (token_is(TOKEN_SYMBOL, "(")) {
        position++;

        ASTNode *node = parse_expression();

        if (node == NULL) {
            return NULL;
        }

        if (!expect(TOKEN_SYMBOL, ")")) {
            ast_free(node);
            return NULL;
        }

        return node;
    }

    parser_error(
        "expected a number, identifier, or parenthesized expression"
    );

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

        ASTNode *combined =
            ast_create_binary(operator, left, right);

        if (combined == NULL) {
            ast_free(left);
            ast_free(right);
            parser_error("failed to create binary expression");
            return NULL;
        }

        left = combined;
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

        ASTNode *combined =
            ast_create_binary(operator, left, right);

        if (combined == NULL) {
            ast_free(left);
            ast_free(right);
            parser_error("failed to create binary expression");
            return NULL;
        }

        left = combined;
    }

    return left;
}

static ASTNode *parse_variable_declaration(void) {
    if (!expect(TOKEN_KEYWORD, "int")) {
        return NULL;
    }

    const Token *name_token = current_token();

    if (name_token == NULL ||
        name_token->type != TOKEN_IDENTIFIER) {
        parser_error("expected variable name");
        return NULL;
    }

    ASTNode *declaration =
        ast_create_variable_declaration(name_token->text);

    if (declaration == NULL) {
        parser_error("failed to create variable declaration");
        return NULL;
    }

    position++;

    if (!expect(TOKEN_SYMBOL, ";")) {
        ast_free(declaration);
        return NULL;
    }

    return declaration;
}

static ASTNode *parse_return_statement(void) {
    if (!expect(TOKEN_KEYWORD, "return")) {
        return NULL;
    }

    ASTNode *value = parse_expression();

    if (value == NULL) {
        return NULL;
    }

    if (!expect(TOKEN_SYMBOL, ";")) {
        ast_free(value);
        return NULL;
    }

    ASTNode *node = ast_create_return(value);

    if (node == NULL) {
        ast_free(value);
        parser_error("failed to create return node");
        return NULL;
    }

    return node;
}

static ASTNode *parse_function(void) {
    if (!expect(TOKEN_KEYWORD, "int")) {
        return NULL;
    }

    const Token *name_token = current_token();

    if (name_token == NULL ||
        name_token->type != TOKEN_IDENTIFIER) {
        parser_error("expected function name");
        return NULL;
    }

    char *function_name = malloc(strlen(name_token->text) + 1);

    if (function_name == NULL) {
        parser_error("out of memory");
        return NULL;
    }

    strcpy(function_name, name_token->text);
    position++;

    if (!expect(TOKEN_SYMBOL, "(") ||
        !expect(TOKEN_KEYWORD, "void") ||
        !expect(TOKEN_SYMBOL, ")") ||
        !expect(TOKEN_SYMBOL, "{")) {
        free(function_name);
        return NULL;
    }

    ASTNode *body = NULL;
    ASTNode *tail = NULL;

    while (token_is(TOKEN_KEYWORD, "int")) {
        ASTNode *declaration = parse_variable_declaration();

        if (declaration == NULL) {
            ast_free(body);
            free(function_name);
            return NULL;
        }

        if (body == NULL) {
            body = declaration;
        } else {
            tail->next = declaration;
        }

        tail = declaration;
    }

    ASTNode *return_node = parse_return_statement();

    if (return_node == NULL) {
        ast_free(body);
        free(function_name);
        return NULL;
    }

    if (body == NULL) {
        body = return_node;
    } else {
        tail->next = return_node;
    }

    if (!expect(TOKEN_SYMBOL, "}")) {
        ast_free(body);
        free(function_name);
        return NULL;
    }

    if (position < token_count &&
    !token_is(TOKEN_EOF, NULL)) {
    parser_error("unexpected tokens after function");
    ast_free(body);
    free(function_name);
    return NULL;
}

    ASTNode *function =
        ast_create_function(function_name, body);

    free(function_name);

    if (function == NULL) {
        ast_free(body);
        parser_error("failed to create function node");
        return NULL;
    }

    return function;
}

ASTNode *parser_parse(const Token *input_tokens, size_t count) {
    if (input_tokens == NULL || count == 0) {
        return NULL;
    }

    tokens = input_tokens;
    token_count = count;
    position = 0;
    parser_failed = 0;

    ASTNode *root = parse_function();

    if (parser_failed) {
        ast_free(root);
        return NULL;
    }

    return root;
}