#ifndef INTERFACE_FIRST_PERSON_WEAPONS_EXTERNS_H
#define INTERFACE_FIRST_PERSON_WEAPONS_EXTERNS_H

// External dependencies used by first_person_weapons.
#include "camera/director.h"

void adopt_old_object_render_message_queue(void *);
extern "C" bool first_person_weapon_raise(unsigned long user_index, long frames);

#include "cache/restricted_memory_layout.h"
#include "interface/first_person_weapons.h"

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapons_allocator> first_person_weapons_allocation_manager;
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_first_person_weapon_orientations_allocator> first_person_weapon_orientations_allocation_manager;

class c_animation_manager;
template<long k_maximum_count> class c_static_flags;
struct render_model_definition;
struct real_orientation;

void object_apply_function_overlay_node_orientations(long object_index, const render_model_definition *render_model,
    c_animation_manager *manager, const c_static_flags<255> *node_mask, long node_count, real_orientation *orientations);

#endif // INTERFACE_FIRST_PERSON_WEAPONS_EXTERNS_H
