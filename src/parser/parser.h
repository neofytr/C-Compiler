#ifndef FAF9272A_6BB8_4D5B_AE9C_2B710AB564BC
#define FAF9272A_6BB8_4D5B_AE9C_2B710AB564BC

#include "../lexer/lexer.h"
#include "../ast/source/ast.h"
#include "../allocator/allocator.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct parser_
{
    token_t *tokens;
    size_t current_index;
    size_t token_count;
} parser_t;

typedef enum
{
    PRECEDENCE_MUL = 130,
    PRECEDENCE_DIV = 130,
    PRECEDENCE_REM = 130,

    PRECEDENCE_ADD = 120,
    PRECEDENCE_SUB = 120,

    PRECEDENCE_SHIFT = 110,

    PRECEDENCE_LESS = 100,
    PRECEDENCE_LESS_EQUAL = 100,
    PRECEDENCE_GREATER = 100,
    PRECEDENCE_GREATER_EQUAL = 100,

    PRECEDENCE_EQUAL = 90,
    PRECEDENCE_NOT_EQUAL = 90,

    PRECEDENCE_BITWISE_AND = 80,

    PRECEDENCE_BITWISE_XOR = 70,

    PRECEDENCE_BITWISE_OR = 60,

    PRECEDENCE_LOGICAL_AND = 50,

    PRECEDENCE_LOGICAL_OR = 40,

    PRECEDENCE_ASSIGN = 30,

    MIN_PRECEDENCE = PRECEDENCE_ASSIGN,
} operator_precedence_t;

parser_t *init_parser(token_t *tokens, size_t token_count);
program_t *parse_program(parser_t *parser);
function_def_t *parse_function(parser_t *parser);
statement_t *parse_statement(parser_t *parser);
expression_t *parse_expression(parser_t *parser, operator_precedence_t min_precedence);
expression_t *parse_factor(parser_t *parser);
identifier_t *parse_identifier(parser_t *parser);

static inline token_t *peek_token(parser_t *parser);
static inline token_t *advance_token(parser_t *parser);
static bool check_token(parser_t *parser, token_type_t expected);
static bool expect_token(parser_t *parser, token_type_t expected, const char *error_msg);
static void error(const char *message, size_t line, size_t column);
static operator_precedence_t token_precedence(token_t *token);

static operator_precedence_t token_precedence(token_t *token)
{
    if (!token)
    {
        return -1;
    }

    switch (token->type)
    {
    case TOKEN_OPERATOR_MUL:
        return PRECEDENCE_MUL;
    case TOKEN_OPERATOR_DIV:
        return PRECEDENCE_DIV;
    case TOKEN_OPERATOR_REM:
        return PRECEDENCE_REM;

    case TOKEN_OPERATOR_PLUS:
        return PRECEDENCE_ADD;
    case TOKEN_OPERATOR_NEGATION:
        return PRECEDENCE_SUB;

    case TOKEN_OPERATOR_BITWISE_LEFT_SHIFT:
    case TOKEN_OPERATOR_BITWISE_RIGHT_SHIFT:
        return PRECEDENCE_SHIFT;

    case TOKEN_OPERATOR_LESS:
        return PRECEDENCE_LESS;
    case TOKEN_OPERATOR_LESS_EQUAL:
        return PRECEDENCE_LESS_EQUAL;
    case TOKEN_OPERATOR_GREATER:
        return PRECEDENCE_GREATER;
    case TOKEN_OPERATOR_GREATER_EQUAL:
        return PRECEDENCE_GREATER_EQUAL;

    case TOKEN_OPERATOR_EQUAL:
        return PRECEDENCE_EQUAL;
    case TOKEN_OPERATOR_NOT_EQUAL:
        return PRECEDENCE_NOT_EQUAL;

    case TOKEN_OPERATOR_BITWISE_AND:
        return PRECEDENCE_BITWISE_AND;
    case TOKEN_OPERATOR_BITWISE_XOR:
        return PRECEDENCE_BITWISE_XOR;
    case TOKEN_OPERATOR_BITWISE_OR:
        return PRECEDENCE_BITWISE_OR;

    case TOKEN_OPERATOR_LOGICAL_AND:
        return PRECEDENCE_LOGICAL_AND;
    case TOKEN_OPERATOR_LOGICAL_OR:
        return PRECEDENCE_LOGICAL_OR;

    case TOKEN_OPERATOR_ASSIGN:
        return PRECEDENCE_ASSIGN;

    default:
        return -1;
    }
}

