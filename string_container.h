#ifndef C0F80EE6_E721_4FCE_8867_4EA72DC18A30
#define C0F80EE6_E721_4FCE_8867_4EA72DC18A30

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum
{
    SUCCESS,
    FAILURE,
    NULL_STRING,
    EMPTY_STRING,
} string_error_t;

typedef struct
{
    char *str;
    size_t len;
} string_t;

string_t conv_to_string(char *str)
{
    return (string_t){.str = str, .len = strlen(str)};
}

string_error_t strip_left(string_t *string)
{
    if (!string)
    {
        return NULL_STRING;
    }

    char *str = string->str;
    size_t len = string->len;

    for (size_t i = 0; i < len; i++)
    {
        if (isspace(str[i]))
        {
            string->str++;
            string->len--;
        }
        else
        {
            break;
        }
    }

    return SUCCESS;
}

string_error_t strip_right(string_t *string)
{
    if (!string->len)
    {
        return EMPTY_STRING;
    }

    if (!string)
    {
        return NULL_STRING;
    }

    char *str = string->str;
    size_t len = string->len;

    for (size_t i = len - 1; i >= 0; i--)
    {
        if (isspace(str[i]))
        {
            string->len--;
        }
        else
        {
            break;
        }
    }

    return SUCCESS;
}

#endif /* C0F80EE6_E721_4FCE_8867_4EA72DC18A30 */
