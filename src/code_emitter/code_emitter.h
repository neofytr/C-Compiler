#ifndef ASSEMBLY_EMITTER_H
#define ASSEMBLY_EMITTER_H

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include "../assembly_gen/assembly_generation.h"

const char *map_register_name(int reg)
{
    switch (reg)
    {
    case ASM_REG_R10:
        return "r10";
    case ASM_REG_RAX:
        return "rax";
    default:
        fprintf(stderr, "Error: Unsupported register in map_register_name\n");
        return "";
    }

    return "";
}

FILE *create_output_file(const char *name)
{
    FILE *output_file = fopen(name, "w");
    if (!output_file)
    {
        fprintf(stderr, "Error creating output file: %s\n", name);
        perror("fopen");
        return NULL;
    }
    return output_file;
}

bool close_output_file(FILE *file)
{
    if (!file)
    {
        fprintf(stderr, "Error: Attempting to close NULL file pointer\n");
        return false;
    }

    if (fclose(file) != 0)
    {
        fprintf(stderr, "Error closing file\n");
        perror("fclose");
        return false;
    }
    return true;
}

bool emit_program_header(FILE *output_file)
{
    if (!output_file)
    {
        fprintf(stderr, "Error: NULL output file in emit_program_header\n");
        return false;
    }

    if (fprintf(output_file, "section .text\n") < 0)
    {
        fprintf(stderr, "Error writing program header\n");
        return false;
    }
    return true;
}

bool emit_function_prologue(asm_function_t *function, FILE *output_file)
{
    if (!function)
    {
        fprintf(stderr, "Error: NULL function in emit_function_prologue\n");
        return false;
    }

    if (!output_file)
    {
        fprintf(stderr, "Error: NULL output file in emit_function_prologue\n");
        return false;
    }

    if (!function->name)
    {
        fprintf(stderr, "Error: Function name is NULL\n");
        return false;
    }

    if (fprintf(output_file, "global %s\n%s:\n",
                function->name->name, function->name->name) < 0)
    {
        fprintf(stderr, "Error writing function name\n");
        return false;
    }

    if (fprintf(output_file, "    push rbp\n") < 0 ||
        fprintf(output_file, "    mov rbp, rsp\n") < 0)
    {
        fprintf(stderr, "Error writing function prologue instructions\n");
        return false;
    }
    return true;
}

bool emit_function_epilogue(FILE *output_file)
{
    if (!output_file)
    {
        fprintf(stderr, "Error: NULL output file in emit_function_epilogue\n");
        return false;
    }

    if (fprintf(output_file, "    mov rsp, rbp\n") < 0 ||
        fprintf(output_file, "    pop rbp\n") < 0 ||
        fprintf(output_file, "    ret\n") < 0)
    {
        fprintf(stderr, "Error writing function epilogue\n");
        return false;
    }
    return true;
}

bool emit_mov_instruction(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction || !output_file)
    {
        fprintf(stderr, "Error: NULL instruction or output file\n");
        return false;
    }

    asm_operand_t *dst = instruction->instr.mov.dst;
    asm_operand_t *src = instruction->instr.mov.src;

    if (!dst || !src)
    {
        fprintf(stderr, "Error: NULL operand in mov instruction\n");
        return false;
    }

    const char *dst_str = NULL;
    const char *src_str = NULL;

    char stack_dst_buf[32];

    switch (dst->type)
    {
    case OPERAND_REGISTER:
        dst_str = map_register_name(dst->operand.reg.reg_no);
        break;
    case OPERAND_PSEUDO:
        dst_str = dst->operand.pseudo.pseudo_name;
        break;
    case OPERAND_STACK:
        snprintf(stack_dst_buf, sizeof(stack_dst_buf), "[rbp-%d]",
                 -dst->operand.stack.offset);
        dst_str = stack_dst_buf;
        break;
    case OPERAND_IMMEDIATE:
        fprintf(stderr, "Error: Cannot use immediate as destination\n");
        return false;
    }

    char imm_buf[32];
    char stack_src_buf[32];

    switch (src->type)
    {
    case OPERAND_REGISTER:
        src_str = map_register_name(src->operand.reg.reg_no);
        break;
    case OPERAND_IMMEDIATE:
        snprintf(imm_buf, sizeof(imm_buf), "%d", src->operand.immediate.value);
        src_str = imm_buf;
        break;
    case OPERAND_PSEUDO:
        src_str = src->operand.pseudo.pseudo_name;
        break;
    case OPERAND_STACK:
        snprintf(stack_src_buf, sizeof(stack_src_buf), "[rbp-%d]",
                 -src->operand.stack.offset);
        src_str = stack_src_buf;
        break;
    }

    if (fprintf(output_file, "    mov %s, %s\n", dst_str, src_str) < 0)
    {
        fprintf(stderr, "Error writing mov instruction\n");
        return false;
    }

    return true;
}

bool emit_ret_instruction(FILE *output_file)
{
    if (!output_file)
    {
        fprintf(stderr, "Error: NULL output file in emit_ret_instruction\n");
        return false;
    }
    return emit_function_epilogue(output_file);
}

