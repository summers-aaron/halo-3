#ifndef INCLUDE_PHYS_MANIFOLD_POINT_ADDED_EVENT_H
#define INCLUDE_PHYS_MANIFOLD_POINT_ADDED_EVENT_H

#include "phys/phys_collision_listener.h"

// The contact point added event a contact manager fires for a new manifold point (hkSimpleConstraintContactMgr and
// hkReportContactMgr addContactPointImpl): the listeners may set the delay before the next process callback.
struct phys_manifold_point_added_event : public phys_contact_added_event
{
    unsigned short m_process_contact_id;
    unsigned short m_next_callback_delay;

    __forceinline phys_manifold_point_added_event(unsigned short id, phys_dynamics_contact_manager *manager,
        const phys_process_collision_input *input, phys_process_collision_output *output, const phys_collision_proxy *agent_body_a,
        const phys_collision_proxy *agent_body_b, phys_contact_point *point, const phys_simplex_cache *cache,
            phys_contact_point_material *material, float velocity)
        : phys_contact_added_event(manager, input, output, agent_body_a, agent_body_b, point, cache, material, velocity, TYPE_MANIFOLD),
          m_process_contact_id(id), m_next_callback_delay(0)
    {
    }
};

#endif // HAVOK_HK_MANIFOLD_POINT_ADDED_EVENT_H
