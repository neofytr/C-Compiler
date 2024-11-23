#ifndef EF013AD5_C306_4868_9D0B_F237FCCEDD09
#define EF013AD5_C306_4868_9D0B_F237FCCEDD09

#include "lexer.h"

typedef struct function_definition_ function_definition_t;
typedef struct program_ program_t;
typedef struct identifier_ identifier_t;
typedef struct statement_ statement_t;
typedef struct return_statement_ return_statement_t;
typedef struct expression_ expression_t;

typedef enum {
    CONSTANT_INT,   
} expression_type_t;

struct expression_ {
    expression_type_t expression_type;  
    union {
        int constant_int;

    } expression;
};

struct return_statement_ {
    expression_t *expression;
};

typedef enum {
    RETURN,
} statement_type_t;

struct statement_ {
    statement_type_t statement_type;  
    union {
        return_statement_t return_statement;
    } statement;
};

struct function_definition_ {
    identifier_t *name;  
    statement_t *body;   
};

struct program_ {
    function_definition_t *function_definition;
};

#endif /* EF013AD5_C306_4868_9D0B_F237FCCEDD09 */