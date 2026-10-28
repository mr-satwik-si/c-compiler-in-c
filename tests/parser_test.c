#include "lexer.h"
#include "parser.h"

#include <stdio.h>
#include <string.h>

static int test_valid_program(void) {
    const char *source = "int main(void) { return 42; }";
    size_t count = 0;
    Token *tokens = lexer_tokenize(source, &count);

    if (tokens == NULL) {
        puts("FAIL: lexer rejected valid input");
        return 0;
    }

    ASTNode *ast = parser_parse(tokens, count);
    lexer_free_tokens(tokens, count);

    if (ast == NULL) {
        puts("FAIL: parser rejected valid input");
        return 0;
    }

    int valid =
        ast->type == AST_FUNCTION &&
        strcmp(ast->as.function.name, "main") == 0 &&
        ast->as.function.body != NULL &&
        ast->as.function.body->type == AST_RETURN &&
        ast->as.function.body->as.return_stmt.value != NULL &&
        ast->as.function.body->as.return_stmt.value->type == AST_NUMBER &&
        ast->as.function.body->as.return_stmt.value->as.number == 42;

    ast_free(ast);

    if (!valid) {
        puts("FAIL: AST structure is incorrect");
        return 0;
    }

    puts("PASS: valid program produces the expected AST");
    return 1;
}

static int test_invalid_program(const char *source) {
    size_t count = 0;
    Token *tokens = lexer_tokenize(source, &count);

    if (tokens == NULL) {
        puts("PASS: invalid input rejected by lexer");
        return 1;
    }

    ASTNode *ast = parser_parse(tokens, count);
    lexer_free_tokens(tokens, count);

    if (ast != NULL) {
        ast_free(ast);
        puts("FAIL: parser accepted invalid input");
        return 0;
    }

    puts("PASS: invalid program rejected");
    return 1;
}

int main(void) {
    int passed = 1;

    passed &= test_valid_program();

    passed &= test_invalid_program(
        "int main(void) { return 42 }"
    );

    passed &= test_invalid_program(
        "int main(void) { return 42;"
    );

    if (!passed) {
        puts("Some parser tests failed.");
        return 1;
    }

    puts("All parser tests passed.");
    return 0;
}
