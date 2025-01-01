#ifndef D36C472B_CEDF_4E5E_B6E9_DDAE1DEA3523
#define D36C472B_CEDF_4E5E_B6E9_DDAE1DEA3523

#include <stdlib.h>

// Forward declarations for all struct types
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
typedef struct asm_pseudo_ asm_pseudo_t;
typedef struct asm_stack_ asm_stack_t;
typedef struct asm_instruction_alloc_stack_ asm_instruction_alloc_stack_t;
typedef struct asm_instruction_unary_ asm_instruction_unary_t;
typedef struct asm_unary_operator_ asm_unary_operator_t;
typedef struct asm_binary_operator_ asm_binary_operator_t;
typedef struct asm_instruction_binary_ asm_instruction_binary_t;
typedef struct asm_instruction_cmp_ asm_instruction_cmp_t;
typedef struct asm_instruction_jmp_ asm_instruction_jmp_t;
typedef struct asm_instruction_jmpcc_ asm_instruction_jmpcc_t;
typedef struct asm_instruction_setcc_ asm_instruction_setcc_t;
typedef struct asm_instruction_label_ asm_instruction_label_t;
typedef struct asm_instruction_cdq_ asm_instruction_cdq_t;
typedef struct asm_instruction_idiv_ asm_instruction_idiv_t;

// AST node types
typedef enum
{
    ASM_NODE_PROGRAM,
    ASM_NODE_FUNCTION,
    ASM_NODE_IDENTIFIER,
    ASM_NODE_INSTRUCTION,
    ASM_NODE_OPERAND,
} asm_ast_node_type_t;

struct asm_ast_node_
{
    asm_ast_node_type_t type;
    asm_ast_node_t *parent;
};

// Operand types
typedef enum
{
    OPERAND_IMMEDIATE,
    OPERAND_REGISTER,
    OPERAND_PSEUDO,
    OPERAND_STACK,
} asm_operand_type_t;

// Instruction types
typedef enum
{
    INSTRUCTION_MOV,
    INSTRUCTION_RET,
    INSTRUCTION_UNARY,
    INSTRUCTION_BINARY,
    INSTRUCTION_CMP,
    INSTRUCTION_IDIV,
    INSTRUCTION_CDQ,
    INSTRUCTION_JMP,
    INSTRUCTION_JMPCC,
    INSTRUCTION_SETCC,
    INSTRUCTION_LABEL,
    INSTRUCTION_ALLOCATE_STACK,
} asm_instruction_type_t;

typedef enum
{
    COND_E,  // Equal
    COND_NE, // Not Equal
    COND_SIGN_G,  // Signed Greater
    COND_SIGN_GE, // Signed Greater or Equal
    COND_SIGN_L,  // Signed Less
    COND_SIGN_LE, // Signed Less or Equal
} asm_condition_code_t;

struct asm_immediate_
{
    int value;
};

typedef enum
{
    ASM_REG_AX,
    ASM_REG_DX,
    ASM_REG_R10,
    ASM_REG_R11,
} asm_reg_no_t;

struct asm_register_
{
    asm_reg_no_t reg_no;
};

struct asm_pseudo_
{
    char *pseudo_name;
};

struct asm_stack_
{
    int offset;
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
        asm_immediate_t immediate;
        asm_register_t reg;
        asm_pseudo_t pseudo;
        asm_stack_t stack;
    } operand;
};

struct asm_instruction_mov_
{
    asm_operand_t *src;
    asm_operand_t *dst;
};

struct asm_instruction_ret_
{
};

typedef enum
{
    UNARY_NEG,
    UNARY_NOT,
} asm_unary_operator_enum_t;

struct asm_unary_operator_
{
    asm_unary_operator_enum_t op;
};

struct asm_instruction_unary_
{
    asm_unary_operator_t *unary_operator;
    asm_operand_t *operand;
};

typedef enum
{
    ASM_BINARY_ADD,
    ASM_BINARY_SUB,
    ASM_BINARY_MULT,
} asm_binary_operator_type_t;

struct asm_binary_operator_
{
    asm_binary_operator_type_t binary_op;
};

struct asm_instruction_binary_
{
    asm_binary_operator_t *binary_operator;
    asm_operand_t *first_operand;
    asm_operand_t *second_operand;
};

struct asm_instruction_cmp_
{
    asm_operand_t *first_operand;
    asm_operand_t *second_operand;
};

struct asm_instruction_jmp_
{
    asm_identifier_t *target;
};

struct asm_instruction_jmpcc_
{
    asm_condition_code_t condition;
    asm_identifier_t *target;
};

struct asm_instruction_setcc_
{
    asm_condition_code_t condition;
    asm_operand_t *dst;
};

struct asm_instruction_label_
{
    asm_identifier_t *label;
};

struct asm_instruction_cdq_
{
};

struct asm_instruction_idiv_
{
    asm_operand_t *operand;
};

struct asm_instruction_alloc_stack_
{
    int alloc_size;
};

struct asm_instruction_
{
    asm_ast_node_t base;
    asm_instruction_type_t type;
    union
    {
        asm_instruction_mov_t mov;
        asm_instruction_ret_t ret;
        asm_instruction_unary_t unary;
        asm_instruction_binary_t binary;
        asm_instruction_cmp_t cmp;
        asm_instruction_idiv_t idiv;
        asm_instruction_cdq_t cdq;
        asm_instruction_jmp_t jmp;
        asm_instruction_jmpcc_t jmpcc;
        asm_instruction_setcc_t setcc;
        asm_instruction_label_t label;
        asm_instruction_alloc_stack_t alloc_stack;
    } instr;
};

struct asm_function_
{
    asm_ast_node_t base;
    asm_identifier_t *name;
    asm_instruction_t **instructions;
    size_t instruction_count;
};

struct asm_program_
{
    asm_ast_node_t base;
    asm_function_t *function;
};

#endif /* D36C472B_CEDF_4E5E_B6E9_DDAE1DEA3523 */