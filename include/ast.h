
#ifndef AST_H
#define AST_H

typedef enum {
    AST_FUNCTION,
    AST_RETURN,
    AST_NUMBER,
    AST_IDENTIFIER,
    AST_BINARY
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

        char *identifier;

        struct {
            char operator;
            ASTNode *left;
            ASTNode *right;
        } binary;
    } as;
};

ASTNode *ast_create_number(long value);
ASTNode *ast_create_identifier(const char *name);
ASTNode *ast_create_binary(char operator, ASTNode *left, ASTNode *right);
ASTNode *ast_create_return(ASTNode *value);
ASTNode *ast_create_function(const char *name, ASTNode *body);

void ast_free(ASTNode *node);

#endif
 