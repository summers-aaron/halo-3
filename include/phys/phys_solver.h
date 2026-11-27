#ifndef INCLUDE_PHYS_SOLVER_H
#define INCLUDE_PHYS_SOLVER_H

// Functions defined in tu/hkconstraintsolver/hkSolver.cpp.

extern "C" void phys_solve_upload(void);

struct phys_solver_info;
class phys_vec4;
class phys_vel_accum;
class phys_schema;
struct phys_solver_elem_temp;

extern "C" int phys_solve_constraints_steps(const phys_solver_info &, int, int, const phys_schema *, phys_vel_accum *, phys_solver_elem_temp *);
extern "C" void phys_solve_step_jacobians(const phys_solver_info &, const phys_schema *, phys_vel_accum *, phys_solver_elem_temp *);

void update_sum_velocities(const phys_solver_info& info, const phys_vec4& vel_field, bool is_last_step, const phys_vec4& factor, phys_vel_accum* vel_accum);
extern "C" void phys_solver_ball_socket_chain(const phys_solver_info& info, const void* step_temp,
    phys_vel_accum* accum_array, phys_schema* schema_in, phys_solver_elem_temp* temp);

extern "C" int phys_solve_toi_violating_vel(void *solver_info, const phys_schema *schema_list,
    const phys_vel_accum *accum_array, int max_num_velocities, float *velocities_out);

#endif // HAVOK_HK_SOLVER_H
