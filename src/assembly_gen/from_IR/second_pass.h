#ifndef EB43219C_096E_4B3E_85A7_6E422D557509
#define EB43219C_096E_4B3E_85A7_6E422D557509

#include "../../ast/assembly/assembly_ast.h"
#include "../../hash_table/hash_table.h"

bool replace_pseudoregisters(asm_program_t *asm_program);
inline int create_new_offset(void);
inline bool replace_pseudoregs_operand(asm_operand_t *asm_operand, hash_table_t *hash_table);

inline bool replace_pseudoregs_operand(asm_operand_t *asm_operand, hash_table_t *hash_table)
{
    if (!asm_operand)
    {
        return false;
    }

    asm_operand_type_t *asm_operand_type = &asm_operand->type;
    if (!asm_operand_type)
    {
        return false;
    }

    if (*asm_operand_type == OPERAND_PSEUDO)
    {
        *asm_operand_type = OPERAND_STACK;
        const char *identifier = asm_operand->operand.pseudo.pseudo_name;
        hash_node_t *node = hash_table_search(hash_table, identifier);
        if (!node)
        {
            asm_operand->operand.stack = {.offset = create_new_offset()};
        }
        else
        {
            asm_operand->operand.stack = {.offset = node->offset};
        }
    }

    return true;
}

inline int create_new_offset(void)
{
    static int current_offset = 0;
    return (current_offset -= 4);
}

bool replace_pseudoregisters(asm_program_t *asm_program)
{
    if (!asm_program)
    {
        return false;
    }

    hash_table_t *hash_table = create_hash_table();
    if (!hash_table)
    {
        return false;
    }

    asm_function_t *asm_function = asm_program->function;
    if (!asm_function)
    {
        destroy_hash_table(hash_table);
        return false;
    }
    asm_instruction_t **asm_instructions = asm_function->instructions;
    if (!asm_instructions)
    {
        destroy_hash_table(hash_table);
        return false;
    }
    size_t asm_instruction_count = asm_function->instruction_count;

    for (size_t counter = 0; counter < asm_instruction_count; counter++)
    {
        asm_instruction_t *asm_instruction = asm_instructions[counter];
        if (!asm_instruction)
        {
            destroy_hash_table(hash_table);
            return false;
        }

        asm_instruction_type_t asm_instruction_type = asm_instruction->type;

        switch (asm_instruction_type)
        {
        case INSTRUCTION_UNARY:
        {
            asm_instruction_unary_t *asm_instruction_unary = &asm_instruction->instr.unary;
            if (!asm_instruction_unary)
            {
                destroy_hash_table(hash_table);
                return false;
            }
            asm_operand_t *asm_unary_operand = asm_instruction_unary->operand;
            if (!asm_unary_operand)
            {
                destroy_hash_table(hash_table);
                return false;
            }

            if (!replace_pseudoregs_operand(asm_unary_operand, hash_table))
            {
                destroy_hash_table(hash_table);
                return false;
            }

            break;
        }
        case INSTRUCTION_MOV:
        {
            asm_instruction_mov_t *asm_instruction_mov = &asm_instruction->instr.mov;
            if (!asm_instruction_mov)
            {
                destroy_hash_table(hash_table);
                return false;
            }
            asm_operand_t *asm_mov_src = asm_instruction_mov->src;
            if (!asm_mov_src)
            {
                destroy_hash_table(hash_table);
                return false;
            }
            asm_operand_t *asm_mov_dst = asm_instruction_mov->dst;
            if (!asm_mov_dst)
            {
                destroy_hash_table(hash_table);
                return false;
            }

            if (!replace_pseudoregs_operand(asm_mov_dst, hash_table) || !replace_pseudoregs_operand(asm_mov_src, hash_table))
            {
                destroy_hash_table(hash_table);
                return false;
            }

            break;
        }
        }
    }

    return true;
}

#endif /* EB43219C_096E_4B3E_85A7_6E422D557509 */
