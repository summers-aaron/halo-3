#ifndef INCLUDE_PHYS_HINGE_DATA_H
#define INCLUDE_PHYS_HINGE_DATA_H

#include "phys/phys_atoms.h"

class phys_hinge_data : public phys_constraint_data
{
public:
    phys_hinge_data();
    __forceinline phys_hinge_data(phys_fixup_tag flag) : phys_constraint_data(flag), m_data_atoms(flag) {}
    struct hinge_atoms {
        __forceinline hinge_atoms() {}
        __forceinline hinge_atoms(phys_fixup_tag flag) : m_transform_atom(flag), m_ang_2d(flag), m_ball_socket(flag) {}
        phys_set_local_transforms_atom m_transform_atom;
        phys_2d_ang_atom_alias m_ang_2d;
        phys_ball_socket_atom m_ball_socket;
        __forceinline const phys_atom *get_atoms() const { return &m_transform_atom; }
        __forceinline int get_size_of_all_atoms() const { return 150; }
    } m_data_atoms;
    virtual void get_constraint_info(phys_constraint_data::constraint_info &info) const;
    virtual void get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const;
    virtual int get_type_id() const;
    virtual phys_bool is_valid_aabb() const;
    void set_in_body_space(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_vec4 &axis_a, const phys_vec4 &axis_b);
    void set_in_world_space(const phys_transform &body_a_transform, const phys_transform &body_b_transform, const phys_vec4 &pivot, const phys_vec4 &axis);
};

#endif
