#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stddef.h>

#define SYMBOL_TABLE_CAPACITY 100

typedef struct {
    char *names[SYMBOL_TABLE_CAPACITY];
    size_t count;
} SymbolTable;

void symbol_table_init(SymbolTable *table);
int symbol_table_add(SymbolTable *table, const char *name);
int symbol_table_contains(const SymbolTable *table, const char *name);
void symbol_table_free(SymbolTable *table);

#endif