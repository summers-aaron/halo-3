// hkconstraintsolver/hkBuildJacobianFromAtoms.obj: from the records 0x666978 0x666a40 0x666aa0 0x666ac0 0x666ad0 0x666af8 0x666b68 0x666c58 0x666cf8 0x666db0 0x666e60 0x666f08 0x666fc8 0x6671c8 0x6673b8 0x667540 0x667680 0x6677c0 0x667820 0x667a28 0x667aa0 0x667b08 0x667c60 0x667dd8 0x667f40 0x667fe0 0x668398 0x668880 0x668928 0x668c80 0x668fe0 0x669018 0x669288 0x669478 0x669640 0x669678 0x6696b0 0x669830
// @compiler xdk4314
// @flags /TP /O2
// @address 0x666aa0 ?phys_skip_solver_results@@YAXEAAVphys_constraint_query_out@@@Z
// @address 0x666ac0 ?phys_undo_begin_constraints@@YAXAAVphys_constraint_query_out@@@Z
// @address 0x666ad0 ?call_bridge_from_atom@@YAXABUphys_bridge_atom@@ABVphys_constraint_query_in@@AAVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x6677c0 phys_1d_ang_vel_motor_commit_jacobian
// @address 0x666a40 ?build_jacobian_from_lin_atom@@YAXABUphys_lin_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x666af8 ?set_local_transform@@YAXABVphys_transform@@0AAV1@@Z
// @address 0x666b68 ?set_local_transforms_from_atom@@YAXABUphys_set_local_transforms_atom@@ABVphys_constraint_query_in@@AAVphys_transform@@2ABVphys_constraint_query_out@@@Z
// @address 0x666c58 ?set_local_translations_from_atom@@YAXABUphys_set_local_translations_atom@@ABVphys_constraint_query_in@@AAVphys_transform@@2ABVphys_constraint_query_out@@@Z
// @address 0x666cf8 ?set_local_rotations_from_atom@@YAXABUphys_set_local_rotations_atom@@ABVphys_constraint_query_in@@AAVphys_transform@@2ABVphys_constraint_query_out@@@Z
// @address 0x666f08 ?build_jacobian_from_viscous_surface@@YAXABUphys_viscous_surface_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x667a28 ?build_jacobian_from_lin_soft_atom@@YAXABUphys_lin_soft_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x667aa0 ?build_jacobian_from_lin_limit_atom@@YAXABUphys_lin_limit_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x668880 ?build_jacobian_from_lin_friction_atom@@YAXABUphys_lin_friction_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x666978 ?build_jacobian_from_stiff_spring_atom@@YAXABUphys_stiff_spring_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x667540 ?begin_1d_ang_vel_motor_jacobian@@YAXABVphys_vec4@@ABVphys_constraint_query_in@@AAVphys_2ang_jacobian@@AAVphys_1d_bilateral_status@@@Z
// @address 0x667680 phys_1d_ang_vel_motor_begin_jacobian
// @address 0x667f40 ?build_jacobian_from_ang_friction_atom@@YAXABUphys_ang_friction_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x6696b0 ?build_jacobian_from_lin_motor_atom@@YAXABUphys_lin_motor_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x82017310 constraint_axis_byte_offsets
// @address 0x666db0 ?build_jacobian_from_2d_ang_atom@@YAXABUphys_2d_ang_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x666e60 build_jacobian_from_ang_atom
// @address 0x667c60 ?build_jacobian_from_twist_limit_atom@@YA?AW4phys_result@@ABUphys_twist_limit_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_1d_ang_limit_info@@AAVphys_constraint_query_out@@@Z
// @address 0x667b08 ?build_jacobian_from_ang_limit_atom@@YA?AW4phys_result@@ABUphys_ang_limit_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_1d_ang_limit_info@@AAVphys_constraint_query_out@@@Z
// @address 0x667dd8 ?build_jacobian_from_cone_limit_atom@@YA?AW4phys_result@@ABUphys_cone_limit_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_1d_ang_limit_info@@AAVphys_constraint_query_out@@@Z
// @address 0x668fe0 ?phys_init_header@@YAXABVphys_constraint_query_in@@PAVphys_solver_results@@HAAVphys_constraint_query_out@@@Z
// @address 0x669288 ?build_jacobian_from_mass_changer@@YAXABUphys_mass_changer_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x669640 ?begin_constraint_batch@@YAXABVphys_constraint_query_in@@AAVphys_constraint_query_out@@PAVphys_solver_results@@H@Z
// @address 0x666fc8 ?build_ball_socket_jacobian@@YAXABVphys_vec4@@0ABVphys_constraint_query_in@@AAVphys_constraint_query_out@@@Z
// @address 0x667820 ?build_jacobian_from_ball_socket_atom@@YAXABUphys_ball_socket_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x6673b8 phys_1d_ang_limit_build_jacobian
// @address 0x6671c8 phys_ball_socket_build_jacobian_no_schema
// @address 0x669678 phys_begin_constraints
// @address 0x667fe0 ?build_jacobian_from_ang_motor_atom@@YAXABUphys_ang_motor_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x668398 ?build_jacobian_from_ragdoll_motor_atom@@YAXABUphys_ragdoll_motor_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x668928 ?phys_pulley_build_jacobian@@YAXABUphys_pulley_info@@ABVphys_constraint_query_in@@AAVphys_constraint_query_out@@@Z
// @address 0x668c80 ?build_jacobian_from_pulley_atom@@YAXABUphys_pulley_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x669018 ?build_jacobian_from_soft_contact@@YAXABUphys_soft_contact_modifier_atom@@ABVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x669478 ?build_jacobian_from_moving_surface@@YAXABUphys_moving_surface_atom@@AAVphys_constraint_query_in@@ABVphys_transform@@2AAVphys_constraint_query_out@@@Z
// @address 0x669830 phys_solver_build_jacobian_non_contact
#include <stddef.h>
#include <xtl.h>
#include <xboxmath.h>
#include "phys/phys_base.h"
#include "phys/phys_math.h"
#include "phys/phys_math_types.h"
#include "phys/phys_vec4_util.h"
#include "phys/phys_vel_accum.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_constraint_query_out.h"
#include "phys/phys_atoms.h"
#include "phys/phys_contact_point.h"
#include "phys/phys_contact_point_props.h"
#include "phys/phys_1d_bilateral_info.h"
#include "phys/phys_1d_motor_solver_info.h"
#include "phys/phys_motor_constraint_info.h"
#include "phys/phys_jacobian_element.h"
#include "phys/phys_header_schema.h"
#include "phys/phys_shift_solver_results_schema.h"
#include "phys/phys_1d_schemas.h"
#include "phys/phys_1d_lin_motor_schema.h"
#include "phys/phys_build_jacobian_from_atoms.h"
#include "phys/phys_solver_build_jacobian.h"

namespace {
struct phys_pulley_2lin_rhs_info {
    const phys_vel_accum *m_body_a;
    const phys_vel_accum *m_body_b;
    phys_vec4 m_pivot_a;
    phys_vec4 m_pivot_b;
    phys_vec4 m_dir_a;
    phys_vec4 m_dir_b;
    phys_vec4 m_pulley_pivot_a;
    phys_vec4 m_pulley_pivot_b;
    float m_rope_length;
    float m_leverage_on_b;
    float m_query_tau;
    float m_virt_mass_factor;
};
}

/* Unit-owned .rdata: the two 65536.0f conversion vectors precede the axis and cone-angle tables so the axis table sits at +0x20. */
static const __vector4 u16_clip_convert_magic = { 65536.0f, 65536.0f, 65536.0f, 65536.0f };
static const __vector4 u16_convert_magic = { 65536.0f, 65536.0f, 65536.0f, 65536.0f };
/* column byte offsets for axis i, i+1, i+2 */
static const unsigned char constraint_axis_byte_offsets[9] = { 0, 16, 32, 0, 16, 32, 0, 16, 32 };

