#include <stdio.h>
#include <stdint.h>
#include "string_container.h"

#define MAX_FILE_SIZE 1024
int main(int argc, char **argv)
{
    /* uint8_t source_file[MAX_FILE_SIZE];
    FILE *source_file_stream = fopen(argv[1], "r");

    size_t source_file_size = fseek(source_file_stream, 0, SEEK_END);
    fseek(source_file_stream, 0, SEEK_SET);

    fread(source_file, sizeof(uint8_t), source_file_size, source_file_stream);
*/
    char spaces[] = "   \n\n\n456 hello\n\n\n\n";
    string_t string = conv_to_string(spaces);

    fprintf(stdout, "%d\n", (int)string.len); 

    strip_left(&string);
    strip_right(&string);
    fprintf(stdout, "%d\n", (int)string.len);
    fprintf(stdout, "%.*s\n", (int)string.len, string.str); 
}