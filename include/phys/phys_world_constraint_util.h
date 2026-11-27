#ifndef INCLUDE_PHYS_WORLD_CONSTRAINT_UTIL_H
#define INCLUDE_PHYS_WORLD_CONSTRAINT_UTIL_H

#include "phys/phys_atoms.h"
#include "phys/phys_constraint.h"

class phys_world;
class phys_entity;
class phys_constraint_owner;

class phys_world_constraint_util {
public:
    static void add_world_constraint(phys_world *world, phys_constraint *constraint);
    static void remove_constraint(phys_constraint *constraint);
    static phys_constraint *get_entity_constraint(const phys_entity *listener_entity_a, const phys_entity *listener_entity_b);
    static void add_modifier(phys_constraint *instance, phys_constraint_owner &owner, phys_modifier_atom *modifier);
    static void remove_constraint_modifier(phys_constraint *instance, phys_constraint_owner &owner, phys_atom::atom_kind type);
    static phys_modifier_atom *find_modifier(phys_constraint *instance, phys_atom::atom_kind type);
    static phys_modifier_atom *find_last_modifier(phys_constraint *instance);
    static void update_moved_atom_parent(phys_constraint *instance, const phys_atom *old_atom,
        const phys_atom *updated_atom, int updated_atom_size);
    static __forceinline phys_atom *get_terminal_atom(const phys_constraint_internal *internal) {
        phys_atom *atom = internal->m_data_atoms;
        while (1) {
            if (!atom->is_modifier_type()) {
                break;
            }
            phys_modifier_atom *modifier = reinterpret_cast<phys_modifier_atom *>(atom);
            atom = modifier->m_child_shape;
        }
        return atom;
    }
};

#endif
