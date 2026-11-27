#ifndef INCLUDE_PHYS_ATOM_H
#define INCLUDE_PHYS_ATOM_H

// Functions defined in tu/hkdynamics/hkConstraintAtom.cpp.

class phys_constraint_data;
class phys_constraint_query_in;
class phys_constraint_query_out;
struct phys_body_vel;

extern "C" {
extern phys_body_vel g_phys_soft_contact_body_vels[2];
}

void phys_bridge_atom_call_data(phys_constraint_data *data, const phys_constraint_query_in &in, phys_constraint_query_out &out);

#endif // HAVOK_HK_CONSTRAINT_ATOM_H
