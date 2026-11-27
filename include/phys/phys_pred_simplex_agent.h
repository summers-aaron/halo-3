#ifndef INCLUDE_PHYS_PRED_SIMPLEX_AGENT_H
#define INCLUDE_PHYS_PRED_SIMPLEX_AGENT_H

#include "phys/phys_simplex_agent.h"

class phys_pred_simplex_agent : public phys_simplex_agent {
public:
    __forceinline phys_pred_simplex_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_contact_manager *pair_contact_manager)
        : phys_simplex_agent(agent_body_a, agent_body_b, pair_contact_manager) {}
    virtual ~phys_pred_simplex_agent() {}
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    static phys_agent *create_pred_simplex_agent(const phys_collision_proxy &, const phys_collision_proxy &,
        const phys_collision_input &, phys_contact_manager *);
    static void register_agent(phys_dispatcher *);
};

#endif
