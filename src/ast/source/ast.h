#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <stddef.h>

// Forward declarations to avoid circular dependencies
typedef struct ast_node_t ast_node_t;
typedef struct function_def_t function_def_t;
typedef struct statement_t statement_t;
typedef struct expression_t expression_t;
typedef struct identifier_t identifier_t;
typedef struct program_t program_t;
typedef struct unary_t unary_t;
typedef struct unary_operator_t unary_operator_t;

/**
 * Enum representing different types of AST nodes
 * Helps with runtime type checking and traversal
 */
typedef enum ast_node_type_e
{
    NODE_PROGRAM,
    NODE_FUNCTION_DEF,
    NODE_STATEMENT,
    NODE_EXPRESSION,
    NODE_IDENTIFIER,
} ast_node_type_t;

/**
 * Enum representing different types of expressions
 * Supports pattern matching and type-specific processing
 */
typedef enum expression_type_e
{
    EXPR_CONSTANT_INT,
    EXPR_UNARY
} expression_type_t;

/**
 * Enum representing different types of statements
 * Allows for extensible statement handling
 */
typedef enum statement_type_e
{
    STMT_RETURN
} statement_type_t;

/**
 * Base AST node with common metadata for all node types
 * Enables hierarchical tree traversal and error reporting
 */
struct ast_node_t
{
    ast_node_type_t type;
    struct
    {
        size_t line;
        size_t column;
    } location;
    ast_node_t *parent; // Use  for immutability
};

/**
 * Represents unary operators with their specific type
 */
struct unary_operator_t
{
    ast_node_t base;
    enum
    {
        BITWISE_COMPLEMENT,
        NEGATE,
    } operator;
};

/**
 * Unary expression combining an operator and an expression
 */
struct unary_t
{
    unary_operator_t *unary_operator;
    expression_t *expression;
};

/**
 * Identifier node for storing variable and function names
 */
struct identifier_t
{
    ast_node_t base;
    char *name;
};

/**
 * Expression node supporting different expression types
 */
struct expression_t
{
    ast_node_t base;
    expression_type_t expr_type;
    union
    {
        unary_t *unary;
        int constant_int;
    } value;
};

/**
 * Statement node supporting different statement types
 */
struct statement_t
{
    ast_node_t base;
    statement_type_t stmt_type;
    union
    {
        expression_t *return_expr;
    } value;
};

/**
 * Function definition node
 */
struct function_def_t
{
    ast_node_t base;
    identifier_t *name;
    statement_t *body;
};

/**
 * Top-level program node
 */
struct program_t
{
    ast_node_t base;
    function_def_t *function;
};

// Optional: Inline function for type checking
static inline bool is_ast_node(ast_node_t *node, ast_node_type_t type)
{
    return node && node->type == type;
}

#endif /* AST_H */