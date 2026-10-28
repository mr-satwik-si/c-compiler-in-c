
#include "semantic.h"

#include <stdio.h>

static int analyze_node(const ASTNode *node) {
    if (node == NULL) {
        fprintf(stderr, "Semantic error: missing AST node\n");
        return 0;
    }

    switch (node->type) {
        case AST_FUNCTION:
            if (node->as.function.name == NULL ||
                node->as.function.body == NULL) {
                fprintf(stderr, "Semantic error: invalid function\n");
                return 0;
            }

            return analyze_node(node->as.function.body);

        case AST_RETURN:
            if (node->as.return_stmt.value == NULL) {
                fprintf(stderr, "Semantic error: empty return statement\n");
                return 0;
            }

            return analyze_node(node->as.return_stmt.value);

        case AST_NUMBER:
            return 1;

        case AST_IDENTIFIER:
            fprintf(stderr,
                    "Semantic error: variable '%s' is not supported yet\n",
                    node->as.identifier != NULL
                        ? node->as.identifier
                        : "<unknown>");
            return 0;

        case AST_BINARY:
            if (node->as.binary.left == NULL ||
                node->as.binary.right == NULL) {
                fprintf(stderr,
                        "Semantic error: invalid binary expression\n");
                return 0;
            }

            return analyze_node(node->as.binary.left) &&
                   analyze_node(node->as.binary.right);
    }

    fprintf(stderr, "Semantic error: unknown AST node type\n");
    return 0;
}

int semantic_analyze(const ASTNode *node) {
    if (node == NULL) {
        fprintf(stderr, "Semantic error: no AST supplied\n");
        return 0;
    }

    return analyze_node(node);
}
