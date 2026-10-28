#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

/*
 * Returns 1 if the AST passes semantic checks.
 * Returns 0 if a semantic error is found.
 */
int semantic_analyze(const ASTNode *node);

#endif
