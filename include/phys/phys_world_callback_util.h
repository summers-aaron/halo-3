#ifndef INCLUDE_PHYS_WORLD_CALLBACK_UTIL_H
#define INCLUDE_PHYS_WORLD_CALLBACK_UTIL_H

class phys_world;
class phys_action;
class phys_island;
class phys_step_info;
class phys_volume;
class phys_constraint;
class phys_entity;
struct phys_contact_added_event;
struct phys_contact_confirmed_event;
struct phys_contact_removed_event;
struct phys_contact_process_event;

// Named methods defined in tu/hkdynamics/hkWorldCallbackUtil.cpp.
class phys_world_callback_util {
public:
    static void notify_action_added(phys_world *world, phys_action *action);
    static void notify_action_removed(phys_world *world, phys_action *action);
    static void notify_entity_added(phys_world *world, phys_entity *entity);
    static void notify_entity_removed(phys_world *world, phys_entity *entity);
    static void notify_entity_shape_set(phys_world *world, phys_entity *entity);
    static void notify_volume_added(phys_world *world, phys_volume *phantom);
    static void notify_volume_removed(phys_world *world, phys_volume *phantom);
    static void notify_world_deleted(phys_world *world);
    static void notify_island_activated(phys_world *world, phys_island *island);
    static void notify_island_deactivated(phys_world *world, phys_island *island);
    static void notify_world_post_integrate(phys_world *world, const phys_step_info &step_info);
    static void notify_island_post_integrate(phys_world *world, phys_island *island, const phys_step_info &step_info);
    static void notify_volume_shape_set(phys_world *world, phys_volume *phantom);
    static void notify_constraint_added(phys_world *world, phys_constraint *constraint);
    static void notify_constraint_removed(phys_world *world, phys_constraint *constraint);
    static void fire_entity_contact_added(phys_world *world, phys_contact_added_event &event);
    static void notify_contact_point_confirmed(phys_world *world, phys_contact_confirmed_event &event);
    static void fire_entity_contact_removed(phys_world *world, phys_contact_removed_event &event);
    static void notify_contact_process(phys_world *world, phys_contact_process_event &event);
    static void notify_inactive_entity_moved(phys_world *world, phys_entity *entity);
    static void notify_world_post_simulation(phys_world *world);
    static void notify_world_post_collide(phys_world *world, const phys_step_info &step_info);
    static void notify_island_post_collide(phys_world *world, phys_island *island, const phys_step_info &step_info);
};

#endif
