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
    case ASM_REG_R11:
        return "r11";
    case ASM_REG_RDX:
        return "rdx";
    case ASM_REG_RCX:
        return "rcx";
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

    if (dst->type == OPERAND_STACK)
    {
        if (fprintf(output_file, "    mov qword %s, %s\n", dst_str, src_str) < 0)
        {
            fprintf(stderr, "Error writing mov instruction\n");
            return false;
        }
    }
    else
    {
        if (fprintf(output_file, "    mov %s, %s\n", dst_str, src_str) < 0)
        {
            fprintf(stderr, "Error writing mov instruction\n");
            return false;
        }
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

    if (operand->type == OPERAND_STACK)
    {
        if (fprintf(output_file, "    %s qword %s\n", op_str, operand_str) < 0)
        {
            fprintf(stderr, "Error writing unary instruction\n");
            return false;
        }
    }
    else
    {
        if (fprintf(output_file, "    %s %s\n", op_str, operand_str) < 0)
        {
            fprintf(stderr, "Error writing unary instruction\n");
            return false;
        }
    }

    return true;
}

bool emit_binary_instruction(asm_instruction_t *asm_instruction, FILE *output_file)
{
    if (!asm_instruction || !output_file)
    {
        fprintf(stderr, "Error: NULL instruction or output file in emit_binary_instruction\n");
        return false;
    }

    asm_instruction_binary_t *binary_instruction = &asm_instruction->instr.binary;
    if (!binary_instruction->binary_operator || !binary_instruction->first_operand || !binary_instruction->second_operand)
    {
        fprintf(stderr, "Error: Incomplete binary instruction\n");
        return false;
    }

    asm_binary_operator_type_t op = binary_instruction->binary_operator->binary_op;
    const char *op_str = NULL;

    switch (op)
    {
    case ASM_BINARY_ADD:
        op_str = "add";
        break;
    case ASM_BINARY_SUB:
        op_str = "sub";
        break;
    case ASM_BINARY_MULT:
        op_str = "imul";
        break;
    case ASM_BINARY_BITWISE_AND:
        op_str = "and";
        break;
    case ASM_BINARY_BITWISE_OR:
        op_str = "or";
        break;
    case ASM_BINARY_BITWISE_XOR:
        op_str = "xor";
        break;
    case ASM_BINARY_BITWISE_SHIFT_LEFT:
        op_str = "sal";
        break;
    case ASM_BINARY_BITWISE_SHIFT_RIGHT:
        op_str = "sar";
        break;
    default:
        fprintf(stderr, "Error: Unsupported binary operator\n");
        return false;
    }

    asm_operand_t *first_operand = binary_instruction->first_operand;
    asm_operand_t *second_operand = binary_instruction->second_operand;

    const char *first_str = NULL;
    const char *second_str = NULL;

    char first_buf[32];
    char second_buf[32];

    switch (first_operand->type)
    {
    case OPERAND_REGISTER:
        first_str = map_register_name(first_operand->operand.reg.reg_no);
        break;
    case OPERAND_PSEUDO:
        first_str = first_operand->operand.pseudo.pseudo_name;
        break;
    case OPERAND_STACK:
        snprintf(first_buf, sizeof(first_buf), "[rbp-%d]", -first_operand->operand.stack.offset);
        first_str = first_buf;
        break;
    case OPERAND_IMMEDIATE:
        snprintf(first_buf, sizeof(first_buf), "%d", first_operand->operand.immediate.value);
        first_str = first_buf;
        break;
    }

    switch (second_operand->type)
    {
    case OPERAND_REGISTER:
        second_str = map_register_name(second_operand->operand.reg.reg_no);
        break;
    case OPERAND_PSEUDO:
        second_str = second_operand->operand.pseudo.pseudo_name;
        break;
    case OPERAND_STACK:
        snprintf(second_buf, sizeof(second_buf), "[rbp-%d]", -second_operand->operand.stack.offset);
        second_str = second_buf;
        break;
    case OPERAND_IMMEDIATE:
        snprintf(second_buf, sizeof(second_buf), "%d", second_operand->operand.immediate.value);
        second_str = second_buf;
        break;
    }

    if (op == ASM_BINARY_BITWISE_SHIFT_LEFT || op == ASM_BINARY_BITWISE_SHIFT_RIGHT)
    {
        if (first_operand->type == OPERAND_STACK)
        {
            if (fprintf(output_file, "    %s qword %s, cl\n", op_str, first_str) < 0)
            {
                fprintf(stderr, "Error writing binary instruction\n");
                return false;
            }
        }
        else
        {
            if (fprintf(output_file, "    %s %s, cl\n", op_str, first_str) < 0)
            {
                fprintf(stderr, "Error writing binary instruction\n");
                return false;
            }
        }

        return true;
    }

    if (first_operand->type == OPERAND_STACK)
    {
        if (fprintf(output_file, "    %s qword %s, %s\n", op_str, first_str, second_str) < 0)
        {
            fprintf(stderr, "Error writing binary instruction\n");
            return false;
        }
    }
    else
    {
        if (fprintf(output_file, "    %s %s, %s\n", op_str, first_str, second_str) < 0)
        {
            fprintf(stderr, "Error writing binary instruction\n");
            return false;
        }
    }

    return true;
}

