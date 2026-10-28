
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

static int test_numeric_return(void) {
    ASTNode *ast = parse_source(
        "int main(void) { return 42; }"
    );

    if (ast == NULL) {
        puts("FAIL: numeric program could not be parsed");
        return 0;
    }

    int valid = semantic_analyze(ast);
    ast_free(ast);

    if (!valid) {
        puts("FAIL: numeric return should pass");
        return 0;
    }

    puts("PASS: parsed numeric return");
    return 1;
}

static int test_numeric_arithmetic(void) {
    ASTNode *ast = parse_source(
        "int main(void) { return 2 + 3 * 4; }"
    );

    if (ast == NULL) {
        puts("FAIL: arithmetic program could not be parsed");
        return 0;
    }

    int valid = semantic_analyze(ast);
    ast_free(ast);

    if (!valid) {
        puts("FAIL: numeric arithmetic should pass");
        return 0;
    }

    puts("PASS: parsed numeric arithmetic");
    return 1;
}

static int test_identifier_rejected(void) {
    ASTNode *ast = parse_source(
        "int main(void) { return x + 10; }"
    );

    if (ast == NULL) {
        puts("FAIL: identifier program could not be parsed");
        return 0;
    }

    int valid = !semantic_analyze(ast);
    ast_free(ast);

    if (!valid) {
        puts("FAIL: unsupported identifier should be rejected");
        return 0;
    }

    puts("PASS: unsupported identifier rejected");
    return 1;
}

static int test_null_ast_rejected(void) {
    int valid = !semantic_analyze(NULL);

    if (!valid) {
        puts("FAIL: missing AST should be rejected");
        return 0;
    }

    puts("PASS: missing AST rejected");
    return 1;
}

int main(void) {
    int passed = 1;

    passed &= test_numeric_return();
    passed &= test_numeric_arithmetic();
    passed &= test_identifier_rejected();
    passed &= test_null_ast_rejected();

    if (!passed) {
        puts("Some semantic tests failed.");
        return 1;
    }

    puts("All semantic tests passed.");
    return 0;
}
