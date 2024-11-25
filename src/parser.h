#ifndef FAF9272A_6BB8_4D5B_AE9C_2B710AB564BC
#define FAF9272A_6BB8_4D5B_AE9C_2B710AB564BC

#include "lexer.h"
#include "ast.h"
#include "allocator.h"
#include <stdio.h>

typedef parser_ parser_t;

struct parser_
{
    token_t *tokens;
    size_t current_index;
    size_t token_count;
};

parser_t *init_parser(token_t *tokens, size_t token_count);
program_t *parse_program(parser_t *parser);
function_def_t *parse_function(parser_t *parser);
inline token_t *current_token(parser_t *parser);
inline token_t *next_token(parser_t *parser);
bool match_token(parser_t *parser, token_type_t expected);
void error(const char *message, size_t line, size_t column);

void error(const char *message, size_t line, size_t column)
{
    fprintf(stderr, "Line %zu, Column: %zu -> Error: %s\n", line, column, message);
}

inline token_t *current_token(parser_t *parser)
{
    return &(parser->tokens[parser->current_index]);
}

inline token_t *next_token(parser_t *parser)
{
    return &(parser->tokens[++(parser->current_index)]);
}

bool match_token(parser_t *parser, token_type_t expected)
{
    token_t *next_token_ptr = next_token(parser);
    if (next_token_ptr->type == expected)
    {
        return true;
    }

    return false;
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
}

function_def_t *parse_function(parser_t *parser)
{
    if (!parser)
    {
        return NULL;
    }

    function_def_t *function = (function_def_t *)allocate(sizeof(function_def_t));
    token_t *current_token_ptr = current_token(parser);

    function->base.location.line = current_token_ptr->line;
    function->base.location.column = current_token_ptr->column;
    function->base.type = NODE_FUNCTION_DEF;

    if (!match_token(parser, TOKEN_KEYWORD_INT))
    {
        error("Expected 'int' for function definition", current_token(parser)->line, current_token(parser)->column);
        return NULL;
    }

    token_t *current_token_ptr = current_token(parser);

    identifier_t *name = parse_identifier(parser);
    if (!name)
    {
        return NULL;
    }
    name->base.parent = &(function->base);
}

program_t *parse_program(parser_t *parser)
{
    if (!parser)
    {
        return NULL;
    }

    token_t *current_token_ptr = current_token(parser);

    program_t *program = (program_t *)allocate(sizeof(program_t));
    program->base.type = NODE_PROGRAM;
    program->base.parent = NULL;
    program->base.location.column = current_token_ptr->column;
    program->base.location.line = current_token_ptr->line;

    program->function = parse_function(parser);
    if (!program->function)
    {
        return NULL;
    }

    program->function->base.parent = &(program->base);
    return program;
}

#endif /* FAF9272A_6BB8_4D5B_AE9C_2B710AB564BC */
