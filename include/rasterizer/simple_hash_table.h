#ifndef RASTERIZER_SIMPLE_HASH_TABLE_H
#define RASTERIZER_SIMPLE_HASH_TABLE_H

#include <string.h>
#include <stdlib.h>

// March 82a25118..82a25778 identifies the structure instance-marker hash
// as another specialization from this header. Its bodies belong to structures.cpp.
template <long bucket_count, long capacity, long first_multiplier, long second_multiplier>
class c_additive_hash
{
public:
    __forceinline c_additive_hash() : mask(bucket_count - 1) {}
    __forceinline void clear() { memset(buckets, -1, sizeof(buckets)); count = 0; }
    struct data_t { long value; unsigned short key; short next; };
    short buckets[bucket_count];
    data_t data[capacity];
    short count;
    unsigned short mask;

    __forceinline short hash_function(long first_hash, long second_hash)
    {
        return (short)(first_hash * first_multiplier + second_hash * second_multiplier) & mask;
    }

    long *element_exists(long first_hash, long second_hash, unsigned short key);
    bool add_element(long first_hash, long second_hash, unsigned short key, long value);
};

// March's assertions identify rasterizer/simple_hash_table.h. The inline
// operations below are shared by the occlusion and lens-flare specializations.
template<class T, long capacity, long multiplier0, long multiplier1, long multiplier2>
class c_simple_hash_table
{
public:
    struct s_entry
    {
        T value;
        long key0, key1, key2;
    };

    long entry_count;
    s_entry data[capacity];
    short hash_entry_count[capacity];
    short original_hash_index[capacity];

    __forceinline long calculate_hash(long key0, long key1, long key2)
    {
        return abs(key0 * multiplier0 + key2 * multiplier2 + key1 * multiplier1) % capacity;
    }

    __forceinline bool entry_is_free(long index)
    {
        return original_hash_index[index] == -1;
    }

    __forceinline long find_new_hash_entry(long original)
    {
        long index = original;
        long count = hash_entry_count[original];
        while (count > 0)
        {
            if (original_hash_index[index++ % capacity] == original)
                --count;
        }
        bool free;
        do
        {
            free = entry_is_free(index % capacity);
            if (!free) ++index;
        } while (!free);
        return index % capacity;
    }

    // match: 0x1835e0: keep iterator_next an ordinary inline COMDAT in the lens-flare loop.
    inline long iterator_next(long index)
    {
        if (entry_count == 0)
            index = -1;
        else
        {
            bool searching = true;
            while (searching)
            {
                if (index == capacity - 1)
                {
                    searching = false;
                    index = -1;
                }
                else
                {
                    index++;
                    searching = original_hash_index[index] == -1;
                }
            }
        }
        return index;
    }

    __forceinline T *get_element(long index)
    {
        return &data[index].value;
    }

    __forceinline void remove_entry(long index)
    {
        hash_entry_count[original_hash_index[index]]--;
        original_hash_index[index] = -1;
        entry_count--;
    }

    long find_entry(long key0, long key1, long key2);
    long add_entry(long key0, long key1, long key2);
};

#endif // RASTERIZER_SIMPLE_HASH_TABLE_H
