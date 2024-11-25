#ifndef FABD87FA_C040_443C_B25F_EE7BD4CA78EA
#define FABD87FA_C040_443C_B25F_EE7BD4CA78EA

#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include "../assembly_gen/assembly_generation.h"

FILE *create_output_file(const char *name);
bool emit_asm_function(asm_function_t *asm_function, FILE *output_file);
bool emit_asm_program(asm_program_t *asm_program, FILE *output_file);
bool emit_asm_instruction(asm_instruction_t *asm_instruction, FILE *output_file);

bool emit_asm(asm_program_t *asm_program, const char *output_file_name);

bool emit_asm(asm_program_t *asm_program, const char *output_file_name)
{
    FILE *output_file = create_output_file(output_file_name);
    if (!output_file)
    {
        return false;
    }

    return emit_asm_program(asm_program, output_file);
}

const char *map_val_to_reg_name(int val)
{
    switch (val)
    {
    case 0:
    {
        return "eax";
        break;
    }

    default:
        break;
    }

    return "";
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

bool emit_asm_program(asm_program_t *asm_program, FILE *output_file)
{
    if (!asm_program || !output_file)
    {
        return false;
    }
    return emit_asm_function(asm_program->function, output_file);
}

bool emit_asm_function(asm_function_t *asm_function, FILE *output_file)
{
    if (!asm_function || !output_file)
    {
        return false;
    }

    fprintf(output_file, "global %s\n%s:\n",
            asm_function->name->name, asm_function->name->name);

    for (size_t counter = 0; counter < asm_function->instruction_count; counter++)
    {
        if (!emit_asm_instruction(asm_function->instructions[counter], output_file))
        {
            return false;
        }
    }
    return true;
}

bool emit_asm_instruction(asm_instruction_t *asm_instruction, FILE *output_file)
{
    if (!asm_instruction || !output_file)
    {
        return false;
    }

    switch (asm_instruction->type)
    {
    case INSTRUCTION_RET:
        fprintf(output_file, "  ret\n");
        return true;

    case INSTRUCTION_MOV:
        if (asm_instruction->instr.mov->dst->type == OPERAND_REGISTER)
        {
            const char *reg_name_dst = map_val_to_reg_name(
                asm_instruction->instr.mov->dst->operand.reg->reg_num);

            if (!reg_name_dst)
            {
                return false;
            }

            if (asm_instruction->instr.mov->src->type == OPERAND_IMMEDIATE)
            {
                int imm_src = asm_instruction->instr.mov->src->operand.immediate->value;
                fprintf(output_file, "  movl %s, %d\n", reg_name_dst, imm_src);
                return true;
            }
        }
        return false;

    default:
        return false; // Unsupported instruction type
    }
}

#endif