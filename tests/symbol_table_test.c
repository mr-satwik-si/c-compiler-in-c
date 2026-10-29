#include "symbol_table.h"

#include <stdio.h>

int main(void) {
    SymbolTable table;
    symbol_table_init(&table);

    if (table.count != 0) {
        puts("FAIL: table should start empty");
        symbol_table_free(&table);
        return 1;
    }
    puts("PASS: initialize empty table");

    if (!symbol_table_add(&table, "x")) {
        puts("FAIL: add variable");
        symbol_table_free(&table);
        return 1;
    }
    puts("PASS: add variable");

    if (!symbol_table_contains(&table, "x")) {
        puts("FAIL: find declared variable");
        symbol_table_free(&table);
        return 1;
    }
    puts("PASS: find declared variable");

    if (symbol_table_contains(&table, "y")) {
        puts("FAIL: undeclared variable should not exist");
        symbol_table_free(&table);
        return 1;
    }
    puts("PASS: undeclared variable not found");

    if (symbol_table_add(&table, "x")) {
        puts("FAIL: duplicate variable should be rejected");
        symbol_table_free(&table);
        return 1;
    }
    puts("PASS: reject duplicate variable");

    symbol_table_free(&table);

    if (table.count != 0) {
        puts("FAIL: free should reset the table");
        return 1;
    }
    puts("PASS: free symbol table");

    puts("All symbol table tests passed.");
    return 0;
}