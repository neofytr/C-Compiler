#ifndef D36C472B_CEDF_4E5E_B6E9_DDAE1DEA3523
#define D36C472B_CEDF_4E5E_B6E9_DDAE1DEA3523

#include <stdlib.h>

typedef struct program_t program_t;
typedef struct function_t function_t;
typedef struct identifier_t identifier_t;
typedef struct instruction_t instruction_t;
typedef struct operand_t operand_t;
typedef struct immediate_t immediate_t;
typedef struct register_t register_t;
typedef struct ast_node_ ast_node_t;

typedef enum
{
    NODE_PROGRAM,
    NODE_FUNCTION,
    NODE_IDENTIFIER,
    NODE_INSTRUCTION_MOV,
    NODE_INSTRUCTION_RET,
    NODE_OPERAND_IMM,
    NODE_OPERAND_REGISTER,
} ast_node_type_t;

struct ast_node_
{
    ast_node_type_t type;
    ast_node_t *parent;
};

typedef enum
{
    OPERAND_IMMEDIATE,
    OPERAND_REGISTER
} operand_type_t;

typedef enum
{
    INSTRUCTION_MOV,
    INSTRUCTION_RET
} instruction_type_t;

struct immediate_t
{
    ast_node_t base;
    int value;
};

struct register_t
{
    ast_node_t base;
    int reg_num;
};

struct identifier_t
{
    ast_node_t base;
    char *name;
};

struct operand_t
{
    ast_node_t base;
    operand_type_t type;
    union
    {
        immediate_t *immediate;
        register_t *reg;
    };
};

struct instruction_mov_t
{
    ast_node_t base;
    operand_t src;
    operand_t dst;
};

struct instruction_ret_t
{
    ast_node_t base;
};

struct instruction_t
{
    ast_node_t base;
    instruction_type_t type;
    union
    {
        instruction_mov_t *mov;
        instruction_ret_t *ret;
    };
};

struct function_t
{
    ast_node_t base;
    identifier_t *name;
    instruction_t **instructions; // Dynamic array of instruction pointers
    size_t instruction_count;
};

struct program_t
{
    ast_node_t base;
    function_t *function;
};

#endif /* D36C472B_CEDF_4E5E_B6E9_DDAE1DEA3523 */