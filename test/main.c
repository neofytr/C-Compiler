/* #include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/assembly_gen/assembly_generation.h"
#include "../src/code_emitter/code_emitter.h"

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
        printf("Visiting Immediate Operand: %d\n", operand->operand.immediate->value);
        break;
    case OPERAND_REGISTER:
        printf("Visiting Register Operand: r%d\n", operand->operand.reg->reg_num);
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

    asm_program_t *asm_program;

    if (ast)
    {
        printf("AST Traversal:\n");
        visit_program(ast);

        asm_program = generate_assembly_ast(ast);
        if (asm_program)
        {
            printf("Assembly AST Traversal:\n");
            visit_asm_program(asm_program);
        }
        else
        {
            printf("Assembly generation failed.\n");
            return EXIT_FAILURE;
        }
    }
    else
    {
        printf("Parsing failed.\n");
        return EXIT_FAILURE;
    }

    if (!emit_asm(asm_program, "output.asm"))
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
 */

#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/ir_gen/ir_generation.h"

// IR AST Visitors
void visit_ir_program(ir_program_t *ir_program);
void visit_ir_function(ir_function_t *ir_function);
void visit_ir_instruction(ir_instruction_t *ir_instruction);
void visit_ir_value(ir_value_t *ir_value);
void visit_ir_identifier(ir_identifier_t *ir_identifier);

void visit_ir_program(ir_program_t *ir_program)
{
    printf("Visiting IR Program\n");
    if (ir_program && ir_program->function)
    {
        visit_ir_function(ir_program->function);
    }
}

void visit_ir_function(ir_function_t *ir_function)
{
    if (!ir_function)
        return;

    printf("Visiting IR Function: %s\n", ir_function->name->name);
    visit_ir_identifier(ir_function->name);

    for (size_t i = 0; i < ir_function->instruction_count; i++)
    {
        visit_ir_instruction(ir_function->body[i]);
    }
}

void visit_ir_instruction(ir_instruction_t *ir_instruction)
{
    if (!ir_instruction)
        return;

    switch (ir_instruction->type)
    {
    case IR_INSTR_RETURN:
        printf("Visiting IR Return Instruction\n");
        if (ir_instruction->instruction.return_instr.value)
        {
            visit_ir_value(ir_instruction->instruction.return_instr.value);
        }
        break;
    case IR_INSTR_UNARY:
        printf("Visiting IR Unary Instruction\n");
        if (ir_instruction->instruction.unary_instr.source)
        {
            visit_ir_value(ir_instruction->instruction.unary_instr.source);
        }
        if (ir_instruction->instruction.unary_instr.destination)
        {
            visit_ir_value(ir_instruction->instruction.unary_instr.destination);
        }
        break;
    default:
        printf("Unknown IR Instruction Type\n");
    }
}

void visit_ir_value(ir_value_t *ir_value)
{
    if (!ir_value)
        return;

    switch (ir_value->type)
    {
    case IR_VAL_CONSTANT_INT:
        printf("Visiting IR Constant Int: %d\n", ir_value->value.constant_int);
        break;
    case IR_VAL_VARIABLE:
        printf("Visiting IR Variable\n");
        if (ir_value->value.variable.identifier)
        {
            visit_ir_identifier(ir_value->value.variable.identifier);
        }
        break;
    default:
        printf("Unknown IR Value Type\n");
    }
}

void visit_ir_identifier(ir_identifier_t *ir_identifier)
{
    if (!ir_identifier)
        return;
    printf("Visiting IR Identifier: %s\n", ir_identifier->name);
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

    free(source);

    parser_t *parser = init_parser(token_list.tokens, token_list.count);
    program_t *ast = parse_program(parser);

    if (!ast)
    {
        printf("Parsing failed.\n");
        return EXIT_FAILURE;
    }

    ir_program_t *ir_program = ir_handle_program(ast);

    if (!ir_program)
    {
        printf("IR generation failed.\n");
        return EXIT_FAILURE;
    }

    printf("IR AST Traversal:\n");
    visit_ir_program(ir_program);

    return EXIT_SUCCESS;
}