static void error(const char *message, size_t line, size_t column)
{
    fprintf(stderr, "Line %zu, Column: %zu -> Error: %s\n", line, column, message);
}

static inline token_t *peek_token(parser_t *parser)
{
    if (parser->current_index >= parser->token_count)
    {
        return NULL;
    }
    return &(parser->tokens[parser->current_index]);
}

static inline token_t *advance_token(parser_t *parser)
{
    token_t *current = peek_token(parser);
    if (current)
    {
        parser->current_index++;
    }
    return current;
}

static bool check_token(parser_t *parser, token_type_t expected)
{
    token_t *current = peek_token(parser);
    return current && current->type == expected;
}

static bool expect_token(parser_t *parser, token_type_t expected, const char *error_msg)
{
    if (!check_token(parser, expected))
    {
        token_t *current = peek_token(parser);
        if (current)
        {
            error(error_msg, current->line, current->column);
        }
        return false;
    }
    advance_token(parser);
    return true;
}

parser_t *init_parser(token_t *tokens, size_t token_count)
{
    if (!tokens)
    {
        return NULL;
    }

    parser_t *parser = (parser_t *)allocate(sizeof(parser_t));
    if (!parser)
    {
        return NULL;
    }

    parser->tokens = tokens;
    parser->current_index = 0;
    parser->token_count = token_count;

    return parser;
}

identifier_t *parse_identifier(parser_t *parser)
{
    token_t *token = peek_token(parser);
    if (!token || token->type != TOKEN_IDENTIFIER)
    {
        if (token)
        {
            error("Expected identifier", token->line, token->column);
        }
        return NULL;
    }

    identifier_t *identifier = (identifier_t *)allocate(sizeof(identifier_t));
    if (!identifier)
    {
        return NULL;
    }

    identifier->base.type = NODE_IDENTIFIER;
    identifier->base.location.line = token->line;
    identifier->base.location.column = token->column;
    identifier->base.parent = NULL;
    identifier->name = token->value;

    advance_token(parser);
    return identifier;
}

