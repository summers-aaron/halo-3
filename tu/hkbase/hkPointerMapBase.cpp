// hkbase/hkPointerMapBase.obj: from the records 0x60d678 0x60d6d0 0x60d720 0x60d778 0x60d798 0x60d7b0 0x60d7d0 0x60d8a8 0x60d900 0x60d970 0x60dab0 0x60db90 0x60dbf0 0x60dd40 0x60ddb0 0x60de18 0x60de90 0x60df60 0x60dfc8 0x60e098
// @flags /TP /O2
#include "phys/phys_base.h"
#include <stddef.h>
// @address 0x60d678 ??0?$phys_ptr_map_base@K@@QAA@PAXH@Z
// @address 0x60d6d0 ?clear@?$phys_ptr_map_base@K@@QAAXXZ
// @address 0x60d720 ??0?$phys_ptr_map_base@_K@@QAA@PAXH@Z
// @address 0x60d778 ?get_size_in_bytes_for@?$phys_ptr_map_base@_K@@SAHH@Z
// @address 0x60d798 ??$pointer_hash@K@@YAKKH@Z
// @address 0x60d7b0 ??$pointer_hash@_K@@YA_K_KH@Z
// @address 0x60d8a8 ?map_find_key@?$phys_ptr_map_base@K@@QBAPAVmap_dummy@@K@Z
// @address 0x60d900 ?get_with_default@?$phys_ptr_map_base@K@@QBAKKK@Z
// @address 0x60db90 ?map_find_key@?$phys_ptr_map_base@_K@@QBAPAVmap_dummy@@_K@Z
// @address 0x60dd40 ??0?$phys_ptr_map_base@K@@QAA@XZ
// @address 0x60ddb0 ??1?$phys_ptr_map_base@K@@QAA@XZ
// @address 0x60de18 ?remove@?$phys_ptr_map_base@K@@QAA?AW4phys_result@@K@Z
// @address 0x60de90 ?resize_hash_table@?$phys_ptr_map_base@K@@IAAXH@Z
// @address 0x60df60 ??1?$phys_ptr_map_base@_K@@QAA@XZ
// @address 0x60dfc8 ?resize_hash_table@?$phys_ptr_map_base@_K@@IAAXH@Z
// @address 0x60e098 ?reserve@?$phys_ptr_map_base@K@@QAAXH@Z

class map_dummy;
extern "C" {

}


#include "phys/phys_ptr_map_base.h"
#include "phys/phys_memory_class.h"
#include "phys/phys_thread_memory.h"
extern "C" {
}

template<typename K> phys_ptr_map_base<K>::phys_ptr_map_base(void *storage, int sizeInBytes)
{
    int max_keys = (unsigned int)sizeInBytes / (2 * sizeof(K));
    m_map_elems = (K *)storage;
    m_map_entry_count = 0x80000000;
    m_hash_mod = max_keys - 1;
    phys_string::mem_fill(m_map_elems, 0, max_keys * sizeof(K));
}

template<> void phys_ptr_map_base<unsigned long>::clear()
{
    int capacity = m_hash_mod + 1;
    phys_string::mem_fill(m_map_elems, 0, capacity * sizeof(unsigned long) * 2);
    m_map_entry_count &= 0x80000000;
}


template<> int phys_ptr_map_base<unsigned __int64>::get_size_in_bytes_for(int agent_key_count)
{
    int capacity = 4;
    while (capacity < agent_key_count) {
        capacity *= 2;
    }
    return capacity * 16 * 2;
}

template<typename T> T pointer_hash(T key, int m_hash_mod)
{
    return ((key >> 4) * 0x9e3779b1ULL) & (T)m_hash_mod;
}

template<typename K> map_dummy *phys_ptr_map_base<K>::map_find_key(K key) const
{
    K index;
    for (index = pointer_hash(key, (int)m_hash_mod); m_map_elems[index] != 0; index = (index + 1) & (K)(int)m_hash_mod) {
        if (m_map_elems[index] == key) {
            return reinterpret_cast<Iterator>(static_cast<uintptr_t>(index));
        } else {
            continue;
        }
    }
    return reinterpret_cast<Iterator>(K(m_hash_mod + 1));
}

template<typename K> K phys_ptr_map_base<K>::get_with_default(K key, K fallback_value) const
{
    unsigned int index = (key >> 4) * 0x9e3779b1 & m_hash_mod;
    K probe_key;

    while ((probe_key = m_map_elems[index]) != 0) {
        if (probe_key == key)
            return m_map_elems[m_hash_mod + index + 1];
        index = (index + 1) & m_hash_mod;
    }
    return fallback_value;
}


template<> phys_ptr_map_base<unsigned long>::phys_ptr_map_base()
{
    phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    m_map_elems = (unsigned long *)((phys_thread_memory *)memory)->allocate_chunk(32 * sizeof(unsigned long), (phys_memory_class)21);
    m_map_entry_count = 0;
    m_hash_mod = 15;
    clear();
}

