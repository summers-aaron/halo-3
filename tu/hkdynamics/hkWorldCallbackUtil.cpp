// hkdynamics/hkWorldCallbackUtil.obj: from the records 0x656f00 0x656f78 0x657000 0x657070 0x6570e0 0x657150 0x6571c0 0x657230 0x6572a0 0x657310 0x657380 0x6573f0 0x657460 0x6574d8 0x657550 0x6575c8 0x657640 0x6576b0 0x6577f0 0x657930 0x6579a0 0x657a18 0x657a90 0x657b08 0x657b80
// @flags /TP /O2 /Oc
// @address 0x656f00 ?compact_null_pointers@@YAXAAV?$phys_array@PAX@@@Z
// @address 0x656f78 ?compact_null_pointers@@YAXAAV?$phys_small_array@PAX@@@Z
// @address 0x657310 ?notify_volume_shape_set@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_volume@@@Z
// @address 0x657380 ?notify_constraint_added@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_constraint@@@Z
// @address 0x6573f0 ?notify_constraint_removed@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_constraint@@@Z
// @address 0x6574d8 ?notify_contact_point_confirmed@phys_world_callback_util@@SAXPAVphys_world@@AAUphys_contact_confirmed_event@@@Z
// @address 0x657550 ?fire_entity_contact_removed@phys_world_callback_util@@SAXPAVphys_world@@AAUphys_contact_removed_event@@@Z
// @address 0x657b80 ?notify_inactive_entity_moved@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_entity@@@Z
#include "phys/phys_dynamics.h"
#include "phys/phys_world_callback_util.h"




#include "phys/phys_collision_listener.h"
#include "phys/phys_island.h"
#include "phys/phys_step_info.h"
#include "phys/phys_action_listener.h"
#include "phys/phys_entity_listener.h"
#include "phys/phys_entity_activation_listener.h"
#include "phys/phys_volume_listener.h"
#include "phys/phys_constraint_listener.h"
#include "phys/phys_world_deletion_listener.h"
#include "phys/phys_island_activation_listener.h"
#include "phys/phys_post_simulation_listener.h"
#include "phys/phys_post_integrate_listener.h"
#include "phys/phys_post_collide_listener.h"
#include "phys/phys_island_post_integrate_listener.h"
#include "phys/phys_island_post_collide_listener.h"


void compact_null_pointers(phys_array<void *> &array)
{
    for (int i = array.m_size - 1; i >= 0; i--) {
        if (array.m_data[i] == 0) {
            array.remove_at_and_copy(i);
        }
    }
}

void compact_null_pointers(phys_small_array<void *> &array)
{
    for (int i = array.m_size - 1; i >= 0; i--) {
        if (array.m_data[i] == 0) {
            array.m_size--;
            for (int j = i; j < array.m_size; j++)
                array.m_data[j] = array.m_data[j + 1];
        }
    }
}

