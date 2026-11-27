#ifndef INCLUDE_PHYS_BATCH_AGENT_VISITOR_INPUT_H
#define INCLUDE_PHYS_BATCH_AGENT_VISITOR_INPUT_H

#include "phys/phys_thread_memory.h"

class phys_collision_proxy;
class phys_shape_container;
struct phys_collision_input;
class phys_contact_manager;
class phys_constraint_owner;

// Input bundle shared by the agent machine's visitors.
struct phys_batch_agent_visitor_input {
    PHYS_DECLARE_NONVIRTUAL_ALLOCATOR(PHYS_MEM_COLLISION_INFO, phys_batch_agent_visitor_input);
    const phys_collision_proxy *m_body_a;
    const phys_collision_proxy *m_compound_proxy_b;
    const phys_shape_container *m_container_shape_b;
    const phys_collision_input *m_input;
    phys_contact_manager *m_contact_manager;
    phys_constraint_owner *m_constraint_owner;
    void *m_visitor_client_data;
};

typedef char phys_batch_agent_visit_input_size[(sizeof(void *) != 4 || ((sizeof(phys_batch_agent_visitor_input) == 28) )) ? 1 : -1];

#endif
