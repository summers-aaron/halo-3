#ifndef INCLUDE_PHYS_SOLVER_FUNCTIONS_H
#define INCLUDE_PHYS_SOLVER_FUNCTIONS_H

class phys_bool;
struct phys_solver_info;
class phys_vel_accum;
class phys_schema;
struct phys_solver_elem_temp;

extern "C" int phys_solve_apply_gravity_steps(const phys_solver_info &, phys_vel_accum *, phys_vel_accum *);
extern "C" int phys_solve_constraints(const phys_solver_info &, const phys_schema *, phys_vel_accum *, phys_solver_elem_temp *);
extern "C" int phys_solve_integrate_vel_steps(const phys_solver_info &, int, phys_vel_accum *, phys_vel_accum *, phys_bool &);

#endif
