// hkinternal/hkRagdollConstraintData.obj: from the records 0x683158 0x683170 0x683180 0x683198 0x6831a0 0x6831a8 0x683260 0x683270 0x683278 0x683288 0x683290 0x6832a0 0x6832b0 0x6832b8 0x683438 0x6835b8 0x6835e0 0x683608 0x6836b8 0x6836e0 0x6836f0 0x683718 0x683780 0x6837a0 0x6837b0 0x6839b8 0x683b60 0x683c50
// @compiler xdk5632
// @flags /TP /O2 /Oc
// @address 0x683158 ?destroy_loaded_ragdoll_data@@YAXPAX@Z
// @address 0x683170 ?get_constraint_info@phys_ragdoll_data@@UBAXAAUconstraint_info@phys_constraint_data@@@Z
// @address 0x683198 ?set_max_friction_torque@phys_ragdoll_data@@QAAXM@Z
// @address 0x6831a0 ?get_twist_motor@phys_ragdoll_data@@QBAPAVphys_motor@@XZ
// @address 0x6831a8 ?set_hinge_motor@phys_ragdoll_data@@QAAXW4motor_slot@1@PAVphys_motor@@@Z
// @address 0x683260 ?set_twist_motor@phys_ragdoll_data@@QAAXPAVphys_motor@@@Z
// @address 0x683270 ?get_cone_motor@phys_ragdoll_data@@QBAPAVphys_motor@@XZ
// @address 0x683278 ?set_cone_motor@phys_ragdoll_data@@QAAXPAVphys_motor@@@Z
// @address 0x683288 ?get_plane_motor@phys_ragdoll_data@@QBAPAVphys_motor@@XZ
// @address 0x683290 ?set_plane_motor@phys_ragdoll_data@@QAAXPAVphys_motor@@@Z
// @address 0x6832a0 ?are_motors_active@phys_ragdoll_data@@QBA?AVphys_bool@@XZ
// @address 0x6832b0 ?get_type_id@phys_ragdoll_data@@UBAHXZ
// @address 0x683608 ??1phys_ragdoll_data@@UAA@XZ
// @address 0x6836b8 ?set_ragdoll_target@phys_ragdoll_data@@QAAXABVphys_mat3@@@Z
// @address 0x6836e0 ?set_target_from_relative_frame@phys_ragdoll_data@@QAAXABVphys_rotation@@@Z
// @address 0x683718 ?set_motors_active@phys_ragdoll_data@@QAAXPAVphys_constraint@@Vphys_bool@@@Z
// @address 0x6837a0 ?get_ragdoll_data_vtable@@YAPBXXZ
#include "phys/phys_base_types.h"
#include "physics/havok_halo_external.h"
#include "phys/phys_math.h"
#include "phys/phys_object.h"
// @address 0x683c50 ??_Gphys_ragdoll_data@@UAAPAXI@Z
// @address 0x6835b8 ?get_constraint_frame_a@phys_ragdoll_data@@QBAXAAVphys_mat3@@@Z
// @address 0x6835e0 ?get_constraint_frame_b@phys_ragdoll_data@@QBAXAAVphys_mat3@@@Z
// @address 0x6836f0 ?get_ragdoll_target@phys_ragdoll_data@@QAAXAAVphys_mat3@@@Z
// @address 0x683780 ?fixup_loaded_ragdoll_data@@YAXPAX@Z
#include "phys/phys_thread_memory.h"
#include <vectorintrinsics.h>
#include <math.h>
#include <ppcintrinsics.h>
#include <stddef.h>
#include "phys/phys_constraint_data.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_constraint.h"


// Reports the size 112 and alignment 12.

struct phys_atom;


#include "phys/phys_atoms.h"
#include "phys/phys_ragdoll_data.h"
#include "phys/phys_vec4_util.h"
// @address 0x7ceb79 g_phys_ragdoll_flying_colors
char g_phys_ragdoll_flying_colors;
#include "phys/phys_solver_check.h"
#include "phys/phys_memory_class.h"

// 0x683158
void destroy_loaded_ragdoll_data(void *p)
{
    static_cast<phys_ragdoll_data *>(p)->~phys_ragdoll_data();
}

// 0x683170
void phys_ragdoll_data::get_constraint_info(phys_constraint_data::constraint_info &info) const
{
    gather_constraint_info(m_data_atoms.get_atoms(), m_data_atoms.get_size_of_all_atoms(), info);
}

