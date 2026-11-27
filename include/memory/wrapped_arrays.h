#ifndef MEMORY_WRAPPED_ARRAYS_H
#define MEMORY_WRAPPED_ARRAYS_H

#include "memory/wrapped_arrays_base.h"

// A count and a pointer to elements someone else owns. The no-constructor form is the one the interface is
// instantiated on; c_wrapped_array adds the constructors.
template<typename t_element>
class c_wrapped_array_no_init : public c_array_operator_interface<c_wrapped_array_no_init<t_element>, t_element> {
public:
    __forceinline long count() const { return m_count; }
    __forceinline t_element *begin() { return m_elements; }
    __forceinline const t_element *begin() const { return m_elements; }
    __forceinline t_element *end() { return m_elements + m_count; }
    __forceinline const t_element *end() const { return m_elements + m_count; }
    __forceinline void set_elements(t_element *elements, long element_count) {
        // wrapped_arrays.h:226 assert(element_count>=0)
        m_elements = elements;
        m_count = element_count;
    }

protected:
    long m_count;
    t_element *m_elements;
};

#include "memory/wrapped_array.h"

// A fixed-capacity array with a current count. March 2007 has end() only non-const, begin() both.
template<typename t_element, long k_maximum_count>
class c_static_sized_dynamic_array
    : public c_resizeable_array_operator_interface<c_static_sized_dynamic_array<t_element, k_maximum_count>, t_element> {
public:
    __forceinline c_static_sized_dynamic_array() : m_count(0) {}
    __forceinline void clear() { resize(0); }
    __forceinline long count() const { return m_count; }
    __forceinline t_element *begin() { return m_storage.begin(); }
    __forceinline const t_element *begin() const { return m_storage.begin(); }
    __forceinline t_element *end() { return begin() + m_count; }
    __forceinline bool full() const { return m_count == m_storage.get_count(); }
    __forceinline long new_element_index() {
        long new_index = m_count;
        // wrapped_arrays.h:433 assert(m_storage.valid(new_index))
        m_count++;
        return new_index;
    }
    __forceinline void resize(long new_size) {
        // wrapped_arrays.h:467 assert(new_size>=0 && new_size<=m_storage.get_count())
        m_count = new_size;
    }

private:
    c_static_array<t_element, k_maximum_count> m_storage;
    long m_count;
};

#endif // MEMORY_WRAPPED_ARRAYS_H
