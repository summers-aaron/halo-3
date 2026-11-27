#ifndef INCLUDE_PHYS_THREAD_MEMORY_H
#define INCLUDE_PHYS_THREAD_MEMORY_H

#include <stddef.h>

// Per-thread memory and class allocators. Chunk and stack requests pass through the game's allocation hooks.

#include "cseries/crt_xdk.h"

#include "phys/phys_memory_class.h"
#include "phys/phys_scalar_types.h"
#include "phys/phys_system_alloc.h"

// The game's hooks (blamlibxbox/havok_memory.obj).
bool havok_override_thread_memory_allocations();
void *havok_thread_memory_allocate_chunk(int size, phys_memory_class memory_class);
void havok_thread_memory_deallocate_chunk(void *block, int size, phys_memory_class memory_class);
bool havok_override_thread_memory_stack_allocations();
void *havok_thread_memory_allocate_stack(int size);
void havok_thread_memory_deallocate_stack(void *block);
// The game's debugger break, called where a Havok check fails.
void halo_external_breakpoint();

// A per-thread value kept in one TLS slot.
template <typename T>
class phys_thread_local_data {
public:
    unsigned long m_slot;
    phys_thread_local_data() { m_slot = TlsAlloc(); }
    ~phys_thread_local_data() { TlsFree(m_slot); }
    __forceinline T get_chain_data() { return (T)(phys_ulong)TlsGetValue(m_slot); }
    __forceinline void lock_set_data(T p) {
        phys_ulong value = (phys_ulong)p;
        TlsSetValue(m_slot, (void *)value);
    }
};

class phys_memory;

class phys_thread_memory {
public:
    phys_thread_memory(phys_memory *memory, int max_free_list_elems);
    // March 0x8345baa0: system allocation, 16-byte alignment, no named local.
    static __forceinline void *operator new(size_t nbytes) {
        return phys_system_malloc(static_cast<int>(nbytes), 16);
    }
    // March 0x82a6b270 and 0x82a6b2a0.
    static __forceinline void *operator new(size_t, void *p) { return p; }
    static __forceinline void operator delete(void *, void *) {}
    static phys_thread_local_data<phys_thread_memory *> s_threadMemoryInstance;

    static __forceinline phys_thread_memory *get_instance_ptr() { return s_threadMemoryInstance.get_chain_data(); }
    // match: 0x61be48: reads the slot itself; March's body (0x826d5588) calls getInstancePtr, and that extra
    // inline level flips an add in hkBvTreeAgent::processCollision.
    static phys_thread_memory &get_singleton_instance() { return *s_threadMemoryInstance.get_chain_data(); }

    virtual void *aligned_allocate(int alignment, int nbytes, phys_memory_class cl);
    virtual void aligned_deallocate(void *p);
    virtual void set_stack_area(void *buffer, int nbytes);
    virtual void release_cached_memory();
    virtual ~phys_thread_memory();

protected:
    virtual void *on_stack_overflow(int nbytes);
    virtual void on_stack_underflow(void *p);

public:
    void *allocate_chunk(int nbytes, phys_memory_class cl);
    void free_chunk(void *p, int nbytes, phys_memory_class cl);
    void verify_cached_memory();
    void *allocate(int nbytes, phys_memory_class cl);
    void deallocate(void *p);
    void add_object_reference();
    void object_remove_ref();
    static void replace_singleton_instance(phys_thread_memory *memory);
    static void init();
    static void quit_system();
    static void operator delete(void *p);

    static __forceinline int const_size_to_row(int size) {
        if (size <= 8) return 1;
        else if (size <= 16) return 2;
        else if (size <= 32) return 3;
        else if (size <= 48) return 4;
        else if (size <= 64) return 5;
        else if (size <= 96) return 6;
        else if (size <= 128) return 7;
        else if (size <= 160) return 8;
        else if (size <= 192) return 9;
        else if (size <= 256) return 10;
        else if (size <= 320) return 11;
        else if (size <= 512) return 12;
        else if (size <= 1024) return 13;
        else if (size <= 2048) return 14;
        else if (size <= 4096) return 15;
        else if (size <= 8192) return 16;
        else {
            halo_external_breakpoint();
            return -1;
        }
    }

