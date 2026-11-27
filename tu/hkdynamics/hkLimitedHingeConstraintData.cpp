// hkdynamics/hkLimitedHingeConstraintData.obj: from the records 0x64cdb0 0x64cdc8 0x64ce78 0x64ce88 0x64cea0 0x64cea8 0x64cf58 0x64d0e8 0x64d260 0x64d2a8 0x64d350 0x64d370 0x64d380 0x64d4d0
// @flags /TP /O2
// @address 0x64cea8 ??1phys_limited_hinge_data@@UAA@XZ
// @address 0x64cf58 ?set_in_world_space@phys_limited_hinge_data@@QAAXABVphys_transform@@0ABVphys_vec4@@1@Z
// @address 0x64d260 ?set_motor_active@phys_limited_hinge_data@@QAAXPAVphys_constraint@@Vphys_bool@@@Z
// @address 0x64d2a8 ?is_valid_aabb@phys_limited_hinge_data@@UBA?AVphys_bool@@XZ
#include "phys/phys_base_types.h"
// @address 0x64cdb0 ?destroy_loaded_limited_hinge_data@@YAXPAX@Z
// @address 0x64cdc8 ?set_hinge_motor@phys_limited_hinge_data@@QAAXPAVphys_motor@@@Z
// @address 0x64ce78 ?get_constraint_info@phys_limited_hinge_data@@UBAXAAUconstraint_info@phys_constraint_data@@@Z
// @address 0x64ce88 ?get_runtime_info@phys_limited_hinge_data@@UBAXVphys_bool@@AAUruntime_info@phys_constraint_data@@@Z
// @address 0x64cea0 ?get_type_id@phys_limited_hinge_data@@UBAHXZ
// @address 0x64d350 ?fixup_loaded_limited_hinge_data@@YAXPAX@Z
// @address 0x64d370 ?get_limited_hinge_data_vtable@@YAPBXXZ
// @address 0x64d4d0 ??_Gphys_limited_hinge_data@@UAAPAXI@Z
// @address 0x64d380 ??0phys_limited_hinge_data@@QAA@XZ
#include "physics/havok_halo_external.h"
#include "phys/phys_math.h"

#include "phys/phys_object.h"
#include "phys/phys_fixup_tag.h"
#include "phys/phys_constraint_data.h"
#include "phys/phys_atoms.h"
#include "phys/phys_constraint.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_vec4_util.h"
#include <vectorintrinsics.h>
#include <math.h>
#include <ppcintrinsics.h>

#include "phys/phys_thread_memory.h"
#include "phys/phys_limited_hinge_data.h"

void destroy_loaded_limited_hinge_data(void *object)
{
    static_cast<phys_limited_hinge_data *>(object)->~phys_limited_hinge_data();
}

void phys_limited_hinge_data::set_hinge_motor(phys_motor *motor)
{
    if (motor) {
        motor->add_object_reference();
    }
    if (m_data_atoms.m_ang_motor.m_schema_motor) {
        m_data_atoms.m_ang_motor.m_schema_motor->object_remove_ref();
    }
    m_data_atoms.m_ang_motor.m_schema_motor = motor;
}

void phys_limited_hinge_data::get_constraint_info(phys_constraint_data::constraint_info &info) const
{
    phys_constraint_data::gather_constraint_info(m_data_atoms.get_atoms(), m_data_atoms.get_size_of_all_atoms(), info);
}

void phys_limited_hinge_data::get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const
{
    info.m_solver_result_count = 8;
    info.m_external_runtime_size = 80;
}

int phys_limited_hinge_data::get_type_id() const
{
    return 2;
}

phys_limited_hinge_data::~phys_limited_hinge_data()
{
    if (m_data_atoms.m_ang_motor.m_schema_motor) {
        m_data_atoms.m_ang_motor.m_schema_motor->object_remove_ref();
    }
}

void phys_limited_hinge_data::set_in_world_space(
    const phys_transform &body_a_transform, const phys_transform &body_b_transform, const phys_vec4 &pivot, const phys_vec4 &axis)
{
    phys_vec4 perpendicular;
    phys_vec4 cross_axis;
    phys_vec4_util::compute_perpendicular_vec(axis, perpendicular);
    perpendicular.normalize_xyz();
    cross_axis.set_cross(axis, perpendicular);
    m_data_atoms.m_transform_atom.m_transform_a.get_column(0).vec_set_rotated_inverse_dir(body_a_transform.get_rotation(), axis);
    m_data_atoms.m_transform_atom.m_transform_a.get_column(1).vec_set_rotated_inverse_dir(body_a_transform.get_rotation(), perpendicular);
    m_data_atoms.m_transform_atom.m_transform_a.get_column(2).vec_set_rotated_inverse_dir(body_a_transform.get_rotation(), cross_axis);
    m_data_atoms.m_transform_atom.m_transform_a.get_column(3).vec_set_transformed_inverse_pos(body_a_transform, pivot);
    m_data_atoms.m_transform_atom.m_transform_b.get_column(0).vec_set_rotated_inverse_dir(body_b_transform.get_rotation(), axis);
    m_data_atoms.m_transform_atom.m_transform_b.get_column(1).vec_set_rotated_inverse_dir(body_b_transform.get_rotation(), perpendicular);
    m_data_atoms.m_transform_atom.m_transform_b.get_column(2).vec_set_rotated_inverse_dir(body_b_transform.get_rotation(), cross_axis);
    m_data_atoms.m_transform_atom.m_transform_b.get_column(3).vec_set_transformed_inverse_pos(body_b_transform, pivot);
}

