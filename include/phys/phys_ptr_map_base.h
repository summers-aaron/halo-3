#ifndef INCLUDE_PHYS_PTR_MAP_BASE_H
#define INCLUDE_PHYS_PTR_MAP_BASE_H

#include "phys/phys_base.h"

class map_dummy;

template<typename K> class phys_ptr_map_base {
public:
    typedef map_dummy *Iterator;
    phys_ptr_map_base();
    phys_ptr_map_base(void *storage, int sizeInBytes);
    ~phys_ptr_map_base();
    void clear();
    static int get_size_in_bytes_for(int agent_key_count);
    Iterator map_find_key(K key) const;
    K get_with_default(K key, K fallback_value) const;
    Iterator get_iterator() const;
    K get_bvh_key(Iterator iterator) const;
    // March 0x834635b0 / 0x83464aa8: the values follow the key table.
    __forceinline K get_map_value(Iterator iterator) const {
        int index = static_cast<int>(reinterpret_cast<uintptr_t>(iterator));
        return m_map_elems[index + m_hash_mod + 1];
    }
    __forceinline void set_map_value(Iterator iterator, K value) {
        int index = static_cast<int>(reinterpret_cast<uintptr_t>(iterator));
        m_map_elems[index + m_hash_mod + 1] = value;
    }
    Iterator get_next_edge(Iterator iterator) const;
    phys_bool has_key(K key) const;
    phys_result get(K key, K *out) const;
    int getSize() const;
    int get_sector_capacity() const;
    phys_bool is_stream_ok() const;
    void swap(phys_ptr_map_base &other);
    phys_bool was_reallocated() const;
    __forceinline phys_bool is_valid_aabb(Iterator iterator) const { return static_cast<int>(reinterpret_cast<uintptr_t>(iterator)) <= m_hash_mod; }
    phys_result remove(K key);
    void remove(Iterator iterator);
    void reserve(int map_element_count);
    void insert(K key, K value);
protected:
    void resize_hash_table(int capacity);
    K *m_map_elems;
    int m_map_entry_count;
    int m_hash_mod;
};

template<> void phys_ptr_map_base<unsigned long>::resize_hash_table(int new_table_capacity);
template<> void phys_ptr_map_base<unsigned __int64>::insert(unsigned __int64 key, unsigned __int64 val);

#endif // HAVOK_HK_POINTER_MAP_BASE_H
