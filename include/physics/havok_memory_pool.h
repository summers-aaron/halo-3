#ifndef PHYSICS_HAVOK_MEMORY_POOL_H
#define PHYSICS_HAVOK_MEMORY_POOL_H

#include <stddef.h>
#include "physics/havok_memory.h"
#include "phys/phys_base.h"
#include "phys/phys_memory.h"
#include "phys/phys_critical_section.h"

class phys_out_stream;
struct phys_memory_stats;

// A first-fit pool over one caller-supplied buffer. Allocation records grow down from the end of the
// buffer; allocations grow up from its start.
// March 2007 debug build: physics\havok_memory_pool.h (asserts at lines 35, 158, 172-176, 187-232, 255-277,
// 297-305, 318-356, 369, 379). Member names come from those asserts (m_buffer, m_buffer_count,
// m_current_allocation_count, m_minimum_allocation_size, allocation.count); access from the mangled names.
// The constructor, the virtuals other than the destructor, alignedAllocate and alignedDeallocate are defined out
// of line. The destructor is an inline COMDAT (March debug map: ??1c_havok_memory_pool in havok_startup_pool.obj).
class c_havok_memory_pool : public phys_memory
{
public:
    friend void havok_compute_memory_in_use(s_havok_memory_usage *usage);
    friend int havok_get_memory_condition_type(int);

    struct s_allocation
    {
        long start;
        unsigned long count;
    };

    c_havok_memory_pool(char *buffer, long buffer_count, long minimum_allocation_size);
    virtual ~c_havok_memory_pool() {}
    virtual void allocate_chunk_batch(void **blocks, int count, int size);
    virtual void free_chunk_batch(void **blocks, int count, int size);
    virtual void *allocate_chunk(int size, phys_memory_class memory_class);
    virtual void free_chunk(void *block, int size, phys_memory_class memory_class);
    virtual void *allocate_runtime_block(int size, phys_memory_class memory_class);
    virtual void free_runtime_block(void *block, int size, phys_memory_class memory_class);
    virtual void preallocate_runtime_block(int size, phys_memory_class memory_class);
    virtual void provide_runtime_block(void *block, int size, phys_memory_class memory_class);
    virtual void release_runtime_blocks();
    virtual void print_memory_stats(phys_out_stream *stream);
    virtual void getStatSynopsis(phys_memory_stats *statistics);

    // March physics\havok_memory_pool.h:158.
    __forceinline s_allocation *get_allocation(long index)
    {
        return (s_allocation *)(m_buffer + m_buffer_count) - (index + 1);
    }

    __forceinline bool can_create_additional_allocation_record();
    __forceinline s_allocation *add_allocation();

    // March 0x82bd46a8 (havok_memory.obj, inlined into the memory statistics).
    __forceinline unsigned long calculate_maximum_allocation_size()
    {
        unsigned long maximum = 0;
        for (long allocation_index = 0; allocation_index < m_current_allocation_count; ++allocation_index)
        {
            unsigned long allocation_size = get_allocation(allocation_index)->count;
            maximum = allocation_size > maximum ? allocation_size : maximum;
        }
        return maximum;
    }

    __forceinline bool contains(void *block)
    {
        char *start = m_buffer;
        return (char *)block >= start && (char *)block <= m_buffer_count + start;
    }

private:
    void *aligned_allocate(int alignment, int nbytes, phys_memory_class memory_class);
    void aligned_deallocate(void *pointer);
    __forceinline void *alignedAllocateInSpace(s_allocation *allocation, long alignment, long nbytes, long start_address,
        long end_address);
    __forceinline long calculate_next_buffer_count();

    friend struct s_havok_memory_pool_layout_asserts;

protected:
    char *m_buffer;
    long m_buffer_count;
    long m_current_allocation_count;
    long m_minimum_allocation_size;
    phys_critical_section m_critical_section;
};

__forceinline long c_havok_memory_pool::calculate_next_buffer_count()
{
    return m_buffer_count - (m_current_allocation_count + 1) * sizeof(s_allocation);
}

__forceinline bool c_havok_memory_pool::can_create_additional_allocation_record()
{
    long end_of_last_allocation = 0;
    if (m_current_allocation_count > 0)
    {
        s_allocation *last_allocation = get_allocation(m_current_allocation_count - 1);
        end_of_last_allocation = last_allocation->start + last_allocation->count;
    }
    const long &next_count = calculate_next_buffer_count();
    return end_of_last_allocation < next_count;
}

__forceinline c_havok_memory_pool::s_allocation *c_havok_memory_pool::add_allocation()
{
    m_current_allocation_count = m_current_allocation_count + 1;
    return get_allocation(m_current_allocation_count - 1);
}

// March physics\havok_memory_pool.h:318-356.
__forceinline void *c_havok_memory_pool::alignedAllocateInSpace(s_allocation *allocation, long alignment, long nbytes,
    long start_address, long end_address)
{
    void *result = 0;
    const long vector_alignment = __alignof(phys_simd_vec);
    if (end_address >= start_address)
    {
        switch (alignment)
        {
        case 1:
            if (end_address >= start_address + nbytes)
            {
                allocation->start = start_address;
                allocation->count = nbytes;
                result = m_buffer + start_address;
            }
            break;
        case 2:
        case 4:
        case 8:
        case vector_alignment:
        {
            long aligned_start = (start_address + (vector_alignment - 1)) & ~(vector_alignment - 1);
            if (end_address >= aligned_start + nbytes)
            {
                allocation->start = aligned_start;
                allocation->count = nbytes;
                result = m_buffer + aligned_start;
            }
            break;
        }
        default:
            break;
        }
    }
    return result;
}

// Compile-time layout checks only; no object is emitted.
struct s_havok_memory_pool_layout_asserts
{
#if defined(_M_PPCBE) || defined(_XBOX)
    typedef char havok_memory_pool_buffer_offset[(sizeof(void *) != 4 || (offsetof(c_havok_memory_pool, m_buffer) == 0xa30)) ? 1 : -1];
    typedef char havok_memory_pool_critical_section_offset[(sizeof(void *) != 4 || (offsetof(c_havok_memory_pool, m_critical_section) == 0xa40)) ? 1 : -1];
    typedef char havok_memory_pool_size[(sizeof(void *) != 4 || (sizeof(c_havok_memory_pool) == 0xa70)) ? 1 : -1];
#endif
};

#endif // PHYSICS_HAVOK_MEMORY_POOL_H
