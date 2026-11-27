// hkdynamics/hkPrismaticConstraintData.obj: from the records 0x64d800 0x64d818 0x64d8c8 0x64d8d8 0x64d8f0 0x64d8f8 0x64d9a8 0x64db18 0x64dbb8 0x64dc00 0x64dc20 0x64dc30 0x64dd80 0x64dfa0
// @flags /TP /O2
#include "phys/phys_base.h"
// @address 0x64d800 ?destroy_loaded_prismatic_data@@YAXPAX@Z
// @address 0x64d818 ?set_hinge_motor@phys_prismatic_data@@QAAXPAVphys_motor@@@Z
// @address 0x64d8c8 ?get_constraint_info@phys_prismatic_data@@UBAXAAUconstraint_info@phys_constraint_data@@@Z
// @address 0x64d8d8 ?get_runtime_info@phys_prismatic_data@@UBAXVphys_bool@@AAUruntime_info@phys_constraint_data@@@Z
// @address 0x64d8f0 ?get_type_id@phys_prismatic_data@@UBAHXZ
// @address 0x64dc00 ?fixup_loaded_prismatic_data@@YAXPAX@Z
// @address 0x64dc20 ?get_prismatic_data_vtable@@YAPBXXZ
#include <vectorintrinsics.h>
#include <ppcintrinsics.h>
#include <math.h>
#include "physics/havok_halo_external.h"
#include "phys/phys_dynamics.h"
#include "phys/phys_atoms.h"
#include "phys/phys_constraint.h"
#include "phys/phys_math.h"
#include "phys/phys_prismatic_data.h"

struct phys_atom;

namespace {
static __forceinline void add_motor_ref(phys_ref_object *object)
{
    if (object->m_mem_size_and_flags != 0) {
        ++object->m_object_ref_count;
    }
}
static __forceinline void remove_motor_ref(phys_ref_object *object)
{
    if (object->m_mem_size_and_flags != 0) {
        if (--object->m_object_ref_count == 0) {
            if (halo_managed_memory_address(object)) {
                object->m_object_ref_count = 1;
            } else {
                delete object;
            }
        }
    }
}
}

// @address 0x64dc30 ??0phys_prismatic_data@@QAA@XZ
phys_prismatic_data::phys_prismatic_data()
{
    m_data_atoms.m_transform_atom.m_transform_a.set_to_identity();
    m_data_atoms.m_transform_atom.m_transform_b.set_to_identity();

    m_data_atoms.m_lin_limit.m_min_limit = -3.40282e+38f;
    m_data_atoms.m_lin_limit.m_max_limit = 3.40282e+38f;
    m_data_atoms.m_lin_limit.m_axis_index = 0;

    m_data_atoms.m_contact_friction.m_max_friction_force = 0.0f;
    m_data_atoms.m_contact_friction.m_friction_axis = 0;
    m_data_atoms.m_schema_motor.m_motor_axis = 0;

    m_data_atoms.m_schema_motor.m_limit_enabled = false;
    m_data_atoms.m_schema_motor.m_target_pos = 0.0f;
    m_data_atoms.m_schema_motor.m_schema_motor = 0;

    m_data_atoms.m_schema_motor.m_motor_axis = 0;

    m_data_atoms.m_schema_motor.m_initialized_offset = 64;
    m_data_atoms.m_schema_motor.m_prev_target_pos_offset = 68;

    m_data_atoms.m_ang_half.m_first_locked_axis = 0;
    m_data_atoms.m_ang_half.m_constrained_axis_count = 3;
    m_data_atoms.m_lin_0.m_axis_index = 1;
    m_data_atoms.m_lin_1.m_axis_index = 2;
}

void destroy_loaded_prismatic_data(void *p)
{
    static_cast<phys_prismatic_data *>(p)->~phys_prismatic_data();
}

void phys_prismatic_data::set_hinge_motor(phys_motor *motor)
{
    if (motor) {
        add_motor_ref(motor);
    }
    if (m_data_atoms.m_schema_motor.m_schema_motor) {
        remove_motor_ref(m_data_atoms.m_schema_motor.m_schema_motor);
    }
    m_data_atoms.m_schema_motor.m_schema_motor = motor;
}

