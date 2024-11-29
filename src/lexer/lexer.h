#ifndef EE5F256B_8CA4_47A7_9F4A_CC9C78595A63
#define EE5F256B_8CA4_47A7_9F4A_CC9C78595A63

#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#define MAX_FILE_SIZE 1024
#define MAX_TOKEN_LENGTH 128
#define MAX_TOKEN_NUMBER 128

/*
If an identifier is not defined in the current scope and is used in the program
but the identifier doesn't have a valid name, then the lexer will catch the error
instead of the program analyzer saying that the identifier is undefined in the current scope
*/

typedef enum
{
    TOKEN_OPENING_BRACE,               // {
    TOKEN_CLOSING_BRACE,               // }
    TOKEN_OPENING_PAREN,               // (
    TOKEN_CLOSING_PAREN,               // )
    TOKEN_KEYWORD_INT,                 // int
    TOKEN_KEYWORD_VOID,                // void
    TOKEN_KEYWORD_RETURN,              // return
    TOKEN_KEYWORD_IF,                  // if
    TOKEN_KEYWORD_ELSE,                // else
    TOKEN_KEYWORD_WHILE,               // while
    TOKEN_KEYWORD_FOR,                 // for
    TOKEN_KEYWORD_BREAK,               // break
    TOKEN_KEYWORD_CONTINUE,            // continue
    TOKEN_KEYWORD_CONST,               // const
    TOKEN_IDENTIFIER,                  // [a-zA-Z_][a-zA-Z_]*
    TOKEN_SEMICOLON,                   // ;
    TOKEN_CONSTANT,                    // [0-9]+(-[0-9]+)*
    TOKEN_OPERATOR_PLUS,               // +
                                       //    TOKEN_OPERATOR_MINUS,         // -
    TOKEN_OPERATOR_MUL,                // *
    TOKEN_OPERATOR_DIV,                // /
    TOKEN_OPERATOR_ASSIGN,             // =
    TOKEN_OPERATOR_EQUAL,              // ==
    TOKEN_OPERATOR_NOT_EQUAL,          // !=
    TOKEN_OPERATOR_LESS,               // <
    TOKEN_OPERATOR_LESS_EQUAL,         // <=
    TOKEN_OPERATOR_GREATER,            // >
    TOKEN_OPERATOR_GREATER_EQUAL,      // >=
    TOKEN_OPERATOR_BITWISE_COMPLEMENT, // ~
    TOKEN_OPERATOR_NEGATION,           // -
    TOKEN_OPERATOR_DECREMENT,          // --
    TOKEN_ERROR                        // For error reporting
} token_type_t;

typedef struct
{
    char value[MAX_TOKEN_LENGTH];
    token_type_t type;
    size_t line;
    size_t column;
} token_t;

typedef struct
{
    const char *keyword;
    token_type_t type;
} keyword_map_t;

static const keyword_map_t KEYWORDS[] = {
    {"int", TOKEN_KEYWORD_INT},
    {"void", TOKEN_KEYWORD_VOID},
    {"return", TOKEN_KEYWORD_RETURN},
    {"if", TOKEN_KEYWORD_IF},
    {"else", TOKEN_KEYWORD_ELSE},
    {"while", TOKEN_KEYWORD_WHILE},
    {"for", TOKEN_KEYWORD_FOR},
    {"break", TOKEN_KEYWORD_BREAK},
    {"continue", TOKEN_KEYWORD_CONTINUE},
    {"const", TOKEN_KEYWORD_CONST},
};

static const size_t KEYWORD_COUNT = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

typedef struct
{
    token_t tokens[MAX_TOKEN_NUMBER];
    size_t count;
} token_list_t;

typedef struct
{
    const uint8_t *input;
    size_t position;
    size_t line;
    size_t column;
} lexer_t;

static inline bool is_alpha(uint8_t c);
static inline bool is_digit(uint8_t c);
static inline bool is_identifier_start(uint8_t c);
static inline bool is_identifier_part(uint8_t c);
static inline bool is_whitespace(uint8_t c);
static void advance_lexer(lexer_t *lexer);
static void retreat_lexer(lexer_t *lexer);
static inline uint8_t peek(lexer_t *lexer);
static token_type_t check_keyword(const char *value);
static bool add_token(token_list_t *list, const char *value, token_type_t type, size_t line, size_t column);
static bool lex_identifier(lexer_t *lexer, token_list_t *list);
static bool lex_number(lexer_t *lexer, token_list_t *list);
static bool lex_symbol(lexer_t *lexer, token_list_t *list);
static bool lex_source(const uint8_t *source, token_list_t *list);
static uint8_t *read_source_file(const char *filename, size_t *file_size);
static const char *token_type_to_string(token_type_t type);

