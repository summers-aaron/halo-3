#ifndef PHYSICS_HAVOK_RUNTIME_POOL_CACHE_ENTRY_TYPES_H
#define PHYSICS_HAVOK_RUNTIME_POOL_CACHE_ENTRY_TYPES_H

struct s_havok_runtime_pool_cache_entry
{
    long key;
    short type;
    short hash;
    short next;
    short previous;
};

#endif // PHYSICS_HAVOK_RUNTIME_POOL_CACHE_ENTRY_TYPES_H
