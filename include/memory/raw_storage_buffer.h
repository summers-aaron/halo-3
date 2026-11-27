#ifndef MEMORY_RAW_STORAGE_BUFFER_H
#define MEMORY_RAW_STORAGE_BUFFER_H

#include "cseries/pointer_utilities.h"
template<typename t_element>
class c_wrapped_array;

template<long k_alignment>
struct s_raw_storage_alignment_bits
{
    enum { value = 1 + s_raw_storage_alignment_bits<(k_alignment >> 1)>::value };
};

template<>
struct s_raw_storage_alignment_bits<1>
{
    enum { value = 0 };
};

// March wrapped_arrays.h:697..750. Kept in a leaf while other users carry copies.
class c_raw_storage_buffer_iterator
{
public:
    __forceinline c_raw_storage_buffer_iterator(void *buffer, long size)
        : m_buffer_begin(buffer), m_buffer_end(offset_pointer(buffer, size)), m_buffer_iterator(buffer)
    {
    }

    __forceinline void *get_array_elements_explicit(unsigned long element_size, long alignment_bits, long element_count)
    {
        void *result = align_pointer(m_buffer_iterator, alignment_bits);
        m_buffer_iterator = offset_pointer(result, element_size * element_count);
        return result;
    }

    template<typename T>
    __forceinline T *get_array_elements(long count)
    {
        T *elements = count == 0 ? 0 : (T *)get_array_elements_explicit(
            sizeof(T), s_raw_storage_alignment_bits<__alignof(T)>::value, count);
        return elements;
    }

    template<typename T>
    __forceinline void set_wrapped_array(c_wrapped_array<T> &wrapped_array, long element_count)
    {
        T *elements = get_array_elements<T>(element_count);
        wrapped_array.set_elements(elements, element_count);
    }

private:
    friend class c_data_array_allocation_description;
    void *m_buffer_begin;
    void *m_buffer_end;
    void *m_buffer_iterator;
};

typedef char raw_storage_buffer_iterator_size[(sizeof(void *) != 4 || ((sizeof(c_raw_storage_buffer_iterator) == 12) )) ? 1 : -1];

#endif // MEMORY_RAW_STORAGE_BUFFER_H
