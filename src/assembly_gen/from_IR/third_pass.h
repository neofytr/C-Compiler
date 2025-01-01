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

        switch (instruction->type)
        {
        case INSTRUCTION_CMP:
        {
            asm_instruction_cmp_t *cmp = &instruction->instr.cmp;

            if (cmp->first_operand->type == OPERAND_IMMEDIATE)
            {
                asm_operand_t *r11_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
                if (!r11_operand)
                    return false;

                r11_operand->base.type = ASM_NODE_OPERAND;
                r11_operand->base.parent = &instruction->base;
                r11_operand->type = OPERAND_REGISTER;
                r11_operand->operand.reg.reg_no = ASM_REG_R11;

                asm_instruction_t *mov_instruction = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
                if (!mov_instruction)
                    return false;

                mov_instruction->base.type = ASM_NODE_INSTRUCTION;
                mov_instruction->base.parent = instruction->base.parent;
                mov_instruction->type = INSTRUCTION_MOV;
                mov_instruction->instr.mov.src = cmp->first_operand;
                mov_instruction->instr.mov.dst = r11_operand;

                asm_operand_t *r11_cmp_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
                if (!r11_cmp_operand)
                    return false;

                *r11_cmp_operand = *r11_operand;
                cmp->first_operand = r11_cmp_operand;

                asm_instruction_t **new_instructions = (asm_instruction_t **)allocate(
                    sizeof(asm_instruction_t *) * (function->instruction_count + 1));
                if (!new_instructions)
                    return false;

                for (size_t j = 0; j < i; j++)
                {
                    new_instructions[j] = function->instructions[j];
                }
                new_instructions[i] = mov_instruction;
                new_instructions[i + 1] = instruction;
                for (size_t j = i + 1; j < function->instruction_count; j++)
                {
                    new_instructions[j + 1] = function->instructions[j];
                }

                deallocate(function->instructions);
                function->instructions = new_instructions;
                function->instruction_count++;
                i++;
            }
            else if (cmp->first_operand->type == OPERAND_STACK &&
                     cmp->second_operand->type == OPERAND_STACK)
            {
                asm_operand_t *r11_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
                if (!r11_operand)
                    return false;

                r11_operand->base.type = ASM_NODE_OPERAND;
                r11_operand->base.parent = &instruction->base;
                r11_operand->type = OPERAND_REGISTER;
                r11_operand->operand.reg.reg_no = ASM_REG_R11;

                asm_instruction_t *mov_instruction = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
                if (!mov_instruction)
                    return false;

                mov_instruction->base.type = ASM_NODE_INSTRUCTION;
                mov_instruction->base.parent = instruction->base.parent;
                mov_instruction->type = INSTRUCTION_MOV;
                mov_instruction->instr.mov.src = cmp->second_operand;
                mov_instruction->instr.mov.dst = r11_operand;

                asm_operand_t *r11_cmp_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
                if (!r11_cmp_operand)
                    return false;

                *r11_cmp_operand = *r11_operand;
                cmp->second_operand = r11_cmp_operand;

                asm_instruction_t **new_instructions = (asm_instruction_t **)allocate(
                    sizeof(asm_instruction_t *) * (function->instruction_count + 1));
                if (!new_instructions)
                    return false;

                for (size_t j = 0; j < i; j++)
                {
                    new_instructions[j] = function->instructions[j];
                }
                new_instructions[i] = mov_instruction;
                new_instructions[i + 1] = instruction;
                for (size_t j = i + 1; j < function->instruction_count; j++)
                {
                    new_instructions[j + 1] = function->instructions[j];
                }

                deallocate(function->instructions);
                function->instructions = new_instructions;
                function->instruction_count++;
                i++;
            }
            break;
        }
        case INSTRUCTION_MOV:
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

                for (size_t j = 0; j <= i; j++)
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
                i++;
            }
            break;
        }

        case INSTRUCTION_IDIV:
        {
            asm_instruction_idiv_t *asm_instruction_idiv = &instruction->instr.idiv;
            asm_operand_t *idiv_operand = asm_instruction_idiv->operand;

            if (!idiv_operand)
                return false;

            if (idiv_operand->type == OPERAND_IMMEDIATE)
            {
                asm_operand_t *r10_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
                if (!r10_operand)
                    return false;

                r10_operand->base.type = ASM_NODE_OPERAND;
                r10_operand->base.parent = &instruction->base;
                r10_operand->type = OPERAND_REGISTER;
                r10_operand->operand.reg.reg_no = ASM_REG_R10;

                asm_instruction_t *mov_instruction = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
                if (!mov_instruction)
                    return false;

                mov_instruction->base.type = ASM_NODE_INSTRUCTION;
                mov_instruction->base.parent = instruction->base.parent;
                mov_instruction->type = INSTRUCTION_MOV;
                mov_instruction->instr.mov.src = idiv_operand;
                mov_instruction->instr.mov.dst = r10_operand;

                asm_instruction_idiv->operand = r10_operand;

                asm_instruction_t **new_instructions = (asm_instruction_t **)allocate(
                    sizeof(asm_instruction_t *) * (function->instruction_count + 1));
                if (!new_instructions)
                    return false;

                for (size_t j = 0; j < i; j++)
                {
                    new_instructions[j] = function->instructions[j];
                }

                new_instructions[i] = mov_instruction;
                new_instructions[i + 1] = instruction;

                for (size_t j = i + 1; j < function->instruction_count; j++)
                {
                    new_instructions[j + 1] = function->instructions[j];
                }

                deallocate(function->instructions);
                function->instructions = new_instructions;
                function->instruction_count++;
                i++;
            }
            break;
        }

        case INSTRUCTION_BINARY:
        {
            asm_instruction_binary_t *binary_instruction = &instruction->instr.binary;

            switch (binary_instruction->binary_operator->binary_op)
            {
            case ASM_BINARY_BITWISE_AND:
            case ASM_BINARY_BITWISE_OR:
            case ASM_BINARY_BITWISE_XOR:
            case ASM_BINARY_ADD:
            case ASM_BINARY_SUB:
            {
                if (binary_instruction->first_operand->type == OPERAND_STACK &&
                    binary_instruction->second_operand->type == OPERAND_STACK)
                {
                    asm_operand_t *r10_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
                    if (!r10_operand)
                        return false;

                    r10_operand->base.type = ASM_NODE_OPERAND;
                    r10_operand->base.parent = &instruction->base;
                    r10_operand->type = OPERAND_REGISTER;
                    r10_operand->operand.reg.reg_no = ASM_REG_R10;

                    asm_instruction_t *first_mov = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
                    if (!first_mov)
                        return false;

                    first_mov->base.type = ASM_NODE_INSTRUCTION;
                    first_mov->base.parent = instruction->base.parent;
                    first_mov->type = INSTRUCTION_MOV;
                    first_mov->instr.mov.src = binary_instruction->first_operand;
                    first_mov->instr.mov.dst = r10_operand;

                    binary_instruction->first_operand = r10_operand;

                    asm_instruction_t **new_instructions = (asm_instruction_t **)allocate(
                        sizeof(asm_instruction_t *) * (function->instruction_count + 1));
                    if (!new_instructions)
                        return false;

                    for (size_t j = 0; j < i; j++)
                    {
                        new_instructions[j] = function->instructions[j];
                    }

                    new_instructions[i] = first_mov;
                    new_instructions[i + 1] = instruction;

                    for (size_t j = i + 1; j < function->instruction_count; j++)
                    {
                        new_instructions[j + 1] = function->instructions[j];
                    }

                    deallocate(function->instructions);
                    function->instructions = new_instructions;
                    function->instruction_count++;
                    i++;
                }
                break;
            }
            case ASM_BINARY_MULT:
            {
                asm_instruction_binary_t *imul_instruction = &instruction->instr.binary;

                if (imul_instruction->first_operand->type == OPERAND_STACK)
                {
                    asm_operand_t *r11_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
                    if (!r11_operand)
                        return false;

                    r11_operand->base.type = ASM_NODE_OPERAND;
                    r11_operand->base.parent = &instruction->base;
                    r11_operand->type = OPERAND_REGISTER;
                    r11_operand->operand.reg.reg_no = ASM_REG_R11;

                    // 1. Move memory to r11
                    // 2. IMUL with r11 as destination
                    // 3. Move r11 back to memory

                    asm_instruction_t *first_mov = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
                    if (!first_mov)
                        return false;

                    first_mov->base.type = ASM_NODE_INSTRUCTION;
                    first_mov->base.parent = instruction->base.parent;
                    first_mov->type = INSTRUCTION_MOV;
                    first_mov->instr.mov.src = imul_instruction->first_operand;
                    first_mov->instr.mov.dst = r11_operand;

                    imul_instruction->first_operand = r11_operand;

                    asm_instruction_t *third_mov = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
                    if (!third_mov)
                        return false;

                    third_mov->base.type = ASM_NODE_INSTRUCTION;
                    third_mov->base.parent = instruction->base.parent;
                    third_mov->type = INSTRUCTION_MOV;
                    third_mov->instr.mov.src = r11_operand;
                    third_mov->instr.mov.dst = first_mov->instr.mov.src;

                    asm_instruction_t **new_instructions = (asm_instruction_t **)allocate(
                        sizeof(asm_instruction_t *) * (function->instruction_count + 2));
                    if (!new_instructions)
                        return false;

                    for (size_t j = 0; j < i; j++)
                    {
                        new_instructions[j] = function->instructions[j];
                    }

                    new_instructions[i] = first_mov;
                    new_instructions[i + 1] = instruction;
                    new_instructions[i + 2] = third_mov;

                    for (size_t j = i + 1; j < function->instruction_count; j++)
                    {
                        new_instructions[j + 2] = function->instructions[j];
                    }

                    deallocate(function->instructions);
                    function->instructions = new_instructions;
                    function->instruction_count += 2;
                    i++;
                }
                break;
            }
            case ASM_BINARY_BITWISE_SHIFT_LEFT:
            case ASM_BINARY_BITWISE_SHIFT_RIGHT:
            {
                asm_operand_t *cl_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
                if (!cl_operand)
                    return false;

                cl_operand->base.type = ASM_NODE_OPERAND;
                cl_operand->base.parent = &instruction->base;
                cl_operand->type = OPERAND_REGISTER;
                cl_operand->operand.reg.reg_no = ASM_REG_RCX;

                asm_instruction_t *mov_to_cl = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
                if (!mov_to_cl)
                    return false;

                mov_to_cl->base.type = ASM_NODE_INSTRUCTION;
                mov_to_cl->base.parent = instruction->base.parent;
                mov_to_cl->type = INSTRUCTION_MOV;
                mov_to_cl->instr.mov.src = binary_instruction->second_operand;
                mov_to_cl->instr.mov.dst = cl_operand;

                binary_instruction->second_operand = cl_operand;

                asm_instruction_t **new_instructions = (asm_instruction_t **)allocate(
                    sizeof(asm_instruction_t *) * (function->instruction_count + 1));
                if (!new_instructions)
                    return false;

                for (size_t j = 0; j < i; j++)
                {
                    new_instructions[j] = function->instructions[j];
                }

                new_instructions[i] = mov_to_cl;
                new_instructions[i + 1] = instruction;

                for (size_t j = i + 1; j < function->instruction_count; j++)
                {
                    new_instructions[j + 1] = function->instructions[j];
                }

                deallocate(function->instructions);
                function->instructions = new_instructions;
                function->instruction_count++;
                i++;

                break;
            }
            default:
            {
                break;
            }
            }
        }

        default:
            break;
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
