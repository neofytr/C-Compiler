#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_FILE_SIZE 1024
#define MAX_TOKEN_LENGTH 128
#define MAX_TOKEN_NUMBER 128

typedef enum
{
    TOKEN_OPENING_BRACES,
    TOKEN_CLOSING_BRACES,
    TOKEN_OPENING_PARENTHESIS,
    TOKEN_CLOSING_PARENTHESIS,
    TOKEN_KEYWORD_INT,
    TOKEN_KEYWORD_VOID,
    TOKEN_KEYWORD_RETURN,
    TOKEN_IDENTIFIER,
    TOKEN_SEMICOLON,
    TOKEN_CONSTANT_INT,
} token_type_t;

typedef struct
{
    char token_value[MAX_TOKEN_LENGTH];
    token_type_t token_type;
} token_t;

static token_t token_list[MAX_TOKEN_NUMBER] = {0};
static size_t token_list_index = 0;

static inline bool is_identifier_part(uint8_t c)
{
    return isalpha(c) || c == '_';
}

static inline bool is_const_int_part(uint8_t c)
{
    return isdigit(c) || c == '_';
}

static bool add_into_token_list(uint8_t *start, uint8_t *end, token_type_t token_type)
{
    if ((uintptr_t)end < (uintptr_t)start)
    {
        return false;
    }

    if ((uintptr_t)end - (uintptr_t)start + 1 > MAX_TOKEN_LENGTH)
    {
        return false;
    }

    uint8_t *ptr = start;
    size_t counter = 0;

    token_list[token_list_index].token_type = token_type;

    for (; (uintptr_t)ptr < (uintptr_t)end; ptr++, counter++)
    {
        token_list[token_list_index].token_value[counter] = *ptr;
    }

    return true;
}

int main(int argc, char **argv)
{
    uint8_t source_file[MAX_FILE_SIZE];
    FILE *source_file_stream;
    if (!(source_file_stream = fopen(argv[1], "r")))
    {
        fprintf(stderr, "Error opening %s\n", argv[1]);
        perror("fopen");
        return EXIT_FAILURE;
    }

    if (fseek(source_file_stream, 0, SEEK_END))
    {
        fprintf(stderr, "Error seeking into %s\n", argv[1]);
        perror("fseek");
        return EXIT_FAILURE;
    }

    size_t source_file_size;
    if ((source_file_size = ftell(source_file_stream)) < 0)
    {
        fprintf(stderr, "Error seeking into %s\n", argv[1]);
        perror("ftell");
        return EXIT_FAILURE;
    }

    if (fseek(source_file_stream, 0, SEEK_SET))
    {
        fprintf(stderr, "Error seeking into %s\n", argv[1]);
        perror("fseek");
        return EXIT_FAILURE;
    }

    if (source_file_size > MAX_FILE_SIZE - 1)
    {
        fprintf(stderr, "Error: File %s size too large\n", argv[1]);
        return EXIT_FAILURE;
    }

    size_t read_source_size = fread(source_file, sizeof(uint8_t), source_file_size, source_file_stream);
    if (read_source_size != source_file_size)
    {
        fprintf(stderr, "Error reading into the file %s\n", argv[1]);
        perror("fread");
        return EXIT_FAILURE;
    }

    source_file[read_source_size] = '\0'; // null-terminate the string file
    uint8_t *source_file_ptr = source_file;

    while (*source_file_ptr != (uint8_t)'\0')
    {
        if (is_identifier_part(*source_file_ptr))
        {
            uint8_t *start = source_file_ptr;
            size_t len = 0;

            do
            {
                len++;
                source_file_ptr++;
            } while (is_identifier_part(*source_file_ptr));

            if (is_const_int_part(*source_file_ptr))
            {
            }
        }

        source_file_ptr++;
    }

    fprintf(stdout, "%s\n", source_file);
}