bool emit_cqo_instruction(asm_instruction_t *asm_instruction, FILE *output_file)
{
    if (!asm_instruction)
    {
        return false;
    }

    if (!output_file)
    {
        return false;
    }

    fprintf(output_file, "    cqo\n");

    return true;
}

bool emit_idiv_instruction(asm_instruction_t *asm_instruction, FILE *output_file)
{
    if (!asm_instruction)
    {
        return false;
    }

    if (!output_file)
    {
        return false;
    }

    asm_instruction_idiv_t *idiv_instruction = &asm_instruction->instr.idiv;
    if (!idiv_instruction)
    {
        return false;
    }

    asm_operand_t *idiv_operand = idiv_instruction->operand;
    if (!idiv_operand)
    {
        return false;
    }

    switch (idiv_operand->type)
    {
    case OPERAND_IMMEDIATE:
    {
        return false;
        break;
    }
    case OPERAND_REGISTER:
    {
        asm_register_t *idiv_reg_operand = &idiv_operand->operand.reg;
        fprintf(output_file, "    idiv %s\n", map_register_name(idiv_reg_operand->reg_no));
        break;
    }
    case OPERAND_STACK:
    {
        asm_stack_t *idiv_stack_operand = &idiv_operand->operand.stack;
        fprintf(output_file, "    idiv qword [rbp-%d]\n", -idiv_stack_operand->offset);
        break;
    }
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

bool emit_jmp_instruction(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction)
    {
        fprintf(stderr, "Error: NULL instruction in emit_jmp_instruction\n");
        return false;
    }

    if (!output_file)
    {
        fprintf(stderr, "Error: NULL output file in emit_jmp_instruction\n");
        return false;
    }

    if (fprintf(output_file, "    jmp %s\n", instruction->instr.jmp.target->name) < 0)
    {
        fprintf(stderr, "Error writing unconditional jump instructions\n");
        return false;
    }

    return true;
}

bool emit_jmpcc_instruction(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction)
    {
        fprintf(stderr, "Error: NULL instruction in emit_jmpcc_instruction\n");
        return false;
    }

    if (!output_file)
    {
        fprintf(stderr, "Error: NULL output file in emit_jmpcc_instruction\n");
        return false;
    }

    switch (instruction->instr.jmpcc.condition)
    {
    case COND_E:
    {
        if (fprintf(output_file, "    je %s\n", instruction->instr.jmpcc.target->name) < 0)
        {
            fprintf(stderr, "Error writing unconditional jump instructions\n");
            return false;
        }
        break;
    }
    case COND_NE:
    {

        if (fprintf(output_file, "    jne %s\n", instruction->instr.jmpcc.target->name) < 0)
        {
            fprintf(stderr, "Error writing unconditional jump instructions\n");
            return false;
        }
        break;
    }
    case COND_G:
    {
        if (fprintf(output_file, "    jg %s\n", instruction->instr.jmpcc.target->name) < 0)
        {
            fprintf(stderr, "Error writing unconditional jump instructions\n");
            return false;
        }
        break;
    }
    case COND_GE:
    {
        if (fprintf(output_file, "    jge %s\n", instruction->instr.jmpcc.target->name) < 0)
        {
            fprintf(stderr, "Error writing unconditional jump instructions\n");
            return false;
        }
        break;
    }
    case COND_L:
    {
        if (fprintf(output_file, "    jl %s\n", instruction->instr.jmpcc.target->name) < 0)
        {
            fprintf(stderr, "Error writing unconditional jump instructions\n");
            return false;
        }
        break;
    }
    case COND_LE:
    {
        if (fprintf(output_file, "    jle %s\n", instruction->instr.jmpcc.target->name) < 0)
        {
            fprintf(stderr, "Error writing unconditional jump instructions\n");
            return false;
        }
        break;
    }
    default:
    {

        return false;
    }
    }

    return true;
}

