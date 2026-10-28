#include "lexer.h"
#include "parser.h"

#include <stdio.h>

static int test_program(const char *source, int expected_result) {
    size_t count = 0;
    Token *tokens = lexer_tokenize(source, &count);

    if (tokens == NULL) {
        if (expected_result == 0) {
            puts("PASS: invalid input rejected by lexer");
            return 1;
        }

        puts("FAIL: valid input rejected by lexer");
        return 0;
    }

    int result = parser_parse(tokens, count);
    lexer_free_tokens(tokens, count);

    if (result == expected_result) {
        puts(expected_result ? "PASS: valid program accepted"
                             : "PASS: invalid program rejected");
        return 1;
    }

    puts("FAIL: unexpected parser result");
    return 0;
}

int main(void) {
    int passed = 1;

    passed &= test_program(
        "int main(void) { return 42; }", 1
    );

    passed &= test_program(
        "int main(void) { return 42 }", 0
    );

    passed &= test_program(
        "int main(void) { return 42;", 0
    );

    if (!passed) {
        puts("Some parser tests failed.");
        return 1;
    }

    puts("All parser tests passed.");
    return 0;
}
