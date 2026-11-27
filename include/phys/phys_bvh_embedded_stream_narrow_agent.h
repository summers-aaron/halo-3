#ifndef INCLUDE_PHYS_BVH_EMBEDDED_STREAM_NARROW_AGENT_H
#define INCLUDE_PHYS_BVH_EMBEDDED_STREAM_NARROW_AGENT_H

#include "phys/phys_dispatcher.h"
#include "phys/phys_narrow_agent.h"

namespace phys_bvh_embedded_stream_narrow_agent {
    void destroy(phys_agent_entry *entry, void *data, phys_contact_manager *agent_contact_manager, phys_constraint_owner &owner, phys_dispatcher *dispatcher);
    void *create(const phys_narrow_agent_input &input, phys_agent_entry *entry, void *data);
    void *process(const phys_narrow_agent_process_input &input, phys_agent_entry *entry, void *data, phys_vec4 *normal, phys_process_collision_output &output);
    void init_agent_funcs(phys_dispatcher::narrow_agent_funcs &functions);
    void register_narrow_agent(phys_dispatcher *dispatcher);
}

#endif
