#ifndef INCLUDE_PHYS_SOLVER_EXPORT_H
#define INCLUDE_PHYS_SOLVER_EXPORT_H

// Functions defined in tu/hkconstraintsolver/hkSolverExport.cpp that other units call.

class phys_schema;
struct phys_solver_elem_temp;
struct phys_solver_info;
class phys_vel_accum;

extern "C" void phys_export_impulses_and_rhs(const phys_solver_info &i, const phys_solver_elem_temp *temp, const phys_schema *schema_list,
	const phys_vel_accum *vel_accums);

#endif // HAVOK_HK_SOLVER_EXPORT_H
