#ifndef INCLUDE_PHYS_LOCAL_ARRAY_H
#define INCLUDE_PHYS_LOCAL_ARRAY_H
#include "phys/phys_base.h"
template <typename T>
class phys_local_array : public phys_array<T> {
public:
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_ARRAY, phys_local_array<T>);
    __forceinline phys_local_array(int capacity) {
        this->m_data = phys_alloc_stack<T>(capacity);
        this->m_capacity_and_flags = capacity | PHYS_ARRAY_DONT_DEALLOCATE_FLAG;
        m_local_memory = this->m_data;
    }
    __forceinline ~phys_local_array() { phys_free_stack(m_local_memory); }
    __forceinline void discard_and_resize(int size) {
        phys_free_stack(m_local_memory);
        m_local_memory = this->m_data = phys_alloc_stack<T>(size);
        this->m_size = size;
        this->m_capacity_and_flags = size | PHYS_ARRAY_DONT_DEALLOCATE_FLAG;
    }
    __forceinline void assert_not_resized() {}
    __forceinline phys_bool was_reallocated() const { return m_local_memory != this->m_data; }
    T *m_local_memory;
};
typedef char phys_local_array_size[(sizeof(void *) != 4 || sizeof(phys_local_array<int>) == 16) ? 1 : -1];
#endif
