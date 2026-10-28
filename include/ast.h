#ifndef AST_H
#define AST_H

typedef enum {
    AST_FUNCTION,
    AST_RETURN,
    AST_NUMBER
} ASTNodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    ASTNodeType type;
    ASTNode *next;

    union {
        struct {
            char *name;
            ASTNode *body;
        } function;

        struct {
            ASTNode *value;
        } return_stmt;

        long number;
    } as;
};

ASTNode *ast_create_number(long value);
ASTNode *ast_create_return(ASTNode *value);
ASTNode *ast_create_function(const char *name, ASTNode *body);

void ast_free(ASTNode *node);

#endif