binary_operator_t *parse_binary_operator(parser_t *parser)
{
    if (!parser)
    {
        return NULL;
    }

    binary_operator_t *binary_operator = (binary_operator_t *)allocate(sizeof(binary_operator_t));
    if (!binary_operator)
    {
        return NULL;
    }

    binary_operator->base.type = NODE_BINARY_OP;

    token_t *curr_tok = peek_token(parser);
    if (!curr_tok)
    {
        free(binary_operator);
        return NULL;
    }

    switch (curr_tok->type)
    {
    case TOKEN_OPERATOR_PLUS:
        binary_operator->binary_operator = BINARY_ADD;
        break;
    case TOKEN_OPERATOR_NEGATION:
        binary_operator->binary_operator = BINARY_SUB;
        break;
    case TOKEN_OPERATOR_MUL:
        binary_operator->binary_operator = BINARY_MUL;
        break;
    case TOKEN_OPERATOR_DIV:
        binary_operator->binary_operator = BINARY_DIV;
        break;
    case TOKEN_OPERATOR_REM:
        binary_operator->binary_operator = BINARY_REM;
        break;

    case TOKEN_OPERATOR_BITWISE_AND:
        binary_operator->binary_operator = BINARY_BITWISE_AND;
        break;
    case TOKEN_OPERATOR_BITWISE_OR:
        binary_operator->binary_operator = BINARY_BITWISE_OR;
        break;
    case TOKEN_OPERATOR_BITWISE_XOR:
        binary_operator->binary_operator = BINARY_BITWISE_XOR;
        break;
    case TOKEN_OPERATOR_BITWISE_LEFT_SHIFT:
        binary_operator->binary_operator = BINARY_LEFT_SHIFT;
        break;
    case TOKEN_OPERATOR_BITWISE_RIGHT_SHIFT:
        binary_operator->binary_operator = BINARY_RIGHT_SHIFT;
        break;

    case TOKEN_OPERATOR_LESS:
        binary_operator->binary_operator = BINARY_LESS_THAN;
        break;
    case TOKEN_OPERATOR_LESS_EQUAL:
        binary_operator->binary_operator = BINARY_LESS_THAN_EQUAL;
        break;
    case TOKEN_OPERATOR_GREATER:
        binary_operator->binary_operator = BINARY_GREATER_THAN;
        break;
    case TOKEN_OPERATOR_GREATER_EQUAL:
        binary_operator->binary_operator = BINARY_GREATER_THAN_EQUAL;
        break;

    case TOKEN_OPERATOR_EQUAL:
        binary_operator->binary_operator = BINARY_EQUAL;
        break;
    case TOKEN_OPERATOR_NOT_EQUAL:
        binary_operator->binary_operator = BINARY_NOT_EQUAL;
        break;

    case TOKEN_OPERATOR_LOGICAL_AND:
        binary_operator->binary_operator = BINARY_LOGICAL_AND;
        break;
    case TOKEN_OPERATOR_LOGICAL_OR:
        binary_operator->binary_operator = BINARY_LOGICAL_OR;
        break;

    default:
        free(binary_operator);
        return NULL;
    }

    advance_token(parser);
    return binary_operator;
}

expression_t *parse_expression(parser_t *parser, operator_precedence_t min_precedence)
{
    if (!parser)
    {
        return NULL;
    }

    token_t *curr_tok = peek_token(parser);
    if (!curr_tok)
    {
        return NULL;
    }

    expression_t *left = parse_factor(parser);
    if (!left)
    {
        return NULL;
    }

    left->base.location.column = curr_tok->column;
    left->base.location.line = curr_tok->line;

    while (true)
    {
        token_t *next_token = peek_token(parser);

        if (!next_token ||
            !(next_token->type == TOKEN_OPERATOR_PLUS ||
              next_token->type == TOKEN_OPERATOR_NEGATION ||
              next_token->type == TOKEN_OPERATOR_DIV ||
              next_token->type == TOKEN_OPERATOR_MUL ||
              next_token->type == TOKEN_OPERATOR_REM ||
              next_token->type == TOKEN_OPERATOR_BITWISE_AND ||
              next_token->type == TOKEN_OPERATOR_BITWISE_OR ||
              next_token->type == TOKEN_OPERATOR_BITWISE_XOR ||
              next_token->type == TOKEN_OPERATOR_BITWISE_LEFT_SHIFT ||
              next_token->type == TOKEN_OPERATOR_BITWISE_RIGHT_SHIFT ||
              next_token->type == TOKEN_OPERATOR_LOGICAL_AND ||
              next_token->type == TOKEN_OPERATOR_LOGICAL_OR ||
              next_token->type == TOKEN_OPERATOR_EQUAL ||
              next_token->type == TOKEN_OPERATOR_NOT_EQUAL ||
              next_token->type == TOKEN_OPERATOR_LESS ||
              next_token->type == TOKEN_OPERATOR_LESS_EQUAL ||
              next_token->type == TOKEN_OPERATOR_GREATER ||
              next_token->type == TOKEN_OPERATOR_GREATER_EQUAL))
        {
            break;
        }

        operator_precedence_t current_precedence = token_precedence(next_token);
        if (current_precedence < min_precedence)
        {
            break;
        }

        expression_t *binary_expr = (expression_t *)allocate(sizeof(expression_t));
        if (!binary_expr)
        {
            return NULL;
        }

        binary_expr->base.type = NODE_EXPRESSION;
        binary_expr->base.location.line = next_token->line;
        binary_expr->base.location.column = next_token->column;
        binary_expr->base.parent = NULL;
        binary_expr->expr_type = EXPR_BINARY;

        binary_operator_t *bin_op = parse_binary_operator(parser);
        if (!bin_op)
        {
            deallocate(binary_expr);
            return NULL;
        }
        bin_op->base.location.column = next_token->column;
        bin_op->base.location.line = next_token->line;

        expression_t *right = parse_expression(parser, current_precedence + 1);
        if (!right)
        {
            deallocate(binary_expr);
            deallocate(bin_op);
            return NULL;
        }

        binary_expr->value.binary.op = bin_op;
        binary_expr->value.binary.left_expr = left;
        binary_expr->value.binary.right_expr = right;

        bin_op->base.parent = &(binary_expr->base);
        right->base.parent = &(binary_expr->base);
        right->base.type = NODE_EXPRESSION;
        left->base.type = NODE_EXPRESSION;
        left->base.parent = &(binary_expr->base);

        left = binary_expr;
    }

    return left;
}