void phys_limited_hinge_data::set_motor_active(phys_constraint *instance, phys_bool active)
{
    m_data_atoms.m_ang_motor.m_limit_enabled = active;
    m_data_atoms.m_ang_friction.m_limit_enabled = !active;
    if (instance && instance->get_runtime()) {
        limited_hinge_runtime *runtime = get_runtime(instance->get_runtime());
        runtime->m_solver_results[0].m_applied_impulse = 0.0f;
        runtime->m_solver_results[0].m_internal_solver_data = 0.0f;
        runtime->m_solver_results[1].m_applied_impulse = 0.0f;
        runtime->m_solver_results[1].m_internal_solver_data = 0.0f;
    }
}

phys_bool phys_limited_hinge_data::is_valid_aabb() const
{
    return m_data_atoms.m_transform_atom.m_transform_a.get_rotation().is_orthonormal(0.001f)
        && m_data_atoms.m_transform_atom.m_transform_b.get_rotation().is_orthonormal(0.001f)
        && (!m_data_atoms.m_ang_motor.m_limit_enabled || m_data_atoms.m_ang_limit.m_limit_enabled);
}


void fixup_loaded_limited_hinge_data(void *p)
{
    phys_fixup_tag flag;
    new (p) phys_limited_hinge_data(flag);
}

const void *get_limited_hinge_data_vtable()
{
    __declspec(align(16)) char storage[sizeof(phys_limited_hinge_data)];
    phys_fixup_tag flag;
    phys_limited_hinge_data *object = new (storage) phys_limited_hinge_data(flag);
    return *reinterpret_cast<const void *const *>(object);
}

phys_limited_hinge_data::phys_limited_hinge_data()
{
    m_data_atoms.m_transform_atom.m_transform_a.set_to_identity();
    m_data_atoms.m_transform_atom.m_transform_b.set_to_identity();

    m_data_atoms.m_ang_2d.m_free_rot_axis = 0;

    m_data_atoms.m_ang_limit.m_limit_axis = 0;
    m_data_atoms.m_ang_limit.m_min_angle = -3.14159265358979f;
    m_data_atoms.m_ang_limit.m_atom_max_angle = 3.14159265358979f;
    m_data_atoms.m_ang_limit.m_ang_limits_tau_factor = 1.0f;

    m_data_atoms.m_ang_friction.m_first_friction_axis = 0;
    m_data_atoms.m_ang_friction.m_friction_axis_count = 1;
    m_data_atoms.m_ang_friction.m_max_friction_torque = 0.0f;

    {
        m_data_atoms.m_ang_motor.m_initialized_offset = 64;
        m_data_atoms.m_ang_motor.m_prev_target_angle_offset = 68;
        m_data_atoms.m_ang_motor.m_ang_limit_result_offset = 16;
    }

    m_data_atoms.m_ang_motor.m_limit_enabled = false;
    m_data_atoms.m_ang_motor.m_target_angle = 0.0f;
    m_data_atoms.m_ang_motor.m_schema_motor = 0;
    m_data_atoms.m_ang_motor.m_motor_axis = 0;
}

namespace phys_math {  }
// @address 0x64d0e8 ?set_in_body_space@phys_limited_hinge_data@@QAAXABVphys_vec4@@00000@Z
void phys_limited_hinge_data::set_in_body_space(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_vec4 &axis_a,
    const phys_vec4 &axis_b, const phys_vec4 &axis_a_perp, const phys_vec4 &axis_b_perp)
{
    m_data_atoms.m_transform_atom.m_transform_a.get_column(0).m_simd_quad = axis_a.m_simd_quad;
    m_data_atoms.m_transform_atom.m_transform_a.get_column(0).normalize_xyz();
    m_data_atoms.m_transform_atom.m_transform_a.get_column(1).m_simd_quad = axis_a_perp.m_simd_quad;
    m_data_atoms.m_transform_atom.m_transform_a.get_column(1).normalize_xyz();
    m_data_atoms.m_transform_atom.m_transform_a.get_column(2).set_cross(axis_a, axis_a_perp);
    m_data_atoms.m_transform_atom.m_transform_a.get_column(3).m_simd_quad = joint_pivot_a.m_simd_quad;
    m_data_atoms.m_transform_atom.m_transform_b.get_column(0).m_simd_quad = axis_b.m_simd_quad;
    m_data_atoms.m_transform_atom.m_transform_b.get_column(0).normalize_xyz();
    m_data_atoms.m_transform_atom.m_transform_b.get_column(1).m_simd_quad = axis_b_perp.m_simd_quad;
    m_data_atoms.m_transform_atom.m_transform_b.get_column(1).normalize_xyz();
    m_data_atoms.m_transform_atom.m_transform_b.get_column(2).set_cross(axis_b, axis_b_perp);
    m_data_atoms.m_transform_atom.m_transform_b.get_column(3).m_simd_quad = joint_pivot_b.m_simd_quad;
}