template<> phys_ptr_map_base<unsigned long>::~phys_ptr_map_base()
{
  if ((int)(m_map_entry_count & 0x80000000) == 0) {
    unsigned int n = m_hash_mod + 1;
    unsigned long *d = m_map_elems;
    phys_thread_memory *tm = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    ((phys_thread_memory *)tm)->free_chunk((void *)(d), (n & 0x1fffffff) << 3, (phys_memory_class)0x15);
  }
}

// @address 0x60d7d0 ?insert@?$phys_ptr_map_base@K@@QAAXKK@Z
template<> void phys_ptr_map_base<unsigned long>::insert(unsigned long key, unsigned long val)
{
    int map_elem_cnt = m_map_entry_count & 0x7fffffff;
    if ((map_elem_cnt + map_elem_cnt) > (int)m_hash_mod) {
        resize_hash_table((int)m_hash_mod + (int)m_hash_mod + 2);
    }
    unsigned long i;
    for (i = pointer_hash(key, (int)m_hash_mod);
        (m_map_elems[i] != 0) && (m_map_elems[i] != key);
        i = (i + 1) & m_hash_mod) {
    }
    m_map_entry_count += (m_map_elems[i] != key);
    m_map_elems[i] = key;
    m_map_elems[i + m_hash_mod + 1] = val;
}

// @address 0x60d970 ?remove@?$phys_ptr_map_base@K@@QAAXPAVmap_dummy@@@Z
template<> void phys_ptr_map_base<unsigned long>::remove(map_dummy *it)
{
    unsigned long i = static_cast<unsigned long>(reinterpret_cast<uintptr_t>(it));
    m_map_entry_count--;
    m_map_elems[i] = 0;
    unsigned long lo = (i + m_hash_mod) & m_hash_mod;
    while (m_map_elems[lo] != 0) {
        lo = (lo + m_hash_mod) & m_hash_mod;
    }
    lo = (lo + 1) & m_hash_mod;
    unsigned long empty = i;
    for (i = (i + 1) & m_hash_mod; m_map_elems[i] != 0; i = (i + 1) & m_hash_mod) {
        unsigned long hash = pointer_hash(m_map_elems[i], (int)m_hash_mod);
        if ((i >= lo) && (hash > empty)) {
            continue;
        } else if ((i < empty) && (hash > empty || hash <= i)) {
            continue;
        } else if (hash > empty && hash < lo) {
            continue;
        }
        m_map_elems[empty] = m_map_elems[i];
        m_map_elems[empty + m_hash_mod + 1] = m_map_elems[i + m_hash_mod + 1];
        m_map_elems[i] = 0;
        empty = i;
    }
}

template<> phys_result phys_ptr_map_base<unsigned long>::remove(unsigned long key)
{
    map_dummy *iterator = map_find_key(key);
    if (is_valid_aabb(iterator)) {
        remove(iterator);
        return PHYS_SUCCESS;
    } else {
        return PHYS_FAILURE;
    }
}

template<> void phys_ptr_map_base<unsigned long>::resize_hash_table(int new_table_capacity)
{
    int allocated = m_map_entry_count & (int)0x80000000;
    int prev_table_cap = m_hash_mod + 1;
    unsigned long *old_table = m_map_elems;
    phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    m_map_elems = (unsigned long *)((phys_thread_memory *)memory)->allocate_chunk(new_table_capacity * sizeof(unsigned long) * 2, (phys_memory_class)21);
    phys_string::mem_fill(m_map_elems, 0, new_table_capacity * sizeof(unsigned long));
    m_map_entry_count = 0;
    m_hash_mod = new_table_capacity - 1;
    for (int i = 0; i < prev_table_cap; ++i) {
        if (old_table[i] != 0) {
            insert(old_table[i], old_table[i + prev_table_cap]);
        }
    }
    if (allocated == 0) {
        memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
        ((phys_thread_memory *)memory)->free_chunk((void *)(old_table), prev_table_cap * sizeof(unsigned long) * 2, (phys_memory_class)21);
    }
}

template<> phys_ptr_map_base<unsigned __int64>::~phys_ptr_map_base()
{
  if ((int)(m_map_entry_count & 0x80000000) == 0) {
    unsigned int n = m_hash_mod + 1;
    unsigned __int64 *d = m_map_elems;
    phys_thread_memory *tm = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    ((phys_thread_memory *)tm)->free_chunk((void *)(d), (n & 0xfffffff) << 4, (phys_memory_class)0x15);
  }
}

