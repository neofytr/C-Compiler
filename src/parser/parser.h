#ifndef FAF9272A_6BB8_4D5B_AE9C_2B710AB564BC
#define FAF9272A_6BB8_4D5B_AE9C_2B710AB564BC

#include "./lexer/lexer.h"
#include "./ast/source/ast.h"
#include "./allocator/allocator.h"
#include <stdio.h>
#include <stdbool.h>

typedef struct parser_
{
    token_t *tokens;
    size_t current_index;
    size_t token_count;
} parser_t;

parser_t *init_parser(token_t *tokens, size_t token_count);
program_t *parse_program(parser_t *parser);
function_def_t *parse_function(parser_t *parser);
statement_t *parse_statement(parser_t *parser);
expression_t *parse_expression(parser_t *parser);
identifier_t *parse_identifier(parser_t *parser);

static inline token_t *peek_token(parser_t *parser);
static inline token_t *advance_token(parser_t *parser);
static bool check_token(parser_t *parser, token_type_t expected);
static bool expect_token(parser_t *parser, token_type_t expected, const char *error_msg);
static void error(const char *message, size_t line, size_t column);

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

expression_t *parse_expression(parser_t *parser)
{
    token_t *int_token = peek_token(parser);
    if (!int_token || !expect_token(parser, TOKEN_CONSTANT, "Expected a constant integer expression"))
    {
        return NULL;
    }

    expression_t *expression = (expression_t *)allocate(sizeof(expression_t));
    if (!expression)
    {
        return NULL;
    }

    expression->base.type = NODE_EXPRESSION;
    expression->base.location.line = int_token->line;
    expression->base.location.column = int_token->column;
    expression->base.parent = NULL;
    expression->expr_type = EXPR_CONSTANT_INT;
    expression->value.constant_int = atoi(int_token->value);

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

    expression_t *expression = parse_expression(parser);
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