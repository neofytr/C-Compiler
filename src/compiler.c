#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./code_emitter/code_emitter.h"
#include "./ir_gen/ir_generation.h"
#include "./assembly_gen/from_IR/first_pass.h"
#include "./assembly_gen/from_IR/second_pass.h"
#include "./assembly_gen/from_IR/third_pass.h"
#include "./parser/parser.h"
#include "./semantic_analyzer/variable_resolution.h"

/*

Most production C compilers evaluate constant expressions at compile time, even when
no optimizations are enabled. They behave this way since some constant expressions, like
static variable initializers, must be evaluated at compile time, and evaluating all
constant expressions at compile time is simpler than evaluating only some.

*/

void print_usage(const char *prog_name)
{
    fprintf(stderr, "Usage: %s <inputfile> [-o <outputfile>]\n", prog_name);
}

int main(int argc, char **argv)
{
    if (argc < 2 || argc > 4)
    {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file = NULL;
    const char *output_file = "output.asm";

    input_file = argv[1];

    if (argc == 4)
    {
        if (strcmp(argv[2], "-o") != 0)
        {
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
        output_file = argv[3];
    }

    size_t file_size;
    uint8_t *source = read_source_file(input_file, &file_size);
    if (!source)
    {
        fprintf(stderr, "Error: Failed to read input file '%s'.\n", input_file);
        return EXIT_FAILURE;
    }

    token_list_t token_list;
    token_t *tokens = (token_t *)allocate(sizeof(token_t) * MAX_TOKEN_NUMBER);
    if (!tokens)
    {
        perror("malloc");
        return EXIT_FAILURE;
    }

    token_list.tokens = tokens;

    token_list.count = 0;
    if (!lex_source(source, &token_list))
    {
        fprintf(stderr, "Error: Lexing failed for file '%s'.\n", input_file);
        free(source);
        return EXIT_FAILURE;
    }

    free(source);

    parser_t *parser = init_parser(token_list.tokens, token_list.count);
    program_t *ast = parse_program(parser);

    if (!ast)
    {
        fprintf(stderr, "Error: Parsing failed for file '%s'.\n", input_file);
        return EXIT_FAILURE;
    }

    if (!var_res_pass(ast))
    {
        fprintf(stderr, "Error: Semantic Analyzer failed\n");
        return EXIT_FAILURE;
    }

    ir_program_t *ir_program = conv_ast_to_ir(ast);
    if (!ir_program)
    {
        fprintf(stderr, "Error: IR generation failed\n");
        return EXIT_FAILURE;
    }

    asm_program_t *asm_program = asm_first_pass(ir_program);
    if (!asm_program)
    {
        fprintf(stderr, "Error: First Assembly Generation pass failed\n");
        return EXIT_FAILURE;
    }

    int final_offset = 0;

    if (!asm_second_pass(asm_program, &final_offset))
    {
        fprintf(stderr, "Error: Second Assembly Generation pass failed\n");
        return EXIT_FAILURE;
    }

    if (!asm_third_pass(asm_program, final_offset))
    {
        fprintf(stderr, "Error: Third Assembly Generation pass failed\n");
        return EXIT_FAILURE;
    }

    if (!emit_asm_program(asm_program, output_file))
    {
        fprintf(stderr, "Error: Failed to write output to '%s'.\n", output_file);
        return EXIT_FAILURE;
    }

    printf("Assembly successfully written to '%s'.\n", output_file);
    return EXIT_SUCCESS;
}
