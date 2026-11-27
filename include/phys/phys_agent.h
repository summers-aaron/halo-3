#ifndef INCLUDE_PHYS_AGENT_H
#define INCLUDE_PHYS_AGENT_H

// hkCollisionAgent: the empty default handlers are defined in tu/hkcollide/hkCollisionAgent.cpp. Times are floats; a
// contact point id is 16 bits.

#include "phys/phys_base.h"

class phys_collision_proxy;
struct phys_collision_input;
struct phys_linear_cast_collision_input;
struct phys_process_collision_input;
class phys_process_collision_output;
class phys_hit_collector;
class phys_proxy_pair_collector;
class phys_contact_manager;
class phys_constraint_owner;

class phys_agent : public phys_ref_object
{
public:
    PHYS_DECLARE_ALLOCATOR(PHYS_MEM_AGENT);

    virtual void get_penetrations(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_proxy_pair_collector &collector) = 0;
    virtual void get_closest_points(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_hit_collector &collector) = 0;
    virtual void sweep_cast(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b, const phys_linear_cast_collision_input &input,
        phys_hit_collector &cast_hit_collector, phys_hit_collector *start_collector) = 0;
    virtual void process_collision(const phys_collision_proxy &a, const phys_collision_proxy &b,
        const phys_process_collision_input &input, phys_process_collision_output &result) = 0;
    virtual void cleanup(phys_constraint_owner &constraint_owner_ref) = 0;
    virtual void update_compound_filter(const phys_collision_proxy &agent_body_a, const phys_collision_proxy &agent_body_b,
        const phys_collision_input &input, phys_constraint_owner &constraint_owner_ref) {}
    virtual void invalidate_sweep_info(phys_collision_input &input);
    virtual void warp_time(float agent_old_time, float agent_new_time, phys_collision_input &input);
    virtual void bridge_remove_point(unsigned short bounding_contact_point_id);
    virtual void commit_potential(unsigned short bounding_contact_point_id);
    virtual void mark_zombie_entry(unsigned short bounding_contact_point_id);

protected:
    inline virtual ~phys_agent() {}
    inline phys_agent(phys_contact_manager *agent_contact_manager) : m_contact_manager(agent_contact_manager) {}

    phys_contact_manager *m_contact_manager;

public:
    inline phys_contact_manager *get_contact_manager() { return m_contact_manager; }
};

#endif // HAVOK_HK_COLLISION_AGENT_H
