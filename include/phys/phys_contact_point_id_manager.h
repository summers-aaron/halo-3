#ifndef INCLUDE_PHYS_CONTACT_POINT_ID_MANAGER_H
#define INCLUDE_PHYS_CONTACT_POINT_ID_MANAGER_H

#include "phys/phys_base.h"

class phys_contact_point_id_manager {
public:
    __forceinline phys_contact_point_id_manager() : m_values(0) {}
    int alloc_contact_id(int value);
    void get_all_used_ids(phys_array<unsigned short> &ids) const;
    __forceinline unsigned char get_value_at(int index) { return m_values[index]; }
    __forceinline void release_contact_id(int id) { m_values[id] = 255; }
    __forceinline int array_index_of(int value) const {
        int i;
        for (i = m_values.getSize() - 1; i >= 0; --i) {
            if (m_values[i] == value) return i;
            else continue;
        }
        return i;
    }
    __forceinline void decrement_values_above(int rel_index) {
        for (int i = m_values.getSize() - 1; i >= 0; --i) {
            if (m_values[i] != 255 && m_values[i] > rel_index)
                m_values[i]--;
        }
    }

    phys_inline_array<unsigned char, 8> m_values;
};

typedef char phys_dynamics_cp_id_manager_size[(sizeof(void *) != 4 || ((sizeof(phys_contact_point_id_manager) == 20) )) ? 1 : -1];

#endif
