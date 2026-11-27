#ifndef MEMORY_DATUM_INDEX_H
#define MEMORY_DATUM_INDEX_H

static __forceinline long datum_index_to_absolute_index(long index)
{
    return index & 0xffff;
}

#endif // MEMORY_DATUM_INDEX_H
