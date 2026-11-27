#ifndef INCLUDE_PHYS_STIFF_SPRING_DATA_H
#define INCLUDE_PHYS_STIFF_SPRING_DATA_H

#include "phys/phys_atoms.h"

class phys_stiff_spring_data : public phys_constraint_data {
public:
    virtual __forceinline ~phys_stiff_spring_data() {}
    phys_stiff_spring_data();
    // March 0x834e1200 forwards the flag to the base and atom aggregate.
    __forceinline phys_stiff_spring_data(phys_fixup_tag flag) : phys_constraint_data(flag), m_data_atoms(flag) {}
    struct __declspec(align(16)) hinge_atoms {
        __forceinline hinge_atoms() {}
        __forceinline hinge_atoms(phys_fixup_tag flag) : m_pivots_atom(flag), m_spring_atom(flag) {}
        phys_set_local_translations_atom m_pivots_atom;
        phys_stiff_spring_atom m_spring_atom;
        __forceinline int get_size_of_all_atoms() const { return 56; }
        __forceinline const phys_atom *get_atoms() const { return &m_pivots_atom; }
    };
    hinge_atoms m_data_atoms;
    virtual void get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const;
    virtual void get_constraint_info(phys_constraint_data::constraint_info &info) const;
    virtual phys_bool is_valid_aabb() const;
    virtual int get_type_id() const;
};

typedef char phys_stiff_spring_atoms_offset[(sizeof(void *) != 4 || ((offsetof(phys_stiff_spring_data, m_data_atoms) == 16) )) ? 1 : -1];
typedef char phys_stiff_spring_data_size[(sizeof(void *) != 4 || ((sizeof(phys_stiff_spring_data) == 80) )) ? 1 : -1];

#endif