// @address 0x683180 ?get_runtime_info@phys_ragdoll_data@@UBAXVphys_bool@@AAUruntime_info@phys_constraint_data@@@Z
void phys_ragdoll_data::get_runtime_info(phys_bool, phys_constraint_data::runtime_info &out) const
{
    out.m_solver_result_count = 12;
    out.m_external_runtime_size = 112;
}

// 0x683198
void phys_ragdoll_data::set_max_friction_torque(float torque)
{
    m_data_atoms.m_ang_friction.m_max_friction_torque = torque;
}

// 0x6831a0
phys_motor *phys_ragdoll_data::get_twist_motor() const
{
    return m_data_atoms.m_ragdoll_motors.m_axis_motors[RAGDOLL_MOTOR_TWIST];
}

// 0x6831a8
void phys_ragdoll_data::set_hinge_motor(motor_slot index, phys_motor *motor)
{
    if (motor) {
        motor->add_object_reference();
    }
    if (m_data_atoms.m_ragdoll_motors.m_axis_motors[index]) {
        m_data_atoms.m_ragdoll_motors.m_axis_motors[index]->object_remove_ref();
    }
    m_data_atoms.m_ragdoll_motors.m_axis_motors[index] = motor;
}

// 0x683260
void phys_ragdoll_data::set_twist_motor(phys_motor *motor)
{
    set_hinge_motor(RAGDOLL_MOTOR_TWIST, motor);
}

// 0x683270
phys_motor *phys_ragdoll_data::get_cone_motor() const
{
    return m_data_atoms.m_ragdoll_motors.m_axis_motors[RAGDOLL_MOTOR_CONE];
}

// 0x683278
void phys_ragdoll_data::set_cone_motor(phys_motor *motor)
{
    set_hinge_motor(RAGDOLL_MOTOR_CONE, motor);
}

// 0x683288
phys_motor *phys_ragdoll_data::get_plane_motor() const
{
    return m_data_atoms.m_ragdoll_motors.m_axis_motors[RAGDOLL_MOTOR_PLANE];
}

// 0x683290
void phys_ragdoll_data::set_plane_motor(phys_motor *motor)
{
    set_hinge_motor(RAGDOLL_MOTOR_PLANE, motor);
}

// 0x6832a0
phys_bool phys_ragdoll_data::are_motors_active() const
{
    return m_data_atoms.m_ragdoll_motors.m_limit_enabled;
}

// 0x6832b0
int phys_ragdoll_data::get_type_id() const
{
    return 7;
}

void phys_ragdoll_data::set_motors_active(phys_constraint *instance, phys_bool active)
{
    m_data_atoms.m_ragdoll_motors.m_limit_enabled = active;
    m_data_atoms.m_ang_friction.m_limit_enabled = (static_cast<bool>(active) == false);
    if (instance == 0)
        return;
    if (instance->get_runtime() == 0)
        return;
    float *runtime = static_cast<float *>(instance->get_runtime());
    runtime[6] = 0.0f;
    runtime[7] = 0.0f;
    runtime[8] = 0.0f;
    runtime[9] = 0.0f;
    runtime[10] = 0.0f;
    runtime[11] = 0.0f;
    runtime[0] = 0.0f;
    runtime[1] = 0.0f;
    runtime[2] = 0.0f;
    runtime[3] = 0.0f;
    runtime[4] = 0.0f;
    runtime[5] = 0.0f;
}

void phys_ragdoll_data::get_constraint_frame_a(phys_mat3 &constraint_frame_a) const
{
    constraint_frame_a = m_data_atoms.m_transform_atom.m_transform_a.get_rotation();
}

void phys_ragdoll_data::get_constraint_frame_b(phys_mat3 &constraint_frame_b) const
{
    constraint_frame_b = m_data_atoms.m_transform_atom.m_transform_b.get_rotation();
}

// 0x683608
phys_ragdoll_data::~phys_ragdoll_data()
{
    for (int i = 0; i < 3; ++i) {
        if (m_data_atoms.m_ragdoll_motors.m_axis_motors[i]) {
            m_data_atoms.m_ragdoll_motors.m_axis_motors[i]->object_remove_ref();
        }
    }
}

// 0x6836b8
void phys_ragdoll_data::set_ragdoll_target(const phys_mat3 &target)
{
    m_data_atoms.m_ragdoll_motors.m_target_frame_a_in_b = target;
}

