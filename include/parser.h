#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

int parser_parse(const Token *tokens, size_t count);

#endif
