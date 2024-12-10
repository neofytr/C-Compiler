#include "../src/lexer/lexer.h"
#include "../src/parser/parser.h"
#include "../src/assembly_gen/assembly_generation.h"
#include "../src/code_emitter/code_emitter.h"
#include "../src/assembly_gen/from_IR/first_pass.h"
#include "../src/assembly_gen/from_IR/second_pass.h"
#include "../src/assembly_gen/from_IR/third_pass.h"

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
    if (!asm_function)
    {
        printf("Visiting NULL Assembly Function\n");
        return;
    }
    printf("Visiting Assembly Function: %s\n", asm_function->name->name);
    for (size_t i = 0; i < asm_function->instruction_count; i++)
    {
        printf("Instruction %zu:\n", i + 1);
        visit_asm_instruction(asm_function->instructions[i]);
    }
}

void visit_asm_instruction(asm_instruction_t *asm_instruction)
{
    if (!asm_instruction)
    {
        printf("Visiting NULL Instruction\n");
        return;
    }

    printf("Visiting Instruction: ");
    switch (asm_instruction->type)
    {
    case INSTRUCTION_MOV:
        printf("MOV\n");
        visit_asm_operand(asm_instruction->instr.mov.dst);
        visit_asm_operand(asm_instruction->instr.mov.src);
        break;

    case INSTRUCTION_RET:
        printf("RET\n");
        break;

    case INSTRUCTION_UNARY:
        printf("Unary\n");
        visit_asm_operand(asm_instruction->instr.unary.operand);
        printf("Unary Operator: %s\n",
               asm_instruction->instr.unary.unary_operator->op == UNARY_NEG ? "NEG" : "NOT");
        break;

    case INSTRUCTION_ALLOCATE_STACK:
        printf("Allocate Stack\n");
        printf("Stack Size: %d\n", asm_instruction->instr.alloc_stack.alloc_size);
        break;

    case INSTRUCTION_CQO:
        printf("CQO\n");
        break;

    case INSTRUCTION_IDIV:
        printf("IDIV\n");
        visit_asm_operand(asm_instruction->instr.idiv.operand);
        break;

    case INSTRUCTION_BINARY:
        printf("Binary\n");
        visit_asm_operand(asm_instruction->instr.binary.first_operand);
        visit_asm_operand(asm_instruction->instr.binary.second_operand);
        printf("Binary Operator: %s\n",
               asm_instruction->instr.binary.binary_operator->binary_op == ASM_BINARY_ADD ? "ADD" : asm_instruction->instr.binary.binary_operator->binary_op == ASM_BINARY_SUB ? "SUB"
                                                                                                                                                                               : "MULT");
        break;

    default:
        printf("Unknown Instruction Type\n");
        break;
    }
}

void visit_asm_operand(asm_operand_t *operand)
{
    if (!operand)
    {
        printf("Visiting NULL Operand\n");
        return;
    }

    printf("Visiting Operand: ");
    switch (operand->type)
    {
    case OPERAND_IMMEDIATE:
        printf("Immediate Value: %d\n", operand->operand.immediate.value);
        break;

    case OPERAND_REGISTER:
        switch (operand->operand.reg.reg_no)
        {
        case ASM_REG_R10:
        {
            printf("Register: R10\n");
            break;
        }
        case ASM_REG_R11:
        {
            printf("Register: R11\n");
            break;
        }
        case ASM_REG_RAX:
        {
            printf("Register: RAX\n");
            break;
        }
        case ASM_REG_RDX:
        {
            printf("Register: RDX\n");
            break;
        }
        }
        break;

    case OPERAND_PSEUDO:
        printf("Pseudo Register: %s\n", operand->operand.pseudo.pseudo_name);
        break;

    case OPERAND_STACK:
        printf("Stack Offset: %d\n", operand->operand.stack.offset);
        break;

    default:
        printf("Unknown Operand Type\n");
        break;
    }
}

void visit_asm_identifier(asm_identifier_t *identifier)
{
    if (!identifier)
    {
        printf("Visiting NULL Identifier\n");
        return;
    }

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
void visit_ir_binary_operator(ir_binary_operator_t *binary_operator);
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
    case IR_INSTR_BINARY:
        printf("Visiting IR Binary Instruction\n");
        if (ir_instruction->instruction.binary_instr.left)
        {
            printf("Left Operand:\n");
            visit_ir_value(ir_instruction->instruction.binary_instr.left);
        }
        if (ir_instruction->instruction.binary_instr.binary_operator)
        {
            printf("Binary Operator:\n");
            visit_ir_binary_operator(ir_instruction->instruction.binary_instr.binary_operator);
        }
        if (ir_instruction->instruction.binary_instr.right)
        {
            printf("Right Operand:\n");
            visit_ir_value(ir_instruction->instruction.binary_instr.right);
        }
        if (ir_instruction->instruction.binary_instr.destination)
        {
            printf("Destination:\n");
            visit_ir_value(ir_instruction->instruction.binary_instr.destination);
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

void visit_ir_binary_operator(ir_binary_operator_t *binary_operator)
{
    printf("Visiting IR Binary Operator\n");
    switch (binary_operator->operator)
    {
    case IR_BINARY_ADD:
        printf("ADD\n");
        break;
    case IR_BINARY_SUBTRACT:
        printf("SUBTRACT\n");
        break;
    case IR_BINARY_MULTIPLY:
        printf("MULTIPLY\n");
        break;
    case IR_BINARY_DIVIDE:
        printf("DIVIDE\n");
        break;
    case IR_BINARY_REM:
        printf("REMAINDER\n");
        break;
    default:
        printf("Unknown Binary Operator\n");
        break;
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

    asm_program_t *asm_program = asm_first_pass(ir_program);
    if (!asm_program)
    {
        printf("ASM generation failed.\n");
        return EXIT_FAILURE;
    }

    int final_offset;

    if (!asm_second_pass(asm_program, &final_offset))
    {
        printf("ASM generation failed.\n");
        return EXIT_FAILURE;
    }

    if (!asm_third_pass(asm_program, final_offset))
    {
        printf("ASM generation failed.\n");
        return EXIT_FAILURE;
    }

    printf("ASM AST Traversal\n");
    visit_asm_program(asm_program);

    return EXIT_SUCCESS;
}