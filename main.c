#include "./src/lexer.h"
#include "./src/parser.h"
#include "./src/assembly_generation.h"

// AST Visitors
void visit_program(program_t *program);
void visit_function_def(function_def_t *function);
void visit_statement(statement_t *statement);
void visit_expression(expression_t *expression);
void visit_identifier(identifier_t *identifier);

// Assembly AST Visitors
void visit_asm_program(asm_program_t *asm_program);
void visit_asm_function(asm_function_t *asm_function);
void visit_asm_instruction(asm_instruction_t *asm_instruction);
void visit_asm_operand(asm_operand_t *operand);
void visit_asm_identifier(asm_identifier_t *identifier);

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

// Assembly AST Visitors
void visit_asm_program(asm_program_t *asm_program)
{
    printf("Visiting Assembly Program\n");
    visit_asm_function(asm_program->function);
}

void visit_asm_function(asm_function_t *asm_function)
{
    printf("Visiting Assembly Function: %s\n", asm_function->name->name);
    for (size_t i = 0; i < asm_function->instruction_count; i++)
    {
        visit_asm_instruction(asm_function->instructions[i]);
    }
}

void visit_asm_instruction(asm_instruction_t *asm_instruction)
{
    switch (asm_instruction->type)
    {
    case INSTRUCTION_MOV:
        printf("Visiting MOV Instruction\n");
        visit_asm_operand(asm_instruction->instr.mov->src);
        visit_asm_operand(asm_instruction->instr.mov->dst);
        break;
    case INSTRUCTION_RET:
        printf("Visiting RET Instruction\n");
        break;
    default:
        printf("Unknown Instruction Type\n");
    }
}

void visit_asm_operand(asm_operand_t *operand)
{
    switch (operand->type)
    {
    case OPERAND_IMMEDIATE:
        printf("Visiting Immediate Operand: %d\n", operand->immediate->value);
        break;
    case OPERAND_REGISTER:
        printf("Visiting Register Operand: r%d\n", operand->reg->reg_num);
        break;
    default:
        printf("Unknown Operand Type\n");
    }
}

void visit_asm_identifier(asm_identifier_t *identifier)
{
    printf("Visiting Assembly Identifier: %s\n", identifier->name);
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
        printf("AST Traversal:\n");
        visit_program(ast);

        asm_program_t *asm_program = generate_assembly_ast(ast);
        if (asm_program)
        {
            printf("Assembly AST Traversal:\n");
            visit_asm_program(asm_program);
        }
        else
        {
            printf("Assembly generation failed.\n");
        }
    }
    else
    {
        printf("Parsing failed.\n");
    }

    return EXIT_SUCCESS;
}
