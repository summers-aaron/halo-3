#ifndef INCLUDE_PHYS_POWERED_CHAIN_JACOBIAN_H
#define INCLUDE_PHYS_POWERED_CHAIN_JACOBIAN_H
class phys_constraint_query_in;
class phys_constraint_query_out;
class phys_vec4;
class phys_2ang_jacobian;
class phys_motor_input;
extern "C" {
void phys_begin_constraints(const phys_constraint_query_in &in, phys_constraint_query_out &out, void *solver_results, int solver_result_stride);
void phys_1d_ang_vel_motor_begin_jacobian(const phys_vec4 &dof_axis, const phys_constraint_query_in &in,
    phys_2ang_jacobian *lin_ang_jac, phys_motor_input &motor_in_out);
}
#endif
