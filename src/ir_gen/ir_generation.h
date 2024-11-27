#ifndef BF4D96C0_1C97_4F20_AB1B_168AC361CE92
#define BF4D96C0_1C97_4F20_AB1B_168AC361CE92

#include "../ast/IR/ir_ast.h"
#include "../ast/source/ast.h"
#include "../allocator/allocator.h"

#define NULL_INSTRUCTION_STRUCT ((ir_instruction_struct_t){0})

ir_function_t *handle_function(function_def_t *source_function);
ir_instruction_struct_t handle_statement(statement_t *source_statement);
ir_instruction_struct_t handle_expression(expression_t *source_expression);
ir_program_t *handle_program(program_t *source_program);
ir_identifier_t *handle_identifier(identifier_t *source_identifier);

ir_instruction_struct_t handle_statement(statement_t *source_statement)
{
    if (!source_statement)
    {
        return NULL_INSTRUCTION_STRUCT;
    }

    statement_type_t source_statement_type = source_statement->stmt_type;

    switch (source_statement_type)
    {
    case STMT_RETURN:
    {

        break;
    }
    default:
    {
        break;
    }
    }

    return NULL_INSTRUCTION_STRUCT;
}

ir_identifier_t *handle_identifier(identifier_t *source_identifier)
{
    ir_identifier_t *ir_identifier = (ir_identifier_t *)allocate(sizeof(ir_identifier_t));
    if (!ir_identifier)
    {
        return NULL;
    }
    ir_identifier->base.type = IR_NODE_IDENTIFIER;

    char *name = source_identifier->name;
    if (!name)
    {
        return NULL;
    }
    ir_identifier->name = source_identifier->name;

    return ir_identifier;
}

ir_program_t *handle_program(program_t *source_program)
{
    if (!source_program)
    {
        return NULL;
    }

    ir_program_t *ir_program = (ir_program_t *)allocate(sizeof(ir_program_t));
    if (!ir_program)
    {
        return NULL;
    }

    ir_program->base.type = IR_NODE_PROGRAM;
    ir_program->base.parent = NULL;
    ir_function_t *ir_function = handle_function(source_program->function);

    if (!ir_function)
    {
        return NULL;
    }

    ir_function->base.parent = &(ir_program->base);
    ir_program->function = ir_function;

    return ir_program;
}

ir_function_t *handle_function(function_def_t *source_function)
{
    if (!source_function)
    {
        return NULL;
    }

    ir_function_t *ir_function = (ir_function_t *)allocate(sizeof(ir_function_t));
    if (!ir_function)
    {
        return NULL;
    }

    ir_function->base.type = IR_NODE_FUNCTION;
    ir_identifier_t *ir_identifier = handle_identifier(source_function->name);

    if (!ir_identifier)
    {
        return NULL;
    }

    ir_identifier->base.parent = &(ir_function->base);

    ir_function->name = ir_identifier;

    ir_instruction_struct_t ir_instruction_struct = handle_statement(source_function->body);
    ir_instruction_t **ir_instructions = ir_instruction_struct.instructions;
    if (!ir_instructions)
    {
        return NULL;
    }

    size_t ir_instruction_count = ir_instruction_struct.instruction_count;

    for (size_t counter = 0; counter < ir_instruction_count; counter++)
    {
        ir_instruction_t *ir_instruction = ir_instructions[counter];
        if (!ir_instruction)
        {
            return NULL;
        }

        ir_instruction->base.parent = &(ir_function->base);
    }

    ir_function->instruction_count = ir_instruction_count;
    ir_function->body = ir_instructions;

    return ir_function;
}

#endif /* BF4D96C0_1C97_4F20_AB1B_168AC361CE92 */
