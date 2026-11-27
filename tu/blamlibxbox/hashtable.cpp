// blamlibxbox/hashtable.obj: from the records 0x286f08 0x286fb0 0x2870a0 0x287140
// @flags /TP /O2 /Oc
// @address 0x286f08 hash_table_reset
// @address 0x286fb0 hash_table_add
// @address 0x2870a0 ?hash_table_find_internal@@YAPAUs_hash_table_bucket@@PAUs_hash_table@@PBX@Z
// @address 0x287140 ?hash_table_remove@@YA_NPAUs_hash_table@@PBX@Z
#include <string.h>
#include "memory/hashtable.h"
#include "memory/hashtable_types.h"

/* the table: hash buckets, then a pool of entries (key, hash, next, then entry_size bytes) */
extern "C" void hash_table_reset(s_hash_table *pool)
{
    int offset;
    int i;
    int stride;
    int header_bytes = pool->count * (int)sizeof(s_hash_table_bucket *);
    memset(pool->buckets, 0, (pool->entry_size + (int)sizeof(s_hash_table_bucket)) * pool->entry_count + header_bytes);
    pool->entries = (char *)pool->buckets + header_bytes;
    stride = pool->entry_size + (int)sizeof(s_hash_table_bucket);
    pool->last = 0;
    offset = 0;
    i = pool->entry_count;
    if (i > 0) do {
        s_hash_table_bucket *entry = (s_hash_table_bucket *)(pool->entries + offset);
        offset += stride;
        entry->next = pool->last;
        pool->last = entry;
        i--;
    } while ((unsigned)i != 0);
}

extern "C" void hash_table_add(s_hash_table *table)
{
    long bucket_count = table->count;
    long entry_stride = table->entry_size + sizeof(s_hash_table_bucket);
    char *relocated_entries = (char *)&table->buckets[bucket_count];
    for (long bucket_index = 0; bucket_index < (long)table->count; ++bucket_index)
    {
        s_hash_table_bucket *bucket = table->buckets[bucket_index];
        if (bucket)
        {
            unsigned long stride = table->entry_size + sizeof(s_hash_table_bucket);
            table->buckets[bucket_index] = (s_hash_table_bucket *)(relocated_entries +
                ((unsigned long)((char *)bucket - table->entries) / stride) * stride);
        }
    }
    if (table->last)
    {
        unsigned long stride = table->entry_size + sizeof(s_hash_table_bucket);
        table->last = (s_hash_table_bucket *)(relocated_entries +
            ((unsigned long)((char *)table->last - table->entries) / stride) * stride);
    }
    char *entry_address = relocated_entries;
    for (long entry_index = 0; entry_index < table->entry_count; ++entry_index)
    {
        s_hash_table_bucket *entry = (s_hash_table_bucket *)entry_address;
        s_hash_table_bucket *next = entry->next;
        if (next)
        {
            unsigned long stride = table->entry_size + sizeof(s_hash_table_bucket);
            entry->next = (s_hash_table_bucket *)(relocated_entries +
                ((unsigned long)((char *)next - table->entries) / stride) * stride);
        }
        entry_address += entry_stride;
    }
    table->entries = relocated_entries;
}

struct s_hash_table_bucket * hash_table_find_internal(struct s_hash_table * table, void const * key)
{
    s_hash_table_bucket *result = 0;
    unsigned long hash = table->hash(((void *)key));
    s_hash_table_bucket *node = table->buckets[hash % table->count];
    while (node) {
        if (hash == node->hash && table->equal(((void *)key), node->key)) {
            result = node;
            break;
        }
        node = node->next;
    }
    return (s_hash_table_bucket *)result;
}

bool hash_table_remove(struct s_hash_table * table, void const * key)
{
    bool result = false;
    unsigned long hash = table->hash((void *)key);
    unsigned long index = hash % table->count;
    s_hash_table_bucket *current = table->buckets[index];
    s_hash_table_bucket *previous = 0;

    while (current != 0) {
        if (table->equal((void *)key, current->key) != 0)
            break;
        previous = current;
        current = current->next;
    }

    if (current != 0) {
        if (previous == 0)
            table->buckets[index] = current->next;
        else
            previous->next = current->next;
        current->next = table->last;
        table->last = current;
        result = true;
    }
    return result;
}
