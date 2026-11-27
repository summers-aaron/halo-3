#ifndef INCLUDE_PHYS_BALL_SOCKET_DATA_H
#define INCLUDE_PHYS_BALL_SOCKET_DATA_H

#include "phys/phys_atoms.h"

class phys_ball_socket_data : public phys_constraint_data
{
public:
    virtual __forceinline ~phys_ball_socket_data() {}
    phys_ball_socket_data();
    __forceinline phys_ball_socket_data(phys_fixup_tag flag) : phys_constraint_data(flag), m_data_atoms(flag) {}
    struct __declspec(align(16)) hinge_atoms
    {
        __forceinline hinge_atoms() {}
        __forceinline hinge_atoms(phys_fixup_tag flag) : m_pivots_atom(flag), m_ball_socket(flag) {}
        phys_set_local_translations_atom m_pivots_atom;
        phys_ball_socket_atom m_ball_socket;
        __forceinline const phys_atom *get_atoms() const { return &m_pivots_atom; }
        __forceinline int get_size_of_all_atoms() const { return 50; }
    } m_data_atoms;
    void set_in_world_space(const phys_transform &body_a_transform, const phys_transform &body_b_transform, const phys_vec4 &pivot);
    void set_in_body_space(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b);
    virtual void get_constraint_info(phys_constraint_data::constraint_info &info) const;
    virtual void get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const;
    virtual phys_bool is_valid_aabb() const;
    virtual int get_type_id() const;
};

#endif