static inline bool is_alpha(uint8_t c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static inline bool is_digit(uint8_t c)
{
    return c >= '0' && c <= '9';
}

static inline bool is_identifier_start(uint8_t c)
{
    return is_alpha(c) || c == '_';
}

static inline bool is_identifier_part(uint8_t c)
{
    return is_alpha(c) || c == '_';
}

static inline bool is_whitespace(uint8_t c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

static void advance_lexer(lexer_t *lexer)
{
    if (lexer->input[lexer->position] == '\n')
    {
        lexer->line++;
        lexer->column = 1;
    }
    else
    {
        lexer->column++;
    }
    lexer->position++;
}

static void retreat_lexer(lexer_t *lexer)
{
    if (lexer->position == 0)
    {
        return;
    }

    lexer->position--;

    if (lexer->input[lexer->position] == '\n')
    {
        lexer->line--;
        lexer->column = 1;
        size_t temp_pos = lexer->position - 1;
        while (temp_pos > 0 && lexer->input[temp_pos] != '\n')
        {
            lexer->column++;
            temp_pos--;
        }
    }
    else
    {
        lexer->column--;
    }
}

static inline uint8_t peek(lexer_t *lexer)
{
    return lexer->input[lexer->position];
}

static token_type_t check_keyword(const char *value)
{
    for (size_t i = 0; i < KEYWORD_COUNT; i++)
    {
        if (strcmp(value, KEYWORDS[i].keyword) == 0)
        {
            return KEYWORDS[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

static bool add_token(token_list_t *list, const char *value, token_type_t type, size_t line, size_t column)
{
    if (list->count >= MAX_TOKEN_NUMBER)
    {
        return false;
    }

    token_t *token = &list->tokens[list->count];
    strncpy(token->value, value, MAX_TOKEN_LENGTH - 1);
    token->value[MAX_TOKEN_LENGTH - 1] = '\0';
    token->type = type;
    token->line = line;
    token->column = column;
    list->count++;
    return true;
}

static bool lex_identifier(lexer_t *lexer, token_list_t *list)
{
    size_t start_column = lexer->column;
    char buffer[MAX_TOKEN_LENGTH] = {0};
    size_t length = 0;
    bool has_error = false;

    if (!is_identifier_start(peek(lexer)))
    {
        return false;
    }

    while ((is_identifier_part(peek(lexer)) || is_digit(peek(lexer))) &&
           length < MAX_TOKEN_LENGTH - 1)
    {
        if (is_digit(peek(lexer)) && !has_error)
        {
            has_error = true;
        }
        buffer[length++] = peek(lexer);
        advance_lexer(lexer);
    }

    buffer[length] = '\0';

    if (has_error)
    {
        fprintf(stderr, "Error at line %zu, column %zu: Invalid identifier '%s' (contains digits)\n",
                lexer->line, start_column, buffer);
        return false;
    }

    token_type_t type = check_keyword(buffer);
    return add_token(list, buffer, type, lexer->line, start_column);
}

static bool lex_number(lexer_t *lexer, token_list_t *list)
{
    size_t start_pos = lexer->position;
    size_t start_column = lexer->column;
    char buffer[MAX_TOKEN_LENGTH] = {0};
    size_t length = 0;
    bool has_error = false;
    bool last_was_underscore = false;
    char error_type[64] = {0};

    if (peek(lexer) == '-')
    {
        buffer[length++] = peek(lexer);
        advance_lexer(lexer);
        if (!is_digit(peek(lexer)))
        {
            has_error = true;
            strncpy(error_type, "Expected digit after minus sign", sizeof(error_type) - 1);
        }
    }

    if (!is_digit(peek(lexer)) && !has_error)
    {
        return false;
    }

    while (length < MAX_TOKEN_LENGTH - 1)
    {
        if (is_digit(peek(lexer)))
        {
            buffer[length++] = peek(lexer);
            advance_lexer(lexer);
            last_was_underscore = false;
        }
        else if (peek(lexer) == '_')
        {
            if (last_was_underscore && !has_error)
            {
                has_error = true;
                strncpy(error_type, "Cannot have consecutive underscores in number", sizeof(error_type) - 1);
            }
            buffer[length++] = peek(lexer);
            advance_lexer(lexer);
            last_was_underscore = true;
        }
        else if (is_alpha(peek(lexer)) || peek(lexer) == '-')
        {
            if (!has_error)
            {
                has_error = true;
                strncpy(error_type, "Invalid number format", sizeof(error_type) - 1);
            }
            buffer[length++] = peek(lexer);
            advance_lexer(lexer);
        }
        else
        {
            break;
        }
    }

    buffer[length] = '\0';

    if (last_was_underscore && !has_error)
    {
        has_error = true;
        strncpy(error_type, "Number cannot end with underscore", sizeof(error_type) - 1);
    }

    if (has_error)
    {
        fprintf(stderr, "Error at line %zu, column %zu: %s '%s'\n",
                lexer->line, start_column, error_type, buffer);
        return false;
    }

    return add_token(list, buffer, TOKEN_CONSTANT, lexer->line, start_column);
}

static bool lex_symbol(lexer_t *lexer, token_list_t *list)
{
    char symbol[3] = {peek(lexer), '\0', '\0'}; // Support for two-character operators
    token_type_t type;
    bool valid = true;

    switch (peek(lexer))
    {
    case '{':
        type = TOKEN_OPENING_BRACE;
        break;
    case '}':
        type = TOKEN_CLOSING_BRACE;
        break;
    case '(':
        type = TOKEN_OPENING_PAREN;
        break;
    case '~':
        type = TOKEN_OPERATOR_BITWISE_COMPLEMENT;
        break;
    case ')':
        type = TOKEN_CLOSING_PAREN;
        break;
    case ';':
        type = TOKEN_SEMICOLON;
        break;
    case '+':
        type = TOKEN_OPERATOR_PLUS;
        break;
    case '-':
        advance_lexer(lexer);
        if (peek(lexer) == '-')
        {
            symbol[1] = '-';
            type = TOKEN_OPERATOR_DECREMENT;
        }
        else
        {
            type = TOKEN_OPERATOR_NEGATION;
            retreat_lexer(lexer);
        }
        break;
    case '*':
        type = TOKEN_OPERATOR_MUL;
        break;
    case '/':
        type = TOKEN_OPERATOR_DIV;
        break;
    case '=':
        advance_lexer(lexer); // Lookahead for ==
        if (peek(lexer) == '=')
        {
            symbol[1] = '=';
            type = TOKEN_OPERATOR_EQUAL;
        }
        else
        {
            type = TOKEN_OPERATOR_ASSIGN;
            retreat_lexer(lexer); // Retract if no ==
        }
        break;
    case '!':
        advance_lexer(lexer); // Lookahead for !=
        if (peek(lexer) == '=')
        {
            symbol[1] = '=';
            type = TOKEN_OPERATOR_NOT_EQUAL;
        }
        else
        {
            valid = false; // '!' alone is not valid in this grammar
            retreat_lexer(lexer);
        }
        break;
    case '<':
        advance_lexer(lexer); // Lookahead for <=
        if (peek(lexer) == '=')
        {
            symbol[1] = '=';
            type = TOKEN_OPERATOR_LESS_EQUAL;
        }
        else
        {
            type = TOKEN_OPERATOR_LESS;
            retreat_lexer(lexer);
        }
        break;
    case '>':
        advance_lexer(lexer); // Lookahead for >=
        if (peek(lexer) == '=')
        {
            symbol[1] = '=';
            type = TOKEN_OPERATOR_GREATER_EQUAL;
        }
        else
        {
            type = TOKEN_OPERATOR_GREATER;
            retreat_lexer(lexer);
        }
        break;
    default:
        valid = false;
    }

    if (valid)
    {
        advance_lexer(lexer);
        return add_token(list, symbol, type, lexer->line, lexer->column - 1);
    }
    return false;
}

static bool lex_source(const uint8_t *source, token_list_t *list)
{
    lexer_t lexer = {
        .input = source,
        .position = 0,
        .line = 1,
        .column = 1};

    while (peek(&lexer) != '\0')
    {
        if (is_whitespace(peek(&lexer)))
        {
            advance_lexer(&lexer);
            continue;
        }

        if (is_identifier_start(peek(&lexer)))
        {
            if (!lex_identifier(&lexer, list))
            {
                return false;
            }
            continue;
        }

        if (is_digit(peek(&lexer)))
        {
            if (!lex_number(&lexer, list))
            {
                return false;
            }
            continue;
        }

        if (!lex_symbol(&lexer, list))
        {
            fprintf(stderr, "Error at line %zu, column %zu: Invalid character '%c'\n",
                    lexer.line, lexer.column, peek(&lexer));
            return false;
        }
    }

    return true;
}

static uint8_t *read_source_file(const char *filename, size_t *file_size)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("fopen");
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0)
    {
        perror("fseek");
        fclose(file);
        return NULL;
    }

    long size = ftell(file);
    if (size < 0 || size > MAX_FILE_SIZE - 1)
    {
        fprintf(stderr, "File size error or file too large\n");
        fclose(file);
        return NULL;
    }

    if (fseek(file, 0, SEEK_SET) != 0)
    {
        perror("fseek");
        fclose(file);
        return NULL;
    }

    uint8_t *buffer = (uint8_t *)malloc(size + 1);
    if (!buffer)
    {
        perror("malloc");
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, size, file);
    if (read_size != (size_t)size)
    {
        perror("fread");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[size] = '\0';
    *file_size = size;
    fclose(file);
    return buffer;
}

static const char *token_type_to_string(token_type_t type)
{
    switch (type)
    {
    case TOKEN_OPENING_BRACE:
        return "OPENING_BRACE";
    case TOKEN_CLOSING_BRACE:
        return "CLOSING_BRACE";
    case TOKEN_OPENING_PAREN:
        return "OPENING_PAREN";
    case TOKEN_CLOSING_PAREN:
        return "CLOSING_PAREN";
    case TOKEN_KEYWORD_INT:
        return "KEYWORD_INT";
    case TOKEN_KEYWORD_VOID:
        return "KEYWORD_VOID";
    case TOKEN_KEYWORD_RETURN:
        return "KEYWORD_RETURN";
    case TOKEN_KEYWORD_IF:
        return "KEYWORD_IF";
    case TOKEN_KEYWORD_ELSE:
        return "KEYWORD_ELSE";
    case TOKEN_KEYWORD_WHILE:
        return "KEYWORD_WHILE";
    case TOKEN_KEYWORD_FOR:
        return "KEYWORD_FOR";
    case TOKEN_KEYWORD_BREAK:
        return "KEYWORD_BREAK";
    case TOKEN_KEYWORD_CONTINUE:
        return "KEYWORD_CONTINUE";
    case TOKEN_KEYWORD_CONST:
        return "KEYWORD_CONST";
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER";
    case TOKEN_SEMICOLON:
        return "SEMICOLON";
    case TOKEN_CONSTANT:
        return "CONSTANT";
    case TOKEN_OPERATOR_PLUS:
        return "OPERATOR_PLUS";
    case TOKEN_OPERATOR_NEGATION:
        return "OPERATOR_NEGATION";
    case TOKEN_OPERATOR_MUL:
        return "OPERATOR_MUL";
    case TOKEN_OPERATOR_DIV:
        return "OPERATOR_DIV";
    case TOKEN_OPERATOR_ASSIGN:
        return "OPERATOR_ASSIGN";
    case TOKEN_OPERATOR_EQUAL:
        return "OPERATOR_EQUAL";
    case TOKEN_OPERATOR_NOT_EQUAL:
        return "OPERATOR_NOT_EQUAL";
    case TOKEN_OPERATOR_LESS:
        return "OPERATOR_LESS";
    case TOKEN_OPERATOR_LESS_EQUAL:
        return "OPERATOR_LESS_EQUAL";
    case TOKEN_OPERATOR_GREATER:
        return "OPERATOR_GREATER";
    case TOKEN_OPERATOR_GREATER_EQUAL:
        return "OPERATOR_GREATER_EQUAL";
    case TOKEN_OPERATOR_BITWISE_COMPLEMENT:
        return "OPERATOR_BITWISE_COMPLEMENT";
    case TOKEN_OPERATOR_DECREMENT:
        return "OPERATOR_DECREMENT";
    case TOKEN_ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

#endif /* EE5F256B_8CA4_47A7_9F4A_CC9C78595A63 */
