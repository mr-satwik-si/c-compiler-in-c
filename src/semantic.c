#include "semantic.h"
#include "symbol_table.h"

#include <stdio.h>

static int analyze_expression(const ASTNode *node,
                              const SymbolTable *table) {
    if (node == NULL) {
        fprintf(stderr, "Semantic error: missing expression\n");
        return 0;
    }

    switch (node->type) {
        case AST_NUMBER:
            return 1;

        case AST_IDENTIFIER:
            if (node->as.identifier == NULL ||
                !symbol_table_contains(table, node->as.identifier)) {
                fprintf(stderr,
                        "Semantic error: undeclared variable '%s'\n",
                        node->as.identifier != NULL
                            ? node->as.identifier
                            : "<unknown>");
                return 0;
            }
            return 1;

        case AST_BINARY:
            if (node->as.binary.left == NULL ||
                node->as.binary.right == NULL) {
                fprintf(stderr,
                        "Semantic error: invalid binary expression\n");
                return 0;
            }

            return analyze_expression(node->as.binary.left, table) &&
                   analyze_expression(node->as.binary.right, table);

        default:
            fprintf(stderr,
                    "Semantic error: invalid expression node\n");
            return 0;
    }
}

int semantic_analyze(const ASTNode *node) {
    if (node == NULL) {
        fprintf(stderr, "Semantic error: no AST supplied\n");
        return 0;
    }

    if (node->type != AST_FUNCTION ||
        node->as.function.name == NULL ||
        node->as.function.body == NULL) {
        fprintf(stderr, "Semantic error: invalid function\n");
        return 0;
    }

    SymbolTable table;
    symbol_table_init(&table);

    const ASTNode *statement = node->as.function.body;

    while (statement != NULL) {
        if (statement->type == AST_VARIABLE_DECLARATION) {
            if (statement->as.variable_declaration.name == NULL) {
                fprintf(stderr,
                        "Semantic error: invalid variable declaration\n");
                symbol_table_free(&table);
                return 0;
            }

            if (!symbol_table_add(
                    &table,
                    statement->as.variable_declaration.name)) {
                fprintf(stderr,
                        "Semantic error: duplicate variable '%s' "
                        "or symbol table capacity exceeded\n",
                        statement->as.variable_declaration.name);
                symbol_table_free(&table);
                return 0;
            }
        } else if (statement->type == AST_RETURN) {
            if (statement->as.return_stmt.value == NULL) {
                fprintf(stderr,
                        "Semantic error: empty return statement\n");
                symbol_table_free(&table);
                return 0;
            }

            int valid = analyze_expression(
                statement->as.return_stmt.value, &table);

            symbol_table_free(&table);
            return valid;
        } else {
            fprintf(stderr,
                    "Semantic error: invalid statement\n");
            symbol_table_free(&table);
            return 0;
        }

        statement = statement->next;
    }

    fprintf(stderr, "Semantic error: missing return statement\n");
    symbol_table_free(&table);
    return 0;
}