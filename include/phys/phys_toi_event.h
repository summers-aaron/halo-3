#ifndef INCLUDE_PHYS_TOI_EVENT_H
#define INCLUDE_PHYS_TOI_EVENT_H

#include "phys/phys_collide.h"

class phys_entity;
class phys_contact_manager;
class phys_world;

// A time-of-impact event: built by the continuous simulation for one agent's contact and handed to the contact
// manager's processToi. hkContinuousSimulation fills m_entities through the collidables' owners, and stores the
// agent's contact manager; hkSimpleConstraintContactMgr and hkReportContactMgr read the same offsets.
struct phys_toi_event {
    float m_time;
    phys_entity *m_island_entities[2];
    float m_separating_vel;
    phys_contact_point_material m_properties;
    phys_contact_manager *m_contact_manager;
    phys_contact_point m_contact_point;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char phys_toi_event_properties_offset[(sizeof(void *) != 4 || (offsetof(phys_toi_event, m_properties) == 0x10)) ? 1 : -1];
typedef char phys_toi_event_contact_manager_offset[(sizeof(void *) != 4 || (offsetof(phys_toi_event, m_contact_manager) == 0x18)) ? 1 : -1];
typedef char phys_toi_event_contact_point_offset[(sizeof(void *) != 4 || (offsetof(phys_toi_event, m_contact_point) == 0x20)) ? 1 : -1];
typedef char phys_toi_event_size[(sizeof(void *) != 4 || (sizeof(phys_toi_event) == 0x40)) ? 1 : -1];
#endif

// Defined in tu/hkinternal/hkContinuousSimulation.cpp: the simple collision response the contact managers apply to
// a time-of-impact event whose bodies are not both fixed or keyframed.
void phys_toi_simple_collision_response(phys_world *world, const phys_toi_event &event, float rotate_normal, phys_array<phys_entity *> &to_be_activated);

#endif // HAVOK_HK_TOI_EVENT_H
