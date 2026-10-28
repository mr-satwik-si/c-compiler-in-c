#include "ast.h"

#include <stdio.h>
#include <string.h>

static int test_basic_nodes(void) {
    ASTNode *number = ast_create_number(42);
    ASTNode *return_node = ast_create_return(number);
    ASTNode *function = ast_create_function("main", return_node);

    if (function == NULL ||
        function->type != AST_FUNCTION ||
        strcmp(function->as.function.name, "main") != 0 ||
        function->as.function.body != return_node ||
        return_node->type != AST_RETURN ||
        return_node->as.return_stmt.value != number ||
        number->type != AST_NUMBER ||
        number->as.number != 42) {
        ast_free(function);
        puts("FAIL: function, return, and number nodes");
        return 0;
    }

    ast_free(function);
    puts("PASS: function, return, and number nodes");
    return 1;
}

static int test_identifier_node(void) {
    ASTNode *node = ast_create_identifier("answer");

    if (node == NULL ||
        node->type != AST_IDENTIFIER ||
        strcmp(node->as.identifier, "answer") != 0) {
        ast_free(node);
        puts("FAIL: identifier node");
        return 0;
    }

    ast_free(node);
    puts("PASS: identifier node");
    return 1;
}

static int test_binary_node(void) {
    ASTNode *left = ast_create_number(2);
    ASTNode *right = ast_create_number(3);

    if (left == NULL || right == NULL) {
        ast_free(left);
        ast_free(right);
        puts("FAIL: binary expression node");
        return 0;
    }

    ASTNode *binary = ast_create_binary('+', left, right);

    if (binary == NULL ||
        binary->type != AST_BINARY ||
        binary->as.binary.operator != '+' ||
        binary->as.binary.left != left ||
        binary->as.binary.right != right) {
        ast_free(binary);
        puts("FAIL: binary expression node");
        return 0;
    }

    ast_free(binary);
    puts("PASS: binary expression node");
    return 1;
}

static int test_variable_declaration_node(void) {
    ASTNode *node = ast_create_variable_declaration("count");

    if (node == NULL ||
        node->type != AST_VARIABLE_DECLARATION ||
        strcmp(node->as.variable_declaration.name, "count") != 0) {
        ast_free(node);
        puts("FAIL: variable declaration node");
        return 0;
    }

    ast_free(node);
    puts("PASS: variable declaration node");
    return 1;
}

int main(void) {
    int passed = 0;

    passed += test_basic_nodes();
    passed += test_identifier_node();
    passed += test_binary_node();
    passed += test_variable_declaration_node();

    if (passed != 4) {
        puts("Some AST tests failed.");
        return 1;
    }

    puts("All AST tests passed.");
    return 0;
}