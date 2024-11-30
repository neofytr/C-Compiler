#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/assembly_gen/assembly_generation.h"
#include "../src/code_emitter/code_emitter.h"
#include "../src/assembly_gen/from_IR/first_pass.h"

// AST Visitors
void visit_program(program_t *program);
void visit_function_def(function_def_t *function);
void visit_statement(statement_t *statement);
void visit_expression(expression_t *expression);
void visit_identifier(identifier_t *identifier);
void visit_binary_operator(binary_operator_t *binary_op);
void visit_unary_operator(unary_operator_t *unary_op);

// Assembly AST Visitors
void visit_asm_program(asm_program_t *asm_program);
void visit_asm_function(asm_function_t *asm_function);
void visit_asm_instruction(asm_instruction_t *asm_instruction);
void visit_asm_operand(asm_operand_t *operand);
void visit_asm_identifier(asm_identifier_t *identifier);

void visit_program(program_t *program)
{
    if (!program)
    {
        printf("Visiting NULL Program\n");
        return;
    }
    printf("Visiting Program\n");
    if (program->function)
    {
        visit_function_def(program->function);
    }
}

void visit_function_def(function_def_t *function)
{
    if (!function)
    {
        printf("Visiting NULL Function\n");
        return;
    }
    printf("Visiting Function\n");

    if (function->name)
    {
        visit_identifier(function->name);
    }

    if (function->body)
    {
        visit_statement(function->body);
    }
}

void visit_statement(statement_t *statement)
{
    if (!statement)
    {
        printf("Visiting NULL Statement\n");
        return;
    }

    switch (statement->stmt_type)
    {
    case STMT_RETURN:
        printf("Visiting Return Statement\n");
        if (statement->value.return_expr)
        {
            visit_expression(statement->value.return_expr);
        }
        break;
    default:
        printf("Unknown Statement Type: %d\n", statement->stmt_type);
    }
}

void visit_expression(expression_t *expression)
{
    if (!expression)
    {
        printf("Visiting NULL Expression\n");
        return;
    }

    switch (expression->expr_type)
    {
    case EXPR_CONSTANT_INT:
        printf("Visiting Constant Expression: %d\n", expression->value.constant_int);
        break;
    case EXPR_BINARY:
        printf("Visiting Binary Expression\n");
        if (expression->value.binary.op)
        {
            visit_binary_operator(expression->value.binary.op);
        }
        if (expression->value.binary.left_expr)
        {
            visit_expression(expression->value.binary.left_expr);
        }
        if (expression->value.binary.right_expr)
        {
            visit_expression(expression->value.binary.right_expr);
        }
        break;
    case EXPR_NESTED:
        printf("Visiting Nested Expression\n");
        if (expression->value.nested_expr)
        {
            visit_expression(expression->value.nested_expr);
        }
        break;
    case EXPR_UNARY:
        printf("Visiting Unary Expression\n");
        if (expression->value.unary.unary_operator)
        {
            visit_unary_operator(expression->value.unary.unary_operator);
        }
        if (expression->value.unary.expression)
        {
            visit_expression(expression->value.unary.expression);
        }
        break;
    default:
        printf("Unknown Expression Type: %d\n", expression->expr_type);
    }
}

void visit_identifier(identifier_t *identifier)
{
    if (!identifier)
    {
        printf("Visiting NULL Identifier\n");
        return;
    }
    printf("Visiting Identifier: %s\n", identifier->name);
}

void visit_binary_operator(binary_operator_t *binary_op)
{
    if (!binary_op)
    {
        printf("Visiting NULL Binary Operator\n");
        return;
    }

    switch (binary_op->binary_operator)
    {
    case BINARY_ADD:
        printf("Visiting Binary Addition Operator\n");
        break;
    case BINARY_SUB:
        printf("Visiting Binary Subtraction Operator\n");
        break;
    case BINARY_MUL:
        printf("Visiting Binary Multiplication Operator\n");
        break;
    case BINARY_DIV:
        printf("Visiting Binary Division Operator\n");
        break;
    case BINARY_REM:
        printf("Visiting Binary Remainder Operator\n");
        break;
    default:
        printf("Unknown Binary Operator Type: %d\n", binary_op->binary_operator);
    }
}

void visit_unary_operator(unary_operator_t *unary_op)
{
    if (!unary_op)
    {
        printf("Visiting NULL Unary Operator\n");
        return;
    }

    switch (unary_op->op)
    {
    case NEGATE:
        printf("Visiting Unary Negation Operator\n");
        break;
    case BITWISE_COMPLEMENT:
        printf("Visiting Bitwise Complement Operator\n");
        break;
    default:
        printf("Unknown Unary Operator Type: %d\n", unary_op->op);
    }
}

// Assembly AST Visitors
void visit_asm_program(asm_program_t *asm_program)
{
    if (!asm_program)
    {
        printf("Visiting NULL Assembly Program\n");
        return;
    }
    printf("Visiting Assembly Program\n");
    if (asm_program->function)
    {
        visit_asm_function(asm_program->function);
    }
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
        visit_asm_operand(asm_instruction->instr.mov.src);
        visit_asm_operand(asm_instruction->instr.mov.dst);
        break;
    case INSTRUCTION_RET:
        printf("Visiting RET Instruction\n");
        break;
    case INSTRUCTION_UNARY:
        printf("Visiting Unary Instruction\n");
        printf("Unary operand: %s\n", asm_instruction->instr.unary.operand->operand.pseudo.pseudo_name);
        printf("Unary operator: %s\n", asm_instruction->instr.unary.unary_operator->op == UNARY_NEG ? "UNARY_NEG" : "UNARY_BITWISE_COMPLEMENT");
    default:
        printf("Unknown Instruction Type\n");
    }
}

void visit_asm_operand(asm_operand_t *operand)
{
    switch (operand->type)
    {
    case OPERAND_IMMEDIATE:
        printf("Visiting Immediate Operand: %d\n", operand->operand.immediate.value);
        break;
    case OPERAND_REGISTER:
        printf("Visiting Register Operand: r%d\n", operand->operand.reg.reg_no);
        break;
    case OPERAND_PSEUDO:
        printf("Visiting Pseudo Operand: %s\n", operand->operand.pseudo.pseudo_name);
    default:
        printf("Unknown Operand Type\n");
    }
}

void visit_asm_identifier(asm_identifier_t *identifier)
{
    printf("Visiting Assembly Identifier: %s\n", identifier->name);
}

/* int main(int argc, char **argv)
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

void visit_ir_unary_operator(ir_unary_operator_t *unary_operator)
{
    printf("Visiting IR Unary Operator\n");
    switch (unary_operator->unary_op)
    {
    case IR_UNARY_BITWISE_COMPLEMENT:
        printf("BITWISE_COMPLEMENT\n");
        break;
    case IR_UNARY_NEGATE:
        printf("NEGATE\n");
        break;
    default:
        break;
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
        if (ir_instruction->instruction.unary_instr.unary_operator)
        {
            visit_ir_unary_operator(ir_instruction->instruction.unary_instr.unary_operator);
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
    }

    ir_program_t *ir_program = ir_handle_program(ast);

    if (!ir_program)
    {
        printf("IR generation failed.\n");
        return EXIT_FAILURE;
    }

    printf("IR AST Traversal:\n");
    visit_ir_program(ir_program);

    asm_program_t *asm_program = handle_ir_program(ir_program);
    if (!asm_program)
    {
        printf("ASM generation failed.\n");
        return EXIT_FAILURE;
    }

    printf("ASM AST Traversal\n");
    visit_asm_program(asm_program);

    return EXIT_SUCCESS;
}