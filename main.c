#include "./src/lexer.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    size_t file_size;
    uint8_t *source = read_source_file(argv[1], &file_size);
    if (!source) {
        return EXIT_FAILURE;
    }

    token_list_t token_list = {0};
    if (!lex_source(source, &token_list)) {
        fprintf(stderr, "Lexing failed\n");
        free(source);
        return EXIT_FAILURE;
    }

    printf("Tokens:\n");
    for (size_t i = 0; i < token_list.count; i++) {
        printf("Line %zu, Col %zu: %-15s '%s'\n", 
               token_list.tokens[i].line,
               token_list.tokens[i].column,
               token_type_to_string(token_list.tokens[i].type),
               token_list.tokens[i].value);
    }

    free(source);
    return EXIT_SUCCESS;
}