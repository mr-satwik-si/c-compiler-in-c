
#include "ast.h"

#include <stdio.h>
#include <string.h>

static int test_existing_ast(void) {
    ASTNode *number = ast_create_number(42);

    if (number == NULL) {
        puts("FAIL: could not create number node");
        return 0;
    }

    ASTNode *return_stmt = ast_create_return(number);

    if (return_stmt == NULL) {
        ast_free(number);
        puts("FAIL: could not create return node");
        return 0;
    }

    ASTNode *function = ast_create_function("main", return_stmt);

    if (function == NULL) {
        ast_free(return_stmt);
        puts("FAIL: could not create function node");
        return 0;
    }

    int valid =
        function->type == AST_FUNCTION &&
        strcmp(function->as.function.name, "main") == 0 &&
        function->as.function.body->type == AST_RETURN &&
        function->as.function.body->as.return_stmt.value->type == AST_NUMBER &&
        function->as.function.body->as.return_stmt.value->as.number == 42;

    ast_free(function);

    if (!valid) {
        puts("FAIL: existing AST structure is incorrect");
        return 0;
    }

    puts("PASS: function, return, and number nodes");
    return 1;
}

static int test_identifier_node(void) {
    ASTNode *identifier = ast_create_identifier("total");

    if (identifier == NULL) {
        puts("FAIL: could not create identifier node");
        return 0;
    }

    int valid =
        identifier->type == AST_IDENTIFIER &&
        strcmp(identifier->as.identifier, "total") == 0;

    ast_free(identifier);

    if (!valid) {
        puts("FAIL: identifier node is incorrect");
        return 0;
    }

    puts("PASS: identifier node");
    return 1;
}

static int test_binary_node(void) {
    ASTNode *left = ast_create_identifier("x");
    ASTNode *right = ast_create_number(10);

    if (left == NULL || right == NULL) {
        ast_free(left);
        ast_free(right);
        puts("FAIL: could not create binary operands");
        return 0;
    }

    ASTNode *binary = ast_create_binary('+', left, right);

    if (binary == NULL) {
        ast_free(left);
        ast_free(right);
        puts("FAIL: could not create binary node");
        return 0;
    }

    int valid =
        binary->type == AST_BINARY &&
        binary->as.binary.operator == '+' &&
        binary->as.binary.left->type == AST_IDENTIFIER &&
        strcmp(binary->as.binary.left->as.identifier, "x") == 0 &&
        binary->as.binary.right->type == AST_NUMBER &&
        binary->as.binary.right->as.number == 10;

    ast_free(binary);

    if (!valid) {
        puts("FAIL: binary expression node is incorrect");
        return 0;
    }

    puts("PASS: binary expression node");
    return 1;
}

int main(void) {
    int passed = 1;

    passed &= test_existing_ast();
    passed &= test_identifier_node();
    passed &= test_binary_node();

    if (!passed) {
        puts("Some AST tests failed.");
        return 1;
    }

    puts("All AST tests passed.");
    return 0;
}
