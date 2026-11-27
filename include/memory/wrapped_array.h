#ifndef MEMORY_WRAPPED_ARRAY_H
#define MEMORY_WRAPPED_ARRAY_H

template<typename t_element> class c_wrapped_array_no_init;
template<typename t_derived, typename t_element> class c_array_operator_interface;

template<typename t_element>
class c_wrapped_array : public c_wrapped_array_no_init<t_element> {
public:
    __forceinline c_wrapped_array() { this->set_elements(0, 0); }
    __forceinline c_wrapped_array(t_element *elements, long element_count) { this->set_elements(elements, element_count); }
    __forceinline c_wrapped_array(t_element *first, t_element *last) { this->set_elements(first, last - first); }
    template<long k_element_count>
    __forceinline c_wrapped_array(t_element (&elements)[k_element_count]) { this->set_elements(elements, k_element_count); }
    template<typename t_other_element>
    __forceinline c_wrapped_array(const c_wrapped_array<t_other_element> &other) {
        this->set_elements(other.get_elements(), other.count());
    }
    template<typename t_other_derived, typename t_other_element>
    __forceinline c_wrapped_array(const c_array_operator_interface<t_other_derived, t_other_element> &other) {
        this->set_elements(other.get_elements(), other.count());
    }
};

template<typename t_element>
__forceinline c_wrapped_array<t_element> make_wrapped_array(t_element *elements, long element_count) {
    return c_wrapped_array<t_element>(elements, element_count);
}

template<typename t_element>
__forceinline c_wrapped_array<t_element> make_wrapped_array(t_element *first, t_element *last) {
    return c_wrapped_array<t_element>(first, last);
}

#endif // MEMORY_WRAPPED_ARRAY_H
