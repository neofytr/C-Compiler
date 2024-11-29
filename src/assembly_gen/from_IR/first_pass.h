#ifndef DFB0D46D_A862_49CB_B2FF_F7679D71B18C
#define DFB0D46D_A862_49CB_B2FF_F7679D71B18C

#include "../../ast/assembly/assembly_ast.h"
#include "../../ast/IR/ir_ast.h"
#include "../../allocator/allocator.h"

#define MAX_INSTR_NO_IN_FXN 1024

typedef struct
{
    asm_instruction_t **instructions;
    size_t instruction_count;
} asm_instruction_struct_t;

#define NULL_INSTRUCTION_STRUCT_ASM ((asm_instruction_struct_t){0})

asm_function_t *handle_ir_function(ir_function_t *ir_function);
asm_program_t *handle_ir_program(ir_program_t *ir_prorgam);
asm_identifier_t *handle_ir_identifier(ir_identifier_t *ir_identifier);
asm_instruction_struct_t handle_ir_instruction(ir_instruction_t *ir_instruction);

asm_instruction_struct_t handle_ir_instruction(ir_instruction_t *ir_instruction)
{
    if (!ir_instruction)
    {
        return NULL_INSTRUCTION_STRUCT_ASM;
    }

    ir_instruction_type_t ir_instruction_type = ir_instruction->type;
    switch (ir_instruction_type)
    {
    case IR_INSTR_RETURN:
    {
        asm_instruction_t *asm_instruction_mov = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
        if (!asm_instruction_mov)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        asm_instruction_mov->type = INSTRUCTION_MOV;
        asm_instruction_mov->base.type = ASM_NODE_INSTRUCTION;

        asm_operand_t *asm_mov_src = (asm_operand_t *)allocate(sizeof(asm_operand_t));
        if (!asm_mov_src)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        asm_operand_t *asm_mov_dst = (asm_operand_t *)allocate(sizeof(asm_operand_t));
        if (!asm_mov_src)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        asm_mov_dst->base.parent = &(asm_instruction_mov->base);
        asm_mov_dst->base.type = ASM_NODE_OPERAND;
        asm_mov_dst->type = OPERAND_REGISTER;
        asm_mov_dst->operand.reg.reg_no = ASM_REG_RAX;

        asm_mov_src->base.parent = &(asm_instruction_mov->base);
        asm_mov_src->base.type = ASM_NODE_OPERAND;

        ir_value_type_t ir_return_value_type = ir_instruction->instruction.return_instr.value->type;
        ir_value_t *ir_return_value = ir_instruction->instruction.return_instr.value;
        if (!ir_return_value)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        switch (ir_return_value_type)
        {
        case IR_VAL_CONSTANT_INT:
        {
            asm_mov_src->type = OPERAND_IMMEDIATE;
            asm_mov_src->operand.immediate.value = ir_return_value->value.constant_int;
            break;
        }
        case IR_VAL_VARIABLE:
        {
            asm_mov_src->type = OPERAND_PSEUDO;
            asm_mov_src->operand.pseudo.pseudo_name = ir_return_value->value.variable.identifier->name;
            break;
        }
        }

        asm_instruction_mov->instr.mov = (asm_instruction_mov_t){.src = asm_mov_src, .dst = asm_mov_dst};
        break;
    }
    case IR_INSTR_UNARY:
    {
        break;
    }
    }
}

asm_identifier_t *handle_ir_identifier(ir_identifier_t *ir_identifier)
{
    if (!ir_identifier)
    {
        return NULL;
    }

    asm_identifier_t *asm_identifier = (asm_identifier_t *)allocate(sizeof(asm_identifier_t));
    if (!asm_identifier)
    {
        return NULL;
    }

    char *ir_name = ir_identifier->name;
    if (!ir_name)
    {
        return NULL;
    }

    asm_identifier->base.type = ASM_NODE_IDENTIFIER;
    asm_identifier->name = ir_name;

    return asm_identifier;
}

asm_function_t *handle_ir_function(ir_function_t *ir_function)
{
    if (!ir_function)
    {
        return NULL;
    }

    asm_function_t *asm_function = (asm_function_t *)allocate(sizeof(asm_function_t));
    if (!asm_function)
    {
        return NULL;
    }

    ir_identifier_t *ir_identifier = ir_function->name;
    if (!ir_identifier)
    {
        return NULL;
    }

    asm_identifier_t *asm_identifier = handle_ir_identifier(ir_identifier);
    if (!asm_identifier)
    {
        return NULL;
    }

    asm_identifier->base.parent = &(asm_function->base);
    asm_function->name = asm_identifier;

    ir_instruction_t **ir_instructions = ir_function->body;
    if (!ir_instructions)
    {
        return NULL;
    }

    asm_instruction_t *asm_instruction_buffer[MAX_INSTR_NO_IN_FXN] = {0};
    size_t total_asm_instruction_count = 0;

    size_t ir_instruction_count = ir_function->instruction_count;
    for (size_t counter = 0; counter < ir_instruction_count; counter++)
    {
        ir_instruction_t *ir_instruction = ir_instructions[counter];
        if (!ir_instruction)
        {
            return NULL;
        }

        asm_instruction_struct_t asm_instruction_struct = handle_ir_instruction(ir_instruction);
        asm_instruction_t **asm_instructions = asm_instruction_struct.instructions;
        size_t asm_instruction_count = asm_instruction_struct.instruction_count;

        for (size_t asm_counter = 0; asm_counter < asm_instruction_count; asm_counter++)
        {
            asm_instruction_t *asm_instruction = asm_instructions[asm_counter];
            if (!asm_instruction)
            {
                return NULL;
            }

            asm_instruction->base.parent = &(asm_function->base);
        }

        memcpy((void *)asm_instruction_buffer, (void *)*asm_instructions, asm_instruction_count * sizeof(asm_instruction_t *));
        total_asm_instruction_count += asm_instruction_count;
        deallocate(asm_instructions);
    }

    asm_instruction_t **total_asm_instructions = (asm_instruction_t **)allocate(sizeof(asm_instruction_t *));
    if (!total_asm_instructions)
    {
        return NULL;
    }
    memcpy((void *)*total_asm_instructions, (void *)asm_instruction_buffer, total_asm_instruction_count * sizeof(asm_instruction_t *));
    asm_function->instructions = total_asm_instructions;
    asm_function->instruction_count = total_asm_instruction_count;

    return asm_function;
}

asm_program_t *handle_ir_program(ir_program_t *ir_program)
{
    if (!ir_program)
    {
        return NULL;
    }

    asm_program_t *asm_program = (asm_program_t *)allocate(sizeof(asm_program_t));
    if (!asm_program)
    {
        return NULL;
    }

    asm_program->base.parent = NULL;
    asm_program->base.type = ASM_NODE_PROGRAM;

    ir_function_t *ir_function = ir_program->function;
    if (!ir_function)
    {
        return NULL;
    }

    asm_function_t *asm_function = (asm_function_t *)handle_ir_function(ir_function);
    if (!asm_function)
    {
        return NULL;
    }

    asm_function->base.parent = &(asm_program->base);
    asm_program->function = asm_function;
    return asm_program;
}

#undef NULL_INSTRUCTION_STRUCT_ASM
#undef MAX_INSTR_NO_IN_FXN

#endif /* DFB0D46D_A862_49CB_B2FF_F7679D71B18C */