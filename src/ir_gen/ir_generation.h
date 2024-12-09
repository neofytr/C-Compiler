#ifndef BF4D96C0_1C97_4F20_AB1B_168AC361CE92
#define BF4D96C0_1C97_4F20_AB1B_168AC361CE92

#include <stdio.h>
#include "../ast/IR/ir_ast.h"
#include "../ast/source/ast.h"
#include "../allocator/allocator.h"

#define NULL_INSTRUCTION_STRUCT ((ir_instruction_struct_t){0})
#define MAX_TEMP_VAR_LENGTH 8

#define DEBUG_NULL_RETURN(func_name) \
    fprintf(stderr, "NULL return in %s at line %d\n", func_name, __LINE__)

size_t temp_var_count = 0;

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
    case BINARY_ADD:
        ir_binary_operator->operator= IR_BINARY_ADD;
        break;
    case BINARY_SUB:
        ir_binary_operator->operator= IR_BINARY_SUBTRACT;
        break;
    case BINARY_MUL:
        ir_binary_operator->operator= IR_BINARY_MULTIPLY;
        break;
    case BINARY_DIV:
        ir_binary_operator->operator= IR_BINARY_DIVIDE;
        break;
    case BINARY_REM:
        ir_binary_operator->operator= IR_BINARY_REM;
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
        binary_t source_binary = source_expression->value.binary;
        expression_t *source_left_expr = source_binary.left_expr;
        expression_t *source_right_expr = source_binary.right_expr;
        binary_operator_t *source_binary_operator = source_binary.op;

        if (!source_left_expr || !source_right_expr || !source_binary_operator)
        {
            fprintf(stderr, "Invalid binary expression components\n");
            return (ir_instruction_struct_t){.instructions = NULL, .instruction_count = 0};
        }

        ir_instruction_struct_t ir_left_instruction_struct = ir_handle_expression(source_left_expr);
        ir_instruction_struct_t ir_right_instruction_struct = ir_handle_expression(source_right_expr);

        /*

        The IR we emit evaluates a binary expression's first operand before the second, but it's just as correct
        to evaluate the second operand before the first. According to the C standard, subexpressions of the
        same operation are usually unsequences; that is, they can be evaluated in any order. If two subexpressions
        would be unsequences, but either or both of them is a function call, they're indeterminately sequenced, meaning that
        either one can execute first, but they can't interleave. In many cases, unsequenced and indeterminately sequenced evaluations
        can lead to unpredictable results. Consider the following program, which includes two indeterminately sequenced calls to
        printf:

        #include <stdio.h>

        int main(void)
        {
            return printf("Hello, ") + printf("World!");
        }

        You could compile this program with a C standard-compliant compiler, run it, and get
        either of these outputs:

        Hello, World!
        World!Hello,

        There are a few exceptions where we must evaluate the first operand first: the logical && and  || operators;
        the conditional ? : operator; and the comma operator.

        This is laid out in section 5.1.2.3 of the C standard (which covers the general rules for evaluation
        order and defines the terms unsequenced and indeterminately sequenced) and section 6.5 paragraphs 1-3 (which
        address the evaluation order for expression operands in particular).

        Unsequenced operations are one example of a broader pattern: there
        are a lot of circumstances where the C standard doesn’t specify exactly how
        programs should behave. By leaving some details about program behavior unspecified, the C
        standard puts a lot of power in the hands of compiler writers, allowing them
        to write sophisticated compiler optimizations. But there’s an obvious tradeoff: it’s easy for programmers to 
        write code that might not behave the way they expect.

        */

        ir_value_t *ir_left_value = (ir_left_instruction_struct.instruction_count > 0)
                                        ? ir_left_instruction_struct.instructions[ir_left_instruction_struct.instruction_count - 1]->instruction.unary_instr.destination
                                        : NULL;

        ir_value_t *ir_right_value = (ir_right_instruction_struct.instruction_count > 0)
                                         ? ir_right_instruction_struct.instructions[ir_right_instruction_struct.instruction_count - 1]->instruction.unary_instr.destination
                                         : NULL;

        if (!ir_left_value)
        {
            ir_left_value = allocate(sizeof(ir_value_t));
            ir_left_value->base.type = IR_NODE_VALUE;
            ir_left_value->type = IR_VAL_CONSTANT_INT;
            ir_left_value->value.constant_int = source_left_expr->value.constant_int;
        }

        if (!ir_right_value)
        {
            ir_right_value = allocate(sizeof(ir_value_t));
            ir_right_value->base.type = IR_NODE_VALUE;
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

        size_t total_instruction_count =
            ir_left_instruction_struct.instruction_count +
            ir_right_instruction_struct.instruction_count + 1;

        ir_instruction_t **combined_instructions = allocate(
            sizeof(ir_instruction_t *) * total_instruction_count);

        size_t current_index = 0;
        for (size_t i = 0; i < ir_left_instruction_struct.instruction_count; i++)
        {
            combined_instructions[current_index++] =
                ir_left_instruction_struct.instructions[i];
        }

        for (size_t i = 0; i < ir_right_instruction_struct.instruction_count; i++)
        {
            combined_instructions[current_index++] =
                ir_right_instruction_struct.instructions[i];
        }

        combined_instructions[current_index++] = ir_binary_instruction;

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

        // Use the last instruction's destination as return value
        ir_return_instruction->instruction.return_instr = (ir_instruction_return_t){
            .value = ir_return_val_instructions[ir_return_val_instruction_count - 1]->instruction.unary_instr.destination};

        // Append return instruction to the existing instructions
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

        // Copy existing instructions
        memcpy(new_instructions, ir_return_val_instructions,
               sizeof(ir_instruction_t *) * ir_return_val_instruction_count);

        // Add return instruction
        new_instructions[ir_return_val_instruction_count] = ir_return_instruction;

        // deallocate the old instructions array
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