    // March 0x82a6b2d8: the free-list pop is FreeList::get, and the pool path is the else arm.
    __forceinline void *allocate_chunk_const_size(int nbytes, phys_memory_class cl) {
        if (havok_override_thread_memory_allocations()) {
            return havok_thread_memory_allocate_chunk(nbytes, cl);
        } else {
            int row = const_size_to_row(nbytes);
            if (void *p = m_block_free_list[row].get()) {
                return p;
            }
            return on_row_empty(row);
        }
    }

    __forceinline void free_chunk_fixed_size(void *p, int nbytes, phys_memory_class cl) {
        if (havok_override_thread_memory_allocations()) {
            havok_thread_memory_deallocate_chunk(p, nbytes, cl);
            return;
        }
        int row = const_size_to_row(nbytes);
        if (m_block_free_list[row].m_elem_count >= m_max_free_list_elems) {
            on_row_full(row, 0);
        }
        m_block_free_list[row].put(p);
    }

    // Game owners with an explicit allocator override use the same cached rows.
    __forceinline void *refill_free_list(int row) { return on_row_empty(row); }
    __forceinline void free_chunk_to_list(void *block, int nbytes) {
        int row = const_size_to_row(nbytes);
        if (m_block_free_list[row].m_elem_count >= m_max_free_list_elems)
            on_row_full(row, 0);
        m_block_free_list[row].put(block);
    }

    // The game can also override stack allocation.
    __forceinline void *allocate_stack(int byte_count_in) {
        if (havok_override_thread_memory_stack_allocations()) {
            return havok_thread_memory_allocate_stack(byte_count_in);
        }
        int actual_bytes = (byte_count_in + 16) & ~15;
        char *p = m_mem_stack.m_current;
        char *end = p + actual_bytes;
        if (end <= m_mem_stack.m_end) {
            m_mem_stack.m_current = end;
            return p;
        }
        return on_stack_overflow(actual_bytes);
    }

    __forceinline void free_stack(void *p) {
        if (havok_override_thread_memory_stack_allocations()) {
            havok_thread_memory_deallocate_stack(p);
            return;
        }
        m_mem_stack.m_current = static_cast<char *>(p);
        if (p == m_mem_stack.m_base) {
            on_stack_underflow(p);
        }
    }

    // The stack record's alignment also aligns the enclosing class to 16 bytes. The game keeps its hkThreadMemory
    // objects 8 bytes off a 16-byte boundary (s_havok_globals base_system_object 0x58, thread_memory 0x388), so
    // other targets drop the alignment, which optimized x86 code would use for aligned vector stores, and keep the
    // offsets and size with unnamed padding in the class.
    struct Stack {
        // March 0x83459240: an empty stack has a sentinel base and no backing buffer.
        __forceinline Stack() : m_current(0), m_prev_link(0), m_base((char *)-1), m_end(0) {}
#if defined(_XBOX) && !defined(__clang__)
        __declspec(align(16)) char *m_current;
#else
        char *m_current;
#endif
        Stack *m_prev_link;
        char *m_base;
        char *m_end;
        __forceinline int get_free_bytes() { return static_cast<int>(m_end - m_current) - 16; }
    };

    struct free_block_list {
        // March 0x83459280.
        __forceinline free_block_list() : m_head(0), m_elem_count(0) {}
        struct free_elem {
            free_elem *m_next;
        };
        __forceinline void put(void *p) {
            m_elem_count += 1;
            free_elem *element = static_cast<free_elem *>(p);
            element->m_next = m_head;
            m_head = element;
        }
        void *get() {
            if (void *p = m_head) {
                m_elem_count -= 1;
                m_head = m_head->m_next;
                return p;
            }
            return 0;
        }
        free_elem *m_head;
        int m_elem_count;
    };

    __forceinline Stack &get_stack() { return m_mem_stack; }

protected:
    void *on_row_empty(int row);
    void on_row_full(int row, int blocks_to_free = 0);

public:
#if !defined(_XBOX) || defined(__clang__)
    int : 32;
    int : 32;
    int : 32;
#endif
    phys_memory *m_memory_pool;
    int m_object_ref_count;
#if !defined(_XBOX) || defined(__clang__)
    int : 32;
    int : 32;
#endif
    Stack m_mem_stack;
    int m_stack_size;
    int m_max_free_list_elems;
    free_block_list m_block_free_list[17];
    // Size tables the constructor fills: block size per row, row per small size, row per large size.
    int m_row_size[17];
    char m_size_to_row[0x201];
    int m_large_size_row[8];
#if !defined(_XBOX) || defined(__clang__)
    int : 32;
    int : 32;
#endif
};

