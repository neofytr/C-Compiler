#ifndef ASSEMBLY_EMITTER_H
#define ASSEMBLY_EMITTER_H

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include "../assembly_gen/assembly_generation.h"

const char *map_register_name(asm_reg_no_t reg);

FILE *create_output_file(const char *name);
bool close_output_file(FILE *file);

bool emit_program_header(FILE *output_file);
bool emit_function_prologue(asm_function_t *function, FILE *output_file);
bool emit_function_epilogue(FILE *output_file);

bool emit_mov_instruction(asm_instruction_t *instruction, FILE *output_file);
bool emit_ret_instruction(FILE *output_file);
bool emit_unary_instruction(asm_instruction_t *instruction, FILE *output_file);
bool emit_stack_allocation(asm_instruction_t *instruction, FILE *output_file);

bool emit_asm_instruction(asm_instruction_t *instruction, FILE *output_file);
bool emit_asm_function(asm_function_t *function, FILE *output_file);
bool emit_asm_program(asm_program_t *program, const char *output_file_name);

const char *map_register_name(asm_reg_no_t reg)
{
    switch (reg)
    {
    case ASM_REG_R10:
        return "r10";
    case ASM_REG_RAX:
        return "rax";
    default:
        return "";
    }
}

FILE *create_output_file(const char *name)
{
    FILE *output_file = fopen(name, "w");
    if (!output_file)
    {
        fprintf(stderr, "Error creating output file\n");
        perror("fopen");
        return NULL;
    }
    return output_file;
}

bool close_output_file(FILE *file)
{
    if (file)
    {
        return fclose(file) == 0;
    }
    return false;
}

bool emit_program_header(FILE *output_file)
{
    fprintf(output_file, "section .text\n");
    return true;
}

bool emit_function_prologue(asm_function_t *function, FILE *output_file)
{
    if (!function || !output_file)
    {
        return false;
    }

    fprintf(output_file, "global %s\n%s:\n",
            function->name->name, function->name->name);
    fprintf(output_file, "    push rbp\n");
    fprintf(output_file, "    mov rbp, rsp\n");
    return true;
}

bool emit_function_epilogue(FILE *output_file)
{
    if (!output_file)
    {
        return false;
    }

    fprintf(output_file, "    mov rsp, rbp\n");
    fprintf(output_file, "    pop rbp\n");
    fprintf(output_file, "    ret\n");
    return true;
}

bool emit_mov_instruction(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction || !output_file)
    {
        return false;
    }

    asm_operand_t *dst = instruction->instr.mov.dst;
    asm_operand_t *src = instruction->instr.mov.src;

    if (dst->type == OPERAND_REGISTER && src->type == OPERAND_IMMEDIATE)
    {
        const char *dst_reg = map_register_name(dst->operand.reg.reg_no);
        int imm_value = src->operand.immediate.value;

        fprintf(output_file, "    mov %s, %d\n", dst_reg, imm_value);
        return true;
    }

    return false;
}

bool emit_ret_instruction(FILE *output_file)
{
    return emit_function_epilogue(output_file);
}

bool emit_unary_instruction(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction || !output_file)
    {
        return false;
    }

    asm_unary_operator_enum_t op = instruction->instr.unary.unary_operator->op;
    const char *op_str = (op == UNARY_NEG) ? "neg" : (op == UNARY_NOT) ? "not"
                                                                       : NULL;

    if (!op_str)
    {
        return false;
    }

    const char *operand_reg = map_register_name(
        instruction->instr.unary.operand->operand.reg.reg_no);

    fprintf(output_file, "    %s %s\n", op_str, operand_reg);
    return true;
}

bool emit_stack_allocation(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction || !output_file)
    {
        return false;
    }

    int alloc_size = instruction->instr.alloc_stack.alloc_size;
    fprintf(output_file, "    sub rsp, %d\n", alloc_size);
    return true;
}

bool emit_asm_instruction(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction || !output_file)
    {
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
        return false;
    }
}

bool emit_asm_function(asm_function_t *function, FILE *output_file)
{
    if (!function || !output_file)
    {
        return false;
    }

    if (!emit_function_prologue(function, output_file))
    {
        return false;
    }

    for (size_t i = 0; i < function->instruction_count; i++)
    {
        if (!emit_asm_instruction(function->instructions[i], output_file))
        {
            return false;
        }
    }

    return true;
}

bool emit_asm_program(asm_program_t *program, const char *output_file_name)
{
    FILE *output_file = create_output_file(output_file_name);
    if (!output_file)
    {
        return false;
    }

    // emit_program_header(output_file);

    bool result = emit_asm_function(program->function, output_file);
    close_output_file(output_file);

    return result;
}

#endif // ASSEMBLY_EMITTER_H