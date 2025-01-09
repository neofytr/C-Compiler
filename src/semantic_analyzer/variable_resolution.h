#ifndef CBF75423_9343_4603_A1BC_2EB70CE7DFB6
#define CBF75423_9343_4603_A1BC_2EB70CE7DFB6

#include "../ast/source/ast.h"
#include "../parser/parser.h"
#include "../hash_table/var_res_hash_table.h"

bool var_res_pass(program_t *program);
bool resolve_statement(statement_t *statement, hash_table_t *hash_table);
bool resolve_declaration(declaration_t *declaration, hash_table_t *hash_table);
bool resolve_expression(expression_t *expression, hash_table_t *hash_table);

bool resolve_expression(expression_t *expression, hash_table_t *hash_table)
{
    if (!expression)
    {
        return false;
    }

    switch (expression->expr_type)
    {
    case EXPR_NESTED:
    {
        return resolve_expression(expression->value.nested_expr, hash_table);
        break;
    }
    case EXPR_ASSIGN:
    {
        assignment_t assign = expression->value.assign;
        return resolve_expression(assign.lvalue, hash_table) && resolve_expression(assign.rvalue, hash_table);
        break;
    }
    case EXPR_BINARY:
    {
        binary_t binary = expression->value.binary;
        return resolve_expression(binary.left_expr, hash_table) && resolve_expression(binary.right_expr, hash_table);
        break;
    }
    case EXPR_CONSTANT_INT:
    {
        return true;
        break;
    }
    case EXPR_UNARY:
    {
        unary_t unary = expression->value.unary;
        return resolve_expression(unary.expression, hash_table);
        break;
    }
    case EXPR_VAR:
    {
        variable_t var = expression->value.var;
    }
    }
}

bool resolve_statement(statement_t *statement, hash_table_t *hash_table)
{
    if (!statement)
    {
        return false;
    }

    switch (statement->stmt_type)
    {
    case STMT_RETURN:
    {
        return resolve_expression(statement->value.return_expr, hash_table);
        break;
    }
    case STMT_EXPR:
    {
        return resolve_expression(statement->value.expr, hash_table);
        break;
    }
    case STMT_NULL:
    {
        return true;
        break;
    }
    default:
    {
        return false;
        break;
    }
    }

    return false;
}

bool var_res_pass(program_t *program)
{
    hash_table_t *hash_table = create_hash_table();
    if (!hash_table)
    {
        return false;
    }

    bool has_failed = false;

    if (!program)
    {
        return false;
    }

    function_def_t *function = program->function;
    if (!function)
    {
        return false;
    }

    size_t block_count = function->block_count;
    block_item_t **arr = function->body;

    for (size_t counter = 0; counter < block_count; counter++)
    {
        block_item_t *block = arr[counter];
        if (!block)
        {
            return false;
        }

        switch (block->type)
        {
        case BLOCK_STATEMENT:
        {
            if (!resolve_statement(block->value.statement, hash_table))
            {
                has_failed = true;
            }
            break;
        }
        case BLOCK_DECLARATION:
        {
            if (!resolve_declaration(block->value.declaration, hash_table))
            {
                has_failed = true;
            }
            break;
        }
        }
    }

    return !has_failed;
}

#endif /* CBF75423_9343_4603_A1BC_2EB70CE7DFB6 */
