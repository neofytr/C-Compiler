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
    uint8_t *str;
    size_t len;
} string_t;

string_t conv_to_string(uint8_t *str)
{
    return (string_t){.str = str, .len = strlen((const char *)str)};
}

string_error_t strip_left(string_t *string)
{
    if (!string)
    {
        return NULL_STRING;
    }

    uint8_t *str = string->str;
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

    uint8_t *str = string->str;
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

string_error_t strip(string_t *string)
{
    string_error_t error = strip_left(string);
    if (error != SUCCESS)
    {
        return error;
    }

    return strip_right(string);
}

#endif /* C0F80EE6_E721_4FCE_8867_4EA72DC18A30 */
