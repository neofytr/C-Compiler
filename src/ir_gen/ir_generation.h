#ifndef BF4D96C0_1C97_4F20_AB1B_168AC361CE92
#define BF4D96C0_1C97_4F20_AB1B_168AC361CE92

#include <stdio.h>
#include "../ast/IR/ir_ast.h"
#include "../ast/source/ast.h"
#include "../allocator/allocator.h"

#define NULL_INSTRUCTION_STRUCT ((ir_instruction_struct_t){0})
#define MAX_TEMP_VAR_LENGTH 8
#define MAX_LABEL_LENGTH 10

#define DEBUG_NULL_RETURN(func_name) \
    fprintf(stderr, "NULL return in %s at line %d\n", func_name, __LINE__)

size_t temp_var_count = 0;
size_t false_label_count = 0;
size_t end_label_count = 0;

ir_program_t *conv_ast_to_ir(program_t *source_program);
ir_function_t *ir_handle_function(function_def_t *source_function);
ir_instruction_struct_t ir_handle_statement(statement_t *source_statement);
ir_instruction_struct_t ir_handle_expression(expression_t *source_expression);
ir_program_t *ir_handle_program(program_t *source_program);
ir_identifier_t *ir_handle_identifier(identifier_t *source_identifier);
ir_unary_operator_t *ir_handle_unary_operator(unary_operator_t *source_unary_operator);
char *new_temp_var_name();

ir_binary_operator_t *ir_handle_binary_operator(binary_operator_t *source_binary_operator)
{
    if (!source_binary_operator)
    {
        DEBUG_NULL_RETURN("ir_handle_binary_operator");
        return NULL;
    }

    ir_binary_operator_t *ir_binary_operator = (ir_binary_operator_t *)allocate(sizeof(ir_binary_operator_t));
    if (!ir_binary_operator)
    {
        DEBUG_NULL_RETURN("ir_handle_binary_operator");
        return NULL;
    }

    ir_binary_operator->base.type = IR_NODE_BINARY_OPERATOR;

    switch (source_binary_operator->binary_operator)
    {
    case BINARY_MUL:
        ir_binary_operator->operator= IR_BINARY_MULTIPLY;
        break;
    case BINARY_DIV:
        ir_binary_operator->operator= IR_BINARY_DIVIDE;
        break;
    case BINARY_REM:
        ir_binary_operator->operator= IR_BINARY_REM;
        break;

    case BINARY_ADD:
        ir_binary_operator->operator= IR_BINARY_ADD;
        break;
    case BINARY_SUB:
        ir_binary_operator->operator= IR_BINARY_SUBTRACT;
        break;

    case BINARY_LEFT_SHIFT:
        ir_binary_operator->operator= IR_BINARY_BITWISE_LEFT_SHIFT;
        break;
    case BINARY_RIGHT_SHIFT:
        ir_binary_operator->operator= IR_BINARY_BITWISE_RIGHT_SHIFT;
        break;

    case BINARY_LESS_THAN:
        ir_binary_operator->operator= IR_BINARY_LESS_THAN;
        break;
    case BINARY_LESS_THAN_EQUAL:
        ir_binary_operator->operator= IR_BINARY_LESS_THAN_EQUAL;
        break;
    case BINARY_GREATER_THAN:
        ir_binary_operator->operator= IR_BINARY_GREATER_THAN;
        break;
    case BINARY_GREATER_THAN_EQUAL:
        ir_binary_operator->operator= IR_BINARY_GREATER_THAN_EQUAL;
        break;

    case BINARY_EQUAL:
        ir_binary_operator->operator= IR_BINARY_EQUAL;
        break;
    case BINARY_NOT_EQUAL:
        ir_binary_operator->operator= IR_BINARY_NOT_EQUAL;
        break;

    case BINARY_BITWISE_AND:
        ir_binary_operator->operator= IR_BINARY_BITWISE_AND;
        break;
    case BINARY_BITWISE_XOR:
        ir_binary_operator->operator= IR_BINARY_BITWISE_XOR;
        break;
    case BINARY_BITWISE_OR:
        ir_binary_operator->operator= IR_BINARY_BITWISE_OR;
        break;

    case BINARY_LOGICAL_AND:
        ir_binary_operator->operator= IR_BINARY_LOGICAL_AND;
        break;
    case BINARY_LOGICAL_OR:
        ir_binary_operator->operator= IR_BINARY_LOGICAL_OR;
        break;

    default:
        DEBUG_NULL_RETURN("ir_handle_binary_operator");
        deallocate(ir_binary_operator);
        return NULL;
    }

    return ir_binary_operator;
}

ir_program_t *conv_ast_to_ir(program_t *source_program)
{
    return ir_handle_program(source_program);
}

char *new_temp_var_name()
{
    char *temp_var_name = (char *)allocate(sizeof(char) * MAX_TEMP_VAR_LENGTH);
    if (!temp_var_name)
    {
        DEBUG_NULL_RETURN("new_temp_var_name");
        return NULL;
    }

    int r = snprintf(temp_var_name, MAX_TEMP_VAR_LENGTH, ".tmp%zu", temp_var_count++);
    if (r > MAX_TEMP_VAR_LENGTH || r < 4)
    {
        deallocate(temp_var_name);
        return NULL;
    }

    return temp_var_name;
}

