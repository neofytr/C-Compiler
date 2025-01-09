#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct ast_node_t ast_node_t;
typedef struct function_def_t function_def_t;
typedef struct statement_t statement_t;
typedef struct expression_t expression_t;
typedef struct identifier_t identifier_t;
typedef struct program_t program_t;
typedef struct unary_t unary_t;
typedef struct unary_operator_t unary_operator_t;
typedef struct binary_t binary_t;
typedef struct binary_operator_t binary_operator_t;
typedef struct variable_t variable_t;
typedef struct assignment_t assignment_t;
typedef struct null_expr_t null_expr_t;
typedef struct declaration_t declaration_t;
typedef struct block_item_t block_item_t;

typedef enum ast_node_type_e
{
    NODE_PROGRAM,
    NODE_FUNCTION_DEF,
    NODE_STATEMENT,
    NODE_EXPRESSION,
    NODE_NULL_EXPRESSION,
    NODE_DECLARATION,
    NODE_IDENTIFIER,
    NODE_UNARY_OP,
    NODE_BINARY_OP,
} ast_node_type_t;

typedef enum expression_type_e
{
    EXPR_CONSTANT_INT,
    EXPR_UNARY,
    EXPR_NESTED,
    EXPR_BINARY,
    EXPR_VAR,
    EXPR_ASSIGN,
} expression_type_t;

typedef enum statement_type_e
{
    STMT_RETURN
} statement_type_t;

/**
 * Base AST node with common metadata for all node types
 */
struct ast_node_t
{
    ast_node_type_t type;
    struct
    {
        size_t line;
        size_t column;
    } location;
    ast_node_t *parent;
};

/**
 * Represents unary operators with their specific type
 */

typedef enum
{
    BITWISE_COMPLEMENT,
    NEGATE,
    NOT,
} unary_operator_type_t;

struct unary_operator_t
{
    ast_node_t base;
    unary_operator_type_t op;
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

typedef enum
{
    // Multiplicative operators
    BINARY_MUL,
    BINARY_DIV,
    BINARY_REM,

    BINARY_ADD,
    BINARY_SUB,

    BINARY_LEFT_SHIFT,
    BINARY_RIGHT_SHIFT,

    BINARY_LESS_THAN,
    BINARY_LESS_THAN_EQUAL,
    BINARY_GREATER_THAN,
    BINARY_GREATER_THAN_EQUAL,

    BINARY_EQUAL,
    BINARY_NOT_EQUAL,

    BINARY_BITWISE_AND,
    BINARY_BITWISE_XOR,
    BINARY_BITWISE_OR,

    BINARY_LOGICAL_AND,
    BINARY_LOGICAL_OR,
} binary_operator_type_t;

struct binary_operator_t
{
    ast_node_t base;
    binary_operator_type_t binary_operator;
};

struct declaration_t
{
    ast_node_t base;
    identifier_t *name;
    expression_t *init_expr;
    bool has_init_expr;
    /* currently, we will only support int type */
};

/* declarations are a separate AST node, nother another kind of statement,
because declarations aren't statements */

/* the difference is that statements are executed when the program runs, whereas
declarations simply tell the compiler that some identifier exists and can be
used later */

/* this distinction will become obvious during IR generation; we'll handle declarations
with initializers like normal variable assignments, but declarations without initializers will just
disappear */

/* the more concrete difference, from the parser's perspective, is that there are parts of a program
where a statement can appear but a declaration can't */

/* for ex, the body of an if statement is always another statement: */

/*

if (a == 2)
    return 4;

*/

/* It can't be a declaration, because declarations aren't statements; so this is invalid: */

/*

if (a == 2)
    int x = 0;

*/

/* A list of statements and declarations wrapped in braces is actually a single statement, called a
compound statement; it can be placed anywhere a statement can, for ex, after an if constructor */

/* we need to distinguish between a declaration and a statement in the ast */

struct binary_t
{
    binary_operator_t *op;
    expression_t *left_expr;
    expression_t *right_expr;
};

struct assignment_t
{
    expression_t *lvalue;
    /* when we parse the program, we'll allow any expression on the left-hand side of an assignment */
    /* in the semantic analysis stage, we'll make sure that it's a valid lvalue */
    /* we validate lvalues during semantic analysis, rather than during parsing, because we'll need to support more complex lvalues later*/
    expression_t *rvalue;
};

struct variable_t
{
    identifier_t *name;
};

struct expression_t
{
    ast_node_t base;
    expression_type_t expr_type;
    union
    {
        unary_t unary;
        int constant_int;
        expression_t *nested_expr;
        binary_t binary;
        variable_t var;      // to support variables in expressions
        assignment_t assign; // since variable assignment is also an expression
    } value;
};

struct null_expr_t
{
    ast_node_t base;
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
        expression_t *expr; // expressions can just as well be statements too; foo = 3 * 3 is also an expression
                            // with value 9 but with the side effect of updating foo to 9
        null_expr_t *null_expr;
    } value;
};

typedef enum
{
    BLOCK_STATEMENT,
    BLOCK_DECLARATION,
} block_type_t;

struct block_item_t
{
    block_type_t type;
    union
    {
        statement_t *statement;
        declaration_t *declaration;
    } value;
};

/**
 * Function definition node
 */
struct function_def_t
{
    ast_node_t base;
    identifier_t *name;
    size_t block_count;
    block_item_t **body;
};

/**
 * Top-level program node
 */
struct program_t
{
    ast_node_t base;
    function_def_t *function;
};

static inline bool is_ast_node(ast_node_t *node, ast_node_type_t type)
{
    return node && node->type == type;
}

#endif /* AST_H */