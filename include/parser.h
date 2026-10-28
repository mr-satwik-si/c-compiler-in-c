#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

ASTNode *parser_parse(const Token *tokens, size_t count);

#endif
