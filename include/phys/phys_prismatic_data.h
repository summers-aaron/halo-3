#ifndef INCLUDE_PHYS_PRISMATIC_DATA_H
#define INCLUDE_PHYS_PRISMATIC_DATA_H

#include "phys/phys_atoms.h"

class phys_constraint;
class phys_prismatic_data : public phys_constraint_data
{
public:
    virtual ~phys_prismatic_data();
    phys_prismatic_data();
    __forceinline phys_prismatic_data(phys_fixup_tag flag) : phys_constraint_data(flag), m_data_atoms(flag) {}
    struct __declspec(align(16)) hinge_atoms {
        phys_set_local_transforms_atom m_transform_atom;
        phys_lin_motor_atom m_schema_motor;
        phys_lin_friction_atom m_contact_friction;
        phys_ang_atom m_ang_half;
        phys_lin_atom m_lin_0;
        phys_lin_atom m_lin_1;
        phys_lin_limit_atom m_lin_limit;
        hinge_atoms() {}
        __forceinline hinge_atoms(phys_fixup_tag flag)
            : m_transform_atom(flag), m_schema_motor(flag), m_contact_friction(flag), m_ang_half(flag), m_lin_0(flag), m_lin_1(flag), m_lin_limit(flag) {}
        __forceinline const phys_atom *get_atoms() const { return &m_transform_atom; }
        __forceinline int get_size_of_all_atoms() const { return 192; }
    } m_data_atoms;
    void set_hinge_motor(phys_motor *motor);
    enum solver_result { PRISMATIC_RESULT_MOTOR = 0, PRISMATIC_RESULT_FRICTION = 1, PRISMATIC_RESULT_MAX = 8 };
    struct limited_hinge_runtime {
        phys_solver_results m_solver_results[PRISMATIC_RESULT_MAX];
        unsigned char m_initialized;
        float m_prev_target_pos;
    };
    static __forceinline limited_hinge_runtime *get_runtime(void *runtime) { return static_cast<limited_hinge_runtime *>(runtime); }
    void set_motor_active(phys_constraint *instance, phys_bool to_be_enabled);
    void set_in_world_space(const phys_transform &body_a_transform, const phys_transform &body_b_transform, const phys_vec4 &pivot, const phys_vec4 &axis);
    void set_in_body_space(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_vec4 &axis_a,
        const phys_vec4 &axis_b, const phys_vec4 &axis_a_perp, const phys_vec4 &axis_b_perp);
    virtual void get_constraint_info(phys_constraint_data::constraint_info &info) const;
    virtual void get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const;
    virtual int get_type_id() const;
    virtual phys_bool is_valid_aabb() const;
};

typedef char phys_prismatic_runtime_size[(sizeof(phys_prismatic_data::limited_hinge_runtime) == 72) ? 1 : -1];

#endif