// @address 0x657000 ?notify_action_added@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_action@@@Z
void phys_world_callback_util::notify_action_added(phys_world *world, phys_action *arg)
{
    phys_array<phys_action_listener *> *a = &world->m_action_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->on_action_added(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x657070 ?notify_action_removed@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_action@@@Z
void phys_world_callback_util::notify_action_removed(phys_world *world, phys_action *arg)
{
    phys_array<phys_action_listener *> *a = &world->m_action_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->on_action_removed(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x6570e0 ?notify_entity_added@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_entity@@@Z
void phys_world_callback_util::notify_entity_added(phys_world *world, phys_entity *arg)
{
    phys_array<phys_entity_listener *> *a = &world->m_entity_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->on_entity_added(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x657150 ?notify_entity_removed@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_entity@@@Z
void phys_world_callback_util::notify_entity_removed(phys_world *world, phys_entity *arg)
{
    phys_array<phys_entity_listener *> *a = &world->m_entity_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->on_entity_removed(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x6571c0 ?notify_entity_shape_set@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_entity@@@Z
void phys_world_callback_util::notify_entity_shape_set(phys_world *world, phys_entity *arg)
{
    phys_array<phys_entity_listener *> *a = &world->m_entity_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->on_entity_shape_set(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x657230 ?notify_volume_added@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_volume@@@Z
void phys_world_callback_util::notify_volume_added(phys_world *world, phys_volume *arg)
{
    phys_array<phys_volume_listener *> *a = &world->m_volume_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->volume_added_callback(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x6572a0 ?notify_volume_removed@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_volume@@@Z
void phys_world_callback_util::notify_volume_removed(phys_world *world, phys_volume *arg)
{
    phys_array<phys_volume_listener *> *a = &world->m_volume_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->volume_removed_callback(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

class phys_constraint;


void phys_world_callback_util::notify_volume_shape_set(phys_world *world, phys_volume *arg)
{
    phys_array<phys_volume_listener *> *a = &world->m_volume_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->volume_shape_set_callback(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

void phys_world_callback_util::notify_constraint_added(phys_world *world, phys_constraint *arg)
{
    phys_array<phys_constraint_listener *> *a = &world->m_constraint_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->constraint_added_callback(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

void phys_world_callback_util::notify_constraint_removed(phys_world *world, phys_constraint *arg)
{
    phys_array<phys_constraint_listener *> *a = &world->m_constraint_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->constraint_removed_callback(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x657460 ?fire_entity_contact_added@phys_world_callback_util@@SAXPAVphys_world@@AAUphys_contact_added_event@@@Z
void phys_world_callback_util::fire_entity_contact_added(phys_world *world, phys_contact_added_event &arg)
{
    phys_array<phys_collision_listener *> *a = &world->m_collision_listeners;
    arg.m_callback_fired_from = 0;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->contact_point_added_callback(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

void phys_world_callback_util::notify_contact_point_confirmed(phys_world *world, phys_contact_confirmed_event &arg)
{
    phys_array<phys_collision_listener *> *a = &world->m_collision_listeners;
    arg.m_callback_fired_from = 0;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->contact_point_confirmed_callback(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

void phys_world_callback_util::fire_entity_contact_removed(phys_world *world, phys_contact_removed_event &arg)
{
    phys_array<phys_collision_listener *> *a = &world->m_collision_listeners;
    arg.m_callback_fired_from = 0;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->contact_point_removed_callback(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x6575c8 ?notify_contact_process@phys_world_callback_util@@SAXPAVphys_world@@AAUphys_contact_process_event@@@Z
void phys_world_callback_util::notify_contact_process(phys_world *world, phys_contact_process_event &arg)
{
    phys_array<phys_collision_listener *> *a = &world->m_collision_listeners;
    arg.m_callback_fired_from = 0;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->contact_process_callback(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x657640 ?notify_world_deleted@phys_world_callback_util@@SAXPAVphys_world@@@Z
void phys_world_callback_util::notify_world_deleted(phys_world *world)
{
    phys_array<phys_world_deletion_listener *> *a = &world->m_world_delete_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->world_deleted_callback(world);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x6576b0 ?notify_island_activated@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_island@@@Z
void phys_world_callback_util::notify_island_activated(phys_world *world, phys_island *sim_island)
{
    world->lock_critical_ops();
    phys_array<phys_island_activation_listener *> *a = &world->m_island_activation_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->on_island_activated(sim_island);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
    for (int e = 0; e < sim_island->m_island_entities.getSize(); e++) {
        phys_small_array<phys_entity_activation_listener *> *s = &sim_island->m_island_entities[e]->m_activation_listeners;
        for (int j = s->m_size - 1; j >= 0; j--) {
            if (s->m_data[j] != 0) {
                s->m_data[j]->on_entity_activated(sim_island->m_island_entities[e]);
            }
        }
        compact_null_pointers(reinterpret_cast<phys_small_array<void *> &>(*s));
    }
    world->unlock_and_flush_ops();
}

// @address 0x6577f0 ?notify_island_deactivated@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_island@@@Z
void phys_world_callback_util::notify_island_deactivated(phys_world *world, phys_island *sim_island)
{
    world->lock_critical_ops();
    phys_array<phys_island_activation_listener *> *a = &world->m_island_activation_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->on_island_deactivated(sim_island);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
    for (int e = 0; e < sim_island->m_island_entities.getSize(); e++) {
        phys_small_array<phys_entity_activation_listener *> *s = &sim_island->m_island_entities[e]->m_activation_listeners;
        for (int j = s->m_size - 1; j >= 0; j--) {
            if (s->m_data[j] != 0) {
                s->m_data[j]->on_entity_deactivated(sim_island->m_island_entities[e]);
            }
        }
        compact_null_pointers(reinterpret_cast<phys_small_array<void *> &>(*s));
    }
    world->unlock_and_flush_ops();
}

// @address 0x657930 ?notify_world_post_simulation@phys_world_callback_util@@SAXPAVphys_world@@@Z
void phys_world_callback_util::notify_world_post_simulation(phys_world *world)
{
    phys_array<phys_post_simulation_listener *> *a = &world->m_post_sim_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->post_simulation_callback(world);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x6579a0 ?notify_world_post_integrate@phys_world_callback_util@@SAXPAVphys_world@@ABVphys_step_info@@@Z
void phys_world_callback_util::notify_world_post_integrate(phys_world *world, const phys_step_info &arg)
{
    phys_array<phys_post_integrate_listener *> *a = &world->m_post_integrate_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->post_integrate_callback(world, arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x657a18 ?notify_world_post_collide@phys_world_callback_util@@SAXPAVphys_world@@ABVphys_step_info@@@Z
void phys_world_callback_util::notify_world_post_collide(phys_world *world, const phys_step_info &arg)
{
    phys_array<phys_post_collide_listener *> *a = &world->m_post_collide_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->post_collide_callback(world, arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x657a90 ?notify_island_post_integrate@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_island@@ABVphys_step_info@@@Z
void phys_world_callback_util::notify_island_post_integrate(phys_world *world, phys_island *island, const phys_step_info &arg)
{
    phys_array<phys_island_post_integrate_listener *> *a = &world->m_island_post_integ_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->post_integrate_callback(island, arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

// @address 0x657b08 ?notify_island_post_collide@phys_world_callback_util@@SAXPAVphys_world@@PAVphys_island@@ABVphys_step_info@@@Z
void phys_world_callback_util::notify_island_post_collide(phys_world *world, phys_island *island, const phys_step_info &arg)
{
    phys_array<phys_island_post_collide_listener *> *a = &world->m_island_post_collide_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->post_collide_callback(island, arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

void phys_world_callback_util::notify_inactive_entity_moved(phys_world *world, phys_entity *arg)
{
    phys_array<phys_post_simulation_listener *> *a = &world->m_post_sim_listeners;
    for (int i = a->m_size - 1; i >= 0; i--) {
        if (a->m_data[i] != 0) {
            a->m_data[i]->on_inactive_entity_moved(arg);
        }
    }
    compact_null_pointers(reinterpret_cast<phys_array<void *> &>(*a));
}

