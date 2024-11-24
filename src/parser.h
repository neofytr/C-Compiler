#ifndef FAF9272A_6BB8_4D5B_AE9C_2B710AB564BC
#define FAF9272A_6BB8_4D5B_AE9C_2B710AB564BC

#include "lexer.h"
#include "ast.h"
#include "allocator.h"

typedef parser_ parser_t;

struct parser_
{
    token_t *tokens;
    size_t current_index;
    size_t token_count;
};

parser_t *init_parser(token_t *tokens);
program_t *parse(parser_t *parser);
token_t *current_token(parser_t *parser);
token_t *next_token(parser_t *parser);
bool match_token(parser_t *parser, token_type_t expected);
void error(const char *message, size_t line, size_t column);

#endif /* FAF9272A_6BB8_4D5B_AE9C_2B710AB564BC */