// match: 0x669830: retained helper references the unit-owned conversion constant preceding the axis table.
inline void phys_vec4_util_to_u16_clip(phys_vec4 &x)
{
    x.m_simd_quad = __vaddfp(x.m_simd_quad, u16_clip_convert_magic);
}
// match: 0x669830: retained helper references the second unit-owned conversion constant preceding the axis table.
inline void phys_vec4_util_to_u16(phys_vec4 &x)
{
    x.m_simd_quad = __vaddfp(x.m_simd_quad, u16_convert_magic);
}

/* walk the modifier chain to the terminal atom */
static __forceinline phys_atom *phys_get_terminal_atom(const phys_modifier_atom &atom)
{
    phys_atom *child = atom.m_child_shape;
    while (child->is_modifier_type())
    {
        const phys_modifier_atom *m = static_cast<const phys_modifier_atom *>(child);
        child = m->m_child_shape;
    }
    return child;
}

static __forceinline void commit_1d_ang_vel_motor(
    phys_1d_motor_info &info, const phys_constraint_query_in &in, phys_2ang_jacobian &jacobian_elem, phys_constraint_query_out &out)
{
    float rhs = info.m_target_pos * in.m_substep_inv_dt;
    jacobian_elem.set_ang_rhs(rhs);
    phys_1d_ang_motor_schema *jacobian_schema = reinterpret_cast<phys_1d_ang_motor_schema *>(out.m_jacobian_schemas.val());
    phys_1d_motor_solver_info &motor = jacobian_schema->init_1d_ang_motor(jacobian_elem);
    motor.m_min_impulse = info.m_max_force * in.m_micro_step_dt;
    motor.m_max_impulse = info.m_min_force * in.m_micro_step_dt;
    motor.m_query_tau = info.m_target_vel;
    motor.m_solver_damping = info.m_query_tau;
    motor.m_rhs_factor = info.m_solver_damping;
    out.m_jacobian_schemas = jacobian_schema + 1;
}

static __forceinline void build_ang_begin(
    const phys_vel_accum *mA, const phys_vel_accum *mB, const phys_vec4 &axis, float virt_mass_factor, phys_2ang_jacobian &lin_ang_jac)
{
    phys_vec4_util::reserve_registers(0xf0000000);
    if (mA->m_mat_is_identity) {
        lin_ang_jac.m_ang_jacobian[0] = axis;
    } else {
        lin_ang_jac.m_ang_jacobian[0].set_rotated_dir(mA->get_core_from_world(), axis);
    }
    if (mB->m_mat_is_identity) {
        lin_ang_jac.m_ang_jacobian[1].set_neg4(axis);
    } else {
        phys_vec4 neg_axis;
        neg_axis.set_neg4(axis);
        lin_ang_jac.m_ang_jacobian[1].set_rotated_dir(mB->get_core_from_world(), neg_axis);
    }
    float diag_term = lin_ang_jac.get_ang_diag(*mA, *mB);
    lin_ang_jac.set_inv_jac_diag(virt_mass_factor / diag_term);
}
static __forceinline void flush_jacobian_builder() {}
static __forceinline void ang_vel_motor_begin(
    const phys_vec4 &axis, const phys_constraint_query_in &in, phys_2ang_jacobian &jacobian_elem, phys_1d_bilateral_status &status)
{
    build_ang_begin(in.m_body_a, in.m_body_b, axis, 1.0f, jacobian_elem);
    phys_vec4_util::release_registers(0xf0000000);
    flush_jacobian_builder();
    status.m_virtual_mass = jacobian_elem.get_inv_jac_diag();
}

static __forceinline void build_from_stiff_spring_atom(const phys_stiff_spring_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    phys_1d_lin_bilateral_info info;
    info.m_pivot_a = base_a.get_translation();
    info.m_pivot_b = base_b.get_translation();
    phys_vec4 direction;
    direction.set_sub4(info.m_pivot_a, info.m_pivot_b);
    float length = direction.normalize_xyz_with_length();
    if (length > 0.0f) {
        info.m_constrained_dof_w = direction;
    } else {
        info.m_constrained_dof_w = phys_transform::get_identity().get_column(0);
        length = 0.0f;
    }
    float error = atom.m_length - length;
    phys_1d_lin_bilateral_build_jacobian_rhs(info, in, out, error);
}
void build_jacobian_from_stiff_spring_atom(const phys_stiff_spring_atom &atom, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_stiff_spring_atom(atom, in, base_a, base_b, out);
}

static __forceinline void build_from_lin_atom(const phys_lin_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    phys_1d_lin_bilateral_info info;
    info.m_pivot_a = base_a.get_translation();
    info.m_pivot_b = base_b.get_translation();
    info.m_constrained_dof_w = base_b.get_column(atom.m_axis_index);
    phys_1d_lin_bilateral_build_jacobian(info, in, out);
}
void build_jacobian_from_lin_atom(const phys_lin_atom &atom, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_lin_atom(atom, in, base_a, base_b, out);
}

static __forceinline void skip_solver_results(unsigned char count, phys_constraint_query_out &out)
{
    phys_shift_solver_results_schema *jacobian_schema = reinterpret_cast<phys_shift_solver_results_schema *>(out.m_jacobian_schemas.val());
    jacobian_schema->init_shift_results_ptr(count);
    out.m_jacobian_schemas = jacobian_schema + 1;
}
void phys_skip_solver_results(unsigned char count, phys_constraint_query_out &out)
{
    skip_solver_results(count, out);
}

static __forceinline void invoke_atom_bridge(
    const phys_bridge_atom &atom, const phys_constraint_query_in &in, phys_transform &, phys_transform &, phys_constraint_query_out &out)
{
    phys_undo_begin_constraints(out);
    atom.m_build_jacobian_func(atom.m_constraint_data, in, out);
}
void call_bridge_from_atom(const phys_bridge_atom &atom, const phys_constraint_query_in &in, phys_transform &base_a,
    phys_transform &base_b, phys_constraint_query_out &out)
{
    invoke_atom_bridge(atom, in, base_a, base_b, out);
}

static __forceinline void compose_local_xform(const phys_transform &b, const phys_transform &a, phys_transform &out)
{
    phys_vec4_util::rotate_points(a.get_rotation(), &b.get_column(0), 4, &out.get_column(0));
    out.get_translation().add4(a.get_translation());
}

void set_local_transform(const phys_transform &b, const phys_transform &a, phys_transform &out)
{
    compose_local_xform(b, a, out);
}

static __forceinline void apply_local_transforms(const phys_set_local_transforms_atom &atom, const phys_constraint_query_in &in,
    phys_transform &base_a, phys_transform &base_b, const phys_constraint_query_out &)
{
    compose_local_xform(atom.m_transform_a, *in.m_transform_a.val(), base_a);
    compose_local_xform(atom.m_transform_b, *in.m_transform_b.val(), base_b);
}
void set_local_transforms_from_atom(const phys_set_local_transforms_atom &atom, const phys_constraint_query_in &in, phys_transform &base_a,
    phys_transform &base_b, const phys_constraint_query_out &out)
{
    apply_local_transforms(atom, in, base_a, base_b, out);
}

static __forceinline void apply_local_translations(const phys_set_local_translations_atom &atom, const phys_constraint_query_in &in,
    phys_transform &base_a, phys_transform &base_b, const phys_constraint_query_out &)
{
    base_a.get_translation().set_transformed_pos(*in.m_transform_a.val(), atom.m_translation_a);
    base_b.get_translation().set_transformed_pos(*in.m_transform_b.val(), atom.m_translation_b);
}
void set_local_translations_from_atom(const phys_set_local_translations_atom &atom, const phys_constraint_query_in &in, phys_transform &base_a,
    phys_transform &base_b, const phys_constraint_query_out &out)
{
    apply_local_translations(atom, in, base_a, base_b, out);
}

static __forceinline void apply_local_rotations(const phys_set_local_rotations_atom &atom, const phys_constraint_query_in &in,
    phys_transform &base_a, phys_transform &base_b, const phys_constraint_query_out &)
{
    phys_vec4_util::rotate_points(in.m_transform_a.val()->get_rotation(), &atom.m_rotation_a.get_column(0), 4, &base_a.get_rotation().get_column(0));
    phys_vec4_util::rotate_points(in.m_transform_b.val()->get_rotation(), &atom.m_rotation_b.get_column(0), 4, &base_b.get_rotation().get_column(0));
}
void set_local_rotations_from_atom(const phys_set_local_rotations_atom &atom, const phys_constraint_query_in &in, phys_transform &base_a,
    phys_transform &base_b, const phys_constraint_query_out &out)
{
    apply_local_rotations(atom, in, base_a, base_b, out);
}

