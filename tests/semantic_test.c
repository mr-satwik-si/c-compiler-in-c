#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"

#include <stdio.h>

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

static int check_program(const char *source, int expected) {
    ASTNode *ast = parse_source(source);

    if (ast == NULL) {
        puts("FAIL: program could not be parsed");
        return 0;
    }

    int result = semantic_analyze(ast);
    ast_free(ast);

    if (result != expected) {
        puts("FAIL: unexpected semantic analysis result");
        return 0;
    }

    return 1;
}

static int test_numeric_return(void) {
    if (!check_program("int main(void) { return 42; }", 1)) {
        return 0;
    }

    puts("PASS: numeric return");
    return 1;
}

static int test_numeric_arithmetic(void) {
    if (!check_program("int main(void) { return 2 + 3 * 4; }", 1)) {
        return 0;
    }

    puts("PASS: numeric arithmetic");
    return 1;
}

static int test_declared_variable(void) {
    if (!check_program(
            "int main(void) { int x; return x + 10; }", 1)) {
        return 0;
    }

    puts("PASS: declared variable accepted");
    return 1;
}

static int test_multiple_declared_variables(void) {
    if (!check_program(
            "int main(void) { int x; int y; return x + y; }", 1)) {
        return 0;
    }

    puts("PASS: multiple declared variables accepted");
    return 1;
}

static int test_undeclared_variable(void) {
    if (!check_program("int main(void) { return y + 10; }", 0)) {
        return 0;
    }

    puts("PASS: undeclared variable rejected");
    return 1;
}

static int test_duplicate_variable(void) {
    if (!check_program(
            "int main(void) { int x; int x; return x; }", 0)) {
        return 0;
    }

    puts("PASS: duplicate variable rejected");
    return 1;
}

static int test_null_ast_rejected(void) {
    if (semantic_analyze(NULL)) {
        puts("FAIL: missing AST should be rejected");
        return 0;
    }

    puts("PASS: missing AST rejected");
    return 1;
}

int main(void) {
    int passed = 0;

    passed += test_numeric_return();
    passed += test_numeric_arithmetic();
    passed += test_declared_variable();
    passed += test_multiple_declared_variables();
    passed += test_undeclared_variable();
    passed += test_duplicate_variable();
    passed += test_null_ast_rejected();

    if (passed != 7) {
        puts("Some semantic tests failed.");
        return 1;
    }

    puts("All semantic tests passed.");
    return 0;
}