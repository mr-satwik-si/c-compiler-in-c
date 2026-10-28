
#include "lexer.h"
#include "parser.h"

#include <stdio.h>
#include <string.h>

static ASTNode *parse_source(const char *source) {
    size_t count = 0;
    Token *tokens = lexer_tokenize(source, &count);

    if (tokens == NULL) {
        return NULL;
    }

    ASTNode *ast = parser_parse(tokens, count);
    lexer_free_tokens(tokens, count);

    return ast;
}

static int test_simple_return(void) {
    ASTNode *ast = parse_source(
        "int main(void) { return 42; }"
    );

    int valid =
        ast != NULL &&
        ast->type == AST_FUNCTION &&
        strcmp(ast->as.function.name, "main") == 0 &&
        ast->as.function.body->type == AST_RETURN &&
        ast->as.function.body->as.return_stmt.value->type == AST_NUMBER &&
        ast->as.function.body->as.return_stmt.value->as.number == 42;

    ast_free(ast);

    if (!valid) {
        puts("FAIL: simple return expression");
        return 0;
    }

    puts("PASS: simple return expression");
    return 1;
}

static int test_arithmetic_precedence(void) {
    ASTNode *ast = parse_source(
        "int main(void) { return 2 + 3 * 4; }"
    );

    if (ast == NULL) {
        puts("FAIL: arithmetic expression was rejected");
        return 0;
    }

    ASTNode *value = ast->as.function.body->as.return_stmt.value;

    int valid =
        value->type == AST_BINARY &&
        value->as.binary.operator == '+' &&
        value->as.binary.left->type == AST_NUMBER &&
        value->as.binary.left->as.number == 2 &&
        value->as.binary.right->type == AST_BINARY &&
        value->as.binary.right->as.binary.operator == '*' &&
        value->as.binary.right->as.binary.left->as.number == 3 &&
        value->as.binary.right->as.binary.right->as.number == 4;

    ast_free(ast);

    if (!valid) {
        puts("FAIL: operator precedence is incorrect");
        return 0;
    }

    puts("PASS: multiplication has higher precedence than addition");
    return 1;
}

static int test_identifier_expression(void) {
    ASTNode *ast = parse_source(
        "int main(void) { return x + 10; }"
    );

    if (ast == NULL) {
        puts("FAIL: identifier expression was rejected");
        return 0;
    }

    ASTNode *value = ast->as.function.body->as.return_stmt.value;

    int valid =
        value->type == AST_BINARY &&
        value->as.binary.operator == '+' &&
        value->as.binary.left->type == AST_IDENTIFIER &&
        strcmp(value->as.binary.left->as.identifier, "x") == 0 &&
        value->as.binary.right->type == AST_NUMBER &&
        value->as.binary.right->as.number == 10;

    ast_free(ast);

    if (!valid) {
        puts("FAIL: identifier expression AST is incorrect");
        return 0;
    }

    puts("PASS: identifier expression");
    return 1;
}

static int test_parenthesized_expression(void) {
    ASTNode *ast = parse_source(
        "int main(void) { return (2 + 3) * 4; }"
    );

    if (ast == NULL) {
        puts("FAIL: parenthesized expression was rejected");
        return 0;
    }

    ASTNode *value = ast->as.function.body->as.return_stmt.value;

    int valid =
        value->type == AST_BINARY &&
        value->as.binary.operator == '*' &&
        value->as.binary.left->type == AST_BINARY &&
        value->as.binary.left->as.binary.operator == '+' &&
        value->as.binary.left->as.binary.left->type == AST_NUMBER &&
        value->as.binary.left->as.binary.left->as.number == 2 &&
        value->as.binary.left->as.binary.right->type == AST_NUMBER &&
        value->as.binary.left->as.binary.right->as.number == 3 &&
        value->as.binary.right->type == AST_NUMBER &&
        value->as.binary.right->as.number == 4;

    ast_free(ast);

    if (!valid) {
        puts("FAIL: parenthesized expression AST is incorrect");
        return 0;
    }

    puts("PASS: parentheses override operator precedence");
    return 1;
}

static int test_variable_declaration(void) {
    ASTNode *ast = parse_source(
        "int main(void) { int x; return 10; }"
    );

    if (ast == NULL) {
        puts("FAIL: variable declaration was rejected");
        return 0;
    }

    ASTNode *declaration = ast->as.function.body;
    ASTNode *return_node =
        declaration != NULL ? declaration->next : NULL;

    int valid =
        declaration != NULL &&
        declaration->type == AST_VARIABLE_DECLARATION &&
        strcmp(declaration->as.variable_declaration.name, "x") == 0 &&
        return_node != NULL &&
        return_node->type == AST_RETURN &&
        return_node->next == NULL;

    ast_free(ast);

    if (!valid) {
        puts("FAIL: variable declaration AST is incorrect");
        return 0;
    }

    puts("PASS: variable declaration before return");
    return 1;
}

static int test_multiple_declarations(void) {
    ASTNode *ast = parse_source(
        "int main(void) { int x; int y; return x + y; }"
    );

    if (ast == NULL) {
        puts("FAIL: multiple declarations were rejected");
        return 0;
    }

    ASTNode *first = ast->as.function.body;
    ASTNode *second = first != NULL ? first->next : NULL;
    ASTNode *return_node = second != NULL ? second->next : NULL;

    int valid =
        first != NULL &&
        first->type == AST_VARIABLE_DECLARATION &&
        strcmp(first->as.variable_declaration.name, "x") == 0 &&
        second != NULL &&
        second->type == AST_VARIABLE_DECLARATION &&
        strcmp(second->as.variable_declaration.name, "y") == 0 &&
        return_node != NULL &&
        return_node->type == AST_RETURN &&
        return_node->next == NULL;

    ast_free(ast);

    if (!valid) {
        puts("FAIL: multiple declaration AST is incorrect");
        return 0;
    }

    puts("PASS: multiple variable declarations");
    return 1;
}

static int test_invalid_program(const char *source) {
    ASTNode *ast = parse_source(source);

    if (ast != NULL) {
        ast_free(ast);
        puts("FAIL: invalid program was accepted");
        return 0;
    }

    puts("PASS: invalid program rejected");
    return 1;
}

int main(void) {
    int passed = 1;

    passed &= test_simple_return();
    passed &= test_arithmetic_precedence();
    passed &= test_identifier_expression();
    passed &= test_parenthesized_expression();
    passed &= test_variable_declaration();
    passed &= test_multiple_declarations();

    passed &= test_invalid_program(
        "int main(void) { return 42 }"
    );

    passed &= test_invalid_program(
        "int main(void) { return 2 + ; }"
    );

    passed &= test_invalid_program(
        "int main(void) { return (2 + 3 * 4; }"
    );

    passed &= test_invalid_program(
        "int main(void) { int x return x; }"
    );

    if (!passed) {
        puts("Some parser tests failed.");
        return 1;
    }

    puts("All parser tests passed.");
    return 0;
}
