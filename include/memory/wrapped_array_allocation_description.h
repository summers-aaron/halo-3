#ifndef MEMORY_WRAPPED_ARRAY_ALLOCATION_DESCRIPTION_H
#define MEMORY_WRAPPED_ARRAY_ALLOCATION_DESCRIPTION_H

#include "memory/allocation_description.h"
template <typename T> class c_wrapped_array;

class c_raw_storage_buffer_iterator;

// March 82e2d400: array at +4, element count at +8 after the allocation-description base.
template <typename T>
class c_wrapped_array_allocation_description : public c_allocation_description {
public:
    __forceinline c_wrapped_array_allocation_description(c_wrapped_array<T> &array, unsigned long count)
        : m_wrapped_array(&array), m_element_count(count) {}
    virtual unsigned long get_unaligned_size() const;
    virtual void assign_storage(c_raw_storage_buffer_iterator &iterator) const;
private:
    c_wrapped_array<T> *m_wrapped_array;
    long m_element_count;
};

#endif // MEMORY_WRAPPED_ARRAY_ALLOCATION_DESCRIPTION_H
