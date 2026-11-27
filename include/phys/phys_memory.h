#ifndef INCLUDE_PHYS_MEMORY_H
#define INCLUDE_PHYS_MEMORY_H

#include <stddef.h>

#include "phys/phys_base_types.h"
#include "phys/phys_thread_memory.h"

class phys_out_stream;

class phys_memory {
public:
    PHYS_DECLARE_SYSTEM_ALLOCATOR();

    phys_memory();
    virtual ~phys_memory();
    virtual void allocate_chunk_batch(void **blocks, int count, int size) = 0;
    virtual void free_chunk_batch(void **blocks, int count, int size) = 0;
    virtual void *allocate_chunk(int size, phys_memory_class memory_class) = 0;
    virtual void free_chunk(void *block, int size, phys_memory_class memory_class) = 0;
    virtual void *allocate_runtime_block(int size, phys_memory_class memory_class) = 0;
    virtual void free_runtime_block(void *block, int size, phys_memory_class memory_class) = 0;
    virtual void preallocate_runtime_block(int size, phys_memory_class memory_class) = 0;
    virtual void provide_runtime_block(void *block, int size, phys_memory_class memory_class) = 0;
    virtual void release_runtime_blocks() = 0;
    virtual void print_memory_stats(phys_out_stream *stream) = 0;
    virtual int get_sector_allocated_size(int size);
    virtual bool is_debug_memory();
    virtual phys_bool is_stream_ok() const;
    virtual void *find_base_address(void *block, int size);
    virtual void lock_mem_block(void *block);
    virtual void unlock_mem_block(void *block);

    // The base class declares no further virtuals; pool subclasses add getStatSynopsis themselves.
    static void replace_singleton_instance(phys_memory *memory);
    static __forceinline phys_memory &get_singleton_instance() { return *s_instance; }
    __forceinline int get_available_memory() {
        int havok_get_available_memory();
        return havok_get_available_memory();
    }

    __forceinline void add_object_reference() { ++m_memory_ref_count; }
    __forceinline void object_remove_ref() {
        if (--m_memory_ref_count == 0) {
            delete this;
        }
    }

    enum memory_state { MEMORY_STATE_OK, MEMORY_STATE_EXHAUSTED };
    memory_state m_memory_state;

private:
    int m_memory_limit;
    int m_memory_ref_count;
    // Existing counter labels are retained pending a complete statistics-field audit.
    int m_sys_alloc_count;
    int m_sys_free_count;
    int m_total_sys_memory;
    int m_current_sys_memory;
    int m_peak_sys_memory;
    int m_runtime_alloc_count;
    int m_runtime_free_count;

protected:
    static phys_memory *s_instance;
    struct big_block_data {
        phys_bool m_available;
        int m_size;
        void *m_memory_pool;
        phys_memory_class m_class_desc;
        phys_bool m_externally_provided;
    };
    enum { MEMORY_LARGE_BLOCK_SLOTS = 128 };
    int m_valid_big_block_count;
    big_block_data m_big_block_data[MEMORY_LARGE_BLOCK_SLOTS];
};

typedef char havok_offset_hkMemory_m_memoryState[(sizeof(void *) != 4 || ((offsetof(phys_memory, m_memory_state) == 4) )) ? 1 : -1];
typedef char havok_size_hkMemory[(sizeof(void *) != 4 || ((sizeof(phys_memory) == 0xa30) )) ? 1 : -1];

#endif // INCLUDE_HAVOK_HK_MEMORY_H
