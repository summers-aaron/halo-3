#ifndef MEMORY_HASHTABLE_H
#define MEMORY_HASHTABLE_H

// Functions defined in tu/blamlibxbox/hashtable.cpp that other units call,
// plus the March inlines release callers expand: hash_table_new and hash_table_dispose.

#include "memory/hashtable_types.h"
#include "memory/hashtable_reset.h"
#include "memory/data.h"
#include "cseries/cseries.h"

struct s_hash_table;

bool hash_table_remove(struct s_hash_table * _sym_p0, void const * key);
extern "C" void hash_table_add(s_hash_table *table);

struct s_hash_table_bucket;

struct s_hash_table_bucket * hash_table_find_internal(struct s_hash_table * table, void const * key);

extern "C" bool hash_table_insert(s_hash_table *table, void *key, void *value);

static __forceinline void hash_table_dispose(s_hash_table *table)
{
    if (table)
        table->allocation->deallocate(table);
}

// March hash_table_new (82c780f0); release inlines it into every caller.
static __forceinline s_hash_table *hash_table_new(const char *name, unsigned int entry_size, long count, long entry_count,
    unsigned long (*hash)(void *), unsigned char (*equal)(void *, void *), c_allocation_base *allocation)
{
    s_hash_table *table = static_cast<s_hash_table *>(allocation->allocate(
        offsetof(s_hash_table, buckets) + count * sizeof(s_hash_table_bucket *) + entry_count * (entry_size + sizeof(s_hash_table_bucket)),
        name));
    if (table) {
        // match: 0x1f15c0: March name.set() is csstrnzcpy; the written-out c_static_string::set lets the +0x34 store pass the +0x1f terminator
        csstrnzcpy(table->name.get_buffer(), name, sizeof(table->name));
        table->count = count;
        table->entry_count = entry_count;
        table->hash = hash;
        table->equal = equal;
        table->allocation = allocation;
        table->entry_size = entry_size;
        hash_table_reset(table);
    }
    return table;
}


// March c_hash_table<t_value, t_key>: the typed owner of one s_hash_table.
template <typename t_value, typename t_key>
class c_hash_table
{
public:
    bool created() const { return m_hash_table != 0; }

    void dispose()
    {
        hash_table_dispose(m_hash_table);
        m_hash_table = 0;
    }

    s_hash_table *m_hash_table;
};

#endif // MEMORY_HASHTABLE_H
