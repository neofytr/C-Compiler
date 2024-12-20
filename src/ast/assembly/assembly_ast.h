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
typedef struct asm_pseudo_ asm_pseudo_t;
typedef struct asm_stack_ asm_stack_t;
typedef struct asm_instruction_alloc_stack_ asm_instruction_alloc_stack_t;
typedef struct asm_instruction_unary_ asm_instruction_unary_t;
typedef struct asm_unary_operator_ asm_unary_operator_t;
typedef struct asm_binary_operator_ asm_binary_operator_t;
typedef struct asm_instruction_binary_ asm_instruction_binary_t;
typedef struct asm_instruction_idiv_ asm_instruction_idiv_t;
typedef struct asm_instruction_cqo_ asm_instruction_cqo_t;

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

typedef enum
{
    OPERAND_IMMEDIATE,
    OPERAND_REGISTER,
    OPERAND_PSEUDO,
    OPERAND_STACK,
} asm_operand_type_t;

typedef enum
{
    INSTRUCTION_MOV,
    INSTRUCTION_RET,
    INSTRUCTION_UNARY,
    INSTRUCTION_ALLOCATE_STACK,
    INSTRUCTION_CQO,
    INSTRUCTION_IDIV,
    INSTRUCTION_BINARY,
} asm_instruction_type_t;

struct asm_immediate_
{
    int value;
};

typedef enum
{
    ASM_REG_RAX,
    ASM_REG_R10,
    ASM_REG_RDX,
    ASM_REG_R11,
    ASM_REG_RCX,
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
}; // ultimately, we need to replace every pseudoreg with a location on the stack;
// we represent those with the stack operand, which indicates the stack address at the
// given offset from RBP; we'd represent the operand (RBP - 4) with AST node Stack(-4)

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
        asm_register_t reg;  // represents a hardware register
        asm_pseudo_t pseudo; // lets us use an arbitrary identifier as a pseudoregister; we use this to
        // refer to the temporary variables produced during IR generation
        asm_stack_t stack;
    } operand;
};

struct asm_instruction_mov_
{
    asm_operand_t *src;
    asm_operand_t *dst;
};

struct asm_instruction_unary_
{
    asm_unary_operator_t *unary_operator;
    asm_operand_t *operand; // is both the source and the destination operand
};

typedef enum
{
    ASM_BINARY_ADD,
    ASM_BINARY_SUB,
    ASM_BINARY_MULT,
    ASM_BINARY_BITWISE_AND,
    ASM_BINARY_BITWISE_OR,
    ASM_BINARY_BITWISE_XOR,
    ASM_BINARY_BITWISE_SHIFT_LEFT,
    ASM_BINARY_BITWISE_SHIFT_RIGHT,
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

struct asm_instruction_idiv_
{
    asm_operand_t *operand;
};

struct asm_instruction_cqo_
{
};

struct asm_instruction_alloc_stack_
{
    int alloc_size; // is the instruction sub rsp, alloc_size
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

struct asm_instruction_ret_
{
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
        asm_instruction_alloc_stack_t alloc_stack;
        asm_instruction_binary_t binary;
        asm_instruction_cqo_t cqo;
        asm_instruction_idiv_t idiv;
    } instr;
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