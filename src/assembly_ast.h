#ifndef D36C472B_CEDF_4E5E_B6E9_DDAE1DEA3523
#define D36C472B_CEDF_4E5E_B6E9_DDAE1DEA3523

#include <stdlib.h>

typedef struct asm_program_ asm_program_t;
typedef struct asm_function_ asm_function_t;
typedef struct asm_identifier_ asm_identifier_t;
typedef struct asm_instruction_ asm_instruction_t;
typedef struct asm_instruction_mov_ asm_instruction_mov_t;
typedef struct asm_instruction_ret_ asm_instruction_ret_t;
typedef struct asm_operand_ asm_operand_t;
typedef struct asm_immediate_ asm_immediate_t;
typedef struct asm_register_ asm_register_t;
typedef struct asm_ast_node_ asm_ast_node_t;

typedef enum
{
    ASM_NODE_PROGRAM,
    ASM_NODE_FUNCTION,
    ASM_NODE_IDENTIFIER,
    ASM_NODE_INSTRUCTION_MOV,
    ASM_NODE_INSTRUCTION_RET,
    ASM_NODE_OPERAND_IMM,
    ASM_NODE_OPERAND_REGISTER,
} asm_ast_node_type_t;

struct asm_ast_node_
{
    asm_ast_node_type_t type;
    asm_ast_node_t *parent;
};

typedef enum
{
    OPERAND_IMMEDIATE,
    OPERAND_REGISTER
} asm_operand_type_t;

typedef enum
{
    INSTRUCTION_MOV,
    INSTRUCTION_RET
} asm_instruction_type_t;

struct asm_immediate_
{
    asm_ast_node_t base;
    int value;
};

struct asm_register_
{
    asm_ast_node_t base;
    int reg_num;
};

struct asm_identifier_
{
    asm_ast_node_t base;
    char *name;
};

struct asm_operand_
{
    asm_ast_node_t base;
    asm_operand_type_t type;
    union
    {
        asm_immediate_t *immediate;
        asm_register_t *reg;
    };
};

struct asm_instruction_mov_
{
    asm_ast_node_t base;
    asm_operand_t src;
    asm_operand_t dst;
};

struct asm_instruction_ret_
{
    asm_ast_node_t base;
};

struct asm_instruction_
{
    asm_ast_node_t base;
    asm_instruction_type_t type;
    union
    {
        asm_instruction_mov_t *mov;
        asm_instruction_ret_t *ret;
    };
};

struct asm_function_
{
    asm_ast_node_t base;
    asm_identifier_t *name;
    asm_instruction_t **instructions; // Dynamic array of instruction pointers
    size_t instruction_count;
};

struct asm_program_
{
    asm_ast_node_t base;
    asm_function_t *function;
};

#endif /* D36C472B_CEDF_4E5E_B6E9_DDAE1DEA3523 */