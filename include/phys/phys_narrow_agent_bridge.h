#ifndef INCLUDE_PHYS_NARROW_AGENT_BRIDGE_H
#define INCLUDE_PHYS_NARROW_AGENT_BRIDGE_H

struct phys_narrow_agent_input;
struct phys_agent_entry;
struct phys_narrow_agent_process_input;
struct phys_process_collision_output;
struct phys_collision_input;
class phys_dispatcher;
class phys_collision_proxy;
class phys_contact_manager;
class phys_constraint_owner;
class phys_vec4;

namespace phys_narrow_agent_bridge {
    void *create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *agent_data);
    void destroy(phys_agent_entry *, void *, phys_contact_manager *, phys_constraint_owner &, phys_dispatcher *);
    void update_agent_filter(phys_agent_entry *, void *, phys_collision_proxy &, phys_collision_proxy &, const phys_collision_input &,
        phys_contact_manager *, phys_constraint_owner &);
    void invalidate_sweep_info(phys_agent_entry *, void *, phys_collision_input &);
    void warp_time(phys_agent_entry *, void *, float, float, phys_collision_input &);
    void bridge_remove_point(phys_agent_entry *, void *, unsigned short);
    void commit_potential(phys_agent_entry *, void *, unsigned short);
    void mark_zombie_entry(phys_agent_entry *, void *, unsigned short);
    void *process(const phys_narrow_agent_process_input &, phys_agent_entry *, void *, phys_vec4 *, phys_process_collision_output &);
    int register_narrow_agent(phys_dispatcher *);
}

#endif
