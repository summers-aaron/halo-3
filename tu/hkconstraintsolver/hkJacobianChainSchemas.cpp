// hkconstraintsolver/hkJacobianChainSchemas.obj: from the records 0x66ce00 0x66ce08 0x66ce10 0x66ce18
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x66ce00 phys_stiff_spring_chain_get_jacobians
// @address 0x66ce08 phys_ball_socket_chain_get_jacobians
// @address 0x66ce10 phys_powered_chain_get_lin_jacobians
// @address 0x66ce18 phys_powered_chain_get_ang_jacobians

#include "phys/phys_chain_schemas.h"
#include "phys/phys_chain_schema_types.h"

extern "C" phys_1lin2ang_jacobian *phys_stiff_spring_chain_get_jacobians(phys_schema *jacobian_schema)
{
  return phys_stiff_spring_chain_schema::get_jacobians(jacobian_schema);
}
extern "C" phys_1lin2ang_jacobian *phys_ball_socket_chain_get_jacobians(phys_schema *jacobian_schema)
{
  return phys_ball_socket_chain_schema::get_jacobians(jacobian_schema);
}
extern "C" phys_1lin2ang_jacobian *phys_powered_chain_get_lin_jacobians(phys_schema *jacobian_schema)
{
  return phys_powered_chain_schema::get_lin_jacobians(jacobian_schema);
}
extern "C" phys_2ang_jacobian *phys_powered_chain_get_ang_jacobians(phys_schema *jacobian_schema, int solver_constraint_count)
{
  return phys_powered_chain_schema::get_ang_jacobians(jacobian_schema, solver_constraint_count);
}

