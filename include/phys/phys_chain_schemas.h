#ifndef INCLUDE_PHYS_CHAIN_SCHEMAS_H
#define INCLUDE_PHYS_CHAIN_SCHEMAS_H

// Functions defined in tu/hkconstraintsolver/hkJacobianChainSchemas.cpp that other units call.

class phys_1lin2ang_jacobian;
class phys_2ang_jacobian;
class phys_schema;

extern "C" phys_1lin2ang_jacobian *phys_stiff_spring_chain_get_jacobians(phys_schema *jacobian_schema);
extern "C" phys_1lin2ang_jacobian *phys_ball_socket_chain_get_jacobians(phys_schema *jacobian_schema);
extern "C" phys_1lin2ang_jacobian *phys_powered_chain_get_lin_jacobians(phys_schema *jacobian_schema);
extern "C" phys_2ang_jacobian *phys_powered_chain_get_ang_jacobians(phys_schema *jacobian_schema, int solver_constraint_count);

#endif // HAVOK_HK_JACOBIAN_CHAIN_SCHEMAS_H