char *new_false_label_name()
{
    char *temp_label_name = (char *)allocate(sizeof(char) * MAX_LABEL_LENGTH);
    if (!temp_label_name)
    {
        return NULL;
    }

    int r = snprintf(temp_label_name, MAX_LABEL_LENGTH, "_false%zu", false_label_count++);
    if (r > MAX_TEMP_VAR_LENGTH || r < 6)
    {
        deallocate(temp_label_name);
        return NULL;
    }

    return temp_label_name;
}

char *new_end_label_name()
{
    char *temp_label_name = (char *)allocate(sizeof(char) * MAX_LABEL_LENGTH);
    if (!temp_label_name)
    {
        return NULL;
    }

    int r = snprintf(temp_label_name, MAX_LABEL_LENGTH, "_end%zu", end_label_count++);
    if (r > MAX_TEMP_VAR_LENGTH || r < 4)
    {
        deallocate(temp_label_name);
        return NULL;
    }

    return temp_label_name;
}

ir_unary_operator_t *ir_handle_unary_operator(unary_operator_t *source_unary_operator)
{
    if (!source_unary_operator)
    {
        DEBUG_NULL_RETURN("ir_handle_unary_operator");
        return NULL;
    }

    ir_unary_operator_t *ir_unary_operator = (ir_unary_operator_t *)allocate(sizeof(ir_unary_operator_t));
    if (!ir_unary_operator)
    {
        DEBUG_NULL_RETURN("ir_handle_unary_operator");
        return NULL;
    }

    ir_unary_operator->base.type = IR_NODE_UNARY_OPERATOR;

    switch (source_unary_operator->op)
    {
    case BITWISE_COMPLEMENT:
        ir_unary_operator->unary_op = IR_UNARY_BITWISE_COMPLEMENT;
        break;
    case NEGATE:
        ir_unary_operator->unary_op = IR_UNARY_NEGATE;
        break;
    case NOT:
        ir_unary_operator->unary_op = IR_UNARY_NOT;
        break;
    default:
        DEBUG_NULL_RETURN("ir_handle_unary_operator");
        return NULL;
    }

    return ir_unary_operator;
}