static __forceinline void build_from_viscous_surface(const phys_viscous_surface_atom &atom,
    const phys_constraint_query_in &in, const phys_transform &, const phys_transform &, phys_constraint_query_out &out)
{
    phys_undo_begin_constraints(out);
    phys_constraint_query_in query_in_copy = in;
    phys_atom *child = phys_get_terminal_atom(atom);
    phys_simple_contact_add_viscosity(&static_cast<phys_simple_contact_atom *>(child)->m_bvh_info);
    phys_solver_build_jacobian_atoms(atom.m_child_shape, atom.m_child_size, query_in_copy, out);
}
void build_jacobian_from_viscous_surface(const phys_viscous_surface_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_viscous_surface(atom, in, base_a, base_b, out);
}

void begin_1d_ang_vel_motor_jacobian(
    const phys_vec4 &axis, const phys_constraint_query_in &in, phys_2ang_jacobian &jacobian_elem, phys_1d_bilateral_status &status)
{
    ang_vel_motor_begin(axis, in, jacobian_elem, status);
}
extern "C" void phys_1d_ang_vel_motor_begin_jacobian(
    const phys_vec4 &axis, const phys_constraint_query_in &in, phys_2ang_jacobian &jacobian_elem, phys_1d_bilateral_status &status)
{
    ang_vel_motor_begin(axis, in, jacobian_elem, status);
}

extern "C" void phys_1d_ang_vel_motor_commit_jacobian(
    phys_1d_motor_info &info, const phys_constraint_query_in &in, phys_2ang_jacobian &jacobian_elem, phys_constraint_query_out &out)
{
    commit_1d_ang_vel_motor(info, in, jacobian_elem, out);
}

static __forceinline void build_from_lin_soft_atom(const phys_lin_soft_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    phys_1d_lin_bilateral_tau_info info;
    info.m_pivot_a = base_a.get_translation();
    info.m_pivot_b = base_b.get_translation();
    info.m_constrained_dof_w = base_b.get_column(atom.m_axis_index);
    info.m_query_tau = atom.m_query_tau;
    info.m_solver_damping = atom.m_solver_damping;
    phys_1d_lin_bilateral_user_tau_build_jacobian(info, in, out);
}
void build_jacobian_from_lin_soft_atom(const phys_lin_soft_atom &atom, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_lin_soft_atom(atom, in, base_a, base_b, out);
}

/* Always uses column 0 here without loading m_axisIndex. */
static __forceinline void build_from_lin_limit_atom(const phys_lin_limit_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    phys_1d_lin_limit_info info;
    info.m_pivot_a = base_a.get_translation();
    info.m_pivot_b = base_b.get_translation();
    info.m_min_limit = atom.m_min_limit;
    info.m_max_limit = atom.m_max_limit;
    info.m_constrained_dof_w = base_b.get_column(0);
    phys_1d_lin_limit_build_jacobian(info, in, out);
}
void build_jacobian_from_lin_limit_atom(const phys_lin_limit_atom &atom, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_lin_limit_atom(atom, in, base_a, base_b, out);
}

static __forceinline void build_from_ang_friction_atom(const phys_ang_friction_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &, phys_constraint_query_out &out)
{
    if (atom.m_limit_enabled != 0 && atom.m_max_friction_torque != 0.0f) {
        phys_1d_ang_friction_info info;
        info.m_constrained_dof_w = &base_a.get_column(atom.m_first_friction_axis);
        info.m_max_friction_torque = atom.m_max_friction_torque;
        info.m_friction_count = atom.m_friction_axis_count;
        info.m_last_solver_results = static_cast<phys_solver_results *>(in.m_constraint_runtime.val());
        phys_1d_ang_friction_build_jacobian(info, in, out);
    } else {
        skip_solver_results(atom.m_friction_axis_count, out);
    }
}
void build_jacobian_from_ang_friction_atom(const phys_ang_friction_atom &atom, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_ang_friction_atom(atom, in, base_a, base_b, out);
}

static __forceinline void build_from_lin_friction_atom(const phys_lin_friction_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    if (atom.m_limit_enabled && atom.m_max_friction_force > 0.0f) {
        phys_1d_lin_friction_info info;
        info.m_bilateral_pivot = base_a.get_translation();
        info.m_constrained_dof_w = base_b.get_column(atom.m_friction_axis);
        info.m_max_friction_force = atom.m_max_friction_force;
        info.m_last_solver_results = static_cast<phys_solver_results *>(in.m_constraint_runtime.val());
        phys_1d_lin_friction_build_jacobian(info, in, out);
    } else {
        skip_solver_results(1, out);
    }
}
void build_jacobian_from_lin_friction_atom(const phys_lin_friction_atom &atom, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_lin_friction_atom(atom, in, base_a, base_b, out);
}

static __forceinline void build_from_ang_atom(const phys_ang_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    phys_1d_ang_bilateral_info info;
    for (int i = atom.m_first_locked_axis; i < atom.m_first_locked_axis + atom.m_constrained_axis_count; i++)
    {
        info.m_constrained_dof_w = *phys_add_byte_offset_const(&base_a.get_column(0), constraint_axis_byte_offsets[i]);
        info.m_zero_err_axis_a_in_w = *phys_add_byte_offset_const(&base_a.get_column(0), constraint_axis_byte_offsets[i + 1]);
        info.m_perp_zero_err_axis_b = *phys_add_byte_offset_const(&base_b.get_column(0), constraint_axis_byte_offsets[i + 2]);
        phys_1d_ang_bilateral_build_jacobian(info, in, out);
    }
}
extern "C" void build_jacobian_from_ang_atom(const phys_ang_atom &atom, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_ang_atom(atom, in, base_a, base_b, out);
}

static __forceinline void build_from_2d_ang_atom(const phys_2d_ang_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    int axis = atom.m_free_rot_axis;
    int axis_offset_a = constraint_axis_byte_offsets[atom.m_free_rot_axis + 1];
    int axis_offset_b = constraint_axis_byte_offsets[atom.m_free_rot_axis + 2];
    phys_1d_ang_bilateral_info info;
    info.m_zero_err_axis_a_in_w = base_a.get_column(axis);
    info.m_perp_zero_err_axis_b = *phys_add_byte_offset_const(&base_b.get_column(0), axis_offset_a);
    info.m_constrained_dof_w = *phys_add_byte_offset_const(&base_b.get_column(0), axis_offset_b);
    phys_1d_ang_bilateral_build_jacobian(info, in, out);
    info.m_perp_zero_err_axis_b = *phys_add_byte_offset_const(&base_b.get_column(0), axis_offset_b);
    info.m_constrained_dof_w.set_neg4(*phys_add_byte_offset_const(&base_b.get_column(0), axis_offset_a));
    phys_1d_ang_bilateral_build_jacobian(info, in, out);
}
void build_jacobian_from_2d_ang_atom(const phys_2d_ang_atom &atom, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_2d_ang_atom(atom, in, base_a, base_b, out);
}

