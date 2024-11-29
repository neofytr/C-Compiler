#ifndef A0994C9B_E1E9_4A37_93B6_749D272B877A
#define A0994C9B_E1E9_4A37_93B6_749D272B877A

#include "../../ast/assembly/assembly_ast.h"
#include <stdbool.h>

bool asm_third_pass(asm_program_t *asm_program);
bool asm_insert_alloc_stack(asm_program_t *asm_program);
bool asm_fix_instruction(asm_program_t *asm_program);

#endif /* A0994C9B_E1E9_4A37_93B6_749D272B877A */
