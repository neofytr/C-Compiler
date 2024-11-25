#include "./src/lexer.h"
#include "./src/parser.h"

void visit_program(program_t *program);
void visit_function_def(function_def_t *function);
void visit_statement(statement_t *statement);
void visit_expression(expression_t *expression);
void visit_identifier(identifier_t *identifier);

void visit_program(program_t *program)
{
    printf("Visiting Program\n");
    visit_function_def(program->function);
}

void visit_function_def(function_def_t *function)
{
    printf("Visiting Function: %s\n", function->name->name);
    visit_identifier(function->name);
    visit_statement(function->body);
}

void visit_statement(statement_t *statement)
{
    switch (statement->stmt_type)
    {
    case STMT_RETURN:
        printf("Visiting Return Statement\n");
        visit_expression(statement->value.return_expr);
        break;
    default:
        printf("Unknown Statement Type\n");
    }
}

void visit_expression(expression_t *expression)
{
    switch (expression->expr_type)
    {
    case EXPR_CONSTANT_INT:
        printf("Visiting Constant Expression: %d\n", expression->value.constant_int);
        break;
    default:
        printf("Unknown Expression Type\n");
    }
}

void visit_identifier(identifier_t *identifier)
{
    printf("Visiting Identifier: %s\n", identifier->name);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    size_t file_size;
    uint8_t *source = read_source_file(argv[1], &file_size);
    if (!source)
    {
        return EXIT_FAILURE;
    }

    token_list_t token_list = {0};
    if (!lex_source(source, &token_list))
    {
        fprintf(stderr, "Lexing failed\n");
        free(source);
        return EXIT_FAILURE;
    }

    printf("Tokens:\n");
    for (size_t i = 0; i < token_list.count; i++)
    {
        printf("Line %zu, Col %zu: %-15s '%s'\n",
               token_list.tokens[i].line,
               token_list.tokens[i].column,
               token_type_to_string(token_list.tokens[i].type),
               token_list.tokens[i].value);
    }

    free(source);

    parser_t *parser = init_parser(token_list.tokens, token_list.count);
    program_t *ast = parse_program(parser);

    if (ast)
    {
        visit_program(ast);
    }
    else
    {
        printf("Parsing failed.\n");
    }
    return EXIT_SUCCESS;
}