static __forceinline void build_from_lin_motor_atom(const phys_lin_motor_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    unsigned char *initialized = static_cast<unsigned char *>(phys_add_byte_offset(in.m_constraint_runtime.val(), atom.m_initialized_offset));
    float *prev_target_pos = static_cast<float *>(phys_add_byte_offset(in.m_constraint_runtime.val(), atom.m_prev_target_pos_offset));
    if (atom.m_limit_enabled && atom.m_schema_motor != 0) {
        phys_motor *motor = atom.m_schema_motor;
        if (!*initialized) {
            *prev_target_pos = atom.m_target_pos;
        }
        phys_1lin2ang_jacobian *jacobian_elem = reinterpret_cast<phys_1lin2ang_jacobian *>(out.m_jacobian_schemas.val());
        phys_motor_input motor_in;
        phys_1d_lin_vel_motor_begin_jacobian(base_b.get_column(atom.m_motor_axis), base_a.get_translation(), in, *jacobian_elem, motor_in);
        phys_vec4 pos_diff;
        pos_diff.set_sub4(base_a.get_translation(), base_b.get_translation());
        motor_in.m_step_info = &in;
        motor_in.m_last_results = *static_cast<const phys_solver_results *>(in.m_constraint_runtime.val());
        motor_in.m_delta_target = atom.m_target_pos - *prev_target_pos;
        motor_in.m_pos_error = *prev_target_pos - float(pos_diff.dot3(base_b.get_column(atom.m_motor_axis)));
        phys_motor_output motor_out;
        phys_compute_motor_data(motor, &motor_in, &motor_out);
        phys_1d_lin_vel_motor_commit_jacobian(motor_out, in, *jacobian_elem, out);
        if (*prev_target_pos != atom.m_target_pos || !*initialized) {
            *prev_target_pos = atom.m_target_pos;
            *initialized = 1;
        }
    } else {
        skip_solver_results(1, out);
    }
}
void build_jacobian_from_lin_motor_atom(const phys_lin_motor_atom &atom, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_lin_motor_atom(atom, in, base_a, base_b, out);
}

static __forceinline void phys_constraint_util_relative_angle(const phys_vec4 &twist_axis_a_ws,
    const phys_vec4 &twist_axis_b_ws, const phys_vec4 &plane_axis_a_ws, const phys_vec4 &plane_axis_b_ws, phys_vec4 &axis_out_vec,
    phys_padded<float> &angle_out, phys_padded<float> &max_tau_out)
{
    phys_vec4 world_twist_axis;
    world_twist_axis.set_add4(twist_axis_b_ws, twist_axis_a_ws);
    const float twist_axis_world_len = world_twist_axis.length_3d();
    if (twist_axis_world_len > 1e-16f)
    {
        max_tau_out = twist_axis_world_len * 0.5f;
        world_twist_axis.mul4(1.0f / twist_axis_world_len);
    }
    else
    {
        max_tau_out = 0.0f;
        world_twist_axis = twist_axis_b_ws;
    }
    phys_rotation m_world_rot;
    {
        phys_vec4 &c0 = m_world_rot.get_column(0);
        phys_vec4 &c1 = m_world_rot.get_column(1);
        phys_vec4 &c2 = m_world_rot.get_column(2);
        c0 = world_twist_axis;
        c1.set_cross(world_twist_axis, plane_axis_b_ws);
        c2.set_cross(c1, c0);
    }
    float d1 = m_world_rot.get_column(1).dot3(plane_axis_a_ws);
    float d2 = m_world_rot.get_column(2).dot3(plane_axis_a_ws);
    float cur_twist_angle = phys_math_atan2f_approx(d1, d2);
    angle_out = cur_twist_angle;
    axis_out_vec = world_twist_axis;
}

static __forceinline phys_result build_from_twist_limit_atom(const phys_twist_limit_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &transform, const phys_transform &transform2, phys_1d_ang_limit_info &info, phys_constraint_query_out &)
{
    info.m_constrained_dof_w = transform.get_column(atom.m_twist_axis);
    info.m_min_limit = atom.m_min_angle;
    info.m_max_limit = atom.m_atom_max_angle;
    phys_padded<float> max_tau;
    phys_constraint_util_relative_angle(transform.get_column(atom.m_twist_axis), transform2.get_column(atom.m_twist_axis),
        transform.get_column(atom.m_ref_axis), transform2.get_column(atom.m_ref_axis), info.m_constrained_dof_w, info.m_computed_angle, max_tau);
    info.m_query_tau = in.m_query_tau * atom.m_ang_limits_tau_factor * max_tau * max_tau;
    return PHYS_SUCCESS;
}
phys_result build_jacobian_from_twist_limit_atom(const phys_twist_limit_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &transform, const phys_transform &transform2, phys_1d_ang_limit_info &info, phys_constraint_query_out &out)
{
    return build_from_twist_limit_atom(atom, in, transform, transform2, info, out);
}

static __forceinline phys_result build_from_ang_limit_atom(const phys_ang_limit_atom &atom,
    const phys_constraint_query_in &in, const phys_transform &xform_a, const phys_transform &xform_b,
    phys_1d_ang_limit_info &obj, phys_constraint_query_out &)
{
    phys_result result;
    if (atom.m_limit_enabled)
    {
        int axis = atom.m_limit_axis;
        int zero_err_axis_offset = constraint_axis_byte_offsets[atom.m_limit_axis + 1];
        int perp_axis_offset = constraint_axis_byte_offsets[atom.m_limit_axis + 2];
        obj.m_query_tau = in.m_query_tau * atom.m_ang_limits_tau_factor;
        obj.m_constrained_dof_w = xform_a.get_column(axis);
        phys_vec4 column_a; column_a = *phys_add_byte_offset_const(&xform_a.get_column(0), zero_err_axis_offset);
        phys_vec4 column_b; column_b = *phys_add_byte_offset_const(&xform_b.get_column(0), perp_axis_offset);
        phys_vec4 axis_b; axis_b = xform_b.get_column(axis);
        obj.m_min_limit = atom.m_min_angle;
        obj.m_max_limit = atom.m_atom_max_angle;

        float sine = column_b.dot3(column_a);
        phys_vec4 cross;
        cross.set_cross(column_b, axis_b);
        float cosine = cross.dot3(column_a);

        obj.m_computed_angle = phys_math_atan2f_approx(sine, cosine);

        float offset = -1.0f * static_cast<phys_solver_results *>(in.m_constraint_runtime.val())->m_internal_solver_data;
        while (offset - obj.m_computed_angle > 3.1415927f)
            obj.m_computed_angle = obj.m_computed_angle + 6.2831855f;
        while (obj.m_computed_angle - offset > 3.1415927f)
            obj.m_computed_angle = obj.m_computed_angle - 6.2831855f;
        result = PHYS_SUCCESS;
    }
    else
    {
        result = PHYS_FAILURE;
    }
    return result;
}
phys_result build_jacobian_from_ang_limit_atom(const phys_ang_limit_atom &atom,
    const phys_constraint_query_in &in, const phys_transform &xform_a, const phys_transform &xform_b,
    phys_1d_ang_limit_info &obj, phys_constraint_query_out &out)
{
    return build_from_ang_limit_atom(atom, in, xform_a, xform_b, obj, out);
}

static __forceinline phys_result build_jacobian_from_cone_limit(const phys_cone_limit_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_1d_ang_limit_info &info, phys_constraint_query_out &)
{
    static const float cone_angle_offset_table[2] = { 0.0f, 1.5707964f };
    static const float cone_angle_sign_table[2] = { 1.0f, -1.0f };
    const phys_vec4 &twist_axis_a_ws = base_a.get_column(atom.m_twist_axis_in_a);
    const phys_vec4 &ref_axis_b_ws = base_b.get_column(atom.m_ref_axis_in_b);
    phys_vec4 perp_vector;
    perp_vector.set_cross(twist_axis_a_ws, ref_axis_b_ws);
    float perp_len_sq = perp_vector.length_sq_3d();
    if (perp_len_sq < 1.1920929e-07f)
        return PHYS_FAILURE;

    float perp_len = phys_math::sqrt(perp_len_sq);
    float weld_cos_angle = twist_axis_a_ws.dot3(ref_axis_b_ws);
    float angle = cone_angle_sign_table[atom.m_angle_measure_mode] * phys_math_atan2f_approx(perp_len, weld_cos_angle)
        + cone_angle_offset_table[atom.m_angle_measure_mode];
    float sign = cone_angle_sign_table[1 - atom.m_angle_measure_mode];
    info.m_query_tau = in.m_query_tau * atom.m_ang_limits_tau_factor;
    float inv_perp_len = phys_math::inv_sqrt(perp_len_sq);
    perp_vector.mul4(phys_simd_scalar(inv_perp_len));
    info.m_constrained_dof_w.set_mul4(phys_simd_scalar(sign), perp_vector);
    info.m_computed_angle = angle;
    info.m_min_limit = atom.m_min_angle;
    info.m_max_limit = atom.m_atom_max_angle;
    return PHYS_SUCCESS;
}
phys_result build_jacobian_from_cone_limit_atom(const phys_cone_limit_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_1d_ang_limit_info &info, phys_constraint_query_out &out)
{
    return build_jacobian_from_cone_limit(atom, in, base_a, base_b, info, out);
}