bool emit_cmp_instruction(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction || !output_file)
    {
        fprintf(stderr, "Error: NULL instruction or output file in emit_cmp_instruction\n");
        return false;
    }

    asm_instruction_cmp_t *cmp_instruction = &instruction->instr.cmp;
    if (!cmp_instruction->first_operand || !cmp_instruction->second_operand)
    {
        fprintf(stderr, "Error: Incomplete compare instruction\n");
        return false;
    }

    asm_operand_t *first_operand = cmp_instruction->first_operand;
    asm_operand_t *second_operand = cmp_instruction->second_operand;

    const char *first_str = NULL;
    const char *second_str = NULL;
    char first_buf[32];
    char second_buf[32];

    switch (first_operand->type)
    {
    case OPERAND_REGISTER:
        first_str = map_register_name(first_operand->operand.reg.reg_no);
        break;
    case OPERAND_PSEUDO:
        first_str = first_operand->operand.pseudo.pseudo_name;
        break;
    case OPERAND_STACK:
        snprintf(first_buf, sizeof(first_buf), "[rbp-%d]", -first_operand->operand.stack.offset);
        first_str = first_buf;
        break;
    case OPERAND_IMMEDIATE:
        snprintf(first_buf, sizeof(first_buf), "%d", first_operand->operand.immediate.value);
        first_str = first_buf;
        break;
    }

    switch (second_operand->type)
    {
    case OPERAND_REGISTER:
        second_str = map_register_name(second_operand->operand.reg.reg_no);
        break;
    case OPERAND_PSEUDO:
        second_str = second_operand->operand.pseudo.pseudo_name;
        break;
    case OPERAND_STACK:
        snprintf(second_buf, sizeof(second_buf), "[rbp-%d]", -second_operand->operand.stack.offset);
        second_str = second_buf;
        break;
    case OPERAND_IMMEDIATE:
        snprintf(second_buf, sizeof(second_buf), "%d", second_operand->operand.immediate.value);
        second_str = second_buf;
        break;
    }

    if (first_operand->type == OPERAND_STACK)
    {
        if (fprintf(output_file, "    cmp qword %s, %s\n", first_str, second_str) < 0)
        {
            fprintf(stderr, "Error writing compare instruction\n");
            return false;
        }
    }
    else
    {
        if (fprintf(output_file, "    cmp %s, %s\n", first_str, second_str) < 0)
        {
            fprintf(stderr, "Error writing compare instruction\n");
            return false;
        }
    }

    return true;
}

bool emit_setcc_instruction(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction || !output_file)
    {
        fprintf(stderr, "Error: NULL instruction or output file in emit_setcc_instruction\n");
        return false;
    }

    asm_instruction_setcc_t *setcc_instruction = &instruction->instr.setcc;
    if (!setcc_instruction->condition || !setcc_instruction->dst)
    {
        fprintf(stderr, "Error: Incomplete setcc instruction\n");
        return false;
    }

    const char *condition_str = NULL;
    switch (setcc_instruction->condition)
    {
    case COND_E:
        condition_str = "e";
        break;
    case COND_NE:
        condition_str = "ne";
        break;
    case COND_L:
        condition_str = "l";
        break;
    case COND_LE:
        condition_str = "le";
        break;
    case COND_G:
        condition_str = "g";
        break;
    case COND_GE:
        condition_str = "ge";
        break;
    default:
        fprintf(stderr, "Error: Unsupported condition type\n");
        return false;
    }

    asm_operand_t *dest_operand = setcc_instruction->dst;
    const char *dest_str = NULL;
    char dest_buf[32];

    switch (dest_operand->type)
    {
    case OPERAND_REGISTER:
        switch (dest_operand->operand.reg.reg_no)
        {
        case ASM_REG_R10:
            dest_str = "r10b";
            break;
        case ASM_REG_RAX:
            dest_str = "ax";
            break;
        case ASM_REG_R11:
            dest_str = "r11b";
            break;
        case ASM_REG_RDX:
            dest_str = "dl";
            break;
        case ASM_REG_RCX:
            dest_str = "cl";
            break;
        default:
            fprintf(stderr, "Error: Unsupported register in map_register_name\n");
            return false;
        }
        break;
    case OPERAND_PSEUDO:
        dest_str = dest_operand->operand.pseudo.pseudo_name;
        break;
    case OPERAND_STACK:
        snprintf(dest_buf, sizeof(dest_buf), "byte [rbp-%d]", -dest_operand->operand.stack.offset);
        dest_str = dest_buf;
        break;
    case OPERAND_IMMEDIATE:
        fprintf(stderr, "Error: Invalid immediate destination for setcc\n");
        return false;
    }

    if (fprintf(output_file, "    set%s %s\n", condition_str, dest_str) < 0)
    {
        fprintf(stderr, "Error writing setcc instruction\n");
        return false;
    }

    return true;
}

bool emit_label_instruction(asm_instruction_t *instruction, FILE *output_file)
{
    if (!instruction || !output_file)
    {
        fprintf(stderr, "Error: NULL instruction or output file in emit_setcc_instruction\n");
        return false;
    }

    asm_instruction_label_t label = instruction->instr.label;

    if (fprintf(output_file, "%s:\n", label.label->name) < 0)
    {
        fprintf(stderr, "Error writing label instruction\n");
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
    case INSTRUCTION_CQO:
        return emit_cqo_instruction(instruction, output_file);
    case INSTRUCTION_IDIV:
        return emit_idiv_instruction(instruction, output_file);
    case INSTRUCTION_BINARY:
        return emit_binary_instruction(instruction, output_file);
    case INSTRUCTION_JMP:
        return emit_jmp_instruction(instruction, output_file);
    case INSTRUCTION_JMPCC:
        return emit_jmpcc_instruction(instruction, output_file);
    case INSTRUCTION_CMP:
        return emit_cmp_instruction(instruction, output_file);
    case INSTRUCTION_SETCC:
        return emit_setcc_instruction(instruction, output_file);
    case INSTRUCTION_LABEL:
        return emit_label_instruction(instruction, output_file);
    default:
        fprintf(stderr, "Error: Unsupported instruction type\n");
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