expression_t *parse_factor(parser_t *parser)
{
    if (!parser)
    {
        return NULL;
    }
    token_t *token = peek_token(parser);
    if (!token)
    {
        return NULL;
    }

    expression_t *expression = (expression_t *)allocate(sizeof(expression_t));
    if (!expression)
    {
        return NULL;
    }

    switch (token->type)
    {
    case TOKEN_CONSTANT:
    {
        expression->base.type = NODE_EXPRESSION;
        expression->base.location.line = token->line;
        expression->base.location.column = token->column;
        expression->base.parent = NULL;
        expression->expr_type = EXPR_CONSTANT_INT;
        expression->value.constant_int = atoi(token->value);

        advance_token(parser);
        break;
    }

    case TOKEN_OPENING_PAREN:
    {
        advance_token(parser);
        expression->base.type = NODE_EXPRESSION;
        expression->base.location.line = token->line;
        expression->base.location.column = token->column;
        expression->base.parent = NULL;
        expression->expr_type = EXPR_NESTED;

        static bool expecting_closing_paren = false;

        expression_t *nested_expression = parse_expression(parser, MIN_PRECEDENCE);
        if (!nested_expression)
        {
            token_t *current_token = peek_token(parser);
            if (!expecting_closing_paren)
            {
                fprintf(stderr, "Line %zu, Column: %zu -> Error: Expected an expression\n",
                        current_token->line, current_token->column);
            }
            else
            {
                fprintf(stderr, "Line %zu, Column: %zu -> Error: Expected closing parenthesis\n",
                        current_token->line, current_token->column);
            }
            return NULL;
        }

        nested_expression->base.parent = &(expression->base);
        expression->value.nested_expr = nested_expression;

        if (!expect_token(parser, TOKEN_CLOSING_PAREN, "Expected closing parenthesis"))
        {
            expecting_closing_paren = true;
            return NULL;
        }
        break;
    }

    case TOKEN_OPERATOR_NEGATION:
    case TOKEN_OPERATOR_NOT:
    case TOKEN_OPERATOR_BITWISE_COMPLEMENT:
    {
        token_type_t unary_op_type = token->type;
        advance_token(parser);

        expression->base.type = NODE_EXPRESSION;
        expression->base.location.line = token->line;
        expression->base.location.column = token->column;
        expression->base.parent = NULL;
        expression->expr_type = EXPR_UNARY;

        expression_t *nested_expression = parse_factor(parser);
        if (!nested_expression)
        {
            token_t *current_token = peek_token(parser);
            fprintf(stderr, "Line %zu, Column: %zu -> Error: Expected an expression\n",
                    current_token->line, current_token->column);
            return NULL;
        }

        nested_expression->base.parent = &(expression->base);
        expression->expr_type = EXPR_UNARY;
        expression->value.unary.expression = nested_expression;

        unary_operator_t *unary_operator = (unary_operator_t *)allocate(sizeof(unary_operator_t));
        if (!unary_operator)
        {
            return NULL;
        }

        unary_operator->base.location.column = token->column;
        unary_operator->base.location.line = token->line;
        unary_operator->base.type = NODE_UNARY_OP;
        switch (unary_op_type)
        {
        case TOKEN_OPERATOR_NEGATION:
            unary_operator->op = NEGATE;
            break;
        case TOKEN_OPERATOR_BITWISE_COMPLEMENT:
            unary_operator->op = BITWISE_COMPLEMENT;
            break;
        case TOKEN_OPERATOR_NOT:
            unary_operator->op = NOT;
            break;
        }

        expression->value.unary.unary_operator = unary_operator;
        break;
    }

    default:
        fprintf(stderr, "Line %zu, Column: %zu -> Unexpected token type\n",
                token->line, token->column);
        return NULL;
    }

    return expression;
}