void phys_undo_begin_constraints(phys_constraint_query_out &out)
{
    phys_header_schema *hs = reinterpret_cast<phys_header_schema *>(out.m_jacobian_schemas.val());
    out.m_jacobian_schemas = hs - 1;
}

void phys_init_header(const phys_constraint_query_in &in, phys_solver_results *sr, int solver_result_stride, phys_constraint_query_out &out)
{
    phys_header_schema *hs = reinterpret_cast<phys_header_schema *>(out.m_jacobian_schemas.val());
    hs->init_schema_header(in.m_accum_a_index, in.m_accum_b_index, sr, solver_result_stride);
    out.m_jacobian_schemas = hs + 1;
}

static __forceinline void build_from_mass_changer(const phys_mass_changer_atom &atom,
    const phys_constraint_query_in &in, const phys_transform &, const phys_transform &, phys_constraint_query_out &out)
{
    float factor_a = atom.m_factor_a;
    float factor_b = atom.m_factor_b;
    phys_constraint_query_in new_query_in = in;
    const phys_atom *child = atom.m_child_shape;
    int child_dma_size = atom.m_child_size;
    phys_undo_begin_constraints(out);
    phys_init_header(new_query_in, 0, 0, out);
    phys_vec4 orig_inv_mass_a = in.m_body_a->m_inv_masses;
    phys_vec4 orig_inv_mass_b = in.m_body_b->m_inv_masses;
    phys_vel_accum agent_body_a = *in.m_body_a;
    phys_vel_accum agent_body_b = *in.m_body_b;
    new_query_in.m_body_a = &agent_body_a;
    new_query_in.m_body_b = &agent_body_b;
    phys_vec4 inv_mass_a; inv_mass_a.set_mul4(factor_a, orig_inv_mass_a);
    phys_vec4 inv_mass_b; inv_mass_b.set_mul4(factor_b, orig_inv_mass_b);
    agent_body_a.m_inv_masses.mul4(factor_a);
    agent_body_b.m_inv_masses.mul4(factor_b);
    phys_inv_mass_build_jacobian(inv_mass_a, inv_mass_b, out);
    phys_solver_build_jacobian_atoms(child, child_dma_size, new_query_in, out);
    phys_inv_mass_build_jacobian(orig_inv_mass_a, orig_inv_mass_b, out);
}
void build_jacobian_from_mass_changer(const phys_mass_changer_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &xform_a, const phys_transform &xform_b, phys_constraint_query_out &out)
{
    build_from_mass_changer(atom, in, xform_a, xform_b, out);
}

static __forceinline void phys_begin_constraints_impl(const phys_constraint_query_in &in, phys_constraint_query_out &out,
    phys_solver_results *sr, int solver_result_stride)
{
    phys_header_schema *hs = reinterpret_cast<phys_header_schema *>(out.m_jacobian_schemas.val());
    hs->init_schema_header(in.m_accum_a_index, in.m_accum_b_index, sr, static_cast<unsigned short>(solver_result_stride));
    out.m_jacobian_schemas = hs + 1;
    phys_vec4_util::reserve_registers(0x0fff0000);
}
void begin_constraint_batch(const phys_constraint_query_in &in, phys_constraint_query_out &out, phys_solver_results *sr, int solver_result_stride)
{
    phys_begin_constraints_impl(in, out, sr, solver_result_stride);
}

static __forceinline void phys_ball_socket_build_lin_and_rhs(const phys_vel_accum *mA, const phys_vel_accum *mB,
    const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_vec4 &dir, float tau, float virt_mass_factor,
        phys_1lin2ang_jacobian &lin_ang_jac)
{
    lin_ang_jac.m_lin_part_0 = dir;
    phys_vec4 rel_a; rel_a.set_sub4(joint_pivot_a, mA->get_center_of_mass_in_world());
    phys_vec4 rel_b; rel_b.set_sub4(joint_pivot_b, mB->get_center_of_mass_in_world());
    {
        phys_vec4 h; h.set_cross(rel_a, dir);
        if (mA->m_mat_is_identity) {
            lin_ang_jac.m_ang_jacobian[0] = h;
        } else {
            lin_ang_jac.m_ang_jacobian[0].set_rotated_dir(mA->get_core_from_world(), h);
        }
    }
    {
        phys_vec4 h; h.set_cross(dir, rel_b);
        if (mB->m_mat_is_identity) {
            lin_ang_jac.m_ang_jacobian[1] = h;
        } else {
            lin_ang_jac.m_ang_jacobian[1].set_rotated_dir(mB->get_core_from_world(), h);
        }
    }
    float diag_term = lin_ang_jac.get_jacobian_diag(*mA, *mB);
    lin_ang_jac.set_user_value(diag_term);
    lin_ang_jac.set_inv_jac_diag(virt_mass_factor / diag_term);
    phys_vec4 dist; dist.set_sub4(joint_pivot_b, joint_pivot_a);
    float rhs = float(dist.dot3(dir)) * tau;
    lin_ang_jac.set_rhs(rhs);
}

static __forceinline void phys_ball_socket_build_jacobian(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b,
    const phys_constraint_query_in &in, phys_constraint_query_out &out)
{
    const phys_vel_accum *agent_body_a = in.m_body_a;
    const phys_vel_accum *agent_body_b = in.m_body_b;
    phys_1d_bilateral_schema *jacobian_schema = reinterpret_cast<phys_1d_bilateral_schema *>(out.m_jacobian_schemas.val());
    const phys_vec4 *dir = &phys_transform::get_identity().get_rotation().get_column(0);
    for (int i = 0; i < 3; i++) {
        phys_1lin2ang_jacobian &lin_ang_jac = jacobian_schema->m_contact_jac;
        phys_ball_socket_build_lin_and_rhs(agent_body_a, agent_body_b, joint_pivot_a, joint_pivot_b, *dir, in.m_rhs_factor, in.m_virt_mass_factor, lin_ang_jac);
        flush_jacobian_builder();
        jacobian_schema->init_1d_bilateral(lin_ang_jac);
        jacobian_schema++;
        dir++;
    }
    out.m_jacobian_schemas = jacobian_schema;
}
void build_ball_socket_jacobian(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_constraint_query_in &in,
    phys_constraint_query_out &out)
{
    phys_ball_socket_build_jacobian(joint_pivot_a, joint_pivot_b, in, out);
}

void build_jacobian_from_ball_socket_atom(const phys_ball_socket_atom &, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    phys_ball_socket_build_jacobian(base_a.get_translation(), base_b.get_translation(), in, out);
}

static __forceinline void phys_1d_ang_limit_build_ang_end(phys_2ang_jacobian &jacobian_elem, float rhs, float factor)
{
    jacobian_elem.set_ang_rhs(rhs * factor);
    phys_vec4_util::release_registers(0xf0000000);
}

static __forceinline void build_1d_ang_limit(const phys_1d_ang_limit_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out)
{
    phys_2ang_jacobian *lin_ang_jac = static_cast<phys_2ang_jacobian *>(static_cast<void *>(out.m_jacobian_schemas.val()));
    float virt_mass_factor = info.m_query_tau;
    build_ang_begin(in.m_body_a, in.m_body_b, info.m_constrained_dof_w, virt_mass_factor, *lin_ang_jac);
    float dt = -in.m_substep_inv_dt;
    float min_impulse = info.m_min_limit * dt * virt_mass_factor;
    float max_impulse = info.m_max_limit * dt * virt_mass_factor;
    float rhs = info.m_computed_angle * dt;
    phys_1d_ang_limit_build_ang_end(*lin_ang_jac, rhs, 1.0f);
    phys_1d_ang_limits_schema *jacobian_schema = static_cast<phys_1d_ang_limits_schema *>(static_cast<void *>(out.m_jacobian_schemas.val()));
    out.m_jacobian_schemas = jacobian_schema + 1;
    flush_jacobian_builder();
    jacobian_schema->init_1d_ang_limits(*lin_ang_jac, min_impulse, max_impulse, virt_mass_factor);
}
extern "C" void phys_1d_ang_limit_build_jacobian(const phys_1d_ang_limit_info &info, const phys_constraint_query_in &in, phys_constraint_query_out &out)
{
    build_1d_ang_limit(info, in, out);
}

