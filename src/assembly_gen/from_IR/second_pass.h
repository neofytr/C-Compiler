#ifndef EB43219C_096E_4B3E_85A7_6E422D557509
#define EB43219C_096E_4B3E_85A7_6E422D557509

#include "../../ast/assembly/assembly_ast.h"
#include "../../hash_table/hash_table.h"

bool asm_second_pass(asm_program_t *asm_program, int *final_offset);
bool replace_pseudoregisters(asm_program_t *asm_program);
static int create_new_offset(void);
bool replace_pseudoregs_operand(asm_operand_t *asm_operand, hash_table_t *hash_table);

bool asm_second_pass(asm_program_t *asm_program, int *final_offset)
{
    if (!replace_pseudoregisters(asm_program))
    {
        return false;
    }

    *final_offset = create_new_offset() + 4;
    return true;
}

bool replace_pseudoregs_operand(asm_operand_t *asm_operand, hash_table_t *hash_table)
{
    if (!asm_operand)
    {
        return false;
    }

    asm_operand_type_t *asm_operand_type = &asm_operand->type;
    if (!asm_operand_type)
    {
        return false;
    } // never initialize strings with NULL

    if (*asm_operand_type == OPERAND_PSEUDO)
    {
        *asm_operand_type = OPERAND_STACK;
        const char *identifier = asm_operand->operand.pseudo.pseudo_name;
        hash_node_t *node = hash_table_search(hash_table, identifier);
        if (!node)
        {
            int offset = create_new_offset();
            asm_operand->operand.stack = (asm_stack_t){.offset = offset};
            hash_table_insert(hash_table, identifier, offset);
        }
        else
        {
            asm_operand->operand.stack = (asm_stack_t){.offset = node->offset};
        }
    }

    return true;
}

static int create_new_offset(void)
{
    static int current_offset = 0;
    current_offset -= 4;
    return current_offset;
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

            if (!replace_pseudoregs_operand(asm_mov_dst, hash_table) ||
                !replace_pseudoregs_operand(asm_mov_src, hash_table))
            {
                destroy_hash_table(hash_table);
                return false;
            }
            break;
        }
        case INSTRUCTION_BINARY:
        {
            asm_instruction_binary_t *asm_instruction_binary = &asm_instruction->instr.binary;
            if (!asm_instruction_binary)
            {
                destroy_hash_table(hash_table);
                return false;
            }
            asm_operand_t *asm_binary_first = asm_instruction_binary->first_operand;
            if (!asm_binary_first)
            {
                destroy_hash_table(hash_table);
                return false;
            }
            asm_operand_t *asm_binary_second = asm_instruction_binary->second_operand;
            if (!asm_binary_second)
            {
                destroy_hash_table(hash_table);
                return false;
            }

            if (!replace_pseudoregs_operand(asm_binary_first, hash_table) ||
                !replace_pseudoregs_operand(asm_binary_second, hash_table))
            {
                destroy_hash_table(hash_table);
                return false;
            }
            break;
        }
        case INSTRUCTION_CMP:
        {
            asm_instruction_cmp_t *asm_instruction_cmp = &asm_instruction->instr.cmp;
            if (!asm_instruction_cmp)
            {
                destroy_hash_table(hash_table);
                return false;
            }

            asm_operand_t *first_operand = asm_instruction_cmp->first_operand;
            asm_operand_t *second_operand = asm_instruction_cmp->second_operand;

            if (!first_operand || !second_operand)
            {
                destroy_hash_table(hash_table);
                return false;
            }

            if (!replace_pseudoregs_operand(first_operand, hash_table) ||
                !replace_pseudoregs_operand(second_operand, hash_table))
            {
                destroy_hash_table(hash_table);
                return false;
            }
            break;
        }
        case INSTRUCTION_SETCC:
        {
            asm_instruction_setcc_t *asm_instruction_setcc = &asm_instruction->instr.setcc;
            if (!asm_instruction_setcc)
            {
                destroy_hash_table(hash_table);
                return false;
            }

            asm_operand_t *dest = asm_instruction_setcc->dst;
            if (!dest)
            {
                destroy_hash_table(hash_table);
                return false;
            }

            if (!replace_pseudoregs_operand(dest, hash_table))
            {
                destroy_hash_table(hash_table);
                return false;
            }
            break;
        }
        case INSTRUCTION_IDIV:
        {
            asm_instruction_idiv_t *asm_instruction_idiv = &asm_instruction->instr.idiv;
            if (!asm_instruction_idiv)
            {
                destroy_hash_table(hash_table);
                return false;
            }

            asm_operand_t *operand_idiv = asm_instruction_idiv->operand;
            if (!operand_idiv)
            {
                destroy_hash_table(hash_table);
                return false;
            }

            if (!replace_pseudoregs_operand(operand_idiv, hash_table))
            {
                destroy_hash_table(hash_table);
                return false;
            }
            break; // Added missing break statement
        }
        }
    }

    destroy_hash_table(hash_table); // Added cleanup for success path
    return true;
}

#endif /* EB43219C_096E_4B3E_85A7_6E422D557509 */
