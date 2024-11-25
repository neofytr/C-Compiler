#ifndef CD739C29_7369_4BA1_8D17_C870A325E960
#define CD739C29_7369_4BA1_8D17_C870A325E960

#include "assembly_ast.h"
#include "ast.h"
#include "allocator.h"

asm_program_t *generate_assembly_ast(program_t *program);
asm_program_t *handle_program(program_t *program);
asm_function_t *handle_function(function_def_t *function);
asm_instruction_t **handle_statement(statement_t *statement);
asm_identifier_t *handle_identifier(identifier_t *identifier);
asm_operand_t *handle_expression(expression_t *expression);

asm_program_t *generate_assembly_ast(program_t *program)
{
    return handle_program(program);
}

asm_identifier_t *handle_identifier(identifier_t *identifier)
{
    if (!identifier || !identifier->name)
    {
        return NULL;
    }

    asm_identifier_t *asm_identifier = (asm_identifier_t *)allocate(sizeof(asm_identifier_t));
    if (!asm_identifier)
    {
        return NULL;
    }

    asm_identifier->base.type = ASM_NODE_IDENTIFIER;

    if (!identifier->name)
    {
        return NULL;
    }
    asm_identifier->name = identifier->name;

    return asm_identifier;
}

asm_operand_t *handle_expression(expression_t *expression)
{
    if (!expression)
    {
        return NULL;
    }

    asm_operand_t *asm_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
    if (!asm_operand)
    {
        return NULL;
    }

    switch (expression->expr_type)
    {
    case EXPR_CONSTANT_INT:
        asm_operand->type = OPERAND_IMMEDIATE;
        asm_operand->base.type = ASM_NODE_OPERAND_IMM;

        asm_immediate_t *immediate = (asm_immediate_t *)allocate(sizeof(asm_immediate_t));
        if (!immediate)
        {
            return NULL;
        }
        immediate->value = expression->value.constant_int;
        asm_operand->operand.immediate = immediate;
        break;

    default:
        return NULL;
    }

    return asm_operand;
}

asm_instruction_t **handle_statement(statement_t *statement)
{
    if (!statement)
    {
        return NULL;
    }

    asm_instruction_t **instructions = (asm_instruction_t **)allocate(sizeof(asm_instruction_t *) * 2);
    if (!instructions)
    {
        return NULL;
    }

    switch (statement->stmt_type)
    {
    case STMT_RETURN:
    {
        asm_instruction_t *mov_inst = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
        if (!mov_inst)
        {
            return NULL;
        }
        mov_inst->base.type = ASM_NODE_INSTRUCTION_MOV;
        mov_inst->type = INSTRUCTION_MOV;

        asm_instruction_mov_t *mov = (asm_instruction_mov_t *)allocate(sizeof(asm_instruction_mov_t));
        if (!mov)
        {
            return NULL;
        }

        mov_inst->instr.mov = mov;

        asm_operand_t *src_operand = handle_expression(statement->value.return_expr);
        if (!src_operand)
        {
            return NULL;
        }
        src_operand->base.parent = &mov_inst->base;
        mov->src = src_operand;

        asm_operand_t *dst_operand = (asm_operand_t *)allocate(sizeof(asm_operand_t));
        if (!dst_operand)
        {
            return NULL;
        }
        dst_operand->base.type = ASM_NODE_OPERAND_REGISTER;
        dst_operand->type = OPERAND_REGISTER;

        asm_register_t *reg = (asm_register_t *)allocate(sizeof(asm_register_t));
        if (!reg)
        {
            return NULL;
        }
        reg->reg_num = 0; // assuming 0 is eax
        dst_operand->operand.reg = reg;
        dst_operand->base.parent = &mov_inst->base;
        mov->dst = dst_operand;

        instructions[0] = mov_inst;

        asm_instruction_t *ret_inst = (asm_instruction_t *)allocate(sizeof(asm_instruction_t));
        if (!ret_inst)
        {
            return NULL;
        }
        ret_inst->base.type = ASM_NODE_INSTRUCTION_RET;
        ret_inst->type = INSTRUCTION_RET;

        asm_instruction_ret_t *ret = (asm_instruction_ret_t *)allocate(sizeof(asm_instruction_ret_t));
        if (!ret)
        {
            return NULL;
        }

        ret_inst->instr.ret = ret;
        instructions[1] = ret_inst;
        break;
    }

    default:
        return NULL;
    }

    return instructions;
}

asm_function_t *handle_function(function_def_t *function)
{
    if (!function)
    {
        return NULL;
    }

    identifier_t *name = function->name;
    statement_t *statement = function->body;

    if (!name || !statement)
    {
        return NULL;
    }

    asm_function_t *asm_function = (asm_function_t *)allocate(sizeof(asm_function_t));
    if (!asm_function)
    {
        return NULL;
    }

    asm_function->base.type = ASM_NODE_FUNCTION;

    asm_identifier_t *asm_identifier = handle_identifier(name);
    if (!asm_identifier)
    {
        return NULL;
    }
    asm_identifier->base.parent = &(asm_function->base);
    asm_function->name = asm_identifier;

    asm_instruction_t **asm_instructions = handle_statement(statement);
    if (!asm_instructions)
    {
        return NULL;
    }

    switch (statement->stmt_type)
    {
    case STMT_RETURN:
        asm_function->instruction_count = 2;
        for (size_t counter = 0; counter < 2; counter++)
        {
            asm_instructions[counter]->base.parent = &(asm_function->base);
        }
        break;

    default:
        return NULL;
    }

    asm_function->instructions = asm_instructions;
    return asm_function;
}

asm_program_t *handle_program(program_t *program)
{
    if (!program)
    {
        return NULL;
    }

    function_def_t *function = program->function;
    if (!function)
    {
        return NULL;
    }

    asm_program_t *asm_program = (asm_program_t *)allocate(sizeof(asm_program_t));
    if (!asm_program)
    {
        return NULL;
    }

    asm_program->base.type = ASM_NODE_PROGRAM;
    asm_program->base.parent = NULL;

    asm_function_t *asm_function = handle_function(function);
    if (!asm_function)
    {
        return NULL;
    }
    asm_function->base.parent = &(asm_program->base);
    asm_program->function = asm_function;

    return asm_program;
}

#endif /* CD739C29_7369_4BA1_8D17_C870A325E960 */