extern "C" void phys_ball_socket_build_jacobian_no_schema(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_constraint_query_in &in,
    phys_1lin2ang_jacobian *jacobian_list)
{
    const phys_vel_accum *agent_body_a = in.m_body_a;
    const phys_vel_accum *agent_body_b = in.m_body_b;
    const phys_vec4 *dir = &phys_transform::get_identity().get_column(0);
    for (int i = 0; i < 3; i++) {
        phys_1lin2ang_jacobian &lin_ang_jac = jacobian_list[i];
        phys_ball_socket_build_lin_and_rhs(agent_body_a, agent_body_b, joint_pivot_a, joint_pivot_b, *dir, in.m_rhs_factor, in.m_virt_mass_factor, lin_ang_jac);
        dir++;
    }
}

extern "C" void phys_begin_constraints(const phys_constraint_query_in &in, phys_constraint_query_out &out, phys_solver_results *sr, int solver_result_stride)
{
    phys_begin_constraints_impl(in, out, sr, solver_result_stride);
}

static __forceinline void build_from_ang_motor_atom(const phys_ang_motor_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    unsigned char *initialized = static_cast<unsigned char *>(phys_add_byte_offset(in.m_constraint_runtime.val(), atom.m_initialized_offset));
    float *prev_target_angle = static_cast<float *>(phys_add_byte_offset(in.m_constraint_runtime.val(), atom.m_prev_target_angle_offset));
    if (atom.m_schema_motor != 0 && atom.m_limit_enabled) {
        phys_motor *motor = atom.m_schema_motor;
        int axis = atom.m_motor_axis;
        int axis_offset_a = constraint_axis_byte_offsets[atom.m_motor_axis + 1];
        int axis_offset_b = constraint_axis_byte_offsets[atom.m_motor_axis + 2];
        const phys_vec4 &perp_axis_a = *phys_add_byte_offset_const(&base_a.get_column(0), axis_offset_a);
        const phys_vec4 &perp_axis_b = *phys_add_byte_offset_const(&base_b.get_column(0), axis_offset_b);
        float edge_weld_sin_angle = perp_axis_b.dot3(perp_axis_a);
        phys_vec4 cross;
        cross.set_cross(perp_axis_b, base_b.get_column(axis));
        float weld_cos_angle = cross.dot3(perp_axis_a);
        float angle = phys_math_atan2f_approx(edge_weld_sin_angle, weld_cos_angle);
        if (atom.m_ang_limit_result_offset) {
            float last_angle = -1.0f *
                static_cast<phys_solver_results *>(
                    phys_add_byte_offset(in.m_constraint_runtime.val(), atom.m_ang_limit_result_offset))
                    ->m_internal_solver_data;
            while (last_angle - angle > 3.1415927f) {
                angle += 6.2831855f;
            }
            while (angle - last_angle > 3.1415927f) {
                angle -= 6.2831855f;
            }
        }
        if (!*initialized) {
            *prev_target_angle = angle;
        }
        phys_motor_input motor_in;
        phys_2ang_jacobian *jacobian_elem = static_cast<phys_2ang_jacobian *>(static_cast<void *>(out.m_jacobian_schemas.val()));
        ang_vel_motor_begin(base_a.get_column(atom.m_motor_axis), in, *jacobian_elem, motor_in);
        motor_in.m_step_info = &in;
        motor_in.m_last_results = *static_cast<const phys_solver_results *>(in.m_constraint_runtime.val());
        phys_padded<float> *delta_target = &motor_in.m_delta_target;
        *delta_target = atom.m_target_angle - *prev_target_angle;
        phys_padded<float> *pos_error_ptr = &motor_in.m_pos_error;
        *pos_error_ptr = *prev_target_angle - angle;
        phys_motor_output motor_out;
        phys_compute_motor_data(motor, &motor_in, &motor_out);
        commit_1d_ang_vel_motor(motor_out, in, *jacobian_elem, out);
        if (*prev_target_angle != atom.m_target_angle || !*initialized) {
            *prev_target_angle = atom.m_target_angle;
            *initialized = 1;
        }
    } else {
        skip_solver_results(1, out);
    }
}
void build_jacobian_from_ang_motor_atom(const phys_ang_motor_atom &atom, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_ang_motor_atom(atom, in, base_a, base_b, out);
}

static __forceinline void build_from_ragdoll_motor(const phys_ragdoll_motor_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    unsigned char *initialized = static_cast<unsigned char *>(phys_add_byte_offset(in.m_constraint_runtime.val(), atom.m_initialized_offset));
    float *prev_target_angles = static_cast<float *>(phys_add_byte_offset(in.m_constraint_runtime.val(), atom.m_prev_target_angles_offset));
    if (atom.m_limit_enabled)
    {
        phys_mat3 target_frame_a_ws;
        phys_mat3_set_mul_mat3(&target_frame_a_ws, in.m_transform_b->get_rotation(), atom.m_target_frame_a_in_b);
        phys_motor_input motor_in;
        motor_in.m_step_info = &in;
        bool atom_state_changed = false;
        int twist_axis_table[3] = { 0, 1, 2 };
        int plane_axis_table[3] = { 1, 0, 0 };
        for (int i = 0; i < 3; i++)
        {
            if (atom.m_axis_motors[i])
            {
                phys_motor *motor = atom.m_axis_motors[i];
                phys_padded<float> cur_angle;
                phys_padded<float> desired_motor_angle;
                phys_padded<float> max_tau;
                phys_vec4 axis;
                phys_constraint_util_relative_angle(base_a.get_column(twist_axis_table[i]), base_b.get_column(twist_axis_table[i]),
                    base_a.get_column(plane_axis_table[i]), base_b.get_column(plane_axis_table[i]), axis, cur_angle, max_tau);
                phys_constraint_util_relative_angle(target_frame_a_ws.get_column(twist_axis_table[i]), base_b.get_column(twist_axis_table[i]),
                    target_frame_a_ws.get_column(plane_axis_table[i]), base_b.get_column(plane_axis_table[i]), axis, desired_motor_angle, max_tau);
                if (!initialized[i])
                {
                    prev_target_angles[i] = cur_angle;
                    initialized[i] = 1;
                    atom_state_changed = true;
                }
                phys_1d_ang_motor_schema *jacobian_schema = reinterpret_cast<phys_1d_ang_motor_schema *>(out.m_jacobian_schemas.val());
                phys_2ang_jacobian *jacobian_elem = &jacobian_schema->m_contact_jac;
                ang_vel_motor_begin(axis, in, *jacobian_elem, motor_in);
                motor_in.m_last_results = static_cast<const phys_solver_results *>(in.m_constraint_runtime.val())[i];
                motor_in.m_delta_target = desired_motor_angle - prev_target_angles[i];
                motor_in.m_pos_error = prev_target_angles[i] - cur_angle;
                phys_motor_output motor_out;
                phys_compute_motor_data(motor, &motor_in, &motor_out);
                commit_1d_ang_vel_motor(motor_out, in, *jacobian_elem, out);
                if (prev_target_angles[i] != desired_motor_angle)
                {
                    prev_target_angles[i] = desired_motor_angle;
                }
            }
            else
            {
                skip_solver_results(1, out);
            }
        }
        (void)atom_state_changed;
    }
    else
    {
        skip_solver_results(3, out);
    }
}
void build_jacobian_from_ragdoll_motor_atom(const phys_ragdoll_motor_atom &atom, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_ragdoll_motor(atom, in, base_a, base_b, out);
}

