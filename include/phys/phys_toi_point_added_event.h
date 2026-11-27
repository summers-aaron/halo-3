#ifndef INCLUDE_PHYS_TOI_POINT_ADDED_EVENT_H
#define INCLUDE_PHYS_TOI_POINT_ADDED_EVENT_H

#include "phys/phys_collision_listener.h"

struct phys_toi_point_added_event : public phys_contact_added_event
{
    float m_time;

    __forceinline phys_toi_point_added_event(phys_dynamics_contact_manager *manager, const phys_process_collision_input *input,
        phys_process_collision_output *output, const phys_collision_proxy *agent_body_a, const phys_collision_proxy *agent_body_b, phys_contact_point *point,
        const phys_simplex_cache *cache, phys_contact_point_material *material, float time, float velocity)
        : phys_contact_added_event(manager, input, output, agent_body_a, agent_body_b, point, cache, material, velocity, COLLISION_EVENT_TOI),
          m_time(time)
    {
    }
};

typedef char phys_toi_point_added_event_size[(sizeof(void *) != 4 || sizeof(phys_toi_point_added_event) == 52) ? 1 : -1];

#endif // HAVOK_HK_TOI_POINT_ADDED_EVENT_H
