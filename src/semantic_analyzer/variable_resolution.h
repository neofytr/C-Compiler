#ifndef CBF75423_9343_4603_A1BC_2EB70CE7DFB6
#define CBF75423_9343_4603_A1BC_2EB70CE7DFB6

#include "../ast/source/ast.h"
#include "../parser/parser.h"
#include "../hash_table/var_res_hash_table.h"

bool var_res_pass(program_t *program);
bool resolve_statement(statement_t *statement, var_table_t *var_table);
bool resolve_declaration(declaration_t *declaration, var_table_t *var_table);
bool resolve_expression(expression_t *expression, var_table_t *var_table);
char *make_unique_name(const char *name);

size_t unique_name_count = 0;

char *make_unique_name(const char *name)
{
    char *new_name = (char *)allocate(sizeof(char) * 256);
    if (!new_name)
    {
        return NULL;
    }

    if (snprintf(new_name, 256, "%s.%zu", name, unique_name_count++) < 0)
    {
        deallocate(new_name);
        return NULL;
    }

    return new_name;
}

bool resolve_declaration(declaration_t *declaration, var_table_t *var_table)
{
    if (!declaration || !var_table)
    {
        return false;
    }

    identifier_t *var = declaration->name;
    if (!var)
    {
        return false;
    }

    char *var_name = var->name;
    if (!var_name)
    {
        return false;
    }

    var_node_t *searched_node = var_table_search(var_table, var_name);
    if (!searched_node)
    {
        char *new_unique_name = make_unique_name(var_name);
        if (!new_unique_name || !var_table_insert(var_table, var_name, new_unique_name))
        {
            deallocate(new_unique_name);
            return false;
        }
        deallocate(new_unique_name);

        if (declaration->has_init_expr)
        {
            if (!declaration->init_expr)
            {
                return false;
            }
            return resolve_expression(declaration->init_expr, var_table);
        }
        return true;
    }

    fprintf(stderr, "Error -> Line: %zu Column: %zu -> Redeclaration of the variable %s\n",
            var->base.location.line, var->base.location.column, var_name);
    return false;
}

bool resolve_expression(expression_t *expression, var_table_t *var_table)
{
    if (!expression || !var_table)
    {
        return false;
    }

    switch (expression->expr_type)
    {
    case EXPR_NESTED:
        return resolve_expression(expression->value.nested_expr, var_table);

    case EXPR_ASSIGN:
    {
        assignment_t assign = expression->value.assign;
        expression_t *lvalue = assign.lvalue;
        if (!lvalue || lvalue->expr_type != EXPR_VAR)
        {
            fprintf(stderr, "Error -> Line: %zu Column: %zu -> Unexpected lvalue, Expected a variable\n",
                    expression->base.location.line, expression->base.location.column);
            return false;
        }

        variable_t var = lvalue->value.var;
        identifier_t *iden = var.name;
        if (!iden || !iden->name)
        {
            return false;
        }

        var_node_t *searched_node = var_table_search(var_table, iden->name);
        if (!searched_node)
        {
            fprintf(stderr, "Error -> Line: %zu Column: %zu -> Undeclared Variable %s\n",
                    expression->base.location.line, expression->base.location.column, iden->name);
            return false;
        }

        char *old_name = iden->name;
        iden->name = strdup(searched_node->unique_name);
        deallocate(old_name);

        return resolve_expression(assign.rvalue, var_table);
    }

    case EXPR_BINARY:
    {
        binary_t binary = expression->value.binary;
        return resolve_expression(binary.left_expr, var_table) &&
               resolve_expression(binary.right_expr, var_table);
    }

    case EXPR_CONSTANT_INT:
        return true;

    case EXPR_UNARY:
    {
        unary_t unary = expression->value.unary;
        return resolve_expression(unary.expression, var_table);
    }

    case EXPR_VAR:
    {
        variable_t var = expression->value.var;
        identifier_t *iden = var.name;
        if (!iden || !iden->name)
        {
            return false;
        }

        var_node_t *searched_node = var_table_search(var_table, iden->name);
        if (!searched_node)
        {
            fprintf(stderr, "Error -> Line: %zu Column: %zu -> %s not declared in the current scope\n",
                    iden->base.location.line, iden->base.location.column, iden->name);
            return false;
        }

        char *old_name = iden->name;
        iden->name = strdup(searched_node->unique_name);
        deallocate(old_name);

        return true;
    }

    default:
        return false;
    }
}

bool resolve_statement(statement_t *statement, var_table_t *var_table)
{
    if (!statement || !var_table)
    {
        return false;
    }

    switch (statement->stmt_type)
    {
    case STMT_RETURN:
        return resolve_expression(statement->value.return_expr, var_table);

    case STMT_EXPR:
        return resolve_expression(statement->value.expr, var_table);

    case STMT_NULL:
        return true;

    default:
        return false;
    }
}

bool var_res_pass(program_t *program)
{
    if (!program)
    {
        return false;
    }

    var_table_t *var_table = create_var_table();
    if (!var_table)
    {
        return false;
    }

    function_def_t *function = program->function;
    if (!function)
    {
        destroy_var_table(var_table);
        return false;
    }

    size_t block_count = function->block_count;
    block_item_t **arr = function->body;

    bool has_failed = false;

    for (size_t counter = 0; counter < block_count; counter++)
    {
        block_item_t *block = arr[counter];
        if (!block)
        {
            destroy_var_table(var_table);
            return false;
        }

        switch (block->type)
        {
        case BLOCK_STATEMENT:
            if (!resolve_statement(block->value.statement, var_table))
            {
                has_failed = true;
            }
            break;

        case BLOCK_DECLARATION:
            if (!resolve_declaration(block->value.declaration, var_table))
            {
                has_failed = true;
            }
            break;
        }
    }

    destroy_var_table(var_table);
    return !has_failed;
}

#endif