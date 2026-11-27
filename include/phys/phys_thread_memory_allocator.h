#ifndef INCLUDE_PHYS_THREAD_MEMORY_ALLOCATOR_H
#define INCLUDE_PHYS_THREAD_MEMORY_ALLOCATOR_H

#include "phys/phys_thread_memory.h"

// Allocation templates from the thread-memory interface, separated from its type declarations.
// March 8345ae20/8345ade8 and 835157d8: element counts are converted to byte counts at the call.
template <typename T>
__forceinline T *phys_alloc(int alloc_object_count, phys_memory_class mem_class) {
    return static_cast<T *>(phys_thread_memory::get_instance_ptr()->allocate(alloc_object_count * sizeof(T), mem_class));
}

template <typename T>
__forceinline void phys_free_memory(T *data) {
    // match: 0x681ba0: retain the named TLS result so array cleanup keeps the shipped register priorities.
    phys_thread_memory *memory = phys_thread_memory::get_instance_ptr();
    memory->deallocate(data);
}

template <typename T>
__forceinline T *phys_alloc_chunk(int alloc_object_count, phys_memory_class mem_class) {
    return static_cast<T *>(phys_thread_memory::get_instance_ptr()->allocate_chunk(alloc_object_count * sizeof(T), mem_class));
}

template <typename T>
__forceinline void phys_free_chunk(T *data, int alloc_object_count, phys_memory_class mem_class) {
    // match: 0x7016f8: retain the chunk helper's named TLS result; getInstance changes the destructor's registers.
    phys_thread_memory *memory = static_cast<phys_thread_memory *>(TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot));
    memory->free_chunk(data, alloc_object_count * sizeof(T), mem_class);
}

#endif
