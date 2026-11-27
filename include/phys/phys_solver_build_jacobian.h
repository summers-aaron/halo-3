#ifndef INCLUDE_PHYS_SOLVER_BUILD_JACOBIAN_H
#define INCLUDE_PHYS_SOLVER_BUILD_JACOBIAN_H

#include "phys/phys_atoms.h"
#include "phys/phys_build_jacobian_from_atoms.h"
#include "phys/phys_constraint_query_in.h"
#include "phys/phys_constraint_query_out.h"

// March 0x8350cad0 dispatches contact atoms to the contact-specific builder.
static __forceinline void phys_solver_build_jacobian_atoms(
    const phys_atom *constraint_atoms, int all_atoms_size, const phys_constraint_query_in &in, phys_constraint_query_out &out)
{
    if (constraint_atoms->m_type == phys_atom::ATOM_CONTACT) {
        phys_simple_contact_atom *contact_atom = static_cast<phys_simple_contact_atom *>(const_cast<phys_atom *>(constraint_atoms));
        const phys_bool write_header_schema = true;
        phys_simple_contact_build_jacobian(contact_atom, in, write_header_schema, out);
    } else {
        phys_solver_build_jacobian_non_contact(constraint_atoms, all_atoms_size, in, out);
    }
}

#endif
