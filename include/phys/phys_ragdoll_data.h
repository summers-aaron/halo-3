#ifndef INCLUDE_PHYS_RAGDOLL_DATA_H
#define INCLUDE_PHYS_RAGDOLL_DATA_H

#include "phys/phys_atoms.h"

class phys_constraint;

// March 0x83545820 forwards the load flag to the base and every atom.
// Ragdoll constraint atoms contain the body frames, motors and angular limits.
class phys_ragdoll_data : public phys_constraint_data {
public:
    enum motor_slot { RAGDOLL_MOTOR_TWIST = 0, RAGDOLL_MOTOR_PLANE = 1, RAGDOLL_MOTOR_CONE = 2 };
    phys_ragdoll_data();
    __forceinline phys_ragdoll_data(phys_fixup_tag flag) : phys_constraint_data(flag), m_data_atoms(flag) {}
    virtual ~phys_ragdoll_data();
    virtual void get_constraint_info(phys_constraint_data::constraint_info &info) const;
    virtual void get_runtime_info(phys_bool, phys_constraint_data::runtime_info &) const;
    void set_max_friction_torque(float torque);
    phys_motor *get_twist_motor() const;
    void set_hinge_motor(motor_slot index, phys_motor *motor);
    void set_twist_motor(phys_motor *motor);
    phys_motor *get_cone_motor() const;
    void set_cone_motor(phys_motor *motor);
    phys_motor *get_plane_motor() const;
    void set_plane_motor(phys_motor *motor);
    phys_bool are_motors_active() const;
    void set_motors_active(phys_constraint *instance, phys_bool active);
    virtual int get_type_id() const;
    virtual phys_bool is_valid_aabb() const;
    void set_ragdoll_target(const phys_mat3 &target);
    void set_target_from_relative_frame(const class phys_rotation &rotation);
    void get_constraint_frame_a(phys_mat3 &constraint_frame_a) const;
    void get_constraint_frame_b(phys_mat3 &constraint_frame_b) const;
    void get_ragdoll_target(phys_mat3 &target);
    void set_in_world_space(const phys_transform &body_a_transform, const phys_transform &body_b_transform, const phys_vec4 &pivot,
        const phys_vec4 &twist_axis_world, const phys_vec4 &plane_axis_world);
    void set_in_body_space(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_vec4 &plane_axis_a, const phys_vec4 &plane_axis_b,
        const phys_vec4 &twist_axis_a, const phys_vec4 &twist_axis_b);

    struct hinge_atoms {
        __forceinline hinge_atoms() {}
        __forceinline hinge_atoms(phys_fixup_tag flag)
            : m_transform_atom(flag), m_ragdoll_motors(flag), m_ang_friction(flag), m_twist_limit(flag),
              m_cone_limit(flag), m_planes_limit(flag), m_ball_socket_atom(flag) {}
        phys_set_local_transforms_atom m_transform_atom;
        phys_ragdoll_motor_atom m_ragdoll_motors;
        phys_ang_friction_atom m_ang_friction;
        phys_twist_limit_atom m_twist_limit;
        phys_cone_limit_atom m_cone_limit;
        phys_cone_limit_atom m_planes_limit;
        phys_ball_socket_atom m_ball_socket_atom;
        __forceinline const phys_atom *get_atoms() const { return &m_transform_atom; }
        __forceinline int get_size_of_all_atoms() const {
            return offsetof(hinge_atoms, m_ball_socket_atom) + sizeof(m_ball_socket_atom);
        }
    };

    hinge_atoms m_data_atoms;
public:
  void set_asymmetric_cone_angle(float min_cone_angle, float max_cone_angle);
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char phys_ragdoll_atoms_size[(sizeof(void *) != 4 || sizeof(phys_ragdoll_data::hinge_atoms) == 0x130) ? 1 : -1];
typedef char phys_ragdoll_data_size[(sizeof(void *) != 4 || sizeof(phys_ragdoll_data) == 0x140) ? 1 : -1];
typedef char phys_ragdoll_atoms_offset[(sizeof(void *) != 4 || offsetof(phys_ragdoll_data, m_data_atoms) == 0x10) ? 1 : -1];
#endif

extern "C" {
// @address 0x7ceb79 g_phys_ragdoll_flying_colors
extern char g_phys_ragdoll_flying_colors;
}

#endif
