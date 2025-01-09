#ifndef B9B56047_F79C_4390_97DE_477FC9D07A9D
#define B9B56047_F79C_4390_97DE_477FC9D07A9D

#include <stdlib.h>
#include <string.h>
#include "../allocator/allocator.h"

#define NUM_BUCKETS_VAR 1024 // should always be a power of two
#define MAX_IDENTIFIER_LENGTH_VAR 256

typedef struct var_node
{
    char var[MAX_IDENTIFIER_LENGTH_VAR];
    char unique_name[MAX_IDENTIFIER_LENGTH_VAR];
    struct var_node *next_node;
} var_node_t;

typedef struct
{
    var_node_t *buckets[NUM_BUCKETS_VAR];
    size_t total_entries;
} var_table_t;

var_table_t *create_var_table();
void destroy_var_table(var_table_t *table);
bool var_table_insert(var_table_t *table, const char *var, const char *unique_name);
var_node_t *var_table_search(var_table_t *table, const char *var);
bool var_table_delete(var_table_t *table, const char *var);
void var_table_clear(var_table_t *table);
size_t var_table_size(var_table_t *table);

size_t get_var_hash(const char *identifier)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *identifier++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % NUM_BUCKETS_VAR;
}

var_table_t *create_var_table()
{
    var_table_t *table = (var_table_t *)allocate(sizeof(var_table_t));
    if (!table)
    {
        return NULL;
    }

    memset(table->buckets, 0, sizeof(table->buckets));
    table->total_entries = 0;

    return table;
}

void destroy_var_table(var_table_t *table)
{
    if (!table)
    {
        return;
    }

    var_table_clear(table);

    deallocate(table);
}

bool var_table_insert(var_table_t *table, const char *var, const char *unique_var)
{
    if (!table || !var || strlen(var) >= MAX_IDENTIFIER_LENGTH_VAR)
    {
        return false;
    }

    if (var_table_search(table, var))
    {
        return false;
    }

    size_t hash = get_var_hash(var);

    var_node_t *new_node = (var_node_t *)allocate(sizeof(var_node_t));
    if (!new_node)
    {
        return false;
    }

    strncpy(new_node->var, var, MAX_IDENTIFIER_LENGTH_VAR - 1);
    new_node->var[MAX_IDENTIFIER_LENGTH_VAR - 1] = '\0';
    strncpy(new_node->unique_name, unique_var, MAX_IDENTIFIER_LENGTH_VAR - 1);
    new_node->unique_name[MAX_IDENTIFIER_LENGTH_VAR - 1] = '\0';

    new_node->next_node = table->buckets[hash];
    table->buckets[hash] = new_node;

    table->total_entries++;

    return true;
}

var_node_t *var_table_search(var_table_t *table, const char *var)
{
    if (!table || !var)
    {
        return NULL;
    }

    size_t hash = get_var_hash(var);
    var_node_t *current = table->buckets[hash];

    while (current)
    {
        if (!strcmp(current->var, var))
        {
            return current;
        }
        current = current->next_node;
    }

    return NULL;
}

bool var_table_delete(var_table_t *table, const char *var)
{
    if (!table || !var)
    {
        return false;
    }

    size_t hash = get_var_hash(var);
    var_node_t *current = table->buckets[hash];
    var_node_t *prev = NULL;

    while (current)
    {
        if (strcmp(current->var, var) == 0)
        {
            if (prev)
            {
                prev->next_node = current->next_node;
            }
            else
            {
                table->buckets[hash] = current->next_node;
            }

            deallocate(current);
            table->total_entries--;

            return true;
        }

        prev = current;
        current = current->next_node;
    }

    return false;
}

void var_table_clear(var_table_t *table)
{
    if (!table)
    {
        return;
    }

    for (size_t i = 0; i < NUM_BUCKETS_VAR; i++)
    {
        var_node_t *current = table->buckets[i];
        while (current)
        {
            var_node_t *temp = current;
            current = current->next_node;
            deallocate(temp);
        }
        table->buckets[i] = NULL;
    }

    table->total_entries = 0;
}

size_t var_table_size(var_table_t *table)
{
    return table ? table->total_entries : 0;
}

#undef NUM_BUCKETS_VAR
#undef MAX_IDENTIFIER_LENGTH_VAR

#endif /* B9B56047_F79C_4390_97DE_477FC9D07A9D */
