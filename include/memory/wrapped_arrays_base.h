#ifndef MEMORY_WRAPPED_ARRAYS_BASE_H
#define MEMORY_WRAPPED_ARRAYS_BASE_H

// Attested in March 2007 debug image: source\memory\wrapped_arrays.h (asserts in c_array_operator_interface,
// c_resizeable_array_operator_interface, c_wrapped_array_no_init and c_static_sized_dynamic_array).
// Bungie's array classes and the shared element-access interface they sit on. Each interface method forwards to the
// derived class through get_super_class(); the derived class supplies count() and begin() (and end() where it has
// one). The asserts are compiled out, as in release.

// c_static_array (memory/static_arrays.h) is the storage of c_static_sized_dynamic_array. It is declared here,
// not included: about 20 units that include this header (through tag_files/tag_types.h) still carry their own
// c_static_array. A unit that instantiates c_static_sized_dynamic_array includes static_arrays.h.
template<typename t_type, long k_count>
class c_static_array;

// No assert names a file for this one; it also serves c_bit_vector_operator_interface, so its true header may be
// a more general one.
template<typename t_derived>
class c_recursive_template_pattern {
public:
    __forceinline t_derived *get_super_class() { return static_cast<t_derived *>(this); }
    __forceinline const t_derived *get_super_class() const { return static_cast<const t_derived *>(this); }
};

template<typename t_derived, typename t_element>
class c_array_operator_interface : public c_recursive_template_pattern<t_derived> {
public:
    // wrapped_arrays.h:35 assert(valid_index(index))
    __forceinline t_element &operator[](long index) { return this->get_super_class()->begin()[index]; }
    // wrapped_arrays.h:41 assert(valid_index(index))
    __forceinline const t_element &operator[](long index) const { return this->get_super_class()->begin()[index]; }
    __forceinline t_element *begin() { return this->get_super_class()->begin(); }
    __forceinline const t_element *begin() const { return this->get_super_class()->begin(); }
    __forceinline t_element *end() { return this->get_super_class()->end(); }
    __forceinline long count() const { return this->get_super_class()->count(); }
    // An index is valid when it lies in [0, count()); a single unsigned compare rejects
    // negative values as well as values past the end, so the
    // body is the unsigned form.
    __forceinline bool valid_index(long index) const { return (unsigned long)index < (unsigned long)count(); }
    // wrapped_arrays.h:67 assert(valid_index(index))
    __forceinline t_element *get_element(long index) { return begin() + index; }
    // wrapped_arrays.h:73 assert(valid_index(index))
    __forceinline const t_element *get_element(long index) const { return begin() + index; }
    __forceinline t_element *get_elements() { return begin(); }
    __forceinline const t_element *get_elements() const { return begin(); }
    __forceinline long get_total_element_size() const { return count() * sizeof(t_element); }
    // March: std::fill(begin(), end(), value). The body stays in users that can include <xutility>:
    // including that here collides with the placement new, memset or strstr
    // that some includers declare themselves (gui_game_details, gui_screen_carnage_report, gui_screen_take_screenshot,
    // user_interface_window_manager, hs_library_external, havok, objects).
    void set_all(const t_element &value);
};

// The interface of the arrays that grow. Its own get_super_class() is private (March: AAA); add_element() calls the
// array interface on this unadjusted, so this derives from c_array_operator_interface.
template<typename t_derived, typename t_element>
class c_resizeable_array_operator_interface : public c_array_operator_interface<t_derived, t_element> {
public:
    __forceinline long new_element_index() { return get_super_class()->new_element_index(); }
    __forceinline t_element &add_element() {
        long new_index = new_element_index();
        // wrapped_arrays.h:127 assert(valid_index(new_index))
        return *this->get_element(new_index);
    }

private:
    __forceinline t_derived *get_super_class() { return static_cast<t_derived *>(this); }
};

#endif // MEMORY_WRAPPED_ARRAYS_BASE_H
