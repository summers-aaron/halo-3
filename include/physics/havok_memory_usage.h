#ifndef PHYSICS_HAVOK_MEMORY_USAGE_H
#define PHYSICS_HAVOK_MEMORY_USAGE_H

struct s_havok_runtime_pool_bucket_usage
{
    long page_count;
    long allocation_count;
    long allocation_size;
};

struct s_havok_pool_memory_usage
{
    union { int pool_size; long memory_size; };
    union { int bytes_in_use; long memory_used; };
    int allocation_count;
    union { int extra; long debug_memory; };
};
struct s_havok_2nd_pool_memory_usage
{
    union { int pool_size; long memory_size; };
    union { int bytes_in_use; long memory_used; };
    union { int extra; long debug_memory; };
    int allocation_count;
    union { int last; long last_allocation_size; };
};
struct s_havok_runtime_memory_usage
{
    union { int pool_size; long memory_size; };
    union { int block_count; long page_count; };
    union { int bytes_in_use; long pages_used; };
    union { int block_size; long page_size; };
    union { int free_blocks; long last_page_free; };
    union { float fragmentation; float memory_fraction; };
    union { char records[192]; s_havok_runtime_pool_bucket_usage buckets[16]; };
    union { int record_count; long debug_memory; };
    union { int maximum_records; long bucket_count; };
};
struct s_havok_memory_usage
{
    union { s_havok_runtime_memory_usage runtime; s_havok_runtime_memory_usage runtime_pool_memory_usage; };
    union { s_havok_pool_memory_usage startup; s_havok_pool_memory_usage startup_pool_memory_usage; };
    union { s_havok_pool_memory_usage overflow; s_havok_pool_memory_usage overflow_pool_memory_usage; };
    union { s_havok_2nd_pool_memory_usage second_overflow; s_havok_2nd_pool_memory_usage second_overflow_pool_memory_usage; };
};

typedef char havok_memory_usage_size[(sizeof(s_havok_memory_usage) == 0x114) ? 1 : -1];

#endif
