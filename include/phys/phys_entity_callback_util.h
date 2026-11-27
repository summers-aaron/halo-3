#ifndef INCLUDE_PHYS_ENTITY_CALLBACK_UTIL_H
#define INCLUDE_PHYS_ENTITY_CALLBACK_UTIL_H

#include "phys/phys_dynamics.h"

struct phys_contact_added_event;
struct phys_contact_confirmed_event;
struct phys_contact_removed_event;
struct phys_contact_process_event;

class phys_entity_callback_util {
public:
    static void notify_entity_added(phys_entity *entity);
    static void notify_entity_removed(phys_entity *entity);
    static void notify_entity_shape_set(phys_entity *entity);
    static void notify_entity_deleted(phys_entity *entity);
    static __forceinline void fire_entity_contact_added(phys_entity *entity, phys_contact_added_event &event) {
        if (entity->m_collision_listeners.getSize())
            notify_contact_point_added_impl(entity, event);
    }
    static __forceinline void notify_contact_point_confirmed(phys_entity *entity, phys_contact_confirmed_event &event) {
        if (entity->m_collision_listeners.getSize())
            notify_contact_point_confirmed_impl(entity, event);
    }
    static __forceinline void fire_entity_contact_removed(phys_entity *entity, phys_contact_removed_event &event) {
        if (entity->m_collision_listeners.getSize())
            notify_contact_point_removed_impl(entity, event);
    }
    static __forceinline void notify_contact_process(phys_entity *entity, phys_contact_process_event &event) {
        if (entity->m_collision_listeners.getSize())
            notify_contact_process_impl(entity, event);
    }
protected:
    static void notify_contact_point_added_impl(phys_entity *entity, phys_contact_added_event &event);
    static void notify_contact_point_confirmed_impl(phys_entity *entity, phys_contact_confirmed_event &event);
    static void notify_contact_point_removed_impl(phys_entity *entity, phys_contact_removed_event &event);
    static void notify_contact_process_impl(phys_entity *entity, phys_contact_process_event &event);
};

#endif
