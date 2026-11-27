#ifndef AI_SWARMS_H
#define AI_SWARMS_H

#include <stddef.h>
#include "ai/cl_engine.h"

// Functions defined in tu/ailibxbox/swarms.cpp.

struct actor_position_data;
struct actor_starting_location_definition;
union real_point3d;
struct swarms_spawner_definition;

void __tls_set_g_spawner_globals_allocator(void *allocator);
void swarms_dispose_from_old_map();
void actor_detach_swarm(long actor_index);
void swarm_spawner_end(short spawner_index);
void swarm_spawners_handle_deleted_object(long object_index);
void swarm_spawners_update();
void swarm_delete(long swarm_index);
long swarm_perception_creature_from_swarm(long swarm_index, actor_position_data *position, long preferred_creature_index);
bool swarm_perception_find_sense_position(long swarm_index, const real_point3d *position, actor_position_data *out);
void swarm_handle_creature_death(long creature_index, long damage_owner, short damage_type);
void swarm_freeze(long swarm_index);
short swarm_spawner_new(swarms_spawner_definition *definition);
long swarm_create_creature(long swarm_index, long character_index, long definition_index,
    actor_starting_location_definition *starting_location, bool exact_position, bool nondeterministic);
void swarm_spawner_new_swarm(short index);
long swarm_place(long character_index, actor_starting_location_definition *starting_location, short flags);
void ai_handle_creature_delete(long creature_index);
void swarm_creature_handle_action_finished(long object_index, long, long);
void swarm_input_update(long swarm_index);
bool ai_swarm_creature_handle_attach(long creature_index, long unit_index);
void swarm_update(long actor_index);
void __tls_set_g_swarm_spawners_allocator(void *allocator);

struct object_marker;
union real_vector3d;

extern "C" void actor_attach_swarm(long actor_index, long swarm_index);
bool swarm_remove_creature(long swarm_index, long creature_index, bool delete_creature, bool delete_empty_swarm);
bool swarm_creature_infect(long creature_index, long parent_index, struct object_marker const * marker, union real_vector3d const * direction);
bool swarm_spawner_starting_location_new(actor_starting_location_definition *starting_location, short spawner_index);
extern "C" void swarm_spawner_spawn_creature(short spawner_index);
void swarm_spawner_accelerate_creature(short spawner_index, long object_index);
extern "C" void swarm_set_data_allocator(void);
extern "C" void swarm_set_spawner_allocator(void);
void swarm_creature_update(long actor_index, long creature_index);
bool swarm_target_get_info(short type, long index, long * object_index, union real_point3d * origin, union real_point3d * center);
// match: 0x5271e8: pointer __restrict qualifier prevents alias reload
extern "C" bool swarm_can_attach_to_unit(long unit_index, bool *__restrict infectable);

struct creature_datum;
struct swarm_creature_iterator;
creature_datum *swarm_creature_iterator_next(swarm_creature_iterator *iterator);

#include "math/real_math_types.h"
#include "ai/ai_flocks.h"

// A swarm (0x34 bytes, TLS g_swarm_data / tls_swarm_data()).
struct swarm_datum {
    long identifier;
    long actor_index;
    real_point3d position;
    short creature_count;
    short initial_member_count;
    long first_object_index;                      // the first creature; swarm_creature_state::next_swarm_creature_index links the rest
    short death_count;                            // compared with character_swarm_properties::scatter_killed_count
    long last_death_time;
    real_point3d anchor_position;
    char anchor_valid;
    bool spawning;
};

// A swarm creature's object header block (creature_datum::creature.creature_reference); March swarm_creature_update_timers.
struct swarm_creature_state {
    short flags;
    long swarm_index;
    long member_swarm_index;
    long next_swarm_creature_index;
    long prop_index;
    real_point3d position;
    long target_object_index;
    short movement_mode;
    short tick_count;
    s_atom_control command_script_control;             // cs_helper, +0x28
    short delay_ticks;
    long prop_ref_index;                          // the prop being infected (infect_get_impulse), NONE when free
    short resurrect_type;                         // actor_resurrect's resurrect_type when the infection completes
    bool returning_to_anchor;
    bool jump_requested;                          // creature_move_update_jumping consumes it (swarm_creature_aim_jump)
    real_vector3d jump_velocity;                  // the velocity a requested jump launches with
    s_perlin_state movement_noise;
    real_vector3d desired_forward;
    real_vector3d desired_up;
    real_vector3d movement;
    bool new_action;
    char action_alignment[3];
    long animation;
    long action;
    short stuck_ticks;
    real_point3d stuck_position;
};

typedef char swarm_creature_action_offset[(offsetof(swarm_creature_state, action) == 0xac) ? 1 : -1];
typedef char swarm_creature_animation_offset[(offsetof(swarm_creature_state, animation) == 0xa8) ? 1 : -1];

typedef char swarm_datum_size[(sizeof(swarm_datum) == 0x34) ? 1 : -1];
typedef char swarm_creature_state_offset_prop_ref[(offsetof(swarm_creature_state, prop_ref_index) == 0x50) ? 1 : -1];
typedef char swarm_creature_state_offset_movement_noise[(offsetof(swarm_creature_state, movement_noise) == 0x64) ? 1 : -1];

typedef char swarm_creature_prop_offset[(offsetof(swarm_creature_state, prop_index) == 0x10) ? 1 : -1];
typedef char swarm_creature_control_offset[(offsetof(swarm_creature_state, command_script_control) == 0x28) ? 1 : -1];

typedef char swarm_creature_state_size[(sizeof(swarm_creature_state) == 0xc0) ? 1 : -1];
typedef char swarm_initial_member_count_offset[(offsetof(swarm_datum, initial_member_count) == 0x16) ? 1 : -1];
typedef char swarm_anchor_position_offset[(offsetof(swarm_datum, anchor_position) == 0x24) ? 1 : -1];
typedef char swarm_anchor_valid_offset[(offsetof(swarm_datum, anchor_valid) == 0x30) ? 1 : -1];
typedef char swarm_member_swarm_offset[(offsetof(swarm_creature_state, member_swarm_index) == 8) ? 1 : -1];
typedef char swarm_returning_offset[(offsetof(swarm_creature_state, returning_to_anchor) == 0x56) ? 1 : -1];

#include "memory/data.h"
#include "ai/ai_tls_data.h"
#include "ai/swarm_creature_iterator_types.h"

// Starts an iterator over the swarm's creatures.
static __forceinline void swarm_creature_iterator_new(long swarm_index, swarm_creature_iterator *iterator)
{
    swarm_datum *swarm = datum_get<swarm_datum>(g_swarm_data, swarm_index);
    iterator->current_object_index = -1;
    iterator->next_object_index = swarm->first_object_index;
}

#endif // AI_SWARMS_H