statement_t *parse_statement(parser_t *parser)
{
    token_t *return_token = peek_token(parser);
    if (!return_token || !expect_token(parser, TOKEN_KEYWORD_RETURN, "Expected 'return' statement"))
    {
        return NULL;
    }

    statement_t *statement = (statement_t *)allocate(sizeof(statement_t));
    if (!statement)
    {
        return NULL;
    }

    statement->base.type = NODE_STATEMENT;
    statement->base.location.line = return_token->line;
    statement->base.location.column = return_token->column;
    statement->base.parent = NULL;
    statement->stmt_type = STMT_RETURN;

    expression_t *expression = parse_expression(parser, MIN_PRECEDENCE);
    if (!expression)
    {
        return NULL;
    }
    expression->base.parent = &(statement->base);
    statement->value.return_expr = expression;

    if (!expect_token(parser, TOKEN_SEMICOLON, "Expected ';' after return expression"))
    {
        return NULL;
    }

    return statement;
}

function_def_t *parse_function(parser_t *parser)
{
    token_t *int_token = peek_token(parser);
    if (!int_token || !expect_token(parser, TOKEN_KEYWORD_INT, "Expected 'int' for function definition"))
    {
        return NULL;
    }

    function_def_t *function = (function_def_t *)allocate(sizeof(function_def_t));
    if (!function)
    {
        return NULL;
    }

    function->base.type = NODE_FUNCTION_DEF;
    function->base.location.line = int_token->line;
    function->base.location.column = int_token->column;
    function->base.parent = NULL;

    identifier_t *name = parse_identifier(parser);
    if (!name)
    {
        return NULL;
    }
    name->base.parent = &(function->base);
    function->name = name;

    if (!expect_token(parser, TOKEN_OPENING_PAREN, "Expected '(' after function name") ||
        !expect_token(parser, TOKEN_KEYWORD_VOID, "Expected 'void' in function parameters") ||
        !expect_token(parser, TOKEN_CLOSING_PAREN, "Expected ')' after 'void'") ||
        !expect_token(parser, TOKEN_OPENING_BRACE, "Expected '{' to start function body"))
    {
        return NULL;
    }

    statement_t *body = parse_statement(parser);
    if (!body)
    {
        return NULL;
    }
    body->base.parent = &(function->base);
    function->body = body;

    if (!expect_token(parser, TOKEN_CLOSING_BRACE, "Expected '}' to close function body"))
    {
        return NULL;
    }

    return function;
}

program_t *parse_program(parser_t *parser)
{
    if (!parser)
    {
        return NULL;
    }

    program_t *program = (program_t *)allocate(sizeof(program_t));
    if (!program)
    {
        return NULL;
    }

    token_t *first_token = peek_token(parser);
    if (!first_token)
    {
        return NULL;
    }

    program->base.type = NODE_PROGRAM;
    program->base.location.line = first_token->line;
    program->base.location.column = first_token->column;
    program->base.parent = NULL;

    function_def_t *function = parse_function(parser);
    if (!function)
    {
        return NULL;
    }

    function->base.parent = &(program->base);
    program->function = function;

    return program;
}

#endif /* FAF9272A_6BB8_4D5B_AE9C_2B710AB564BC */