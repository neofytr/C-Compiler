#ifndef F68732A3_D9B4_483E_A152_AB2FB4E19B12
#define F68732A3_D9B4_483E_A152_AB2FB4E19B12

#include <stdlib.h>

typedef enum
{
    IR_NODE_PROGRAM,
    IR_NODE_FUNCTION,
    IR_NODE_INSTRUCTION,
    IR_NODE_IDENTIFIER,
    IR_NODE_VALUE,
    IR_NODE_UNARY_OPERATOR,
} ir_ast_node_type_t;

typedef struct ir_ast_node_e
{
    ir_ast_node_type_t type;
    struct ir_ast_node_e *parent;
} ir_ast_node_t;

typedef struct ir_unary_operator_e
{
    ir_ast_node_t base;
    enum
    {
        IR_UNARY_BITWISE_COMPLEMENT,
        IR_UNARY_NEGATE,
    } unary_op;
} ir_unary_operator_t;

typedef struct ir_identifier_e
{
    ir_ast_node_t base;
    char *name;
} ir_identifier_t;

typedef struct ir_variable_e
{
    ir_identifier_t *identifier;
} ir_variable_t;

typedef enum
{
    IR_VAL_CONSTANT_INT,
    IR_VAL_VARIABLE,
} ir_value_type_t;

typedef struct ir_value_e
{
    ir_ast_node_t base;
    ir_value_type_t type;
    union
    {
        int constant_int;
        ir_variable_t variable;
    } value;
} ir_value_t;

typedef enum
{
    IR_INSTR_RETURN,
    IR_INSTR_UNARY,
} ir_instruction_type_t;

typedef struct
{
    ir_value_t *value;
} ir_instruction_return_t;

typedef struct
{
    ir_unary_operator_t *unary_operator;
    ir_value_t *source;
    ir_value_t *destination;
} ir_instruction_unary_t;

typedef struct ir_instruction_e
{
    ir_ast_node_t base;
    ir_instruction_type_t type;
    union
    {
        ir_instruction_return_t return_instr;
        ir_instruction_unary_t unary_instr;
    } instruction;
} ir_instruction_t;

typedef struct
{
    ir_ast_node_t base;
    ir_function_t *function;
} ir_program_t;

typedef struct
{
    ir_instruction_t **instructions;
    size_t instruction_count;
} ir_instruction_struct_t;

typedef struct
{
    ir_ast_node_t base;
    ir_identifier_t *name;
    ir_instruction_t **body;
    size_t instruction_count;
} ir_function_t;

#endif /* F68732A3_D9B4_483E_A152_AB2FB4E19B12 */