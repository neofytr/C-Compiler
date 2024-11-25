#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct ast_node ast_node_t;
typedef struct function_def function_def_t;
typedef struct statement statement_t;
typedef struct expression expression_t;
typedef struct identifier identifier_t;
typedef struct program program_t;

typedef enum
{
    NODE_PROGRAM,
    NODE_FUNCTION_DEF,
    NODE_STATEMENT,
    NODE_EXPRESSION,
    NODE_IDENTIFIER,
} ast_node_type_t;

typedef enum
{
    EXPR_CONSTANT_INT
} expression_type_t;

typedef enum
{
    STMT_RETURN
} statement_type_t;

struct ast_node
{
    ast_node_type_t type;
    struct
    {
        size_t line;
        size_t column;
    } location;
    ast_node_t *parent;
};

struct identifier
{
    ast_node_t base;
    char *name;
};

struct expression
{
    ast_node_t base;
    expression_type_t expr_type;
    union
    {
        int constant_int;
    } value;
};

struct statement
{
    ast_node_t base;
    statement_type_t stmt_type;
    union
    {
        expression_t *return_expr;
    } value;
};

struct function_def
{
    ast_node_t base;
    identifier_t *name;
    statement_t *body;
};

struct program
{
    ast_node_t base;
    function_def_t *function;
};

#endif /* AST_H */