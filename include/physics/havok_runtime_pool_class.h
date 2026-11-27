#ifndef PHYSICS_HAVOK_RUNTIME_POOL_CLASS_H
#define PHYSICS_HAVOK_RUNTIME_POOL_CLASS_H

#include "phys/phys_memory.h"
#include "phys/phys_critical_section.h"
#include <stddef.h>

// Runtime page allocator.
class c_runtime_pool : public phys_memory
{
public:
    c_runtime_pool();
    __forceinline virtual ~c_runtime_pool() {}
    virtual void allocate_chunk_batch(void **blocks, int count, int size);
    virtual void *allocate_chunk(int size, phys_memory_class memory_class);
    virtual void *allocate_runtime_block(int size, phys_memory_class memory_class);
    virtual void free_runtime_block(void *block, int size, phys_memory_class memory_class);
    virtual void preallocate_runtime_block(int size, phys_memory_class memory_class);
    virtual void provide_runtime_block(void *block, int size, phys_memory_class memory_class);
    virtual void release_runtime_blocks();
    virtual void getStatSynopsis(phys_memory_stats *statistics);
    virtual void free_chunk_batch(void **blocks, int count, int size);
    virtual void print_memory_stats(phys_out_stream *stream);
    virtual void free_chunk(void *block, int size, phys_memory_class memory_class);
    void *allocate_chunk_internal(int size, phys_memory_class memory_class);
    void deallocate_chunk_internal(void *block, int size, phys_memory_class memory_class);

private:
    __forceinline void deallocate_chunk(void *block, int size)
    {
        int row = get_row(size);
        putOnFreeList(record_chopped_allocation_state_change(block, row, false), row);
    }

    __forceinline int get_row(int size)
    {
        if (size <= 512)
            return m_small_rows[size];
        else
            return m_large_rows[(size - 1) >> 10];
    }

    __forceinline void *record_chopped_allocation_state_change(void *block, long row, bool used)
    {
        if (used)
            m_free_count[row]--;
        else
            m_free_count[row]++;
        return block;
    }

    __forceinline void putOnFreeList(void *block, int row)
    {
        m_allocations_in_use[row].allocations_in_use--;
        void **entry = (void **)block;
        *entry = m_free_lists[row];
        m_free_lists[row] = entry;
    }

    __forceinline void *record_chopped_allocation(void *block, long row, bool used)
    {
        m_chopped_count[row]++;
        if (!used)
            m_free_count[row]++;
        return block;
    }

    __forceinline int get_total_page_count()
    {
        return m_small_page_count + m_large_page_count;
    }

    __forceinline void *allocate_and_chop_page(int size, int row)
    {
        if (512 >= size)
        {
            int row_index = row;
            int row_size = m_row_sizes[row_index];
            int chopped_size = row_size;
            if (row_size + m_page_next > m_page_end)
            {
                if (get_total_page_count() < 450)
                {
                    char *page = m_pages[m_small_page_count++];
                    m_page_start = page;
                    m_page_next = m_page_start;
                    m_page_end = m_page_next + 8128;
                    m_page_count++;
                }
                else
                    return 0;
            }
            char *block = m_page_next;
            m_page_next = row_size + m_page_next;
            int allocations_in_use = m_allocations_in_use[row_index].allocations_in_use;
            while ((chopped_size < 512 || (chopped_size & 0xff) != 0) && row_size + m_page_next < m_page_end)
            {
                putOnFreeList(record_chopped_allocation(m_page_next, row_index, false), row_index);
                m_page_next = row_size + m_page_next;
                chopped_size += row_size;
            }
            m_allocations_in_use[row_index].allocations_in_use = allocations_in_use;
            return record_chopped_allocation(block, row_index, true);
        }
        else
        {
            int row_size = m_row_sizes[row];
            if (get_total_page_count() < 450)
            {
                m_large_page_count++;
                char *page = m_pages[450 - m_large_page_count];
                m_page_count++;
                int allocations_in_use = m_allocations_in_use[row].allocations_in_use;
                char *chunk = page + row_size;
                for (int chopped_size = row_size; chopped_size < 8192; chopped_size += row_size)
                {
                    putOnFreeList(record_chopped_allocation(chunk, row, false), row);
                    chunk += row_size;
                }
                m_allocations_in_use[row].allocations_in_use = allocations_in_use;
                return record_chopped_allocation(page, row, true);
            }
            else
                return 0;
        }
    }

public:
    __forceinline long get_pages_in_use() const { return m_small_page_count + m_large_page_count; }
    __forceinline bool contains(void *block)
    {
        return (char *)block >= m_pages[0] && (char *)block <= m_pages[0] + sizeof(m_pages);
    }

    char *m_page_start;
    char *m_page_end;
    char *m_page_next;
    void *m_free_lists[17];
    int m_row_sizes[17];
    signed char m_small_rows[513];
    int m_large_rows[8];
    int m_page_count;
    int m_system_allocation_count;
    int m_system_allocation_size;
    int m_peak_system_allocation_size;
    int m_page_size;
    int m_page_overhead;
    struct havok_runtime_pool_size_statistics
    {
        int allocations_in_use;
        __forceinline havok_runtime_pool_size_statistics() : allocations_in_use(0) {}
    };
    havok_runtime_pool_size_statistics m_allocations_in_use[17];
    __declspec(align(16)) char m_pages[450][8192];
    phys_critical_section m_critical_section;
    int m_small_page_count;
    int m_large_page_count;
    int m_chopped_count[17];
    int m_free_count[17];
};

typedef char runtime_pool_size[(sizeof(void *) != 4 || ((sizeof(c_runtime_pool) == 0x384e10))) ? 1 : -1];
typedef char runtime_pool_pages_offset[(sizeof(void *) != 4 || ((offsetof(c_runtime_pool, m_pages) == 0xd50))) ? 1 : -1];
typedef char runtime_pool_page_counts_offset[(sizeof(void *) != 4 || ((offsetof(c_runtime_pool, m_small_page_count) == 0x384d80))) ? 1 : -1];

#endif