ir_instruction_struct_t ir_handle_expression(expression_t *source_expression)
{
    if (!source_expression)
    {
        DEBUG_NULL_RETURN("ir_handle_expression");
        return NULL_INSTRUCTION_STRUCT;
    }

    expression_type_t source_expression_type = source_expression->expr_type;

    switch (source_expression_type)
    {
    case EXPR_CONSTANT_INT:
    {
        ir_value_t *ir_constant_value = (ir_value_t *)allocate(sizeof(ir_value_t));
        if (!ir_constant_value)
        {
            DEBUG_NULL_RETURN("ir_handle_expression");
            return NULL_INSTRUCTION_STRUCT;
        }

        ir_constant_value->base.type = IR_NODE_VALUE;
        ir_constant_value->type = IR_VAL_CONSTANT_INT;
        ir_constant_value->value.constant_int = source_expression->value.constant_int;

        return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
    }
    case EXPR_BINARY:
    {
        expression_t *source_left_expr = source_expression->value.binary.left_expr;
        expression_t *source_right_expr = source_expression->value.binary.right_expr;
        binary_operator_t *source_binary_operator = source_expression->value.binary.op;

        if (!source_left_expr || !source_right_expr || !source_binary_operator)
        {
            fprintf(stderr, "Invalid binary expression components\n");
            return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
        }

        ir_instruction_struct_t ir_left_instruction_struct = ir_handle_expression(source_left_expr);
        ir_instruction_struct_t ir_right_instruction_struct = ir_handle_expression(source_right_expr);
        ir_value_t *ir_left_value = NULL;
        ir_value_t *ir_right_value = NULL;

        if (ir_left_instruction_struct.instruction_count > 0)
        {
            ir_left_value = allocate(sizeof(ir_value_t));
            ir_left_value->base.type = IR_NODE_VALUE;
            ir_left_value->base.parent = NULL;
            ir_left_value->type = IR_VAL_VARIABLE;

            ir_instruction_t *last_instruction = ir_left_instruction_struct.instructions[ir_left_instruction_struct.instruction_count - 1];
            switch (last_instruction->type)
            {
            case IR_INSTR_BINARY:
                ir_left_value->value.variable.identifier =
                    last_instruction->instruction.binary_instr.destination->value.variable.identifier;
                break;
            case IR_INSTR_UNARY:
                ir_left_value->value.variable.identifier =
                    last_instruction->instruction.unary_instr.destination->value.variable.identifier;
                break;
            case IR_INSTR_RETURN:
                ir_left_value->value.variable.identifier =
                    last_instruction->instruction.return_instr.value->value.variable.identifier;
                break;
            default:
                ir_left_value = NULL;
                break;
            }
        }

        if (ir_right_instruction_struct.instruction_count > 0)
        {
            ir_right_value = allocate(sizeof(ir_value_t));
            ir_right_value->base.type = IR_NODE_VALUE;
            ir_right_value->base.parent = NULL;
            ir_right_value->type = IR_VAL_VARIABLE;

            ir_instruction_t *last_instruction = ir_right_instruction_struct.instructions[ir_right_instruction_struct.instruction_count - 1];
            switch (last_instruction->type)
            {
            case IR_INSTR_BINARY:
                ir_right_value->value.variable.identifier =
                    last_instruction->instruction.binary_instr.destination->value.variable.identifier;
                break;
            case IR_INSTR_UNARY:
                ir_right_value->value.variable.identifier =
                    last_instruction->instruction.unary_instr.destination->value.variable.identifier;
                break;
            case IR_INSTR_RETURN:
                ir_right_value->value.variable.identifier =
                    last_instruction->instruction.return_instr.value->value.variable.identifier;
                break;
            default:
                ir_right_value = NULL;
                break;
            }
        }

        if (!ir_left_value)
        {
            ir_left_value = allocate(sizeof(ir_value_t));
            ir_left_value->base.type = IR_NODE_VALUE;
            ir_left_value->base.parent = NULL;
            ir_left_value->type = IR_VAL_CONSTANT_INT;
            ir_left_value->value.constant_int = source_left_expr->value.constant_int;
        }

        if (!ir_right_value)
        {
            ir_right_value = allocate(sizeof(ir_value_t));
            ir_right_value->base.type = IR_NODE_VALUE;
            ir_right_value->base.parent = NULL;
            ir_right_value->type = IR_VAL_CONSTANT_INT;
            ir_right_value->value.constant_int = source_right_expr->value.constant_int;
        }

        ir_binary_operator_t *ir_binary_operator = ir_handle_binary_operator(source_binary_operator);
        if (!ir_binary_operator)
        {
            fprintf(stderr, "Failed to create binary operator\n");
            if (ir_left_instruction_struct.instructions)
                deallocate(ir_left_instruction_struct.instructions);
            if (ir_right_instruction_struct.instructions)
                deallocate(ir_right_instruction_struct.instructions);
            return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
        }

        ir_value_t *ir_destination_value = allocate(sizeof(ir_value_t));
        ir_destination_value->base.type = IR_NODE_VALUE;
        ir_destination_value->type = IR_VAL_VARIABLE;

        char *temp_var_name = new_temp_var_name();
        if (!temp_var_name)
        {
            return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
        }
        ir_destination_value->value.variable.identifier = allocate(sizeof(ir_identifier_t));
        ir_destination_value->value.variable.identifier->base.type = IR_NODE_IDENTIFIER;
        ir_destination_value->value.variable.identifier->name = temp_var_name;

        ir_instruction_t *ir_binary_instruction = allocate(sizeof(ir_instruction_t));
        ir_binary_instruction->base.type = IR_NODE_INSTRUCTION;
        ir_binary_instruction->type = IR_INSTR_BINARY;
        ir_binary_instruction->instruction.binary_instr.binary_operator = ir_binary_operator;
        ir_binary_instruction->instruction.binary_instr.left = ir_left_value;
        ir_binary_instruction->instruction.binary_instr.right = ir_right_value;
        ir_binary_instruction->instruction.binary_instr.destination = ir_destination_value;

        size_t total_instruction_count = 0;
        if (ir_binary_operator->operator== IR_BINARY_LOGICAL_AND)
        {
            total_instruction_count = ir_left_instruction_struct.instruction_count +
                                      ir_right_instruction_struct.instruction_count + 7;
        }
        else if (ir_binary_operator->operator== IR_BINARY_LOGICAL_OR)
        {
            total_instruction_count =
                ir_left_instruction_struct.instruction_count +
                ir_right_instruction_struct.instruction_count + 7;
        }
        else
        {
            total_instruction_count =
                ir_left_instruction_struct.instruction_count +
                ir_right_instruction_struct.instruction_count + 1;
        }

        ir_instruction_t **combined_instructions = allocate(
            sizeof(ir_instruction_t *) * total_instruction_count);

        size_t current_index = 0;
        for (size_t i = 0; i < ir_left_instruction_struct.instruction_count; i++)
        {
            combined_instructions[current_index++] =
                ir_left_instruction_struct.instructions[i];
        }

        if (ir_binary_operator->operator== IR_BINARY_LOGICAL_AND)
        {
            ir_value_t *result_var = (ir_value_t *)allocate(sizeof(ir_value_t));
            if (!result_var)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            result_var->base.type = IR_NODE_VALUE;
            result_var->type = IR_VAL_VARIABLE;

            ir_identifier_t *result_identifier = (ir_identifier_t *)allocate(sizeof(ir_identifier_t));
            if (!result_identifier)
            {
                deallocate(result_var);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            result_identifier->base.type = IR_NODE_IDENTIFIER;
            result_identifier->base.parent = &result_var->base;
            result_identifier->name = new_temp_var_name();
            if (!result_identifier->name)
            {
                deallocate(result_var);
                deallocate(result_identifier);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            result_var->value.variable.identifier = result_identifier;

            ir_identifier_t *false_label = (ir_identifier_t *)allocate(sizeof(ir_identifier_t));
            if (!false_label)
            {
                deallocate(result_var);
                deallocate(result_identifier);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            false_label->base.type = IR_NODE_IDENTIFIER;
            false_label->name = new_false_label_name();
            if (!false_label->name)
            {
                deallocate(result_var);
                deallocate(result_identifier);
                deallocate(false_label);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }

            ir_instruction_t *first_jz_instruction = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!first_jz_instruction)
            {
                deallocate(result_var);
                deallocate(result_identifier);
                deallocate(false_label);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            first_jz_instruction->base.type = IR_NODE_INSTRUCTION;
            first_jz_instruction->type = IR_INSTR_JUMP_IF_ZERO;
            false_label->base.parent = &first_jz_instruction->base;

            ir_identifier_t *first_jz_target = (ir_identifier_t *)allocate(sizeof(ir_identifier_t));
            if (!first_jz_target)
            {
                deallocate(result_var);
                deallocate(result_identifier);
                deallocate(false_label);
                deallocate(first_jz_instruction);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            first_jz_target->base.type = IR_NODE_IDENTIFIER;
            first_jz_target->base.parent = &first_jz_instruction->base;
            first_jz_target->name = strdup(false_label->name); // Assuming strdup is available
            if (!first_jz_target->name)
            {
                deallocate(result_var);
                deallocate(result_identifier);
                deallocate(false_label);
                deallocate(first_jz_instruction);
                deallocate(first_jz_target);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }

            first_jz_instruction->instruction.jz_instr.condition = ir_left_value;
            first_jz_instruction->instruction.jz_instr.target = first_jz_target;
            ir_left_value->base.parent = &first_jz_instruction->base;

            combined_instructions[current_index++] = first_jz_instruction;

            for (size_t i = 0; i < ir_right_instruction_struct.instruction_count; i++)
            {
                combined_instructions[current_index++] = ir_right_instruction_struct.instructions[i];
                ir_right_instruction_struct.instructions[i]->base.parent = &result_var->base;
            }

            ir_instruction_t *second_jz_instruction = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!second_jz_instruction)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            second_jz_instruction->base.type = IR_NODE_INSTRUCTION;
            second_jz_instruction->type = IR_INSTR_JUMP_IF_ZERO;

            ir_identifier_t *second_jz_target = (ir_identifier_t *)allocate(sizeof(ir_identifier_t));
            if (!second_jz_target)
            {
                deallocate(second_jz_instruction);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            second_jz_target->base.type = IR_NODE_IDENTIFIER;
            second_jz_target->base.parent = &second_jz_instruction->base;
            second_jz_target->name = strdup(false_label->name);
            if (!second_jz_target->name)
            {
                deallocate(second_jz_instruction);
                deallocate(second_jz_target);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }

            second_jz_instruction->instruction.jz_instr.condition = ir_right_value;
            second_jz_instruction->instruction.jz_instr.target = second_jz_target;
            ir_right_value->base.parent = &second_jz_instruction->base;

            combined_instructions[current_index++] = second_jz_instruction;

            ir_instruction_t *copy_true = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!copy_true)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            copy_true->base.type = IR_NODE_INSTRUCTION;
            copy_true->type = IR_INSTR_COPY;

            ir_value_t *true_value = (ir_value_t *)allocate(sizeof(ir_value_t));
            if (!true_value)
            {
                deallocate(copy_true);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            true_value->base.type = IR_NODE_VALUE;
            true_value->base.parent = &copy_true->base;
            true_value->type = IR_VAL_CONSTANT_INT;
            true_value->value.constant_int = 1;

            copy_true->instruction.copy_instr.destination = result_var;
            copy_true->instruction.copy_instr.source = true_value;
            result_var->base.parent = &copy_true->base;

            combined_instructions[current_index++] = copy_true;

            ir_identifier_t *end_label = (ir_identifier_t *)allocate(sizeof(ir_identifier_t));
            if (!end_label)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            end_label->base.type = IR_NODE_IDENTIFIER;
            end_label->name = new_end_label_name();
            if (!end_label->name)
            {
                deallocate(end_label);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }

            ir_instruction_t *unconditional_jmp = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!unconditional_jmp)
            {
                deallocate(end_label);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            unconditional_jmp->base.type = IR_NODE_INSTRUCTION;
            unconditional_jmp->type = IR_INSTR_JUMP;
            end_label->base.parent = &unconditional_jmp->base;

            ir_identifier_t *jmp_target = (ir_identifier_t *)allocate(sizeof(ir_identifier_t));
            if (!jmp_target)
            {
                deallocate(unconditional_jmp);
                deallocate(end_label);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            jmp_target->base.type = IR_NODE_IDENTIFIER;
            jmp_target->base.parent = &unconditional_jmp->base;
            jmp_target->name = strdup(end_label->name);
            if (!jmp_target->name)
            {
                deallocate(unconditional_jmp);
                deallocate(end_label);
                deallocate(jmp_target);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }

            unconditional_jmp->instruction.jmp_instr.target = jmp_target;
            combined_instructions[current_index++] = unconditional_jmp;

            ir_instruction_t *false_label_instr = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!false_label_instr)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            false_label_instr->base.type = IR_NODE_INSTRUCTION;
            false_label_instr->type = IR_INSTR_LABEL;
            false_label_instr->instruction.label_instr.identifier = false_label;
            false_label->base.parent = &false_label_instr->base;

            combined_instructions[current_index++] = false_label_instr;

            ir_instruction_t *copy_false = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!copy_false)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            copy_false->base.type = IR_NODE_INSTRUCTION;
            copy_false->type = IR_INSTR_COPY;

            ir_value_t *false_value = (ir_value_t *)allocate(sizeof(ir_value_t));
            if (!false_value)
            {
                deallocate(copy_false);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            false_value->base.type = IR_NODE_VALUE;
            false_value->base.parent = &copy_false->base;
            false_value->type = IR_VAL_CONSTANT_INT;
            false_value->value.constant_int = 0;

            copy_false->instruction.copy_instr.destination = result_var;
            copy_false->instruction.copy_instr.source = false_value;
            result_var->base.parent = &copy_false->base;

            combined_instructions[current_index++] = copy_false;

            ir_instruction_t *end_label_instr = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!end_label_instr)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            end_label_instr->base.type = IR_NODE_INSTRUCTION;
            end_label_instr->type = IR_INSTR_LABEL;
            end_label_instr->instruction.label_instr.identifier = end_label;
            end_label->base.parent = &end_label_instr->base;

            combined_instructions[current_index++] = end_label_instr;
        }
        else if (ir_binary_operator->operator== IR_BINARY_LOGICAL_OR)
        {
            ir_instruction_t *jnz_instruction = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!jnz_instruction)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            jnz_instruction->base.type = IR_NODE_INSTRUCTION;
            jnz_instruction->type = IR_INSTR_JUMP_IF_NOT_ZERO;

            ir_identifier_t *true_label = (ir_identifier_t *)allocate(sizeof(ir_identifier_t));
            if (!true_label)
            {
                deallocate(jnz_instruction);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            true_label->base.type = IR_NODE_IDENTIFIER;
            true_label->base.parent = &jnz_instruction->base;
            true_label->name = new_false_label_name();
            if (!true_label->name)
            {
                deallocate(jnz_instruction);
                deallocate(true_label);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }

            jnz_instruction->instruction.jnz_instr.condition = ir_left_value;
            jnz_instruction->instruction.jnz_instr.target = true_label;
            ir_left_value->base.parent = &jnz_instruction->base;

            combined_instructions[current_index++] = jnz_instruction;

            for (size_t i = 0; i < ir_right_instruction_struct.instruction_count; i++)
            {
                combined_instructions[current_index++] = ir_right_instruction_struct.instructions[i];
                ir_right_instruction_struct.instructions[i]->base.parent = &jnz_instruction->base;
            }

            ir_instruction_t *jnz_instruction2 = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!jnz_instruction2)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            jnz_instruction2->base.type = IR_NODE_INSTRUCTION;
            jnz_instruction2->type = IR_INSTR_JUMP_IF_NOT_ZERO;
            jnz_instruction2->instruction.jnz_instr.condition = ir_right_value;
            jnz_instruction2->instruction.jnz_instr.target = true_label;
            ir_right_value->base.parent = &jnz_instruction2->base;

            combined_instructions[current_index++] = jnz_instruction2;

            ir_instruction_t *copy_zero = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!copy_zero)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            copy_zero->base.type = IR_NODE_INSTRUCTION;
            copy_zero->type = IR_INSTR_COPY;

            ir_value_t *zero_value = (ir_value_t *)allocate(sizeof(ir_value_t));
            if (!zero_value)
            {
                deallocate(copy_zero);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            zero_value->base.type = IR_NODE_VALUE;
            zero_value->base.parent = &copy_zero->base;
            zero_value->type = IR_VAL_CONSTANT_INT;
            zero_value->value.constant_int = 0;

            copy_zero->instruction.copy_instr.source = zero_value;
            copy_zero->instruction.copy_instr.destination = ir_destination_value;
            ir_destination_value->base.parent = &copy_zero->base;

            combined_instructions[current_index++] = copy_zero;

            ir_instruction_t *jmp_end = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!jmp_end)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            jmp_end->base.type = IR_NODE_INSTRUCTION;
            jmp_end->type = IR_INSTR_JUMP;

            ir_identifier_t *end_label = (ir_identifier_t *)allocate(sizeof(ir_identifier_t));
            if (!end_label)
            {
                deallocate(jmp_end);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            end_label->base.type = IR_NODE_IDENTIFIER;
            end_label->base.parent = &jmp_end->base;
            end_label->name = new_end_label_name();
            if (!end_label->name)
            {
                deallocate(jmp_end);
                deallocate(end_label);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }

            jmp_end->instruction.jmp_instr.target = end_label;
            combined_instructions[current_index++] = jmp_end;

            ir_instruction_t *true_label_instr = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!true_label_instr)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            true_label_instr->base.type = IR_NODE_INSTRUCTION;
            true_label_instr->type = IR_INSTR_LABEL;
            true_label_instr->instruction.label_instr.identifier = true_label;
            true_label->base.parent = &true_label_instr->base;

            combined_instructions[current_index++] = true_label_instr;

            ir_instruction_t *copy_one = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!copy_one)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            copy_one->base.type = IR_NODE_INSTRUCTION;
            copy_one->type = IR_INSTR_COPY;

            ir_value_t *one_value = (ir_value_t *)allocate(sizeof(ir_value_t));
            if (!one_value)
            {
                deallocate(copy_one);
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            one_value->base.type = IR_NODE_VALUE;
            one_value->base.parent = &copy_one->base;
            one_value->type = IR_VAL_CONSTANT_INT;
            one_value->value.constant_int = 1;

            copy_one->instruction.copy_instr.source = one_value;
            copy_one->instruction.copy_instr.destination = ir_destination_value;
            ir_destination_value->base.parent = &copy_one->base;

            combined_instructions[current_index++] = copy_one;

            ir_instruction_t *end_label_instr = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
            if (!end_label_instr)
            {
                return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
            }
            end_label_instr->base.type = IR_NODE_INSTRUCTION;
            end_label_instr->type = IR_INSTR_LABEL;
            end_label_instr->instruction.label_instr.identifier = end_label;
            end_label->base.parent = &end_label_instr->base;

            combined_instructions[current_index++] = end_label_instr;
        }
        else
        {
            for (size_t i = 0; i < ir_right_instruction_struct.instruction_count; i++)
            {
                combined_instructions[current_index++] = ir_right_instruction_struct.instructions[i];
            }

            combined_instructions[current_index++] = ir_binary_instruction;
        }

        if (ir_left_instruction_struct.instructions)
            deallocate(ir_left_instruction_struct.instructions);
        if (ir_right_instruction_struct.instructions)
            deallocate(ir_right_instruction_struct.instructions);

        return (ir_instruction_struct_t){
            .instructions = combined_instructions,
            .instruction_count = total_instruction_count};
    }
    case EXPR_UNARY:
    {
        unary_t source_unary = source_expression->value.unary;
        unary_operator_t *source_unary_operator = source_unary.unary_operator;
        expression_t *source_unary_expression = source_unary.expression;

        if (!source_unary_operator || !source_unary_expression)
        {
            DEBUG_NULL_RETURN("ir_handle_expression");
            return NULL_INSTRUCTION_STRUCT;
        }

        ir_instruction_struct_t ir_source_instruction_struct = ir_handle_expression(source_unary_expression);

        ir_unary_operator_t *ir_unary_operator = ir_handle_unary_operator(source_unary_operator);
        if (!ir_unary_operator)
        {
            if (ir_source_instruction_struct.instructions)
            {
                deallocate(ir_source_instruction_struct.instructions);
            }
            DEBUG_NULL_RETURN("ir_handle_expression");
            return NULL_INSTRUCTION_STRUCT;
        }

        ir_value_t *ir_source_value = ir_source_instruction_struct.instruction_count > 0
                                          ? ir_source_instruction_struct.instructions[ir_source_instruction_struct.instruction_count - 1]->instruction.unary_instr.destination
                                          : NULL;

        if (!ir_source_value)
        {
            ir_source_value = (ir_value_t *)allocate(sizeof(ir_value_t));
            if (!ir_source_value)
            {
                if (ir_source_instruction_struct.instructions)
                {
                    deallocate(ir_source_instruction_struct.instructions);
                }
                DEBUG_NULL_RETURN("ir_handle_expression");
                return NULL_INSTRUCTION_STRUCT;
            }
            ir_source_value->base.type = IR_NODE_VALUE;
            ir_source_value->type = IR_VAL_CONSTANT_INT;
            ir_source_value->value.constant_int = source_unary_expression->value.constant_int;
        }

        ir_value_t *ir_destination_value = (ir_value_t *)allocate(sizeof(ir_value_t));
        if (!ir_destination_value)
        {
            if (ir_source_instruction_struct.instructions)
            {
                deallocate(ir_source_instruction_struct.instructions);
            }
            DEBUG_NULL_RETURN("ir_handle_expression");
            return NULL_INSTRUCTION_STRUCT;
        }
        ir_destination_value->base.type = IR_NODE_VALUE;
        ir_destination_value->type = IR_VAL_VARIABLE;

        char *temp_var_name = new_temp_var_name();
        if (!temp_var_name)
        {
            if (ir_source_instruction_struct.instructions)
            {
                deallocate(ir_source_instruction_struct.instructions);
            }
            deallocate(ir_destination_value);
            DEBUG_NULL_RETURN("ir_handle_expression");
            return NULL_INSTRUCTION_STRUCT;
        }
        ir_destination_value->value.variable.identifier = (ir_identifier_t *)allocate(sizeof(ir_identifier_t));
        if (!ir_destination_value->value.variable.identifier)
        {
            if (ir_source_instruction_struct.instructions)
            {
                deallocate(ir_source_instruction_struct.instructions);
            }
            deallocate(ir_destination_value);
            deallocate(temp_var_name);
            DEBUG_NULL_RETURN("ir_handle_expression");
            return NULL_INSTRUCTION_STRUCT;
        }
        ir_destination_value->value.variable.identifier->base.type = IR_NODE_IDENTIFIER;
        ir_destination_value->value.variable.identifier->name = temp_var_name;

        ir_instruction_t *ir_unary_instruction = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
        if (!ir_unary_instruction)
        {
            if (ir_source_instruction_struct.instructions)
            {
                deallocate(ir_source_instruction_struct.instructions);
            }
            deallocate(ir_destination_value);
            DEBUG_NULL_RETURN("ir_handle_expression");
            return NULL_INSTRUCTION_STRUCT;
        }

        ir_unary_instruction->base.type = IR_NODE_INSTRUCTION;
        ir_unary_instruction->type = IR_INSTR_UNARY;
        ir_unary_instruction->instruction.unary_instr.unary_operator = ir_unary_operator;
        ir_unary_instruction->instruction.unary_instr.source = ir_source_value;
        ir_unary_instruction->instruction.unary_instr.destination = ir_destination_value;

        if (ir_source_instruction_struct.instruction_count > 0)
        {
            ir_instruction_t **new_instructions = (ir_instruction_t **)allocate(
                sizeof(ir_instruction_t *) * (ir_source_instruction_struct.instruction_count + 1));

            if (!new_instructions)
            {
                deallocate(ir_source_instruction_struct.instructions);
                deallocate(ir_unary_instruction);
                DEBUG_NULL_RETURN("ir_handle_expression");
                return NULL_INSTRUCTION_STRUCT;
            }

            for (size_t counter = 0; counter < ir_source_instruction_struct.instruction_count; counter++)
            {
                new_instructions[counter] = ir_source_instruction_struct.instructions[counter];
            }

            new_instructions[ir_source_instruction_struct.instruction_count] = ir_unary_instruction;

            deallocate(ir_source_instruction_struct.instructions);

            return (ir_instruction_struct_t){
                .instructions = new_instructions,
                .instruction_count = ir_source_instruction_struct.instruction_count + 1};
        }
        else
        {
            ir_instruction_t **new_instructions = (ir_instruction_t **)allocate(sizeof(ir_instruction_t *));
            if (!new_instructions)
            {
                deallocate(ir_unary_instruction);
                DEBUG_NULL_RETURN("ir_handle_expression");
                return NULL_INSTRUCTION_STRUCT;
            }
            new_instructions[0] = ir_unary_instruction;

            return (ir_instruction_struct_t){
                .instructions = new_instructions,
                .instruction_count = 1};
        }
    }
    case EXPR_NESTED:
    {
        return ir_handle_expression(source_expression->value.nested_expr);
    }
    default:
    {
        DEBUG_NULL_RETURN("ir_handle_expression");
        return NULL_INSTRUCTION_STRUCT;
    }
    }
}

ir_instruction_struct_t ir_handle_statement(statement_t *source_statement)
{
    if (!source_statement)
    {
        DEBUG_NULL_RETURN("ir_handle_statement");
        return NULL_INSTRUCTION_STRUCT;
    }

    statement_type_t source_statement_type = source_statement->stmt_type;
    switch (source_statement_type)
    {
    case STMT_RETURN:
    {
        // return(val)
        expression_t *source_return_expression = source_statement->value.return_expr;
        if (!source_return_expression)
        {
            DEBUG_NULL_RETURN("ir_handle_statement");
            return NULL_INSTRUCTION_STRUCT;
        }

        ir_instruction_struct_t ir_return_val_instruction_struct = ir_handle_expression(source_return_expression);
        if (!ir_return_val_instruction_struct.instructions)
        {
            DEBUG_NULL_RETURN("ir_handle_statement");
            return NULL_INSTRUCTION_STRUCT;
        }

        ir_instruction_t **ir_return_val_instructions = ir_return_val_instruction_struct.instructions;
        size_t ir_return_val_instruction_count = ir_return_val_instruction_struct.instruction_count;

        ir_instruction_t *ir_return_instruction = (ir_instruction_t *)allocate(sizeof(ir_instruction_t));
        if (!ir_return_instruction)
        {
            // deallocate previously allocated memory
            for (size_t i = 0; i < ir_return_val_instruction_count; i++)
            {
                deallocate(ir_return_val_instructions[i]);
            }
            deallocate(ir_return_val_instructions);
            DEBUG_NULL_RETURN("ir_handle_statement");
            return NULL_INSTRUCTION_STRUCT;
        }

        ir_return_instruction->base.type = IR_NODE_INSTRUCTION;
        ir_return_instruction->type = IR_INSTR_RETURN;

        if (ir_return_val_instruction_count == 0)
        {
            ir_value_t *ir_instruction_return_value = (ir_value_t *)allocate(sizeof(ir_value_t));
            if (!ir_instruction_return_value)
            {
                deallocate(ir_return_instruction);
                DEBUG_NULL_RETURN("ir_handle_statement");
                return NULL_INSTRUCTION_STRUCT;
            }

            ir_instruction_return_value->base.type = IR_NODE_VALUE;
            ir_instruction_return_value->base.parent = &(ir_return_instruction->base);
            ir_instruction_return_value->type = IR_VAL_CONSTANT_INT;
            ir_instruction_return_value->value.constant_int = source_return_expression->value.constant_int;

            ir_return_instruction->instruction.return_instr = (ir_instruction_return_t){.value = ir_instruction_return_value};

            return (ir_instruction_struct_t){
                .instructions = &ir_return_instruction,
                .instruction_count = 1};
        }

        ir_instruction_t *last_instruction = ir_return_val_instructions[ir_return_val_instruction_count - 1];
        switch (last_instruction->type)
        {
        case IR_INSTR_LABEL:
        {
            ir_instruction_t *copy_result = ir_return_val_instructions[ir_return_val_instruction_count - 2];
            ir_return_instruction->instruction.return_instr = (ir_instruction_return_t){
                .value = copy_result->instruction.copy_instr.destination,
            };
            break;
        }
        case IR_INSTR_BINARY:
        {
            ir_return_instruction->instruction.return_instr = (ir_instruction_return_t){
                .value = last_instruction->instruction.binary_instr.destination,
            };
            break;
        }
        case IR_INSTR_UNARY:
        {
            ir_return_instruction->instruction.return_instr = (ir_instruction_return_t){
                .value = last_instruction->instruction.unary_instr.destination};
            break;
        }
        default:
            break;
        }

        ir_instruction_t **new_instructions = (ir_instruction_t **)allocate(
            sizeof(ir_instruction_t *) * (ir_return_val_instruction_count + 1));

        if (!new_instructions)
        {
            deallocate(ir_return_instruction);
            for (size_t i = 0; i < ir_return_val_instruction_count; i++)
            {
                deallocate(ir_return_val_instructions[i]);
            }
            deallocate(ir_return_val_instructions);
            DEBUG_NULL_RETURN("ir_handle_statement");
            return NULL_INSTRUCTION_STRUCT;
        }

        memcpy(new_instructions, ir_return_val_instructions,
               sizeof(ir_instruction_t *) * ir_return_val_instruction_count);

        new_instructions[ir_return_val_instruction_count] = ir_return_instruction;

        deallocate(ir_return_val_instructions);

        return (ir_instruction_struct_t){
            .instructions = new_instructions,
            .instruction_count = ir_return_val_instruction_count + 1};
    }
    default:
    {
        DEBUG_NULL_RETURN("ir_handle_statement");
        return NULL_INSTRUCTION_STRUCT;
    }
    }
}

ir_identifier_t *ir_handle_identifier(identifier_t *source_identifier)
{
    if (!source_identifier)
    {
        DEBUG_NULL_RETURN("ir_handle_identifier");
        return NULL;
    }

    ir_identifier_t *ir_identifier = (ir_identifier_t *)allocate(sizeof(ir_identifier_t));
    if (!ir_identifier)
    {
        DEBUG_NULL_RETURN("ir_handle_identifier");
        return NULL;
    }
    ir_identifier->base.type = IR_NODE_IDENTIFIER;

    char *name = source_identifier->name;
    if (!name)
    {
        deallocate(ir_identifier);
        DEBUG_NULL_RETURN("ir_handle_identifier");
        return NULL;
    }
    ir_identifier->name = source_identifier->name;

    return ir_identifier;
}

ir_program_t *ir_handle_program(program_t *source_program)
{
    if (!source_program)
    {
        DEBUG_NULL_RETURN("ir_handle_program");
        return NULL;
    }

    ir_program_t *ir_program = (ir_program_t *)allocate(sizeof(ir_program_t));
    if (!ir_program)
    {
        DEBUG_NULL_RETURN("ir_handle_program");
        return NULL;
    }

    ir_program->base.type = IR_NODE_PROGRAM;
    ir_program->base.parent = NULL;
    ir_function_t *ir_function = ir_handle_function(source_program->function);

    if (!ir_function)
    {
        deallocate(ir_program);
        DEBUG_NULL_RETURN("ir_handle_program");
        return NULL;
    }

    ir_function->base.parent = &(ir_program->base);
    ir_program->function = ir_function;

    return ir_program;
}

ir_function_t *ir_handle_function(function_def_t *source_function)
{
    if (!source_function)
    {
        DEBUG_NULL_RETURN("ir_handle_function");
        return NULL;
    }

    ir_function_t *ir_function = (ir_function_t *)allocate(sizeof(ir_function_t));
    if (!ir_function)
    {
        DEBUG_NULL_RETURN("ir_handle_function");
        return NULL;
    }

    ir_function->base.type = IR_NODE_FUNCTION;
    ir_identifier_t *ir_identifier = ir_handle_identifier(source_function->name);

    if (!ir_identifier)
    {
        deallocate(ir_function);
        DEBUG_NULL_RETURN("ir_handle_function");
        return NULL;
    }

    ir_identifier->base.parent = &(ir_function->base);

    ir_function->name = ir_identifier;

    ir_instruction_struct_t ir_instruction_struct = ir_handle_statement(source_function->body);
    ir_instruction_t **ir_instructions = ir_instruction_struct.instructions;
    if (!ir_instructions)
    {
        deallocate(ir_function);
        deallocate(ir_identifier);
        DEBUG_NULL_RETURN("ir_handle_function");
        return NULL;
    }

    size_t ir_instruction_count = ir_instruction_struct.instruction_count;

    for (size_t counter = 0; counter < ir_instruction_count; counter++)
    {
        ir_instruction_t *ir_instruction = ir_instructions[counter];
        if (!ir_instruction)
        {
            // deallocate previously allocated memory
            for (size_t i = 0; i < counter; i++)
            {
                deallocate(ir_instructions[i]);
            }
            deallocate(ir_instructions);
            deallocate(ir_function);
            deallocate(ir_identifier);
            DEBUG_NULL_RETURN("ir_handle_function");
            return NULL;
        }

        ir_instruction->base.parent = &(ir_function->base);
    }

    ir_function->instruction_count = ir_instruction_count;
    ir_function->body = ir_instructions;

    return ir_function;
}

#undef NULL_INSTRUCTION_STRUCT
#undef DEBUG_NULL_RETURN

#endif /* BF4D96C0_1C97_4F20_AB1B_168AC361CE92 */
