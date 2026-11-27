#ifndef INCLUDE_PHYS_ITERATIVE_LINEAR_CAST_AGENT_H
#define INCLUDE_PHYS_ITERATIVE_LINEAR_CAST_AGENT_H

#include "phys/phys_agent.h"

class phys_iterative_linear_cast_agent : public phys_agent {
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_AGENT);
    __forceinline phys_iterative_linear_cast_agent(phys_contact_manager *pair_contact_manager) : phys_agent(pair_contact_manager) {}
    __forceinline ~phys_iterative_linear_cast_agent() {}
    virtual void sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
        phys_hit_collector &collector, phys_hit_collector *start_collector);
    static void static_sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_linear_cast_collision_input &input,
        phys_hit_collector &collector, phys_hit_collector *start_collector);
};

#endif
