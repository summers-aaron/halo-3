#ifndef INCLUDE_PHYS_BUILD_JACOBIAN_FROM_ATOMS_H
#define INCLUDE_PHYS_BUILD_JACOBIAN_FROM_ATOMS_H

// Functions defined in tu/hkconstraintsolver/hkBuildJacobianFromAtoms.cpp.

#include "phys/phys_scalar_types.h"

class phys_1d_ang_limit_info;
class phys_1d_bilateral_status;
class phys_1d_motor_info;
class phys_1lin2ang_jacobian;
class phys_2ang_jacobian;
struct phys_2d_ang_atom;
struct phys_ang_atom;
struct phys_ang_friction_atom;
struct phys_ang_limit_atom;
struct phys_ang_motor_atom;
struct phys_ball_socket_atom;
struct phys_bridge_atom;
struct phys_cone_limit_atom;
struct phys_atom;
class phys_motor_input;
class phys_constraint_query_in;
class phys_constraint_query_out;
struct phys_lin_atom;
struct phys_lin_friction_atom;
struct phys_lin_limit_atom;
struct phys_lin_motor_atom;
struct phys_lin_soft_atom;
struct phys_mass_changer_atom;
struct phys_moving_surface_atom;
struct phys_pulley_atom;
struct phys_ragdoll_motor_atom;
struct phys_set_local_rotations_atom;
struct phys_set_local_transforms_atom;
struct phys_set_local_translations_atom;
struct phys_soft_contact_modifier_atom;
class phys_solver_results;
struct phys_stiff_spring_atom;
class phys_transform;
struct phys_twist_limit_atom;
class phys_vec4;
struct phys_viscous_surface_atom;

void phys_undo_begin_constraints(phys_constraint_query_out &out);
void phys_skip_solver_results(unsigned char count, phys_constraint_query_out &out);
void call_bridge_from_atom(const phys_bridge_atom &atom, const phys_constraint_query_in &in, phys_transform &base_a,
	phys_transform &base_b, phys_constraint_query_out &out);
void set_local_transform(const phys_transform &b, const phys_transform &a, phys_transform &out);
void set_local_transforms_from_atom(
	const phys_set_local_transforms_atom &atom,
	const phys_constraint_query_in &in,
	phys_transform &base_a,
	phys_transform &base_b,
	const phys_constraint_query_out &out);
void set_local_translations_from_atom(
	const phys_set_local_translations_atom &atom,
	const phys_constraint_query_in &in,
	phys_transform &base_a,
	phys_transform &base_b,
	const phys_constraint_query_out &out);
void set_local_rotations_from_atom(
	const phys_set_local_rotations_atom &atom,
	const phys_constraint_query_in &in,
	phys_transform &base_a,
	phys_transform &base_b,
	const phys_constraint_query_out &out);
void build_jacobian_from_stiff_spring_atom(
	const phys_stiff_spring_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_lin_atom(
	const phys_lin_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_lin_soft_atom(
	const phys_lin_soft_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_lin_limit_atom(
	const phys_lin_limit_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_lin_friction_atom(
	const phys_lin_friction_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_lin_motor_atom(
	const phys_lin_motor_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_2d_ang_atom(
	const phys_2d_ang_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
phys_result build_jacobian_from_ang_limit_atom(
	const phys_ang_limit_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_1d_ang_limit_info &info,
	phys_constraint_query_out &out);
phys_result build_jacobian_from_twist_limit_atom(
	const phys_twist_limit_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_1d_ang_limit_info &info,
	phys_constraint_query_out &out);
phys_result build_jacobian_from_cone_limit_atom(
	const phys_cone_limit_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_1d_ang_limit_info &info,
	phys_constraint_query_out &out);
void build_jacobian_from_ang_friction_atom(
	const phys_ang_friction_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_ang_motor_atom(
	const phys_ang_motor_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_ragdoll_motor_atom(
	const phys_ragdoll_motor_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_ball_socket_atom(
	const phys_ball_socket_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_pulley_atom(
	const phys_pulley_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_viscous_surface(
	const phys_viscous_surface_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_mass_changer(
	const phys_mass_changer_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_soft_contact(
	const phys_soft_contact_modifier_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void build_jacobian_from_moving_surface(
	const phys_moving_surface_atom &atom,
	phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void begin_1d_ang_vel_motor_jacobian(
	const phys_vec4 &axis,
	const phys_constraint_query_in &in,
	phys_2ang_jacobian &jacobian_elem,
	phys_1d_bilateral_status &status);
void build_ball_socket_jacobian(const phys_vec4 &joint_pivot_a, const phys_vec4 &joint_pivot_b, const phys_constraint_query_in &in,
	phys_constraint_query_out &out);
void phys_init_header(const phys_constraint_query_in &in, phys_solver_results *sr, int solver_result_stride, phys_constraint_query_out &out);
void begin_constraint_batch(const phys_constraint_query_in &in, phys_constraint_query_out &out, phys_solver_results *sr, int solver_result_stride);

extern "C" {
void phys_solver_build_jacobian_non_contact(const phys_atom *constraint_atoms, int all_atoms_size,
	const phys_constraint_query_in &orig_query_in, phys_constraint_query_out &out);
void build_jacobian_from_ang_atom(
	const phys_ang_atom &atom,
	const phys_constraint_query_in &in,
	const phys_transform &base_a,
	const phys_transform &base_b,
	phys_constraint_query_out &out);
void phys_1d_ang_vel_motor_begin_jacobian(
	const phys_vec4 &axis,
	const phys_constraint_query_in &in,
	phys_2ang_jacobian &jacobian_elem,
	phys_1d_bilateral_status &status);
void phys_1d_ang_vel_motor_commit_jacobian(phys_1d_motor_info &info, const phys_constraint_query_in &in,
	phys_2ang_jacobian &jacobian_elem, phys_constraint_query_out &out);
void phys_1d_lin_vel_motor_begin_jacobian(
	const phys_vec4 &constraint_dir,
	const phys_vec4 &pivot,
	const phys_constraint_query_in &in,
	phys_1lin2ang_jacobian &lin_ang_jac,
	phys_motor_input &status_out);
void phys_1d_lin_vel_motor_commit_jacobian(
	const phys_1d_motor_info &info,
	const phys_constraint_query_in &in,
	phys_1lin2ang_jacobian &jacobian_elem,
	phys_constraint_query_out &out);
void phys_begin_constraints(const phys_constraint_query_in &in, phys_constraint_query_out &out, phys_solver_results *sr, int solver_result_stride);
}

#endif // HAVOK_HK_BUILD_JACOBIAN_FROM_ATOMS_H
