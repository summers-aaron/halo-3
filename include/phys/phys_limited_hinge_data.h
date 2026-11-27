#ifndef INCLUDE_PHYS_LIMITED_HINGE_DATA_H
#define INCLUDE_PHYS_LIMITED_HINGE_DATA_H

#include "phys/phys_atoms.h"
#include "phys/phys_constraint.h"

class phys_limited_hinge_data : public phys_constraint_data
{
public:
    virtual ~phys_limited_hinge_data();
    phys_limited_hinge_data();
    __forceinline phys_limited_hinge_data(phys_fixup_tag flag) : phys_constraint_data(flag), m_data_atoms(flag) {}
    virtual void get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const;
    virtual void get_constraint_info(phys_constraint_data::constraint_info &info) const;
    virtual int get_type_id() const;
    void set_hinge_motor(phys_motor *motor);
    struct __declspec(align(16)) hinge_atoms
    {
        phys_set_local_transforms_atom m_transform_atom;
        phys_ang_motor_atom m_ang_motor;
        phys_ang_friction_atom m_ang_friction;
        phys_ang_limit_atom m_ang_limit;
        phys_2d_ang_atom_alias m_ang_2d;
        phys_ball_socket_atom m_ball_socket;
        __forceinline hinge_atoms() {}
        __forceinline hinge_atoms(phys_fixup_tag flag) : m_transform_atom(flag), m_ang_motor(flag), m_ang_friction(flag),
            m_ang_limit(flag), m_ang_2d(flag), m_ball_socket(flag) {}
#ifdef HALO3_64BIT
        __forceinline int get_size_of_all_atoms() const
        {
            return offsetof(hinge_atoms, m_ball_socket) + sizeof(m_ball_socket);
        }
#else
        __forceinline int get_size_of_all_atoms() const { return 198; }
#endif
        __forceinline const phys_atom *get_atoms() const { return reinterpret_cast<const phys_atom *>(&m_transform_atom); }
    };
    void set_in_world_space(const phys_transform &body_a_transform, const phys_transform &body_b_transform, const phys_vec4 &pivot, const phys_vec4 &axis);
    void set_in_body_space(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_vec4 &axis_a,
        const phys_vec4 &axis_b, const phys_vec4 &axis_a_perp, const phys_vec4 &axis_b_perp);
    void set_motor_active(phys_constraint *instance, phys_bool active);
    virtual phys_bool is_valid_aabb() const;
    struct limited_hinge_runtime {
        __declspec(align(16)) phys_solver_results m_solver_results[8];
        unsigned char m_initialized;
        float m_prev_target_angle;
    };
    static __forceinline limited_hinge_runtime *get_runtime(void *runtime) { return static_cast<limited_hinge_runtime *>(runtime); }
    hinge_atoms m_data_atoms;
};

typedef char phys_limited_hinge_runtime_size[(sizeof(phys_limited_hinge_data::limited_hinge_runtime) == 0x50) ? 1 : -1];
typedef char phys_limited_hinge_runtime_initialized_offset[(offsetof(phys_limited_hinge_data::limited_hinge_runtime, m_initialized) == 0x40) ? 1 : -1];
typedef char phys_limited_hinge_runtime_target_offset[(offsetof(phys_limited_hinge_data::limited_hinge_runtime, m_prev_target_angle) == 0x44) ? 1 : -1];

#endif
