#ifndef INCLUDE_PHYS_LIST_NARROW_AGENT_H
#define INCLUDE_PHYS_LIST_NARROW_AGENT_H

#include "phys/phys_dispatcher.h"

// Functions defined in tu/hkinternal/hkListAgent3.cpp.

extern "C" void phys_list_narrow_agent_invalidate_sweep(phys_agent_entry *entry, void *agent_data, phys_collision_input &input);
extern "C" void phys_list_narrow_agent_warp_time(phys_agent_entry *entry, void *agent_data, float agent_old_time, float agent_new_time,
    phys_collision_input &input);
extern "C" void phys_list_narrow_agent_update_compound_filter(phys_agent_entry *entry, void *agent_data,
    phys_collision_proxy &agent_body_a, phys_collision_proxy &agent_body_b, const phys_collision_input &input,
        phys_contact_manager *pair_contact_manager, phys_constraint_owner &constraint_owner_ref);

class phys_dispatcher;
namespace phys_list_narrow_agent {
    void destroy(phys_agent_entry *entry, void *data, phys_contact_manager *agent_contact_manager, phys_constraint_owner &owner, phys_dispatcher *dispatcher);
    void *create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *data);
    void *process(const phys_narrow_agent_process_input &input, phys_agent_entry *entry, void *data, phys_vec4 *normal, phys_process_collision_output &output);
    void init_agent_funcs(phys_dispatcher::narrow_agent_funcs &dispatcher_fns);
    void register_narrow_agent(phys_dispatcher *dispatcher);
}

#endif // HAVOK_HK_LIST_AGENT3_H