namespace {
static __forceinline void phys_pulley_build_2lin_rhs(const phys_pulley_2lin_rhs_info &info, phys_2lin2ang_jacobian &lin_ang_jac)
{
    phys_vec4 dir_b; dir_b.set_mul4(phys_simd_scalar(info.m_leverage_on_b), info.m_dir_b);
    lin_ang_jac.m_lin_parts[0] = info.m_dir_a;
    lin_ang_jac.m_lin_parts[1] = dir_b;
    phys_vec4 rel_a; rel_a.set_sub4(info.m_pivot_a, info.m_body_a->get_center_of_mass_in_world());
    phys_vec4 rel_b; rel_b.set_sub4(info.m_pivot_b, info.m_body_b->get_center_of_mass_in_world());
    phys_vec4 ang_jac_h0; ang_jac_h0.set_cross(rel_a, info.m_dir_a);
    if (info.m_body_a->m_mat_is_identity) {
        lin_ang_jac.m_ang_jacobian[0] = ang_jac_h0;
    } else {
        lin_ang_jac.m_ang_jacobian[0].set_rotated_dir(info.m_body_a->get_core_from_world(), ang_jac_h0);
    }
    phys_vec4 h1; h1.set_cross(dir_b, rel_b);
    if (info.m_body_b->m_mat_is_identity) {
        lin_ang_jac.m_ang_jacobian[1] = h1;
    } else {
        lin_ang_jac.m_ang_jacobian[1].set_rotated_dir(info.m_body_b->get_core_from_world(), h1);
    }
    float diag_term = lin_ang_jac.get_jacobian_diag(*info.m_body_a, *info.m_body_b, info.m_leverage_on_b);
    lin_ang_jac.set_inv_jac_diag(info.m_virt_mass_factor / diag_term);
    phys_vec4 pulley_len_a; pulley_len_a.set_sub4(info.m_pulley_pivot_a, info.m_pivot_a);
    phys_vec4 pulley_len_b; pulley_len_b.set_sub4(info.m_pivot_b, info.m_pulley_pivot_b);
    float length = pulley_len_a.length_3d();
    length += float(pulley_len_b.length_3d()) * info.m_leverage_on_b;
    length -= info.m_rope_length;
    float rhs = length * info.m_query_tau;
    lin_ang_jac.set_rhs(rhs);
}
}

static __forceinline void phys_pulley_build_jacobian_impl(const phys_pulley_info &pulley_info, const phys_constraint_query_in &in,
    phys_constraint_query_out &out)
{
    const phys_vel_accum *agent_body_a = in.m_body_a;
    const phys_vel_accum *agent_body_b = in.m_body_b;
    phys_2lin2ang_jacobian *lin_ang_jac = static_cast<phys_2lin2ang_jacobian *>(static_cast<void *>(out.m_jacobian_schemas.val()));
    phys_pulley_2lin_rhs_info info;
    info.m_dir_a.set_sub4(pulley_info.m_pulley_pivot_a, pulley_info.m_pos_a);
    info.m_dir_b.set_sub4(pulley_info.m_pos_b, pulley_info.m_pulley_pivot_b);
    if (float(info.m_dir_a.length_sq_3d()) > 1.1920929e-07f) {
        info.m_dir_a.normalize_xyz();
    } else {
        info.m_dir_a = phys_transform::get_identity().get_column(0);
    }
    if (float(info.m_dir_b.length_sq_3d()) > 1.1920929e-07f) {
        info.m_dir_b.normalize_xyz();
    } else {
        info.m_dir_b.set_neg4(phys_transform::get_identity().get_column(0));
    }
    info.m_body_a = agent_body_a;
    info.m_body_b = agent_body_b;
    info.m_pivot_a = pulley_info.m_pos_a;
    info.m_pivot_b = pulley_info.m_pos_b;
    info.m_pulley_pivot_a = pulley_info.m_pulley_pivot_a;
    info.m_pulley_pivot_b = pulley_info.m_pulley_pivot_b;
    info.m_rope_length = pulley_info.m_rope_length;
    info.m_leverage_on_b = pulley_info.m_leverage_on_b;
    info.m_query_tau = in.m_substep_inv_dt;
    info.m_virt_mass_factor = in.m_virt_mass_factor;
    phys_pulley_build_2lin_rhs(info, *lin_ang_jac);
    phys_1d_pulley_schema *jacobian_schema = static_cast<phys_1d_pulley_schema *>(static_cast<void *>(out.m_jacobian_schemas.val()));
    out.m_jacobian_schemas = jacobian_schema + 1;
    flush_jacobian_builder();
    jacobian_schema->init_1d_pulley(*lin_ang_jac);
}
void phys_pulley_build_jacobian(const phys_pulley_info &pulley_info, const phys_constraint_query_in &in, phys_constraint_query_out &out)
{
    phys_pulley_build_jacobian_impl(pulley_info, in, out);
}

static __forceinline void build_from_pulley_atom(const phys_pulley_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    phys_pulley_info info;
    info.m_pos_a = base_a.get_translation();
    info.m_pos_b = base_b.get_translation();
    info.m_pulley_pivot_a = atom.m_fixed_pivot_a_world;
    info.m_pulley_pivot_b = atom.m_fixed_pivot_b_world;
    info.m_rope_length = atom.m_rope_length;
    info.m_leverage_on_b = atom.m_leverage_on_b;
    phys_pulley_build_jacobian_impl(info, in, out);
}
void build_jacobian_from_pulley_atom(const phys_pulley_atom &atom, const phys_constraint_query_in &in, const phys_transform &base_a,
    const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_pulley_atom(atom, in, base_a, base_b, out);
}

static __forceinline void build_from_soft_contact(const phys_soft_contact_modifier_atom &atom,
    const phys_constraint_query_in &in, const phys_transform &, const phys_transform &, phys_constraint_query_out &out)
{
    phys_undo_begin_constraints(out);
    phys_constraint_query_in modified_query_in;
    float factor = atom.m_query_tau;
    modified_query_in = in;
    modified_query_in.m_virt_mass_factor = in.m_virt_mass_factor * factor;
    modified_query_in.m_query_tau = in.m_query_tau * factor;
    modified_query_in.m_solver_damping = in.m_solver_damping * factor;
    phys_atom *child = phys_get_terminal_atom(atom);
    phys_simple_contact_atom *contact = static_cast<phys_simple_contact_atom *>(child);
    int contact_reserve_count = contact->m_contact_point_count;
    phys_contact_point *cp = contact->get_contact_points();
    phys_contact_point_props *props = contact->get_contact_point_props();
    for (int i = 0; i < contact_reserve_count; i++) {
        float distance = cp[i].get_distance();
        float max_impulse = props[i].m_internal_data_a;
        float allowed = atom.m_max_accel * in.m_micro_step_dt * in.m_substep_dt / atom.m_query_tau;
        if (distance + allowed < max_impulse) {
            props[i].m_internal_data_a = distance + allowed;
        }
    }
    phys_solver_build_jacobian_atoms(atom.m_child_shape, atom.m_child_size, modified_query_in, out);
}
void build_jacobian_from_soft_contact(const phys_soft_contact_modifier_atom &atom, const phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_soft_contact(atom, in, base_a, base_b, out);
}

static __forceinline void build_from_moving_surface(const phys_moving_surface_atom &atom,
    phys_constraint_query_in &in, const phys_transform &, const phys_transform &, phys_constraint_query_out &out)
{
    phys_vec4 velocity = atom.get_atom_velocity();
    phys_constraint_query_in modified_query_in = in;
    phys_atom *child = atom.m_child_shape;
    int child_dma_size = atom.m_child_size;
    (void)child_dma_size;
    phys_simple_contact_atom *contact_atom = static_cast<phys_simple_contact_atom *>(child);
    phys_undo_begin_constraints(out);
    phys_contact_point_props *props = contact_atom->get_contact_point_props();
    phys_init_header(in, props, sizeof(phys_contact_point_props), out);
    phys_bool body_a_is_fixed = (in.m_accum_a_index == 0);
    phys_vel_accum vel_accum;
    if (!body_a_is_fixed)
    {
        vel_accum = *in.m_body_a;
        in.m_body_a = &vel_accum;
    }
    else
    {
        vel_accum = *in.m_body_b;
        in.m_body_b = &vel_accum;
        velocity.set_neg3(velocity);
    }
    const phys_vec4 normal = contact_atom->get_contact_points()->get_contact_normal();
    phys_simd_scalar d = normal.dot3(velocity);
    phys_vec4 vel; vel = velocity;
    vel.m_simd_quad = __vnmsubfp(normal.m_simd_quad, d.get_quad(), vel.m_simd_quad);
    vel.set_neg4(vel);
    phys_add_vel_build_jacobian(vel, body_a_is_fixed, out);
    vel_accum.m_lin_vel.add4(vel);
    phys_bool fixed_a = false;
    phys_simple_contact_build_jacobian(contact_atom, modified_query_in, fixed_a, out);
    vel.set_neg4(vel);
    phys_add_vel_build_jacobian(vel, body_a_is_fixed, out);
}
void build_jacobian_from_moving_surface(const phys_moving_surface_atom &atom, phys_constraint_query_in &in,
    const phys_transform &base_a, const phys_transform &base_b, phys_constraint_query_out &out)
{
    build_from_moving_surface(atom, in, base_a, base_b, out);
}

