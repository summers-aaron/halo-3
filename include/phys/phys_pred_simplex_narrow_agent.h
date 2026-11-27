#ifndef INCLUDE_PHYS_PRED_SIMPLEX_NARROW_AGENT_H
#define INCLUDE_PHYS_PRED_SIMPLEX_NARROW_AGENT_H

class phys_dispatcher;
class phys_constraint_owner;
class phys_contact_manager;
class phys_vec4;
struct phys_narrow_agent_input;
struct phys_agent_entry;
#include "phys/phys_shape_type.h"

// Functions defined in tu/hkinternal/hkPredGskAgent3.cpp that other agents reuse.
namespace phys_pred_simplex_narrow_agent {
    void *create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *data);
    void *cleanup(phys_agent_entry *entry, void *data, phys_contact_manager *agent_contact_manager, phys_constraint_owner &owner);
    void bridge_remove_point(phys_agent_entry *entry, void *data, unsigned short id);
    void commit_potential(phys_agent_entry *entry, void *data, unsigned short id);
    void mark_zombie_entry(phys_agent_entry *entry, void *data, unsigned short id);
    void destroy(phys_agent_entry *entry, void *data, phys_contact_manager *agent_contact_manager, phys_constraint_owner &owner, phys_dispatcher *dispatcher);
    void compute_sep_normal(const phys_narrow_agent_input &input, void *data, phys_vec4 &normal);
    void register_narrow_agent(phys_dispatcher *dispatcher, phys_shape_type shape_type_a, phys_shape_type shape_type_b);
}

#endif