void phys_prismatic_data::get_constraint_info(phys_constraint_data::constraint_info &info) const
{
    phys_constraint_data::gather_constraint_info(m_data_atoms.get_atoms(), m_data_atoms.get_size_of_all_atoms(), info);
}

void phys_prismatic_data::get_runtime_info(phys_bool want_runtime, phys_constraint_data::runtime_info &info) const
{
    info.m_solver_result_count = 8;
    info.m_external_runtime_size = 72;
}

int phys_prismatic_data::get_type_id() const
{
    return 6;
}

// @address 0x64d8f8 ??1phys_prismatic_data@@UAA@XZ
phys_prismatic_data::~phys_prismatic_data()
{
    if (m_data_atoms.m_schema_motor.m_schema_motor) {
        remove_motor_ref(m_data_atoms.m_schema_motor.m_schema_motor);
    }
}
// match: 0x242920: Direct reciprocal square root preserves the shared constant anchor.
static __forceinline void phys_prismatic_data_normalize3(phys_vec4 &v)
{
    __vector4 temp = __vmsum3fp(v.m_simd_quad, v.m_simd_quad);
    __vector4 scale;
    scale.v[0] = 1.0f / __fsqrts(temp.v[0]);
    scale = __vspltw(scale, 0);
    v.m_simd_quad = __vmulfp(v.m_simd_quad, scale);
}

