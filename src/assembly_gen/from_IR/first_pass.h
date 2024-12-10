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

asm_program_t *asm_first_pass(ir_program_t *ir_program);
asm_function_t *handle_ir_function(ir_function_t *ir_function);
asm_program_t *handle_ir_program(ir_program_t *ir_prorgam);
asm_identifier_t *handle_ir_identifier(ir_identifier_t *ir_identifier);
asm_instruction_struct_t handle_ir_instruction(ir_instruction_t *ir_instruction);

asm_program_t *asm_first_pass(ir_program_t *ir_program)
{
    return handle_ir_program(ir_program);
}

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

        asm_instruction_t *asm_instruction_return = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
        if (!asm_instruction_return)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        asm_instruction_return->base.type = ASM_NODE_INSTRUCTION;
        asm_instruction_return->type = INSTRUCTION_RET;

        asm_instruction_return->instr.ret = (asm_instruction_ret_t){};

        asm_instruction_t **asm_instructions = (asm_instruction_t **)allocate(sizeof(asm_instruction_t *) * 2);
        if (!asm_instructions)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }
        size_t asm_instruction_count = 2;

        asm_instructions[0] = asm_instruction_mov;
        asm_instructions[1] = asm_instruction_return;

        asm_instruction_struct_t asm_instructions_struct = (asm_instruction_struct_t){
            .instructions = asm_instructions,
            .instruction_count = asm_instruction_count,
        };

        return asm_instructions_struct;
        break;
    }
    case IR_INSTR_UNARY:
    {
        ir_instruction_unary_t ir_unary_instr = ir_instruction->instruction.unary_instr;

        asm_instruction_struct_t result = {
            .instructions = (asm_instruction_t **)allocate(2 * sizeof(asm_instruction_t *)),
            .instruction_count = 2};

        if (!result.instructions)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        asm_instruction_t *asm_instruction_mov = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
        if (!asm_instruction_mov)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        asm_instruction_mov->type = INSTRUCTION_MOV;
        asm_instruction_mov->base.type = ASM_NODE_INSTRUCTION;

        asm_operand_t *asm_mov_src = (asm_operand_t *)allocate(sizeof(asm_operand_t));
        asm_operand_t *asm_mov_dst = (asm_operand_t *)allocate(sizeof(asm_operand_t));

        if (!asm_mov_src || !asm_mov_dst)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        switch (ir_unary_instr.source->type)
        {
        case IR_VAL_CONSTANT_INT:
        {
            asm_mov_src->type = OPERAND_IMMEDIATE;
            asm_mov_src->operand.immediate.value = ir_unary_instr.source->value.constant_int;
            break;
        }
        case IR_VAL_VARIABLE:
        {
            asm_mov_src->type = OPERAND_PSEUDO;
            asm_mov_src->operand.pseudo.pseudo_name = ir_unary_instr.source->value.variable.identifier->name;
            break;
        }
        default:
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        asm_mov_dst->type = OPERAND_PSEUDO;
        asm_mov_dst->operand.pseudo.pseudo_name = ir_unary_instr.destination->value.variable.identifier->name;

        asm_instruction_mov->instr.mov = (asm_instruction_mov_t){.src = asm_mov_src, .dst = asm_mov_dst};

        asm_instruction_t *asm_instruction_unary = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
        if (!asm_instruction_unary)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        asm_instruction_unary->type = INSTRUCTION_UNARY;
        asm_instruction_unary->base.type = ASM_NODE_INSTRUCTION;

        asm_unary_operator_t *asm_unary_op = (asm_unary_operator_t *)allocate(sizeof(asm_unary_operator_t));
        if (!asm_unary_op)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        switch (ir_unary_instr.unary_operator->unary_op)
        {
        case IR_UNARY_BITWISE_COMPLEMENT:
            asm_unary_op->op = UNARY_NOT;
            break;
        case IR_UNARY_NEGATE:
            asm_unary_op->op = UNARY_NEG;
            break;
        default:
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        asm_operand_t *asm_unary_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
        if (!asm_unary_operand)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }

        asm_unary_operand->type = OPERAND_PSEUDO;
        asm_unary_operand->operand.pseudo.pseudo_name = ir_unary_instr.destination->value.variable.identifier->name;

        asm_instruction_unary->instr.unary = (asm_instruction_unary_t){
            .unary_operator = asm_unary_op,
            .operand = asm_unary_operand};

        result.instructions[0] = asm_instruction_mov;
        result.instructions[1] = asm_instruction_unary;

        return result;
    }
    case IR_INSTR_BINARY:
    {
        ir_instruction_binary_t ir_instruction_binary = ir_instruction->instruction.binary_instr;
        ir_binary_operator_t *ir_binary_operator = ir_instruction_binary.binary_operator;
        if (!ir_binary_operator)
        {
            return NULL_INSTRUCTION_STRUCT_ASM;
        }
        ir_binary_operator_type_t ir_binary_operator_type = ir_binary_operator->operator;

        switch (ir_binary_operator_type)
        {
        case IR_BINARY_DIVIDE:
        {
            asm_instruction_struct_t result = {
                .instructions = (asm_instruction_t **)allocate(4 * sizeof(asm_instruction_t *)),
                .instruction_count = 4,
            };

            if (!result.instructions)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_t *asm_instruction_mov_first = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
            if (!asm_instruction_mov_first)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_mov_first->type = INSTRUCTION_MOV;
            asm_instruction_mov_first->base.type = ASM_NODE_INSTRUCTION;

            asm_operand_t *asm_mov_first_src = (asm_operand_t *)allocate(sizeof(asm_operand_t));
            asm_operand_t *asm_mov_first_dst = (asm_operand_t *)allocate(sizeof(asm_operand_t));

            if (!asm_mov_first_src || !asm_mov_first_dst)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            switch (ir_instruction_binary.left->type)
            {
            case IR_VAL_CONSTANT_INT:
                asm_mov_first_src->type = OPERAND_IMMEDIATE;
                asm_mov_first_src->operand.immediate.value = ir_instruction_binary.left->value.constant_int;
                break;
            case IR_VAL_VARIABLE:
                asm_mov_first_src->type = OPERAND_PSEUDO;
                asm_mov_first_src->operand.pseudo.pseudo_name = ir_instruction_binary.left->value.variable.identifier->name;
                break;
            default:
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_mov_first_dst->type = OPERAND_REGISTER;
            asm_mov_first_dst->operand.reg.reg_no = ASM_REG_RAX;

            asm_instruction_mov_first->instr.mov = (asm_instruction_mov_t){
                .src = asm_mov_first_src,
                .dst = asm_mov_first_dst};

            asm_instruction_t *asm_instruction_cqo = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
            if (!asm_instruction_cqo)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_cqo->type = INSTRUCTION_CQO;
            asm_instruction_cqo->base.type = ASM_NODE_INSTRUCTION;
            asm_instruction_cqo->instr.ret = (asm_instruction_ret_t){};

            asm_instruction_t *asm_instruction_idiv = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
            if (!asm_instruction_idiv)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_idiv->type = INSTRUCTION_IDIV;
            asm_instruction_idiv->base.type = ASM_NODE_INSTRUCTION;

            asm_operand_t *asm_idiv_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
            if (!asm_idiv_operand)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            switch (ir_instruction_binary.right->type)
            {
            case IR_VAL_CONSTANT_INT:
                asm_idiv_operand->type = OPERAND_IMMEDIATE;
                asm_idiv_operand->operand.immediate.value = ir_instruction_binary.right->value.constant_int;
                break;
            case IR_VAL_VARIABLE:
                asm_idiv_operand->type = OPERAND_PSEUDO;
                asm_idiv_operand->operand.pseudo.pseudo_name = ir_instruction_binary.right->value.variable.identifier->name;
                break;
            default:
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_idiv->instr.idiv = (asm_instruction_idiv_t){.operand = asm_idiv_operand};

            asm_instruction_t *asm_instruction_mov_result = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
            if (!asm_instruction_mov_result)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_mov_result->type = INSTRUCTION_MOV;
            asm_instruction_mov_result->base.type = ASM_NODE_INSTRUCTION;

            asm_operand_t *asm_mov_result_src = (asm_operand_t *)allocate(sizeof(asm_operand_t));
            asm_operand_t *asm_mov_result_dst = (asm_operand_t *)allocate(sizeof(asm_operand_t));

            if (!asm_mov_result_src || !asm_mov_result_dst)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_mov_result_src->type = OPERAND_REGISTER;
            asm_mov_result_src->operand.reg.reg_no = ASM_REG_RAX;

            asm_mov_result_dst->type = OPERAND_PSEUDO;
            asm_mov_result_dst->operand.pseudo.pseudo_name = ir_instruction_binary.destination->value.variable.identifier->name;

            asm_instruction_mov_result->instr.mov = (asm_instruction_mov_t){
                .src = asm_mov_result_src,
                .dst = asm_mov_result_dst};

            result.instructions[0] = asm_instruction_mov_first;
            result.instructions[1] = asm_instruction_cqo;
            result.instructions[2] = asm_instruction_idiv;
            result.instructions[3] = asm_instruction_mov_result;

            return result;
        }

        case IR_BINARY_REM:
        {
            asm_instruction_struct_t result = {
                .instructions = (asm_instruction_t **)allocate(4 * sizeof(asm_instruction_t *)),
                .instruction_count = 4,
            };

            if (!result.instructions)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_t *asm_instruction_mov_first = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
            if (!asm_instruction_mov_first)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_mov_first->type = INSTRUCTION_MOV;
            asm_instruction_mov_first->base.type = ASM_NODE_INSTRUCTION;

            asm_operand_t *asm_mov_first_src = (asm_operand_t *)allocate(sizeof(asm_operand_t));
            asm_operand_t *asm_mov_first_dst = (asm_operand_t *)allocate(sizeof(asm_operand_t));

            if (!asm_mov_first_src || !asm_mov_first_dst)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            switch (ir_instruction_binary.left->type)
            {
            case IR_VAL_CONSTANT_INT:
                asm_mov_first_src->type = OPERAND_IMMEDIATE;
                asm_mov_first_src->operand.immediate.value = ir_instruction_binary.left->value.constant_int;
                break;
            case IR_VAL_VARIABLE:
                asm_mov_first_src->type = OPERAND_PSEUDO;
                asm_mov_first_src->operand.pseudo.pseudo_name = ir_instruction_binary.left->value.variable.identifier->name;
                break;
            default:
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_mov_first_dst->type = OPERAND_REGISTER;
            asm_mov_first_dst->operand.reg.reg_no = ASM_REG_RAX;

            asm_instruction_mov_first->instr.mov = (asm_instruction_mov_t){
                .src = asm_mov_first_src,
                .dst = asm_mov_first_dst};

            asm_instruction_t *asm_instruction_cqo = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
            if (!asm_instruction_cqo)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_cqo->type = INSTRUCTION_CQO;
            asm_instruction_cqo->base.type = ASM_NODE_INSTRUCTION;
            asm_instruction_cqo->instr.ret = (asm_instruction_ret_t){};

            asm_instruction_t *asm_instruction_idiv = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
            if (!asm_instruction_idiv)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_idiv->type = INSTRUCTION_IDIV;
            asm_instruction_idiv->base.type = ASM_NODE_INSTRUCTION;

            asm_operand_t *asm_idiv_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
            if (!asm_idiv_operand)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            switch (ir_instruction_binary.right->type)
            {
            case IR_VAL_CONSTANT_INT:
                asm_idiv_operand->type = OPERAND_IMMEDIATE;
                asm_idiv_operand->operand.immediate.value = ir_instruction_binary.right->value.constant_int;
                break;
            case IR_VAL_VARIABLE:
                asm_idiv_operand->type = OPERAND_PSEUDO;
                asm_idiv_operand->operand.pseudo.pseudo_name = ir_instruction_binary.right->value.variable.identifier->name;
                break;
            default:
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_idiv->instr.idiv = (asm_instruction_idiv_t){.operand = asm_idiv_operand};

            asm_instruction_t *asm_instruction_mov_result = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
            if (!asm_instruction_mov_result)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_mov_result->type = INSTRUCTION_MOV;
            asm_instruction_mov_result->base.type = ASM_NODE_INSTRUCTION;

            asm_operand_t *asm_mov_result_src = (asm_operand_t *)allocate(sizeof(asm_operand_t));
            asm_operand_t *asm_mov_result_dst = (asm_operand_t *)allocate(sizeof(asm_operand_t));

            if (!asm_mov_result_src || !asm_mov_result_dst)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_mov_result_src->type = OPERAND_REGISTER;
            asm_mov_result_src->operand.reg.reg_no = ASM_REG_RDX;

            asm_mov_result_dst->type = OPERAND_PSEUDO;
            asm_mov_result_dst->operand.pseudo.pseudo_name = ir_instruction_binary.destination->value.variable.identifier->name;

            asm_instruction_mov_result->instr.mov = (asm_instruction_mov_t){
                .src = asm_mov_result_src,
                .dst = asm_mov_result_dst};

            result.instructions[0] = asm_instruction_mov_first;
            result.instructions[1] = asm_instruction_cqo;
            result.instructions[2] = asm_instruction_idiv;
            result.instructions[3] = asm_instruction_mov_result;

            return result;
        }

        default:
        {
            asm_instruction_struct_t result = {
                .instructions = (asm_instruction_t **)allocate(2 * sizeof(asm_instruction_t *)),
                .instruction_count = 2,
            };

            if (!result.instructions)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_t *asm_instruction_mov_first = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
            if (!asm_instruction_mov_first)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_mov_first->type = INSTRUCTION_MOV;
            asm_instruction_mov_first->base.type = ASM_NODE_INSTRUCTION;

            asm_operand_t *asm_mov_first_src = (asm_operand_t *)allocate(sizeof(asm_operand_t));
            asm_operand_t *asm_mov_first_dst = (asm_operand_t *)allocate(sizeof(asm_operand_t));

            if (!asm_mov_first_src || !asm_mov_first_dst)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            switch (ir_instruction_binary.left->type)
            {
            case IR_VAL_CONSTANT_INT:
                asm_mov_first_src->type = OPERAND_IMMEDIATE;
                asm_mov_first_src->operand.immediate.value = ir_instruction_binary.left->value.constant_int;
                break;
            case IR_VAL_VARIABLE:
                asm_mov_first_src->type = OPERAND_PSEUDO;
                asm_mov_first_src->operand.pseudo.pseudo_name = ir_instruction_binary.left->value.variable.identifier->name;
                break;
            default:
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_mov_first_dst->type = OPERAND_PSEUDO;
            asm_mov_first_dst->operand.pseudo.pseudo_name = ir_instruction_binary.destination->value.variable.identifier->name;

            asm_instruction_mov_first->instr.mov = (asm_instruction_mov_t){
                .src = asm_mov_first_src,
                .dst = asm_mov_first_dst};

            asm_instruction_t *asm_instruction_binary = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
            if (!asm_instruction_binary)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_binary->type = INSTRUCTION_BINARY;
            asm_instruction_binary->base.type = ASM_NODE_INSTRUCTION;

            asm_binary_operator_t *asm_binary_op = (asm_binary_operator_t *)allocate(sizeof(asm_binary_operator_t));
            if (!asm_binary_op)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_operand_t *asm_binary_second_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
            if (!asm_binary_second_operand)
            {
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            switch (ir_instruction_binary.right->type)
            {
            case IR_VAL_CONSTANT_INT:
                asm_binary_second_operand->type = OPERAND_IMMEDIATE;
                asm_binary_second_operand->operand.immediate.value = ir_instruction_binary.right->value.constant_int;
                break;
            case IR_VAL_VARIABLE:
                asm_binary_second_operand->type = OPERAND_PSEUDO;
                asm_binary_second_operand->operand.pseudo.pseudo_name = ir_instruction_binary.right->value.variable.identifier->name;
                break;
            default:
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            switch (ir_binary_operator_type)
            {
            case IR_BINARY_ADD:
                asm_binary_op->binary_op = ASM_BINARY_ADD;
                break;
            case IR_BINARY_SUBTRACT:
                asm_binary_op->binary_op = ASM_BINARY_SUB;
                break;
            case IR_BINARY_MULTIPLY:
                asm_binary_op->binary_op = ASM_BINARY_MULT;
                break;
            case IR_BINARY_BITWISE_AND:
                asm_binary_op->binary_op = ASM_BINARY_BITWISE_AND;
                break;
            case IR_BINARY_BITWISE_OR:
                asm_binary_op->binary_op = ASM_BINARY_BITWISE_OR;
                break;
            case IR_BINARY_BITWISE_XOR:
                asm_binary_op->binary_op = ASM_BINARY_BITWISE_XOR;
                break;
            case IR_BINARY_BITWISE_LEFT_SHIFT:
                asm_binary_op->binary_op = ASM_BINARY_BITWISE_SHIFT_LEFT;
                break;
            case IR_BINARY_BITWISE_RIGHT_SHIFT:
                asm_binary_op->binary_op = ASM_BINARY_BITWISE_SHIFT_RIGHT;
                break;
            default:
                return NULL_INSTRUCTION_STRUCT_ASM;
            }

            asm_instruction_binary->instr.binary = (asm_instruction_binary_t){
                .binary_operator = asm_binary_op,
                .first_operand = asm_mov_first_dst, // use the mov destination as the first operand
                .second_operand = asm_binary_second_operand};

            result.instructions[0] = asm_instruction_mov_first;
            result.instructions[1] = asm_instruction_binary;

            return result;
        }
        }
    }
        return NULL_INSTRUCTION_STRUCT_ASM;
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

        for (size_t counter = 0; counter < asm_instruction_count; counter++)
        {
            asm_instruction_buffer[total_asm_instruction_count + counter] = asm_instructions[counter];
        }
        total_asm_instruction_count += asm_instruction_count;
        deallocate(asm_instructions);
    }

    asm_instruction_t **total_asm_instructions = (asm_instruction_t **)allocate(sizeof(asm_instruction_t *) * total_asm_instruction_count);
    if (!total_asm_instructions)
    {
        return NULL;
    }
    for (size_t counter = 0; counter < total_asm_instruction_count; counter++)
    {
        total_asm_instructions[counter] = asm_instruction_buffer[counter];
    }
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
