#include "symbol_table.h"

#include <stdlib.h>
#include <string.h>

void symbol_table_init(SymbolTable *table) {
    if (table == NULL) {
        return;
    }

    table->count = 0;

    for (size_t i = 0; i < SYMBOL_TABLE_CAPACITY; i++) {
        table->names[i] = NULL;
    }
}

int symbol_table_add(SymbolTable *table, const char *name) {
    if (table == NULL || name == NULL || name[0] == '\0') {
        return 0;
    }

    if (symbol_table_contains(table, name)) {
        return 0;
    }

    if (table->count >= SYMBOL_TABLE_CAPACITY) {
        return 0;
    }

    size_t length = strlen(name) + 1;
    char *copy = malloc(length);

    if (copy == NULL) {
        return 0;
    }

    memcpy(copy, name, length);

    table->names[table->count] = copy;
    table->count++;

    return 1;
}

int symbol_table_contains(const SymbolTable *table, const char *name) {
    if (table == NULL || name == NULL) {
        return 0;
    }

    for (size_t i = 0; i < table->count; i++) {
        if (strcmp(table->names[i], name) == 0) {
            return 1;
        }
    }

    return 0;
}

void symbol_table_free(SymbolTable *table) {
    if (table == NULL) {
        return;
    }

    for (size_t i = 0; i < table->count; i++) {
        free(table->names[i]);
        table->names[i] = NULL;
    }

    table->count = 0;
}