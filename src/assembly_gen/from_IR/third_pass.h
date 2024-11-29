#ifndef A0994C9B_E1E9_4A37_93B6_749D272B877A
#define A0994C9B_E1E9_4A37_93B6_749D272B877A

#include "../../ast/assembly/assembly_ast.h"
#include "../../allocator/allocator.h"
#include <stdbool.h>

bool asm_third_pass(asm_program_t *asm_program, int final_offset);
bool asm_insert_alloc_stack(asm_program_t *asm_program, int final_offset);
bool asm_fix_instruction(asm_program_t *asm_program);

bool asm_third_pass(asm_program_t *asm_program, int final_offset)
{
    return asm_insert_alloc_stack(asm_program, final_offset) && asm_fix_instruction(asm_program);
}

bool asm_fix_instruction(asm_program_t *asm_program)
{
    if (!asm_program || !asm_program->function)
        return false;

    asm_function_t *function = asm_program->function;

    for (size_t i = 0; i < function->instruction_count; i++)
    {
        asm_instruction_t *instruction = function->instructions[i];

        if (instruction->type == INSTRUCTION_MOV)
        {
            asm_instruction_mov_t *mov = &instruction->instr.mov;

            if (mov->src->type == OPERAND_STACK && mov->dst->type == OPERAND_STACK)
            {
                asm_operand_t *r10_src_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
                asm_operand_t *r10_dst_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
                if (!r10_src_operand || !r10_dst_operand)
                    return false;

                r10_src_operand->base.type = ASM_NODE_OPERAND;
                r10_src_operand->base.parent = &instruction->base;
                r10_src_operand->type = OPERAND_REGISTER;
                r10_src_operand->operand.reg.reg_no = ASM_REG_R10;

                r10_dst_operand->base.type = ASM_NODE_OPERAND;
                r10_dst_operand->base.parent = &instruction->base;
                r10_dst_operand->type = OPERAND_REGISTER;
                r10_dst_operand->operand.reg.reg_no = ASM_REG_R10;

                asm_instruction_mov_t first_mov = {
                    .src = mov->src,
                    .dst = r10_src_operand};

                asm_instruction_mov_t second_mov = {
                    .src = r10_dst_operand,
                    .dst = mov->dst};

                instruction->instr.mov = first_mov;

                asm_instruction_t *second_mov_instruction = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
                if (!second_mov_instruction)
                    return false;

                second_mov_instruction->base.type = ASM_NODE_INSTRUCTION;
                second_mov_instruction->base.parent = instruction->base.parent;
                second_mov_instruction->type = INSTRUCTION_MOV;
                second_mov_instruction->instr.mov = second_mov;

                asm_instruction_t **new_instructions = (asm_instruction_t **)allocate(
                    sizeof(asm_instruction_t *) * (function->instruction_count + 1));
                if (!new_instructions)
                    return false;

                for (size_t j = 0; j < i + 1; j++)
                {
                    new_instructions[j] = function->instructions[j];
                }
                new_instructions[i + 1] = second_mov_instruction;
                for (size_t j = i + 1; j < function->instruction_count; j++)
                {
                    new_instructions[j + 1] = function->instructions[j];
                }

                deallocate(function->instructions);
                function->instructions = new_instructions;
                function->instruction_count++;
            }
        }
    }

    return true;
}

bool asm_insert_alloc_stack(asm_program_t *asm_program, int final_offset)
{
    asm_instruction_t *asm_instruction_alloc_stack = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
    if (!asm_instruction_alloc_stack)
    {
        return false;
    }

    asm_instruction_alloc_stack->base.parent = NULL;
    asm_instruction_alloc_stack->base.type = ASM_NODE_INSTRUCTION;
    asm_instruction_alloc_stack->type = INSTRUCTION_ALLOCATE_STACK;

    asm_instruction_alloc_stack->instr.alloc_stack.alloc_size = -final_offset;

    asm_function_t *asm_function = asm_program->function;
    if (!asm_function)
    {
        deallocate(asm_instruction_alloc_stack);
        return false;
    }

    asm_instruction_t **asm_instructions = asm_function->instructions;
    if (!asm_instructions)
    {
        deallocate(asm_instruction_alloc_stack);
        return false;
    }

    size_t asm_instruction_count = asm_function->instruction_count;

    asm_instruction_t **new_asm_instructions = (asm_instruction_t **)allocate(sizeof(asm_instruction_t *) * (++asm_instruction_count));
    if (!new_asm_instructions)
    {
        deallocate(asm_instruction_alloc_stack);
        return false;
    }

    for (size_t counter = 1; counter < asm_instruction_count; counter++)
    {
        asm_instruction_t *asm_current_instruction = asm_instructions[counter - 1];
        if (!asm_current_instruction)
        {
            deallocate(asm_instruction_alloc_stack);
            deallocate(new_asm_instructions);
            return false;
        }
        new_asm_instructions[counter] = asm_instructions[counter - 1];
    }

    deallocate(asm_instructions);
    new_asm_instructions[0] = asm_instruction_alloc_stack;
    new_asm_instructions[1]->base.parent = &(new_asm_instructions[0]->base);

    asm_function->instructions = new_asm_instructions;
    asm_function->instruction_count = asm_instruction_count;

    return true;
}

#endif /* A0994C9B_E1E9_4A37_93B6_749D272B877A */
