#include "ast.h"

#include <stdlib.h>
#include <string.h>

static ASTNode *ast_create_node(ASTNodeType type) {
    ASTNode *node = malloc(sizeof(*node));

    if (node == NULL) {
        return NULL;
    }

    node->type = type;
    node->next = NULL;
    return node;
}

ASTNode *ast_create_number(long value) {
    ASTNode *node = ast_create_node(AST_NUMBER);

    if (node == NULL) {
        return NULL;
    }

    node->as.number = value;
    return node;
}

ASTNode *ast_create_return(ASTNode *value) {
    ASTNode *node = ast_create_node(AST_RETURN);

    if (node == NULL) {
        return NULL;
    }

    node->as.return_stmt.value = value;
    return node;
}

ASTNode *ast_create_function(const char *name, ASTNode *body) {
    if (name == NULL) {
        return NULL;
    }

    ASTNode *node = ast_create_node(AST_FUNCTION);

    if (node == NULL) {
        return NULL;
    }

    size_t length = strlen(name) + 1;
    node->as.function.name = malloc(length);

    if (node->as.function.name == NULL) {
        free(node);
        return NULL;
    }

    memcpy(node->as.function.name, name, length);
    node->as.function.body = body;

    return node;
}

void ast_free(ASTNode *node) {
    while (node != NULL) {
        ASTNode *next = node->next;

        switch (node->type) {
            case AST_FUNCTION:
                free(node->as.function.name);
                ast_free(node->as.function.body);
                break;

            case AST_RETURN:
                ast_free(node->as.return_stmt.value);
                break;

            case AST_NUMBER:
                break;
        }

        free(node);
        node = next;
    }
}
