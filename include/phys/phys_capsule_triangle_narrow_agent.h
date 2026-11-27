#ifndef INCLUDE_PHYS_CAPSULE_TRIANGLE_NARROW_AGENT_H
#define INCLUDE_PHYS_CAPSULE_TRIANGLE_NARROW_AGENT_H

struct phys_agent_entry;
struct phys_narrow_agent_input;
struct phys_narrow_agent_process_input;
struct phys_process_collision_output;
class phys_constraint_owner;
class phys_contact_manager;
class phys_dispatcher;
class phys_vec4;

namespace phys_capsule_triangle_narrow_agent {
    void bridge_remove_point(phys_agent_entry *entry, void *data, unsigned short id);
    void commit_potential(phys_agent_entry *entry, void *data, unsigned short id);
    void mark_zombie_entry(phys_agent_entry *entry, void *data, unsigned short id);
    void *create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *data);
    void *cleanup(phys_agent_entry *entry, void *data, phys_contact_manager *agent_contact_manager, phys_constraint_owner &owner);
    void destroy(phys_agent_entry *entry, void *data, phys_contact_manager *agent_contact_manager, phys_constraint_owner &owner, phys_dispatcher *dispatcher);
    void compute_sep_normal(const phys_narrow_agent_input &input, void *data, phys_vec4 &normal);
    void *process(const phys_narrow_agent_process_input &input, phys_agent_entry *entry, void *data, phys_vec4 *normal, phys_process_collision_output &output);
    void register_narrow_agent(phys_dispatcher *dispatcher);
}

#endif
