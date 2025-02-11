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
    IR_NODE_BINARY_OPERATOR,
} ir_ast_node_type_t;

typedef struct ir_ast_node_e
{
    ir_ast_node_type_t type;
    struct ir_ast_node_e *parent;
} ir_ast_node_t;

typedef enum
{
    IR_UNARY_BITWISE_COMPLEMENT,
    IR_UNARY_NEGATE,
    IR_UNARY_NOT,
} ir_unary_operator_type_t;

typedef struct ir_unary_operator_e
{
    ir_ast_node_t base;
    ir_unary_operator_type_t unary_op;
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

typedef enum ir_binary_operator_type_e
{
    IR_BINARY_MULTIPLY,
    IR_BINARY_DIVIDE,
    IR_BINARY_REM,

    IR_BINARY_ADD,
    IR_BINARY_SUBTRACT,

    IR_BINARY_BITWISE_LEFT_SHIFT,
    IR_BINARY_BITWISE_RIGHT_SHIFT,

    IR_BINARY_LESS_THAN,
    IR_BINARY_LESS_THAN_EQUAL,
    IR_BINARY_GREATER_THAN,
    IR_BINARY_GREATER_THAN_EQUAL,

    IR_BINARY_EQUAL,
    IR_BINARY_NOT_EQUAL,

    IR_BINARY_BITWISE_AND,
    IR_BINARY_BITWISE_XOR,
    IR_BINARY_BITWISE_OR,

    IR_BINARY_LOGICAL_AND,
    IR_BINARY_LOGICAL_OR,
} ir_binary_operator_type_t;

typedef struct ir_binary_operator_e
{
    ir_ast_node_t base;
    ir_binary_operator_type_t operator;
} ir_binary_operator_t;

typedef struct ir_instruction_binary_e
{
    ir_binary_operator_t *binary_operator;
    ir_value_t *left;
    ir_value_t *right;
    ir_value_t *destination;
} ir_instruction_binary_t;

typedef struct ir_instruction__copy_e
{
    ir_value_t *source;
    ir_value_t *destination;
} ir_instruction_copy_t;

typedef struct ir_instruction_jump_e
{
    ir_identifier_t *target;
} ir_instruction_jump_t;

typedef struct ir_instruction_jz_e
{
    ir_value_t *condition;
    ir_identifier_t *target;
} ir_instruction_jz_t;

typedef struct ir_instruction_jnz_e
{
    ir_value_t *condition;
    ir_identifier_t *target;
} ir_instruction_jnz_t;

typedef struct ir_instruction_label_e
{
    ir_identifier_t *identifier;
} ir_instruction_label_t;

typedef enum
{
    IR_INSTR_RETURN,
    IR_INSTR_UNARY,
    IR_INSTR_BINARY,
    IR_INSTR_COPY,
    IR_INSTR_JUMP,
    IR_INSTR_JUMP_IF_ZERO,
    IR_INSTR_JUMP_IF_NOT_ZERO,
    IR_INSTR_LABEL,
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
        ir_instruction_binary_t binary_instr;
        ir_instruction_copy_t copy_instr;
        ir_instruction_jump_t jmp_instr;
        ir_instruction_jz_t jz_instr;
        ir_instruction_jnz_t jnz_instr;
        ir_instruction_label_t label_instr;
    } instruction;
} ir_instruction_t;

typedef struct
{
    ir_ast_node_t base;
    ir_identifier_t *name;
    ir_instruction_t **body;
    size_t instruction_count;
} ir_function_t;

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

#endif /* F68732A3_D9B4_483E_A152_AB2FB4E19B12 */