typedef char havok_offset_hkThreadMemory_m_memory[(sizeof(void *) != 4 || ((offsetof(phys_thread_memory, m_memory_pool) == 0x10))) ? 1 : -1];
typedef char havok_offset_hkThreadMemory_m_stack[(sizeof(void *) != 4 || ((offsetof(phys_thread_memory, m_mem_stack) == 0x20))) ? 1 : -1];
typedef char havok_offset_hkThreadMemory_m_free_list[(sizeof(void *) != 4 || ((offsetof(phys_thread_memory, m_block_free_list) == 0x38) )) ? 1 : -1];
typedef char havok_offset_hkThreadMemory_m_row_of_large_size[(sizeof(void *) != 4 || ((offsetof(phys_thread_memory, m_large_size_row) == 0x308) )) ? 1 : -1];
typedef char havok_size_hkThreadMemory[(sizeof(void *) != 4 || ((sizeof(phys_thread_memory) == 0x330) )) ? 1 : -1];

template <typename T>
__forceinline T *phys_alloc_stack(int n) {
    return static_cast<T *>(phys_thread_memory::get_singleton_instance().allocate_stack(n * static_cast<int>(sizeof(T))));
}

template <typename T>
__forceinline void phys_free_stack(T *p) {
    phys_thread_memory::get_singleton_instance().free_stack(p);
}

// Aligned heap blocks (March: hkAlignedAllocate<char> 0x834627b0, hkAlignedDeallocate<char> 0x83462808). March also
// has hkAllocate, hkDeallocate, hkAllocateChunk and hkDeallocateChunk; they wait for the units that keep their own
// copies of them (hkMonitorStream, hkCollisionDispatcher, hkDynamicsJobs, hkMultithreadedSimulation).
template <typename T>
__forceinline T *phys_aligned_alloc(int alignment, int count, phys_memory_class cl) {
    return static_cast<T *>(
        phys_thread_memory::get_singleton_instance().aligned_allocate(alignment, count * static_cast<int>(sizeof(T)), cl));
}

template <typename T>
__forceinline void phys_aligned_free(T *p) {
    phys_thread_memory::get_singleton_instance().aligned_deallocate(static_cast<void *>(p));
}

// Class allocators for scalar and placement allocation.
#define PHYS_DECLARE_ALLOCATOR(TYPE)                                                                     \
    static __forceinline void *operator new(size_t nbytes) {                                                 \
        phys_ref_object *b = static_cast<phys_ref_object *>(                                           \
            phys_thread_memory::get_singleton_instance().allocate_chunk(static_cast<int>(nbytes), TYPE));                    \
        b->m_mem_size_and_flags = static_cast<unsigned short>(nbytes);                                          \
        return b;                                                                                            \
    }                                                                                                        \
    static __forceinline void operator delete(void *p) {                                                     \
        phys_ref_object *b = static_cast<phys_ref_object *>(p);                                        \
        phys_thread_memory::get_singleton_instance().free_chunk(p, b->m_mem_size_and_flags, TYPE);                        \
    }                                                                                                        \
    static __forceinline void *operator new(size_t, void *p) { return p; }                                   \
    static __forceinline void operator delete(void *, void *) {}

#define PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(TYPE, SELF_CLASS)                                              \
    static __forceinline void *operator new(size_t nbytes) {                                                 \
        return phys_thread_memory::get_singleton_instance().allocate_chunk_const_size(static_cast<int>(nbytes), TYPE);         \
    }                                                                                                        \
    static __forceinline void operator delete(void *p) {                                                     \
        phys_thread_memory::get_singleton_instance().free_chunk_fixed_size(p, sizeof(SELF_CLASS), TYPE);                 \
    }                                                                                                        \
    static __forceinline void *operator new(size_t, void *p) { return p; }                                   \
    static __forceinline void operator delete(void *, void *) {}


// The system allocator (hkMemory and the memory system objects): straight to the platform heap.

#define PHYS_DECLARE_SYSTEM_ALLOCATOR()                                                                        \
    static __forceinline void *operator new(size_t nbytes) {                                                 \
        return phys_system_malloc(static_cast<int>(nbytes), 16);                                               \
    }                                                                                                        \
    static __forceinline void operator delete(void *p) { phys_system_free(p); }                                  \
    static __forceinline void *operator new(size_t, void *p) { return p; }                                   \
    static __forceinline void operator delete(void *, void *) {}

#endif // INCLUDE_HAVOK_HK_THREAD_MEMORY_H
