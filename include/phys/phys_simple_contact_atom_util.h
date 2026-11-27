#ifndef INCLUDE_PHYS_SIMPLE_CONTACT_ATOM_UTIL_H
#define INCLUDE_PHYS_SIMPLE_CONTACT_ATOM_UTIL_H

struct phys_simple_contact_atom;

class phys_simple_contact_atom_util {
public:
    static phys_simple_contact_atom *allocate_contact_atom(int atom_contact_count);
    static void free_contact_atom(phys_simple_contact_atom *atom);
    static void copy_atom_contents(phys_simple_contact_atom *destination,
                             const phys_simple_contact_atom *source);
    static phys_simple_contact_atom *expand_one(phys_simple_contact_atom *atom);
    static void remove_at_and_copy(phys_simple_contact_atom *atom, int index);
    static phys_simple_contact_atom *optimize_capacity(phys_simple_contact_atom *atom, int free_contact_point_count);
};

#endif