static __forceinline void phys_end_constraints() {}

extern "C" void phys_solver_build_jacobian_non_contact(
    const phys_atom *constraint_atoms, int all_atoms_size, const phys_constraint_query_in &orig_query_in, phys_constraint_query_out &out)
{
    const phys_atom *cur_atom = constraint_atoms;
    phys_constraint_query_in in = orig_query_in;
    const phys_atom *atoms_end = phys_add_byte_offset_const<const phys_atom>(cur_atom, all_atoms_size);
    phys_transform base_a(*in.m_transform_a.val());
    phys_transform base_b(*in.m_transform_b.val());
    phys_1d_ang_limit_info ang_limit_info_storage;
    phys_1d_ang_limit_info &ang_limit_info = ang_limit_info_storage;
    phys_begin_constraints_impl(in, out, static_cast<phys_solver_results *>(in.m_constraint_runtime.val()), sizeof(phys_solver_results));
    while (cur_atom < atoms_end)
    {
next_atom:
        switch (cur_atom->m_type)
        {
        case phys_atom::ATOM_INVALID:
            cur_atom = reinterpret_cast<const phys_atom *>((reinterpret_cast<size_t>(cur_atom) + 15) & ~static_cast<size_t>(15));
            goto next_atom;
        case phys_atom::ATOM_BRIDGE: {
            const phys_bridge_atom *atom = static_cast<const phys_bridge_atom *>(cur_atom);
            invoke_atom_bridge(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_SET_LOCAL_TRANSFORMS: {
            const phys_set_local_transforms_atom *atom = static_cast<const phys_set_local_transforms_atom *>(cur_atom);
            apply_local_transforms(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_SET_LOCAL_TRANSLATIONS: {
            const phys_set_local_translations_atom *atom = static_cast<const phys_set_local_translations_atom *>(cur_atom);
            apply_local_translations(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_SET_LOCAL_ROTATIONS: {
            const phys_set_local_rotations_atom *atom = static_cast<const phys_set_local_rotations_atom *>(cur_atom);
            apply_local_rotations(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_BALL_SOCKET: {
            const phys_ball_socket_atom *atom = static_cast<const phys_ball_socket_atom *>(cur_atom);
            build_jacobian_from_ball_socket_atom(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_STIFF_SPRING: {
            const phys_stiff_spring_atom *atom = static_cast<const phys_stiff_spring_atom *>(cur_atom);
            build_from_stiff_spring_atom(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_LIN: {
            const phys_lin_atom *atom = static_cast<const phys_lin_atom *>(cur_atom);
            build_from_lin_atom(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_LIN_SOFT: {
            const phys_lin_soft_atom *atom = static_cast<const phys_lin_soft_atom *>(cur_atom);
            build_from_lin_soft_atom(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_LIN_LIMIT: {
            const phys_lin_limit_atom *atom = static_cast<const phys_lin_limit_atom *>(cur_atom);
            build_from_lin_limit_atom(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_LIN_FRICTION: {
            const phys_lin_friction_atom *atom = static_cast<const phys_lin_friction_atom *>(cur_atom);
            build_from_lin_friction_atom(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_LIN_MOTOR: {
            const phys_lin_motor_atom *atom = static_cast<const phys_lin_motor_atom *>(cur_atom);
            build_from_lin_motor_atom(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_2D_ANG: {
            const phys_2d_ang_atom *atom = static_cast<const phys_2d_ang_atom *>(cur_atom);
            build_from_2d_ang_atom(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_ANG: {
            const phys_ang_atom *atom = static_cast<const phys_ang_atom *>(cur_atom);
            build_from_ang_atom(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_ANG_LIMIT: {
            const phys_ang_limit_atom *atom = static_cast<const phys_ang_limit_atom *>(cur_atom);
            phys_result result = build_from_ang_limit_atom(*atom, in, base_a, base_b, ang_limit_info, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            if (result == PHYS_SUCCESS) { goto build_ang_limit; } else { goto skip_ang_limit; }
            }
        case phys_atom::ATOM_CONE_LIMIT: {
            const phys_cone_limit_atom *atom = static_cast<const phys_cone_limit_atom *>(cur_atom);
            phys_result result = build_jacobian_from_cone_limit(*atom, in, base_a, base_b, ang_limit_info, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            if (result == PHYS_SUCCESS) { goto build_ang_limit; } else { goto skip_ang_limit; }
            }
        case phys_atom::ATOM_TWIST_LIMIT: {
            const phys_twist_limit_atom *atom = static_cast<const phys_twist_limit_atom *>(cur_atom);
            phys_result result = build_from_twist_limit_atom(*atom, in, base_a, base_b, ang_limit_info, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            if (result == PHYS_SUCCESS) { goto build_ang_limit; } else { goto skip_ang_limit; }
            }
        case phys_atom::ATOM_ANG_FRICTION: {
            const phys_ang_friction_atom *atom = static_cast<const phys_ang_friction_atom *>(cur_atom);
            build_from_ang_friction_atom(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_ANG_MOTOR: {
            const phys_ang_motor_atom *atom = static_cast<const phys_ang_motor_atom *>(cur_atom);
            build_from_ang_motor_atom(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_RAGDOLL_MOTOR: {
            const phys_ragdoll_motor_atom *atom = static_cast<const phys_ragdoll_motor_atom *>(cur_atom);
            build_from_ragdoll_motor(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_PULLEY: {
            const phys_pulley_atom *atom = static_cast<const phys_pulley_atom *>(cur_atom);
            build_from_pulley_atom(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_MOD_SOFT_CONTACT: {
            const phys_soft_contact_modifier_atom *atom = static_cast<const phys_soft_contact_modifier_atom *>(cur_atom);
            const phys_atom *next_atom_ptr = atom + 1;
            build_from_soft_contact(*atom, in, base_a, base_b, out);
            cur_atom = next_atom_ptr;
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                void *runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                void *main_runtime_ptr = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
                in.m_constraint_runtime = runtime;
                in.m_runtime_in_main_memory = main_runtime_ptr;
            }
            break; }
        case phys_atom::ATOM_MOD_VISCOUS_SURFACE: {
            const phys_viscous_surface_atom *atom = static_cast<const phys_viscous_surface_atom *>(cur_atom);
            build_from_viscous_surface(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_MOD_MASS_CHANGER: {
            const phys_mass_changer_atom *atom = static_cast<const phys_mass_changer_atom *>(cur_atom);
            build_from_mass_changer(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        case phys_atom::ATOM_MOD_MOVING_SURFACE: {
            const phys_moving_surface_atom *atom = static_cast<const phys_moving_surface_atom *>(cur_atom);
            build_from_moving_surface(*atom, in, base_a, base_b, out);
            cur_atom = atom->next();
            {
                int atom_result_bytes = atom->solver_result_count() * sizeof(phys_solver_results);
                in.m_constraint_runtime = phys_add_byte_offset(in.m_constraint_runtime.val(), atom_result_bytes);
                in.m_runtime_in_main_memory = phys_add_byte_offset(in.m_runtime_in_main_memory.val(), atom_result_bytes);
            }
            break; }
        default:
            break;
build_ang_limit:
            build_1d_ang_limit(ang_limit_info, in, out);
            break;
skip_ang_limit:
            skip_solver_results(1, out);
        }
    }
    phys_end_constraints();
}