// 0x6836e0
void phys_ragdoll_data::set_target_from_relative_frame(const phys_rotation &rotation)
{
    m_data_atoms.m_ragdoll_motors.m_target_frame_a_in_b.set_mul(rotation,
        m_data_atoms.m_transform_atom.m_transform_a.get_rotation());
}

void phys_ragdoll_data::get_ragdoll_target(phys_mat3 &target)
{
    target = m_data_atoms.m_ragdoll_motors.m_target_frame_a_in_b;
}

void fixup_loaded_ragdoll_data(void *p)
{
    phys_fixup_tag flag;
    new (p) phys_ragdoll_data(flag);
}

// 0x6837a0
const void *get_ragdoll_data_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_ragdoll_data)];
    phys_fixup_tag flag;
    phys_ragdoll_data *object = new (storage) phys_ragdoll_data(flag);
    return *reinterpret_cast<const void *const *>(object);
}


// @address 0x6837b0 ??0phys_ragdoll_data@@QAA@XZ
phys_ragdoll_data::phys_ragdoll_data()
{
    if (g_phys_ragdoll_flying_colors == 0) {
        g_phys_ragdoll_flying_colors = process_flying_colors(1);
        if (g_phys_ragdoll_flying_colors == 0) {
            return;
        }
    }
    m_data_atoms.m_transform_atom.m_transform_a.set_to_identity();
    m_data_atoms.m_transform_atom.m_transform_b.set_to_identity();

    m_data_atoms.m_ang_friction.m_first_friction_axis = 0;
    m_data_atoms.m_ang_friction.m_friction_axis_count = 3;
    m_data_atoms.m_ang_friction.m_max_friction_torque = 0.0f;

    m_data_atoms.m_ragdoll_motors.m_limit_enabled = false;
    m_data_atoms.m_ragdoll_motors.m_initialized_offset = 96;
    m_data_atoms.m_ragdoll_motors.m_prev_target_angles_offset = 100;
    m_data_atoms.m_ragdoll_motors.m_target_frame_a_in_b.set_to_identity();
    m_data_atoms.m_ragdoll_motors.m_axis_motors[0] = 0;
    m_data_atoms.m_ragdoll_motors.m_axis_motors[1] = 0;
    m_data_atoms.m_ragdoll_motors.m_axis_motors[2] = 0;

    m_data_atoms.m_twist_limit.m_twist_axis = 0;
    m_data_atoms.m_twist_limit.m_ref_axis = 1;
    m_data_atoms.m_twist_limit.m_min_angle = -0.5236f;
    m_data_atoms.m_twist_limit.m_atom_max_angle = 0.5236f;
    m_data_atoms.m_twist_limit.m_ang_limits_tau_factor = 0.8f;

    m_data_atoms.m_cone_limit.m_twist_axis_in_a = 0;
    m_data_atoms.m_cone_limit.m_ref_axis_in_b = 0;
    m_data_atoms.m_cone_limit.m_angle_measure_mode = phys_cone_limit_atom::ANG_ZERO_WHEN_ALIGNED;
    m_data_atoms.m_cone_limit.m_min_angle = -100.0f;
    m_data_atoms.m_cone_limit.m_atom_max_angle = 1.0f;
    m_data_atoms.m_cone_limit.m_ang_limits_tau_factor = 0.8f;

    m_data_atoms.m_planes_limit.m_twist_axis_in_a = 0;
    m_data_atoms.m_planes_limit.m_ref_axis_in_b = 1;
    m_data_atoms.m_planes_limit.m_angle_measure_mode = phys_cone_limit_atom::ANG_ZERO_WHEN_PERPENDICULAR;
    m_data_atoms.m_planes_limit.m_min_angle = -0.025f;
    m_data_atoms.m_planes_limit.m_atom_max_angle = 0.025f;
    m_data_atoms.m_planes_limit.m_ang_limits_tau_factor = 0.8f;
}

// @address 0x683438 ?is_valid_aabb@phys_ragdoll_data@@UBA?AVphys_bool@@XZ
phys_bool phys_ragdoll_data::is_valid_aabb() const
{
    phys_bool valid = true;
    valid = valid && m_data_atoms.m_transform_atom.m_transform_a.get_rotation().is_orthonormal(0.001f);
    valid = valid && m_data_atoms.m_transform_atom.m_transform_b.get_rotation().is_orthonormal(0.001f);
    valid = valid && m_data_atoms.m_cone_limit.m_min_angle == -100.0f;
    valid = valid && m_data_atoms.m_cone_limit.m_atom_max_angle >= 0.0f;
    valid = valid && m_data_atoms.m_cone_limit.m_atom_max_angle < 3.1415927f;
    valid = valid && m_data_atoms.m_planes_limit.m_min_angle <= m_data_atoms.m_planes_limit.m_atom_max_angle;
    valid = valid && m_data_atoms.m_twist_limit.m_min_angle <= m_data_atoms.m_twist_limit.m_atom_max_angle;
    return valid;
}

