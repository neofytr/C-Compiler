#ifndef CD739C29_7369_4BA1_8D17_C870A325E960
#define CD739C29_7369_4BA1_8D17_C870A325E960

#include "assembly_ast.h"
#include "ast.h"
#include "allocator.h"

typedef struct generator_ generator_t;

struct generator_
{

} generator_t;

asm_program_t *handle_program(program_t *program);
asm_function_t *handle_function(function_def_t *function);
asm_instruction_t **handle_statement(statement_t *statement);
asm_identifier_t *handle_identifier(identifier_t *identifier);
asm_operand_t *handle_expression(expression_t *expression);

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

    asm_identifier_t *asm_identifier = handle_identifier(name);
    if (!asm_identifier)
    {
        return NULL;
    }
    asm_identifier->base.parent = &(asm_function->base);

    asm_function->name = asm_identifier;

    asm_instruction_t **asm_instructions = handle_statement(statement);
    if (!asm_instructions && !(*asm_instructions))
    {
        return NULL;
    }

    switch (statement->stmt_type)
    {
    case STMT_RETURN:
        asm_function->instruction_count = 2;
        for (size_t counter = 0; counter < 2; counter++)
        {
            (*asm_instructions)[counter].base.parent = &(asm_function->base);
        }
        break;

    default:
        break;
    }

    asm_function->instructions = asm_instructions;
    asm_function->base.type = ASM_NODE_FUNCTION;
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

    asm_program->base.parent = NULL;
    asm_program->base.type = ASM_NODE_FUNCTION;

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
