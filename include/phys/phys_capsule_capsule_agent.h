#ifndef INCLUDE_PHYS_CAPSULE_CAPSULE_AGENT_H
#define INCLUDE_PHYS_CAPSULE_CAPSULE_AGENT_H

#include "phys/phys_iterative_linear_cast_agent.h"

class phys_dispatcher;

class phys_capsule_capsule_agent : public phys_iterative_linear_cast_agent {
public:
    static void register_agent(phys_dispatcher *dispatcher);
    virtual inline void process_collision(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_process_collision_input &input, phys_process_collision_output &result);
    virtual void get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_hit_collector &collector);
    static void static_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_hit_collector &collector);
    virtual void get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_proxy_pair_collector &collector);
    static void static_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_proxy_pair_collector &collector);
    virtual void cleanup(phys_constraint_owner &owner);

protected:
    __forceinline phys_capsule_capsule_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &,
        phys_contact_manager *pair_contact_manager)
        : phys_iterative_linear_cast_agent(pair_contact_manager) {
        m_process_contact_id[0] = 0xffff;
        m_process_contact_id[1] = 0xffff;
        m_process_contact_id[2] = 0xffff;
    }
    static phys_agent *create_capsule_capsule_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_contact_manager *pair_contact_manager);
    unsigned short m_process_contact_id[3];
};

typedef char havok_size_hkCapsuleCapsuleAgent[(sizeof(void *) != 4 || ((sizeof(phys_capsule_capsule_agent) == 20) )) ? 1 : -1];

#endif