// @address 0x6832b8 ?set_in_body_space@phys_ragdoll_data@@QAAXABVphys_vec4@@00000@Z
void phys_ragdoll_data::set_in_body_space(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_vec4 &plane_axis_a,
    const phys_vec4 &plane_axis_b, const phys_vec4 &twist_axis_a, const phys_vec4 &twist_axis_b)
{
    phys_vec4 *base_a = &m_data_atoms.m_transform_atom.m_transform_a.get_rotation().m_column0;
    base_a[0] = twist_axis_a; base_a[0].normalize_xyz();
    base_a[1] = plane_axis_a; base_a[1].normalize_xyz();
    base_a[2].set_cross(base_a[0], base_a[1]);
    m_data_atoms.m_transform_atom.m_transform_a.m_translation = joint_pivot_a;

    phys_vec4 *base_b = &m_data_atoms.m_transform_atom.m_transform_b.get_rotation().m_column0;
    base_b[0] = twist_axis_b; base_b[0].normalize_xyz();
    base_b[1] = plane_axis_b; base_b[1].normalize_xyz();
    base_b[2].set_cross(base_b[0], base_b[1]);
    m_data_atoms.m_transform_atom.m_transform_b.m_translation = joint_pivot_b;

    m_data_atoms.m_ragdoll_motors.m_target_frame_a_in_b = m_data_atoms.m_transform_atom.m_transform_b.get_rotation();
}

// @address 0x6839b8 ?set_in_world_space@phys_ragdoll_data@@QAAXABVphys_transform@@0ABVphys_vec4@@11@Z
void phys_ragdoll_data::set_in_world_space(const phys_transform &body_a_transform, const phys_transform &body_b_transform, const phys_vec4 &pivot,
    const phys_vec4 &twist_axis_world, const phys_vec4 &plane_axis_world)
{
    phys_rotation constraint_base_world;
    constraint_base_world.m_column0 = twist_axis_world; constraint_base_world.m_column0.normalize_xyz();
    constraint_base_world.m_column1 = plane_axis_world; constraint_base_world.m_column1.normalize_xyz();
    constraint_base_world.m_column2.set_cross(constraint_base_world.m_column0, constraint_base_world.m_column1);
    phys_vec4_util::rotate_inverse_points(body_a_transform.get_rotation(), &constraint_base_world.m_column0, 3,
        &m_data_atoms.m_transform_atom.m_transform_a.get_rotation().m_column0);
    phys_vec4_util::rotate_inverse_points(body_b_transform.get_rotation(), &constraint_base_world.m_column0, 3,
        &m_data_atoms.m_transform_atom.m_transform_b.get_rotation().m_column0);
    m_data_atoms.m_transform_atom.m_transform_a.m_translation.vec_set_transformed_inverse_pos(body_a_transform, pivot);
    m_data_atoms.m_transform_atom.m_transform_b.m_translation.vec_set_transformed_inverse_pos(body_b_transform, pivot);
    m_data_atoms.m_ragdoll_motors.m_target_frame_a_in_b = m_data_atoms.m_transform_atom.m_transform_b.get_rotation();
}

extern "C" {

}
// @address 0x683b60 ?set_asymmetric_cone_angle@phys_ragdoll_data@@QAAXMM@Z
void phys_ragdoll_data::set_asymmetric_cone_angle(float min_cone_angle, float max_cone_angle)
{
    float cone_center = (max_cone_angle + min_cone_angle) * 0.5f;
    float cone_half_span = (max_cone_angle - min_cone_angle) * 0.5f;

    phys_transform &base_b = m_data_atoms.m_transform_atom.m_transform_b;
    phys_quat base_b_adjust; base_b_adjust.set_axis_angle(base_b.get_rotation().get_column(1), -cone_center);
    base_b.get_rotation().get_column(0).vec_set_rotated_dir(base_b_adjust, base_b.get_rotation().get_column(0));
    base_b.get_rotation().get_column(2).set_cross(base_b.get_rotation().get_column(0), base_b.get_rotation().get_column(1));

    m_data_atoms.m_cone_limit.m_atom_max_angle = cone_half_span;
}

