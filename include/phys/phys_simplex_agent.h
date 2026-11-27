#ifndef INCLUDE_PHYS_SIMPLEX_AGENT_H
#define INCLUDE_PHYS_SIMPLEX_AGENT_H

#include "phys/phys_simplex_agent_base.h"
#include "phys/phys_simplex_manifold.h"

class phys_simplex_agent : public phys_simplex_agent_base {
public:
    virtual void cleanup(phys_constraint_owner &owner);
    virtual void bridge_remove_point(unsigned short bounding_contact_point_id);
    virtual void commit_potential(unsigned short bounding_contact_point_id);
    virtual void mark_zombie_entry(unsigned short bounding_contact_point_id);
    static phys_agent *create_simplex_agent(const phys_collision_proxy &, const phys_collision_proxy &, const phys_collision_input &, phys_contact_manager *);
    static void register_agent(phys_dispatcher *dispatcher);
    virtual void process_collision(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
    phys_simplex_manifold m_box_manifold;
protected:
#if defined(_XBOX) && !defined(__clang__)
    inline phys_simplex_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, phys_contact_manager *pair_contact_manager);
#else
    phys_simplex_agent(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, phys_contact_manager *pair_contact_manager);
#endif
    virtual ~phys_simplex_agent() {}
    void process_collision_no_sweep(const phys_collision_proxy &, const phys_collision_proxy &, const phys_process_collision_input &,
        phys_process_collision_output &);
};

typedef char phys_simplex_agent_size[(sizeof(void *) != 4 || ((sizeof(phys_simplex_agent) == 0x80) )) ? 1 : -1];

#endif