// @address 0x64d9a8 ?set_in_body_space@phys_prismatic_data@@QAAXABVphys_vec4@@00000@Z
void phys_prismatic_data::set_in_body_space(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_vec4 &axis_a,
    const phys_vec4 &axis_b, const phys_vec4 &axis_a_perp, const phys_vec4 &axis_b_perp)
{
    m_data_atoms.m_transform_atom.m_transform_a.get_translation() = joint_pivot_a;
    m_data_atoms.m_transform_atom.m_transform_b.get_translation() = joint_pivot_b;
    phys_vec4 *base_a = &m_data_atoms.m_transform_atom.m_transform_a.get_column(0);
    base_a[0] = axis_a; phys_prismatic_data_normalize3(base_a[0]);
    base_a[1] = axis_a_perp; phys_prismatic_data_normalize3(base_a[1]);
    base_a[2].set_cross(base_a[0], base_a[1]);
    phys_vec4 *base_b = &m_data_atoms.m_transform_atom.m_transform_b.get_column(0);
    base_b[0] = axis_b; phys_prismatic_data_normalize3(base_b[0]);
    base_b[1] = axis_b_perp; phys_prismatic_data_normalize3(base_b[1]);
    base_b[2].set_cross(base_b[0], base_b[1]);
}
// @address 0x64db18 ?is_valid_aabb@phys_prismatic_data@@UBA?AVphys_bool@@XZ
phys_bool phys_prismatic_data::is_valid_aabb() const
{
    return phys_bool(
        m_data_atoms.m_transform_atom.m_transform_a.get_rotation().is_orthonormal(0.001f) &&
        m_data_atoms.m_transform_atom.m_transform_b.get_rotation().is_orthonormal(0.001f) &&
        m_data_atoms.m_lin_limit.m_min_limit <= m_data_atoms.m_lin_limit.m_max_limit);
}
// @address 0x64dbb8 ?set_motor_active@phys_prismatic_data@@QAAXPAVphys_constraint@@Vphys_bool@@@Z
void phys_prismatic_data::set_motor_active(phys_constraint *instance, phys_bool to_be_enabled)
{
    m_data_atoms.m_schema_motor.m_limit_enabled = to_be_enabled;
    m_data_atoms.m_contact_friction.m_limit_enabled = !to_be_enabled;
    if (instance && instance->get_runtime()) {
        limited_hinge_runtime *runtime = get_runtime(instance->get_runtime());
        runtime->m_solver_results[PRISMATIC_RESULT_MOTOR].m_applied_impulse = 0.0f;
        runtime->m_solver_results[PRISMATIC_RESULT_MOTOR].m_internal_solver_data = 0.0f;
        runtime->m_solver_results[PRISMATIC_RESULT_FRICTION].m_applied_impulse = 0.0f;
        runtime->m_solver_results[PRISMATIC_RESULT_FRICTION].m_internal_solver_data = 0.0f;
    }
}
// @address 0x64dfa0 ??_Gphys_prismatic_data@@UAAPAXI@Z
namespace {
__forceinline void prismatic_perp_vec(const phys_vec4 &direction, phys_vec4 &perpendicular)
{
    int min_axis_idx = 0;
    int first_axis_idx = 1;
    int second_axis_idx = 2;
    float minimum = (float)fabs(direction(0));
    float y = (float)fabs(direction(1));
    float z = (float)fabs(direction(2));
    if (y < minimum) {
        first_axis_idx = 0;
        min_axis_idx = 1;
        minimum = y;
    }
    if (z < minimum) {
        second_axis_idx = min_axis_idx;
    }
    perpendicular.set_zero4();
    perpendicular(first_axis_idx) = direction(second_axis_idx);
    perpendicular(second_axis_idx) = -direction(first_axis_idx);
}
__forceinline void prismatic_inv_rotate_points(const phys_rotation &rotation, const phys_vec4 *vecs_in, int rotate_vec_count, phys_vec4 *vecs_out)
{
    __vector4 c0 = rotation.m_column0.m_simd_quad;
    __vector4 c1 = rotation.m_column1.m_simd_quad;
    __vector4 c2 = rotation.m_column2.m_simd_quad;
    int i = rotate_vec_count - 1;
    do {
        __vector4 v = vecs_in[i].m_simd_quad;
        __vector4 d0 = __vmsum3fp(c0, v);
        __vector4 d1 = __vmsum3fp(c1, v);
        __vector4 d2 = __vmsum3fp(c2, v);
        __vector4 d02 = __vmrghw(d0, d2);
        __vector4 dir1_merged = __vmrghw(d1, phys_simd_vec_0000);
        vecs_out[i].m_simd_quad = __vmrghw(d02, dir1_merged);
    } while (--i >= 0);
}
}
// @address 0x64dd80 ?set_in_world_space@phys_prismatic_data@@QAAXABVphys_transform@@0ABVphys_vec4@@1@Z
void phys_prismatic_data::set_in_world_space(
    const phys_transform &body_a_transform, const phys_transform &body_b_transform, const phys_vec4 &pivot, const phys_vec4 &axis)
{
    phys_vec4 constraint_base_world[3];
    constraint_base_world[0] = axis;
    phys_prismatic_data_normalize3(constraint_base_world[0]);
    prismatic_perp_vec(constraint_base_world[0], constraint_base_world[1]);
    phys_prismatic_data_normalize3(constraint_base_world[1]);
    constraint_base_world[2].set_cross(constraint_base_world[0], constraint_base_world[1]);
    prismatic_inv_rotate_points(body_a_transform.get_rotation(), constraint_base_world, 3, &m_data_atoms.m_transform_atom.m_transform_a.m_rotation.m_column0);
    prismatic_inv_rotate_points(body_b_transform.get_rotation(), constraint_base_world, 3, &m_data_atoms.m_transform_atom.m_transform_b.m_rotation.m_column0);
    m_data_atoms.m_transform_atom.m_transform_a.m_translation.vec_set_transformed_inverse_pos(body_a_transform, pivot);
    m_data_atoms.m_transform_atom.m_transform_b.m_translation.vec_set_transformed_inverse_pos(body_b_transform, pivot);
}

void fixup_loaded_prismatic_data(void *object)
{
    phys_fixup_tag flag;
    flag.m_fixup_finishing = 1;
    new (object) phys_prismatic_data(flag);
}
const void *get_prismatic_data_vtable()
{
    union { __declspec(align(16)) void *ptr; char buffer[sizeof(phys_prismatic_data)]; } object;
    phys_fixup_tag flag;
    new (object.buffer) phys_prismatic_data(flag);
    return object.ptr;
}
