#include "ast.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    ASTNode *number = ast_create_number(42);

    if (number == NULL) {
        puts("FAIL: could not create number node");
        return 1;
    }

    ASTNode *return_stmt = ast_create_return(number);

    if (return_stmt == NULL) {
        ast_free(number);
        puts("FAIL: could not create return node");
        return 1;
    }

    ASTNode *function = ast_create_function("main", return_stmt);

    if (function == NULL) {
        ast_free(return_stmt);
        puts("FAIL: could not create function node");
        return 1;
    }

    int valid =
        function->type == AST_FUNCTION &&
        strcmp(function->as.function.name, "main") == 0 &&
        function->as.function.body->type == AST_RETURN &&
        function->as.function.body->as.return_stmt.value->type == AST_NUMBER &&
        function->as.function.body->as.return_stmt.value->as.number == 42;

    ast_free(function);

    if (!valid) {
        puts("FAIL: AST structure is incorrect");
        return 1;
    }

    puts("PASS: AST nodes created and verified");
    puts("PASS: AST memory cleanup completed");
    puts("All AST tests passed.");

    return 0;
}
