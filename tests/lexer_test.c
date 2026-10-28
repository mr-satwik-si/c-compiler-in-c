#include "lexer.h"

#include <stdio.h>
#include <string.h>

static int check_token(
    const Token *token,
    TokenType expected_type,
    const char *expected_text,
    size_t expected_line,
    size_t expected_column
) {
    return token->type == expected_type &&
           strcmp(token->text, expected_text) == 0 &&
           token->line == expected_line &&
           token->column == expected_column;
}

int main(void) {
    const char *source = "int value2(123);\nreturn 7;";
    size_t count = 0;

    Token *tokens = lexer_tokenize(source, &count);

    if (tokens == NULL) {
        fprintf(stderr, "FAIL: lexer rejected valid input\n");
        return 1;
    }

    const TokenType types[] = {
        TOKEN_KEYWORD,
        TOKEN_IDENTIFIER,
        TOKEN_SYMBOL,
        TOKEN_NUMBER,
        TOKEN_SYMBOL,
        TOKEN_SYMBOL,
        TOKEN_KEYWORD,
        TOKEN_NUMBER,
        TOKEN_SYMBOL,
        TOKEN_EOF
    };

    const char *texts[] = {
        "int", "value2", "(", "123", ")",
        ";", "return", "7", ";", ""
    };

    const size_t lines[] = {1, 1, 1, 1, 1, 1, 2, 2, 2, 2};
    const size_t columns[] = {1, 5, 11, 12, 15, 16, 1, 8, 9, 10};
    const size_t expected_count = sizeof(types) / sizeof(types[0]);

    if (count != expected_count) {
        fprintf(stderr, "FAIL: expected %zu tokens, got %zu\n",
                expected_count, count);
        lexer_free_tokens(tokens, count);
        return 1;
    }

    for (size_t i = 0; i < expected_count; i++) {
        if (!check_token(&tokens[i], types[i], texts[i],
                         lines[i], columns[i])) {
            fprintf(stderr, "FAIL: token %zu is incorrect\n", i);
            lexer_free_tokens(tokens, count);
            return 1;
        }
    }

    lexer_free_tokens(tokens, count);
    puts("PASS: token types, text, and positions");

    count = 0;
    tokens = lexer_tokenize("int @", &count);

    if (tokens != NULL || count != 0) {
        fprintf(stderr, "FAIL: invalid character was not rejected\n");
        lexer_free_tokens(tokens, count);
        return 1;
    }

    puts("PASS: invalid character rejected");
    puts("All lexer tests passed.");
    return 0;
}
