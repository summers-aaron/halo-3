#ifndef ZLIB_BLAM_ZLIB_MEMORY_TYPES_H
#define ZLIB_BLAM_ZLIB_MEMORY_TYPES_H

struct s_memory_pool;

// The opaque a zlib stream's blam_zlib_malloc / blam_zlib_free receive: the fixed pool the stream allocates from.
struct s_blam_zlib_memory
{
    s_memory_pool *heap;
};

#endif // ZLIB_BLAM_ZLIB_MEMORY_TYPES_H
