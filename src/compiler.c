#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./code_emitter/code_emitter.h"
#include "./parser/parser.h"

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

    token_list_t token_list = {0};
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

    asm_program_t *asm_program = generate_assembly_ast(ast);
    if (!asm_program)
    {
        fprintf(stderr, "Error: Assembly generation failed.\n");
        return EXIT_FAILURE;
    }

    if (!emit_asm(asm_program, output_file))
    {
        fprintf(stderr, "Error: Failed to write output to '%s'.\n", output_file);
        return EXIT_FAILURE;
    }

    printf("Assembly successfully written to '%s'.\n", output_file);
    return EXIT_SUCCESS;
}
