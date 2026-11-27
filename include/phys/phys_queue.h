#ifndef INCLUDE_PHYS_QUEUE_H
#define INCLUDE_PHYS_QUEUE_H

#include "phys/phys_base_types.h"
#include "phys/phys_thread_memory_allocator.h"

// March 8378c7e8, 8378c830 and 83515740: five words, with release before destruction.
template <class T> class phys_queue {
public:
    __forceinline phys_queue() : m_data(0), m_small_capacity(0), m_head(0), m_tail(0), m_used_count(0) {}
    __forceinline ~phys_queue() { release_memory(); }
    void set_capacity(int array_requested_capacity);
    void enqueue_front(const T &entry);
    void enqueue(const T &entry);
    __forceinline void dequeue(T &entry) {
        entry = m_data[m_head];
        if (++m_head == m_small_capacity) m_head = 0;
        m_used_count--;
    }
    __forceinline int getSize() const { return m_used_count; }
    __forceinline phys_bool is_array_empty() const { return m_used_count == 0; }

private:
    __forceinline void grow_capacity() {
        if (m_small_capacity == 0) set_capacity(8);
        else set_capacity(m_small_capacity * 2);
    }
    __forceinline void release_memory() {
        if (m_small_capacity != 0)
            phys_free_chunk(m_data, m_small_capacity, PHYS_MEM_ARRAY);
    }
    T *m_data;
    int m_small_capacity;
    int m_head;
    int m_tail;
    int m_used_count;
};

#endif