template<> void phys_ptr_map_base<unsigned __int64>::resize_hash_table(int new_table_capacity)
{
    int allocated = m_map_entry_count & (int)0x80000000;
    int prev_table_cap = m_hash_mod + 1;
    unsigned __int64 *old_table = m_map_elems;
    phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    m_map_elems = (unsigned __int64 *)((phys_thread_memory *)memory)->allocate_chunk(new_table_capacity * 16, (phys_memory_class)21);
    phys_string::mem_fill(m_map_elems, 0, new_table_capacity * 8);
    m_map_entry_count = 0;
    m_hash_mod = new_table_capacity - 1;
    for (int i = 0; i < prev_table_cap; ++i) {
        if (old_table[i] != 0) {
            unsigned __int64 value = old_table[i + prev_table_cap];
            unsigned __int64 key = old_table[i];
            insert(key, value);
        }
    }
    if (allocated == 0) {
        memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
        ((phys_thread_memory *)memory)->free_chunk((void *)(old_table), prev_table_cap * 16, (phys_memory_class)21);
    }
}

template<> void phys_ptr_map_base<unsigned long>::reserve(int map_element_count)
{
    map_element_count *= 3;
    int capacity = 4;
    while (capacity < map_element_count) {
        capacity *= 2;
    }
    resize_hash_table(capacity);
}

// @address 0x60dab0 ?insert@?$phys_ptr_map_base@_K@@QAAX_K0@Z
template<> void phys_ptr_map_base<unsigned __int64>::insert(unsigned __int64 key, unsigned __int64 val)
{
    int map_elem_cnt = m_map_entry_count & 0x7fffffff;
    if ((map_elem_cnt + map_elem_cnt) > (int)m_hash_mod) {
        resize_hash_table((int)m_hash_mod + (int)m_hash_mod + 2);
    }
    unsigned __int64 i;
    for (i = pointer_hash(key, (int)m_hash_mod);
        (m_map_elems[i] != 0) && (m_map_elems[i] != key);
        i = (i + 1) & (unsigned __int64)(int)m_hash_mod) {
    }
    m_map_entry_count += (m_map_elems[i] != key);
    m_map_elems[i] = key;
    m_map_elems[i + m_hash_mod + 1] = val;
}

// @address 0x60dbf0 ?remove@?$phys_ptr_map_base@_K@@QAAXPAVmap_dummy@@@Z
template<> void phys_ptr_map_base<unsigned __int64>::remove(map_dummy *it)
{
    unsigned __int64 i = reinterpret_cast<intptr_t>(it);
    m_map_entry_count--;
    m_map_elems[i] = 0;
    unsigned __int64 lo = (i + (int)m_hash_mod) & (int)m_hash_mod;
    while (m_map_elems[lo] != 0) {
        lo = (lo + (int)m_hash_mod) & (int)m_hash_mod;
    }
    lo = (lo + 1) & (int)m_hash_mod;
    unsigned __int64 empty = i;
    for (i = (i + 1) & (int)m_hash_mod; m_map_elems[i] != 0; i = (i + 1) & (int)m_hash_mod) {
        unsigned __int64 hash = pointer_hash(m_map_elems[i], (int)m_hash_mod);
        if ((i >= lo) && (hash > empty)) {
            continue;
        } else if ((i < empty) && (hash > empty || hash <= i)) {
            continue;
        } else if (hash > empty && hash < lo) {
            continue;
        }
        m_map_elems[empty] = m_map_elems[i];
        m_map_elems[empty + (int)m_hash_mod + 1] = m_map_elems[i + (int)m_hash_mod + 1];
        m_map_elems[i] = 0;
        empty = i;
    }
}

#ifdef HALO3_64BIT
template<> void phys_ptr_map_base<unsigned __int64>::clear()
{
    int capacity = m_hash_mod + 1;
    phys_string::mem_fill(m_map_elems, 0, capacity * sizeof(unsigned __int64) * 2);
    m_map_entry_count &= 0x80000000;
}

template<> phys_ptr_map_base<unsigned __int64>::phys_ptr_map_base()
{
    phys_thread_memory *memory = (phys_thread_memory *)TlsGetValue(phys_thread_memory::s_threadMemoryInstance.m_slot);
    m_map_elems = (unsigned __int64 *)((phys_thread_memory *)memory)->allocate_chunk(32 * sizeof(unsigned __int64), (phys_memory_class)21);
    m_map_entry_count = 0;
    m_hash_mod = 15;
    clear();
}

template<> phys_result phys_ptr_map_base<unsigned __int64>::remove(unsigned __int64 key)
{
    map_dummy *iterator = map_find_key(key);
    if (is_valid_aabb(iterator)) {
        remove(iterator);
        return PHYS_SUCCESS;
    }
    return PHYS_FAILURE;
}

template<> void phys_ptr_map_base<unsigned __int64>::reserve(int map_element_count)
{
    map_element_count *= 3;
    int capacity = 4;
    while (capacity < map_element_count) {
        capacity *= 2;
    }
    resize_hash_table(capacity);
}
#endif

// This unit owns the two key-width instantiations.
template class phys_ptr_map_base<unsigned long>;
template class phys_ptr_map_base<unsigned __int64>;