bool emit_unary_instruction(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction || !output_file)
    {
        fprintf(stderr, "Error: NULL instruction or output file\n");
        return false;
    }

    asm_unary_operator_enum_t op = instruction->instr.unary.unary_operator->op;
    const char *op_str = (op == UNARY_NEG) ? "neg" : (op == UNARY_NOT) ? "not"
                                                                       : NULL;

    if (!op_str)
    {
        fprintf(stderr, "Error: Unsupported unary operator\n");
        return false;
    }

    asm_operand_t *operand = instruction->instr.unary.operand;
    if (!operand)
    {
        fprintf(stderr, "Error: NULL operand in unary instruction\n");
        return false;
    }

    const char *operand_str = NULL;
    char stack_buf[32];

    switch (operand->type)
    {
    case OPERAND_REGISTER:
        operand_str = map_register_name(operand->operand.reg.reg_no);
        break;
    case OPERAND_PSEUDO:
        operand_str = operand->operand.pseudo.pseudo_name;
        break;
    case OPERAND_STACK:
        snprintf(stack_buf, sizeof(stack_buf), "[rbp-%d]",
                 -operand->operand.stack.offset);
        operand_str = stack_buf;
        break;
    case OPERAND_IMMEDIATE:
        fprintf(stderr, "Error: Cannot perform unary operation on immediate\n");
        return false;
    }

    if (fprintf(output_file, "    %s %s\n", op_str, operand_str) < 0)
    {
        fprintf(stderr, "Error writing unary instruction\n");
        return false;
    }
    return true;
}

bool emit_stack_allocation(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction)
    {
        fprintf(stderr, "Error: NULL instruction in emit_stack_allocation\n");
        return false;
    }

    if (!output_file)
    {
        fprintf(stderr, "Error: NULL output file in emit_stack_allocation\n");
        return false;
    }

    int alloc_size = instruction->instr.alloc_stack.alloc_size;

    if (fprintf(output_file, "    sub rsp, %d\n", alloc_size) < 0)
    {
        fprintf(stderr, "Error writing stack allocation instruction\n");
        return false;
    }
    return true;
}

bool emit_asm_instruction(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction)
    {
        fprintf(stderr, "Error: NULL instruction in emit_asm_instruction\n");
        return false;
    }

    if (!output_file)
    {
        fprintf(stderr, "Error: NULL output file in emit_asm_instruction\n");
        return false;
    }

    switch (instruction->type)
    {
    case INSTRUCTION_MOV:
        return emit_mov_instruction(instruction, output_file);
    case INSTRUCTION_RET:
        return emit_ret_instruction(output_file);
    case INSTRUCTION_UNARY:
        return emit_unary_instruction(instruction, output_file);
    case INSTRUCTION_ALLOCATE_STACK:
        return emit_stack_allocation(instruction, output_file);
    default:
        fprintf(stderr, "Error: Unsupported instruction type\n");
        return false;
    }
}

bool emit_asm_function(asm_function_t *function, FILE *output_file)
{
    if (!function)
    {
        fprintf(stderr, "Error: NULL function in emit_asm_function\n");
        return false;
    }

    if (!output_file)
    {
        fprintf(stderr, "Error: NULL output file in emit_asm_function\n");
        return false;
    }

    if (!emit_function_prologue(function, output_file))
    {
        fprintf(stderr, "Error: Failed to emit function prologue\n");
        return false;
    }

    for (size_t i = 0; i < function->instruction_count; i++)
    {
        if (!function->instructions[i])
        {
            fprintf(stderr, "Error: NULL instruction at index %zu\n", i);
            return false;
        }

        if (!emit_asm_instruction(function->instructions[i], output_file))
        {
            fprintf(stderr, "Error: Failed to emit instruction at index %zu\n", i);
            return false;
        }
    }

    return true;
}

bool emit_asm_program(asm_program_t *program, const char *output_file_name)
{
    if (!program)
    {
        fprintf(stderr, "Error: NULL program passed to emit_asm_program\n");
        return false;
    }

    if (!output_file_name)
    {
        fprintf(stderr, "Error: NULL output file name\n");
        return false;
    }

    FILE *output_file = create_output_file(output_file_name);
    if (!output_file)
    {
        fprintf(stderr, "Error: Could not create output file\n");
        return false;
    }

    if (!emit_program_header(output_file))
    {
        fprintf(stderr, "Error: Failed to emit program header\n");
        close_output_file(output_file);
        return false;
    }

    if (!program->function)
    {
        fprintf(stderr, "Error: Null function in program\n");
        close_output_file(output_file);
        return false;
    }

    bool result = emit_asm_function(program->function, output_file);
    if (!result)
    {
        fprintf(stderr, "Error: Failed to emit assembly function\n");
    }

    if (!close_output_file(output_file))
    {
        fprintf(stderr, "Error: Failed to close output file\n");
        return false;
    }

    return result;
}

#endif // ASSEMBLY_EMITTER_H