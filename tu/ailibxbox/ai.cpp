// ailibxbox/ai.obj: from the records 0xa8a60 0xa95b0 0xb01f0 0xb8070 0xc4370 0xc8960 0xc8a60 0xc8ad8 0xcc3b8 0xf4f78 0xfac38 0x111030 0x236d90 0x4f71c8 0x4f71d8 0x4f75d8 0x4f7608 0x4f77d8 0x4f78a0 0x4f7a38 0x4f7bb8 0x4f7be0 0x4f7cb0 0x4f80c0 0x4f81a8 0x4f82c8 0x4f8308 0x4f8528 0x4f85f0 0x4f86a8 0x4f8a38 0x4f8bd8 0x4f8e28 0x4f8f38 0x4f8fc8 0x4f96c8 0x4f9908 0x4f9a68 0x4f9c80 0x4f9dc0 0x4f9fe0 0x4fa0d8 0x4fa238 0x4fa3a0 0x4fa480 0x4fa568 0x4faac8 0x4fab48 0x4fabe8 0x4fac00 0x54a620 0x55ac80 0x7089b0
#include "cseries/string_literal_pool.h"
#include "cache/restricted_memory.h"
#include "ai/ai_scratch.h"
#include "ai/actor_dynamic_firing_position.h"
#include "ai/ai_tls_data.h"
#include "effects/effect_event_data.h"
#include "ai/character_behavior_properties.h"
#include "ai/ai_dialogue_globals.h"
#include "ai/ai_information_data.h"
#include "memory/hashtable_reset.h"
#include "memory/hashtable.h"
#include "math/real_math_functions.h"
#include "ai/character_weapon_properties.h"
#include "physics/collision_test_vector.h"
#include "cseries/sort.h"
#include "game/game_globals.h"
#include "tag_files/tag_types.h"
#include "ai/clump_importance_types.h"
#include "ai/ai_player_types.h"
#include "ai/squad_data.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "game/game_globals_storage.h"
#include "math/matrix_math_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_types.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "units/unit_passenger_iterator.h"
#include "ai/actor_globals.h"
#include "ai/clump_datum.h"
#include "ai/prop_ref_datum.h"
#include "ai/actor_datum.h"
#include "ai/ai_releasable_entry.h"
#include "ai/actor_type_flood_types.h"
#include "ai/prop_datum.h"
#include "ai/clump_actor_cursor_types.h"
#include <math.h>
#include <stdlib.h>
#include "game/game_time_types.h"
#include "objects/object_datum.h"
#include "objects/object_placement.h"
#include "objects/object_iterator.h"
#include "ai/actors.h"
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "objects/object_header_access.h"
#include "scenario/scenario.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "ai/actor_firing_position.h"
#include "memory/data_types.h"
#include "ai/squad_types.h"
#include "ai/behavior_stimulus_types.h"
#include "units/vehicle_auto_control.h"
#include "scenario/scenario_tag.h"
#include "phys/phys_memory.h"
#include "ai/actor_prop_ref_iterator.h"
#include "ai/actor_stimulus.h"
#include "ai/actor_perception.h"
#include "ai/ai.h"
#include "ai/ai_debug_throttled_global.h"
#include "ai/ai_dialogue.h"
#include "ai/ai_script.h"
#include "ai/command_script_data.h"
#include "ai/objective_storage.h"
#include "ai/ai_dialogue_queue.h"
#include "ai/ai_player.h"
#include "ai/ai_reference_frame.h"
#include "ai/ai_vehicles.h"
#include "ai/behavior.h"
#include "ai/clump.h"
#include "ai/event_handling.h"
#include "ai/joint_behavior.h"
#include "ai/objectives.h"
#include "ai/prop_dialogue.h"
#include "ai/props.h"
#include "ai/squads.h"
#include "ai/swarms.h"
#include "effects/effects.h"
#include "objects/damage.h"
#include "objects/damage_owner.h"
#include "units/bipeds.h"
#include "units/vehicles.h"
#include "units/vehicle_datum.h"
#include "units/vehicle_definitions.h"
#include "units/unit_definitions.h"
#include "ai/squad_definitions.h"
#include "ai/ai_flocks.h"
#include "motor/action_system.h"
#include "physics/havok_memory.h"
#include "units/vehicle_type_component.h"
#include "ai/character_definitions.h"
#include "memory/data.h"
#include "memory/tls_globals.h"
#include "ai/ai_memory.h"
#include "game/game_allegiance.h"
#include "game/game_allegiance_types.h"
#include "cache/restricted_memory_layout.h"
#include "physics/collision_types.h"
#include "ai/ai_sound_volume_types.h"
#include "physics/memory_allocator_types.h"
#include "ai/swarm_creature_iterator_types.h"
#include "ai/sound_types.h"
#include "ai/prop_stimulus_types.h"
#include "ai/ai_scenario_definitions.h"
#include "cache/cache_files.h"
#include "ai/squad_iterator_types.h"
#include "memory/hashtable_types.h"
#include "ai/actor_firing_position_hash.h"
#include "game/game_time_inline.h"
#include "ai/ai_globals.h"
#include "ai/ai_player_tls.h"
#include "math/real_constant_pool.h"
#include <string.h>
#include "ai/actor_iterator.h"
#include "units/biped_types.h"
#include "ai/prop_view.h"
#include "objects/object_definitions.h"
#include "models/model_types.h"
#include "creatures/creature_datum.h"
#include "ai/swarm_spawner_definition.h"
#include "ai/swarm_spawner_types.h"
#include "items/weapon_definitions.h"
#include "physics/havok_memory_globals.h"
#include "ai/ai_globals_definition.h"
#include "ai/dialogue_event.h"
#include "ai/flock_instance_types.h"
#include "ai/team_dialogue_state.h"
#include "units/unit_dialogue_internals.h"
#include "items/equipment_definitions.h"
#include "effects/effect_datum.h"
#include "effects/effect_definitions.h"
#include "effects/effect_event_definitions.h"
#include "effects/effect_message.h"
#include "objects/damage_data.h"
#include "models/model_damage_info.h"
#include "ai/event_handling_sound_types.h"
#include "items/projectile_definitions.h"
#include "cseries/crt_xdk.h"
#include "physics/collisions.h"
#include "ai/ai_avoidance.h"
#include "cseries/cseries.h"
#include "tag_files/tag_groups.h"
#include "math/random_math_range.h"
#include "ai/clump_iterator.h"
#include "memory/datum_index.h"
#include "cseries/bit_operations.h"

struct line_of_fire_pill {
    bool spherical;
    real_point3d position;
    real_vector3d axis;
    long owner;
    long object;
    float radius;
};

struct u_ai_prop_iterator { prop_ref_datum *reference;s_data_iterator iterator; };

template <typename T> struct u_ai_sort_compare { typedef bool (*type)(T, T, const void *); };

template <typename T> struct u_ai_data_iterator {
    T *m_datum;
    s_data_iterator m_iterator;
    __forceinline void begin(s_data_array *data)
    {
        m_iterator.absolute_index = -1;
        m_iterator.index = -1;
        m_iterator.data = data;
    }
    __forceinline bool next()
    {
        m_datum = (T *)data_iterator_next(&m_iterator);
        return m_datum != 0;
    }
    __forceinline T *get_datum() const { return m_datum; }
    __forceinline long get_index() const { return m_iterator.index; }
};

union dispose_iterators { actor_iterator actors; clump_actor_cursor squads; };



union u_ai_update_iterators {
    clump_actor_cursor joint;
    actor_iterator freeze;
};

// @address 0x11df670 ai_globals
__declspec(thread) ai_globals_type *ai_globals;
// @address 0x897e00 g_ai_scratch_buffers
s_ai_scratch_block g_ai_scratch_buffers[2];
// @address 0x8c4949 g_ai_scratch_flags
char g_ai_scratch_flags[2];
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x5025c8 ?actor_erase@@YAXJ_N@Z
// @address 0xa8bf0 ?actor_iterator_next@@YAPAUactor_datum@@PAUactor_iterator@@@Z
// @address 0x4fad58 ?ai_player_state_get@@YAPAUai_player_state@@J@Z
// @address 0x223918 ?game_allegiance_provoke@@YAXW4e_game_team@@0@Z
// @address 0xac748 ?object_try_and_get_and_verify_type@@YAPAXJK@Z

// @address 0x11c3d0 ?firing_position_ref_hash_compare@@YA_NPBX0@Z
namespace {
static __forceinline void hash_table_set_functions(s_hash_table *table, unsigned long (*hash)(void *),
    unsigned char (*equal)(void *, void *)) {
 table->hash = hash;
 table->equal = equal;
}
}

typedef void (*field_proc_t)(void *field);
/* Ordering predicate: true when a sorts before b (by the word at +8 descending, then by the byte at +0 ascending). */
// @address 0x11df714 g_game_allegiance_globals
// @address 0x2251c k_real_zero_storage
// @address 0x3aac8 k_real_math_identity_matrix_storage


// @address 0xc4370 ?ai_chance_this_tick@@YAMM@Z
float ai_chance_this_tick(float chance) {
 chance = ((chance > 0.0f ? chance : 0.0f) < 1.0f) ? (chance > 0.0f ? chance : 0.0f) : 1.0f;
 return 1.0f - (float)pow((double)(1.0f - chance), (double)game_tick_length());
}


// @address 0x4f71c8 ?ai_globals_set_allocator_address@@YAXPAX@Z
void ai_globals_set_allocator_address(void *address)
{
 ai_globals = (ai_globals_type *)address;
}

// @address 0x4f75d8 ?ai_dispose@@YAXXZ
void ai_dispose() {
 hash_table_dispose(g_actor_firing_position_hash);
}

// @address 0x4f7bb8 ?ai_initialize_for_saved_game@@YAXJ@Z
void ai_initialize_for_saved_game(long saved_game_index) {
 hash_table_set_functions(g_actor_firing_position_hash, (unsigned long (*)(void *))firing_position_ref_hash_function,
     (unsigned char (*)(void *, void *))firing_position_ref_hash_compare);
}

// @address 0x4f7be0 ?ai_globals_initialize_for_new_map@@YAXXZ
void ai_globals_initialize_for_new_map()
{
 memset(ai_globals, 0, sizeof(ai_globals_type));
 ai_globals->active = true;
 ai_globals->initialized = true;
 ai_globals->first_squadless_actor = -1;
 ai_globals->perception_enabled = true;
 ai_globals->dialogue_enabled = true;
 ai_globals->current_squad = -1;
 ai_globals->current_actor = -1;
 ai_globals->current_objective = -1;
 ai_globals->count = 0;
 memset(ai_globals->actor_indices, -1, sizeof(ai_globals->actor_indices));
 memset(ai_globals->squad_indices, -1, sizeof(ai_globals->squad_indices));
 memset(ai_globals->objective_indices, -1, sizeof(ai_globals->objective_indices));
 for (short index = 0; index < 8; ++index) {
  ai_globals->mission_critical[index].unit_index = -1;
  ai_globals->mission_critical[index].object_index = -1;
  ai_globals->mission_critical[index].time = 0.0f;
  ai_globals->mission_critical[index].bring_forward_ticks = 0;
  ai_globals->mission_critical[index].flags = 0;
 }
 ai_globals->current_mission_critical = -1;
}

// @address 0x11ab20 ?squad_actor_iterator_new@@YAXPAUsquad_actor_iterator@@J_N@Z

// 0x4f80c0
void ai_erase(long squad_index, bool immediately)
{
 if (ai_globals->gate) {
  if (squad_index != -1) {
   squad_actor_iterator squad_iterator;
   squad_actor_iterator_new(&squad_iterator, squad_index, false);
   if (::squad_actor_iterator_next(&squad_iterator) == 0)
    return;
   do {
    actor_erase(squad_iterator.index, immediately);
   } while (::squad_actor_iterator_next(&squad_iterator) != 0);
   return;
  }
  actor_iterator iterator;
  iterator.m_iterator.absolute_index = -1;
  iterator.m_iterator.index = -1;
  iterator.active_only = false;
  iterator.m_iterator.data = actor_data;
  if (actor_iterator_next(&iterator) != 0) {
   do {
    actor_erase(iterator.index, immediately);
   } while (actor_iterator_next(&iterator) != 0);
  }
 }
}

// @address 0x4f82c8 compare_releasable_inactive_squads
extern "C" int compare_releasable_inactive_squads(const s_ai_releasable_entry *a, const s_ai_releasable_entry *b)
{
    long pa = a->release_index;
    long pb = b->release_index;
    if (pb < pa)
        return 1;
    if (pb > pa)
        return 0;
    return a->is_actor < b->is_actor;
}



namespace {
}
namespace {
static __forceinline biped_datum *ai_unit_get_and_verify_type(long object_index, unsigned long type_mask)
{
    return (biped_datum *)datum_get<s_object_header>(object_header_data, object_index)->datum;
}
static __forceinline real_vector3d *ai_set_vector3d(real_vector3d *vector, float i, float j, float k)
{
    vector->i = i;
    vector->j = j;
    vector->k = k;
    return vector;
}
}
// @address 0x4f85f0 ?ai_generate_line_of_fire_pill@@YAXJJPAUline_of_fire_pill@@@Z
void ai_generate_line_of_fire_pill(long object_index, long owner_index, line_of_fire_pill *pill)
{
    biped_datum *object = ai_unit_get_and_verify_type(object_index, 1);
    real_matrix4x3 *unit = (real_matrix4x3 *)((char *)object + object->object.node_matrices_offset);
    float height;
    float radius;
    ::biped_get_physics_pill_from_definition(object_index, &pill->position, (float *)(&height), (float *)(&radius));
    pill->position.x = unit->position.x;
    pill->position.y = unit->position.y;
    pill->spherical = height == 0.0f;
    ai_set_vector3d(&pill->axis, 0.0f, 0.0f, height);
    pill->radius = radius + 0.15f;
    pill->owner = owner_index;
    pill->object = object_index;
}



// @address 0x4f8f38 ?ai_get_unit_responsible_for_damage@@YAJJ_N@Z
long ai_get_unit_responsible_for_damage(long object_index, bool prefer_vehicle)
{
 long responsible_index = -1;
 if (object_index != -1) {
  unit_datum *datum = (unit_datum *)object_try_and_get_and_verify_type(object_index, 0x1003);
  if (datum != 0) {
   if (prefer_vehicle && datum->unit.gunner_object_index != -1) {
    responsible_index = datum->unit.gunner_object_index;
   } else if (datum->unit.driver_object_index != -1) {
    responsible_index = datum->unit.driver_object_index;
   } else {
    responsible_index = object_index;
   }
  }
 }
 return responsible_index;
}


namespace {
static __forceinline prop_view *ai_damage_prop_view_get(const prop_ref_datum *reference)
{
    prop_view *view = 0;
    if (reference->view_index != -1) {
        tracking_datum *datum = datum_get<tracking_datum>(tracking_data, reference->view_index);
        if (datum)
            view = (prop_view *)datum->view;
    }
    return view;
}
// match: 0x4f9dc0: the boolean helper preserves the parent-flag extraction before the root walk.
static __forceinline bool ai_damage_object_has_parent(const object_datum *object)
{
    return TEST_BIT(object->flags, _object_data_flag_26);
}
static __forceinline long ai_damage_object_get_root(long object_index)
{
    while (object_index != -1) {
        biped_datum *object = ai_unit_get_and_verify_type(object_index, -1);
        if (!ai_damage_object_has_parent(&object->object))
            break;
        object_index = object->object.parent_object_index;
    }
    return object_index;
}
}
// @address 0x4f9dc0 ?ai_handle_damage@@YAXJJFJMPBTreal_vector3d@@_N@Z
void ai_handle_damage(long object_index, long damaging_object_index, short damage_type, long damage_definition_index, float damage,
    const real_vector3d *direction, bool ignore_actor)
{
    if (ai_globals->gate && damage >= 0.0f) {
        long responsible_unit_index = ai_get_unit_responsible_for_damage(damaging_object_index, damage_type != 9);
        biped_datum *responsible_unit = responsible_unit_index == -1 ? 0 : ai_unit_get_and_verify_type(responsible_unit_index, 0x1003);
        biped_datum *unit = ai_unit_get_and_verify_type(object_index, 0x1003);
        if (responsible_unit && responsible_unit->unit.actor_index != -1) {
            long reference_index = ::prop_ref_get_acknowledged_by_object_index(responsible_unit->unit.actor_index, object_index);
            if (reference_index != -1) {
                prop_ref_datum *reference = datum_get<prop_ref_datum>(prop_ref_data, reference_index);
                prop_view *view = ai_damage_prop_view_get(reference);
                if (view)
                    view->vitality += 0.2f;
            }
            if (responsible_unit->unit.player_index != -1 && unit->unit.parent_seat_index != -1 &&
                !game_team_is_enemy((e_game_team)responsible_unit->unit.team, (e_game_team)unit->unit.team)) {
                ai_player_state *player = ai_player_state_get(responsible_unit->unit.player_index);
                if (player) {
                    player->last_damage_time = game_time_globals->time;
                    player->last_damage_object = ai_damage_object_get_root(unit->object.parent_object_index);
                }
            }
        }
        if (unit->unit.actor_index != -1) {
            if (!ignore_actor && damage_type != 1 && unit->unit.actor_index != -1)
                actor_handle_damage(unit->unit.actor_index, damage_type, damage_definition_index, responsible_unit_index,
                    damage, direction);
            if (responsible_unit)
                game_allegiance_provoke(responsible_unit->unit.team, unit->unit.team);
        }
    }
}

// @address 0x4fab48 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_ai_globals_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_ai_globals_allocator>::reserve_memory(const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int member_index = g_restricted_regions[3].add_member(name, type, size, 0, ai_globals_set_allocator_address, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(member_index);
  void *address = g_restricted_address[3] + offset;
  int owner = g_registered_thread_index;
  m_thread_id = owner;
  m_member_index = member_index;
  ai_globals = (ai_globals_type *)address;
  return g_restricted_address[3] + g_restricted_regions[3].get_member_offset(member_index);
}

// @address 0x4fabe8 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_ai_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_ai_globals_allocator>::allocate(unsigned int size, const char *name)
{
  return reserve_memory(name, "", size, 0);
}

// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z

// @address 0x4fac00 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_ai_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_ai_globals_allocator>::deallocate(void *memory)
{
  int member_index = m_member_index;
  ai_globals = 0;
  restricted_region_free_member(3, member_index);
}

// Shared natural math helpers used by the AI queries.
namespace {
static __forceinline real_vector3d *u_ai_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *v) {
 v->i=b->x-a->x; v->j=b->y-a->y; v->k=b->z-a->z; return v;
}
static __forceinline real_point3d *u_ai_point_from_line(const real_point3d *p, const real_vector3d *v, float t, real_point3d *out) {
 const real_vector3d *direction=v;
 out->x=p->x+direction->i*t; out->y=p->y+direction->j*t; out->z=p->z+direction->k*t; return out;
}
static __forceinline real_vector3d *u_ai_cross(const real_vector3d *a, const real_vector3d *b, real_vector3d *out) {
 float k=a->i*b->j-a->j*b->i;
 float j=a->k*b->i-a->i*b->k;
 out->i=a->j*b->k-a->k*b->j; out->j=j; out->k=k; return out;
}
static __forceinline float u_ai_dot(const real_vector3d *a, const real_vector3d *b) {
 return a->i*b->i+a->j*b->j+a->k*b->k;
}
static __forceinline float u_ai_magnitude_squared(const real_vector3d *v) { return v->i*v->i+v->j*v->j+v->k*v->k; }
static __forceinline float u_ai_distance_squared(const real_point3d *a,const real_point3d *b) {
 real_vector3d v; return u_ai_magnitude_squared(u_ai_vector_from_points(a,b,&v));
}
static __forceinline real_vector3d *u_ai_scale(const real_vector3d *v,float scale,real_vector3d *out) {
 out->i=scale*v->i; out->j=scale*v->j; out->k=scale*v->k; return out;
}
static __forceinline float u_ai_triple(const real_vector3d *a,const real_vector3d *b,const real_vector3d *c) {
 real_vector3d cross; return u_ai_dot(u_ai_cross(a,b,&cross),c);
}
}
namespace {
static __forceinline float u_ai_distance(const real_point3d *a,const real_point3d *b) { return sqrtf(u_ai_distance_squared(a,b)); }
}


// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0xae510 ?actor_get_weapon@@YAJJ@Z
// @address 0xb4838 ?actor_is_giant@@YA_NPAUactor_datum@@@Z
// @address 0xc0430 ?character_weapon_properties_get@@YAPAUcharacter_weapon_properties@@JJ@Z
namespace {
static __forceinline actor_datum *u_ai_actor_get(long index) { return datum_get<actor_datum>(actor_data, index); }
static __forceinline long u_ai_root_object(long object_index) {
 long root=-1;
 while(object_index!=-1) { root=object_index;object_index=ai_unit_get_and_verify_type(object_index,-1)->object.parent_object_index; }
 return root;
}
static __forceinline void u_ai_prop_begin(u_ai_prop_iterator *iterator) {
 iterator->iterator.data=prop_ref_data;
 iterator->iterator.absolute_index=-1;iterator->iterator.index=-1;
}
static __forceinline bool u_ai_prop_next(u_ai_prop_iterator *iterator) {
 iterator->reference=(prop_ref_datum *)::data_iterator_next(&iterator->iterator);
 return iterator->reference!=0;
}
static __forceinline long u_ai_seconds_to_ticks_round(float seconds) { return (long)((float)game_time_globals->ticks_per_second*seconds+0.5f); }
}
// @rdata 0x82022dbc
static const float ai_swarm_spawner_minimum_delay = 0.1f;
// @rdata 0x82022dc0
static const float ai_swarm_spawner_maximum_delay = 0.5f;
// @address 0x4fa480 ?ai_swarm_spawn_from_effect@@YAXHFHFMMPBUreal_matrix4x3@@@Z
void ai_swarm_spawn_from_effect(int p0, short p1, int p2, short p3, float p7, float p8, const real_matrix4x3 *p6)
{
    if (ai_globals->gate) {
        character_general_properties *props = character_definition_general_properties_get(p0);
        if ((props->flags & 1) != 0) {
            swarms_spawner_definition def;
            def.reference.marker_name = 0;
            def.values[4] = p7;
            def.values[5] = p8;
            def.reference.definition_index = p0;
            def.count = p1;
            def.reference.starting_location_index = p2;
            def.node_index = p3;
            def.matrix = p6;
            def.values[0] = 0.0f;
            def.values[1] = 0.0f;
            def.values[2] = 0.0f;
            def.values[3] = 0.0f;
            def.values[6] = 0.0f;
            def.values[7] = 0.0f;
            def.minimum_delay = ai_swarm_spawner_minimum_delay;
            def.maximum_delay = ai_swarm_spawner_maximum_delay;
            short spawner = swarm_spawner_new(&def);
            if (spawner != -1) {
                ((s_swarm_spawner *)g_swarm_spawners)[spawner].flags |= 1;
                swarm_spawner_new_swarm(spawner);
            }
        }
    }
}
// @address 0x4fa568 ?actor_endangering_player@@YAJ_N0PAK@Z
long actor_endangering_player(bool require_target,bool all_players,unsigned long *player_mask) {
 long time=game_time_globals->time;
 long result=-1;
 if(all_players) *player_mask=0;
 u_ai_prop_iterator iterator;
 u_ai_prop_begin(&iterator);
 while(u_ai_prop_next(&iterator)) {
  prop_ref_datum *reference=iterator.reference;
  prop_datum *prop=datum_get<prop_datum>(prop_data, reference->prop_index);
  if(prop->player && prop->valid) {
   biped_datum *unit=ai_unit_get_and_verify_type(prop->object_index,0x2003);
   if(unit->unit.player_index!=-1) {
    long actor_index=reference->actor_index;
    actor_datum *actor=u_ai_actor_get(actor_index);
    bool ignore=false;
    if(!actor->active) ignore=true;
    else if(actor->unit_index!=-1) {
     biped_datum *actor_unit=ai_unit_get_and_verify_type(actor->unit_index,-1);
     s_object_definition *definition=(s_object_definition *)tag_get(0, actor_unit->object.definition_index);
     if(definition->model.index!=-1) {
      s_model_definition *model=(s_model_definition *)tag_get(0, definition->model.index);
      if((model->flags&16) && reference->distance>2.5f) ignore=true;
     }
    } else if(actor->swarm_index!=-1 && unit->object.shield_vitality>=0.99f) {
     swarm_creature_iterator creature_iterator;
     creature_iterator.next_object_index=datum_get<swarm_datum>(g_swarm_data, actor->swarm_index)->first_object_index;
     creature_iterator.current_object_index=-1;
     ignore=true;
     creature_datum *creature=swarm_creature_iterator_next(&creature_iterator);
     while(creature) {
      bool flying=false;
      s_object_definition *definition =
          (s_object_definition *)tag_get(0, creature->object.definition_index);
      if(definition->model.index!=-1) {
       s_model_definition *model=(s_model_definition *)tag_get(0, definition->model.index);
       if(model->flags&16) flying=true;
      }
      if(!flying) {ignore=false;break;}
      creature=swarm_creature_iterator_next(&creature_iterator);
     }
    }
    if(require_target && reference->distance>15.0f) {
     if(!actor->target_state || actor->target_prop_index!=iterator.iterator.index) ignore=true;
    }
    if(!ignore) {
     bool target=false;
     real_point3d origin;
     object_get_origin(reference->object_index,&origin);
     float distance=u_ai_distance(&actor->position.body,&origin);
     if(distance<4.0f) {
      if(!all_players) return actor_index;
      result=actor_index;
      *player_mask|=1<<(unsigned short)unit->unit.player_index;
      continue;
     }
     {
      if(actor->target_prop_index!=-1) {
       if(actor->target_prop_index==iterator.iterator.index) target=true;
       else {
        prop_ref_datum *target_reference=datum_get<prop_ref_datum>(prop_ref_data, actor->target_prop_index);
        if(target_reference->object_index!=-1
            && u_ai_root_object(target_reference->object_index)==u_ai_root_object(prop->object_index)) target=true;
       }
      }
      if(target) {
       long weapon_index=actor_get_weapon(actor_index);
       float range=actor_is_giant(actor)?60.0f:15.0f;
       if(weapon_index!=-1) {
        biped_datum *weapon=ai_unit_get_and_verify_type(weapon_index,4);
        character_weapon_properties *properties=character_weapon_properties_get(actor->character_index,weapon->object.definition_index);
        if(properties && ((properties->flags&4) || actor_is_giant(actor) || actor->controlling_other_unit)) {
         range=range>properties->maximum_range+5.0f?range:properties->maximum_range+5.0f;
        }
       }
       if(reference->awareness_class>=1 && reference->awareness_class<=2 && distance<range) {
        if(!all_players) return actor_index;
      result=actor_index;
      *player_mask|=1<<(unsigned short)unit->unit.player_index;
        continue;
       }
       else if(reference->awareness_class>=3) {
        prop_view *view=ai_damage_prop_view_get(reference);
        if(view && distance<(actor_is_giant(actor)?50.0f:12.0f) && time-view->tracking_index<u_ai_seconds_to_ticks_round(5.0f)) {
         if(!all_players) return actor_index;
      result=actor_index;
      *player_mask|=1<<(unsigned short)unit->unit.player_index;
        }
       }
      }
     }

    }
   }
  }
 }
 return all_players?result:-1;
}
// @address 0x4fa3a0 ?ai_handle_weapon_trade@@YAXJJJJ@Z
// @address 0x564e60 ?actor_dialogue_event@@YA_NJFJPAUai_information_data@@@Z
static __forceinline void u_ai_actor_handle_weapon_trade(long actor_index, long player_index, long old_weapon, long new_weapon)
{
 player_datum *player = datum_get<player_datum>(player_data, player_index);
 float new_value = 0.0f;
 float old_value = 0.0f;
 if (old_weapon != -1)
  old_value = ((weapon_definition *)tag_get(0, ai_unit_get_and_verify_type(old_weapon, 4)->object.definition_index))
                  ->scariness;
 if (new_weapon != -1)
  new_value = ((weapon_definition *)tag_get(0, ai_unit_get_and_verify_type(new_weapon, 4)->object.definition_index))
                  ->scariness;
 if (new_value > old_value)
  actor_dialogue_event(actor_index, 90, player->unit_index, 0);
 else if (new_value < old_value)
  actor_dialogue_event(actor_index, 91, player->unit_index, 0);
 else
  actor_dialogue_event(actor_index, 92, player->unit_index, 0);
}
void ai_handle_weapon_trade(long actor_index, long player_index, long old_weapon, long new_weapon)
{
 if (ai_globals->gate)
  u_ai_actor_handle_weapon_trade(actor_index, player_index, old_weapon, new_weapon);
}

// @address 0x4faac8 ?ai_enemies_can_see_player@@YA_NPAJ@Z
bool ai_enemies_can_see_player(long *unit_index) {
 long actor_index=actor_endangering_player(false,false,0);
 if(actor_index!=-1) {
  *unit_index=u_ai_actor_get(actor_index)->unit_index;
  return true;
 }
 return false;
}

// @address 0xa430e0 g_havok_memory_globals



static __forceinline biped_datum *u_ai_object_get_and_verify_type(long index)
{
    return (biped_datum *)datum_get<s_object_header>(object_header_data, index)->datum;
}
static __forceinline void u_ai_clump_iterator_new(clump_actor_cursor *it)
{
    if (ai_globals->gate)
    {
        it->iterator.absolute_index = -1;
        it->iterator.index = -1;
        it->iterator.data = clump_data;
    }
}
static __forceinline real_vector2d *u_ai_set_vector2d(real_vector2d *v, float i, float j) { v->i = i; v->j = j; return v; }
static __forceinline float u_ai_magnitude_squared2d(const real_vector2d *v) { return v->i * v->i + v->j * v->j; }
static __forceinline float u_ai_magnitude2d(const real_vector2d *v) { return sqrtf(u_ai_magnitude_squared2d(v)); }
static __forceinline real_vector2d *u_ai_scale_vector2d(const real_vector2d *v, float scale, real_vector2d *result)
{
    result->i = v->i * scale;
    result->j = v->j * scale;
    return result;
}
static __forceinline float u_ai_normalize2d(real_vector2d *v)
{
    float length = u_ai_magnitude2d(v);
    if (fabsf(length - 0.0f) < 0.0001f)
        length = 0.0f;
    else
        u_ai_scale_vector2d(v, 1.0f / length, v);
    return length;
}
// @address 0xa95b0 ai_can_resurrect_unit
extern "C" bool ai_can_resurrect_unit(long index)
{
 s_game_globals *g = global_game_globals;
 biped_datum *biped = u_ai_object_get_and_verify_type(index);
 bool result = true;
 float delay = 4.0f, pill_radius = 5.0f, clump_radius = 5.0f, combat_radius = 10.0f;
 if (g && g->ai_globals.count() > 0) {
  const s_ai_globals_definition *d = (const s_ai_globals_definition *)g->ai_globals.address;
  delay = d->delay; pill_radius = d->pill_radius; clump_radius = d->clump_radius; combat_radius = d->combat_radius;
 }
 if (game_time_globals->time - biped->unit.type_change_time < u_ai_seconds_to_ticks_round(delay)) {
  result = false;
 } else {
  real_point3d origin;
  object_get_origin(index, &origin);
  c_object_iterator<biped_datum> projectiles;
  projectiles.begin(32, 0);
  for (;;) {
   if (projectiles.next()) {
    real_point3d projectile_origin;
    object_get_origin(projectiles.get_index(), &projectile_origin);
    real_vector2d direction;
    u_ai_set_vector2d(&direction, projectiles.get_datum()->object.translational_velocity.i,
        projectiles.get_datum()->object.translational_velocity.j);
    u_ai_normalize2d(&direction);
    if (point_in_pill2d(&origin.xy, &projectile_origin.xy, &direction, pill_radius)) {
     result = false;
     break;
    }
   } else {
    clump_actor_cursor it;
    u_ai_clump_iterator_new(&it);
    clump_datum *c;
    while ((c = (clump_datum *)clump_actor_cursor_next(&it)) != 0) {
     if (c->active && game_team_is_enemy((e_game_team)biped->unit.team, (e_game_team)c->team)) {
      float radius = c->combat_state > 0 ? combat_radius : clump_radius;
      float distance = distance3d(&origin, &c->position);
      if (distance < radius) {
       result = false;
       break;
      }
     }
    }
    break;
   }
  }
 }
 return result;
}

// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z
// @address 0x524e58 ?swarm_delete@@YAXJ@Z

// @address 0x4f81a8 ?ai_release_inactive_swarms@@YAJJPAEJPA_NPADJ@Z
static __forceinline void u_ai_swarm_iterator_new(clump_actor_cursor *it)
{
 if (ai_globals->gate) {
  it->iterator.data = g_swarm_data;
  it->iterator.absolute_index = -1;
  it->iterator.index = -1;
  it->index = -1;
 }
}
static __forceinline swarm_datum *u_ai_swarm_iterator_next(clump_actor_cursor *it)
{
 swarm_datum *result = 0;
 if (ai_globals->gate) {
  it->datum = (swarm_datum *)data_iterator_next(&it->iterator);
  if (it->datum != 0)
   result = (swarm_datum *)it->datum;
  it->index = it->iterator.index;
 }
 return result;
}

long ai_release_inactive_swarms(
    long unused, unsigned char *working_memory, long working_memory_size, bool *more_to_release, char *description, long description_size)
{
 long released_count = 0;
 long released_actor_count = 0;
 clump_actor_cursor iterator;
 swarm_datum *swarm;
 u_ai_swarm_iterator_new(&iterator);
 while ((swarm = u_ai_swarm_iterator_next(&iterator)) != 0) {
  bool release = true;
  if (swarm->actor_index != -1) {
   actor_datum *actor = datum_get<actor_datum>(actor_data, swarm->actor_index);
   release = !actor->active && actor->deactivation_ticks != -1;
  }
  if (release) {
   released_count++;
   released_actor_count += swarm->creature_count;
   if (swarm->actor_index != -1)
    actor_erase(swarm->actor_index, true);
   else
    swarm_delete(iterator.index);
  }
 }
 *more_to_release = false;
 return released_actor_count;
}

// @address 0x4f8528 ?ai_release_inactive_squads@@YAJJPAEJPA_NPADJ@Z
long ai_release_inactive_squads(
    long unused, unsigned char *working_memory, long working_memory_size, bool *more_to_release, char *description, long description_size)
{
 s_ai_releasable_storage *storage = (s_ai_releasable_storage *)working_memory;
 long released_count = 0;
 if (storage->released_count < storage->count) {
  s_ai_releasable_entry *entry = &storage->entries[storage->released_count];
  if (entry->is_actor) {
   released_count++;
   actor_erase(entry->index, true);
  } else {
   squad_datum *squad = &((squad_datum *)squad_data->data)[(unsigned short)entry->index];
   released_count += squad->count0;
   ai_erase(entry->index, true);
  }
  storage->released_count++;
 }
 s_ai_releasable_storage *s = storage;
 *more_to_release = s->released_count < s->count;
 return released_count;
}

// @address 0xcc3b8 ?ai_scratch_allocate@@YAPAXI@Z
void *ai_scratch_allocate(unsigned int size)
{
    unsigned char *flags = (unsigned char *)g_ai_scratch_flags;
    int i;
    void *result = 0;
    if (!flags[1]) {
        flags[0] = g_havok_memory_globals.overflow_pool != 0;
        if (flags[0])
            havok_memory_dispose_overflow_pool();
        char *memory = g_havok_memory_globals.overflow_pool_default_memory;
        unsigned long buffer_size = sizeof(g_havok_memory_globals.overflow_pool_default_memory) / 2;
        for (i = 0; i < 2; i++)
        {
            g_ai_scratch_buffers[i].in_use = 0;
            g_ai_scratch_buffers[i].size = buffer_size;
            g_ai_scratch_buffers[i].address = memory + buffer_size * i;
        }
        flags[1] = 1;
    }
    for (i = 0; i < 2; i++) {
        if (!g_ai_scratch_buffers[i].in_use) {
            g_ai_scratch_buffers[i].in_use = 1;
            result = g_ai_scratch_buffers[i].address;
            break;
        }
    }
    return result;
}


// @address 0x10f7e8 ?damage_compute_damage_owner_from_object_index@@YAXJPAUs_damage_owner@@@Z
// @address 0x3542a0 ?vehicle_set_auto_turret@@YAXJJMMMJ@Z
namespace {
}
// @address 0x4f9c80 ai_unit_setup_auto_turret
extern "C" void ai_unit_setup_auto_turret(long object_index, long owner_index)
{
    s_object_header *header = datum_get<s_object_header>(object_header_data, object_index);
    if ((1 << header->object_type) & 2) {
        biped_datum *unit = (biped_datum *)header->datum;
        vehicle_definition *definition =
            (vehicle_definition *)tag_get(0, unit->object.definition_index);
        if ((bool)(_rotl(definition->flags, 19) & 1)) {
            unit->unit.team = _game_team_default;
            if (game_globals->options.game_mode == 1) {
                long character_index = ((unit_definition *)definition)->spawned_actor.index;
                if (character_index != -1) {
                    s_damage_owner owner;
                    ::actor_create(object_index, character_index, -1, false);
                    damage_compute_damage_owner_from_object_index(object_index, (s_damage_owner *)&owner);
                    object_set_damage_owner(object_index, (const s_damage_owner *)&owner, true);
                    s_vehicle_auto_control *control
                        = vehicle_get_auto_control((char *)ai_unit_get_and_verify_type(object_index, 1) + 0x458, object_index);
                    if (control != 0) {
                        s_vehicle_auto_control *auto_control = control;
                        damage_compute_damage_owner_from_object_index(owner_index, &auto_control->damage_owner);
                        auto_control->active = 1;
                    }
                }
            } else {
                vehicle_set_auto_turret(object_index, -1, k_real_zero_storage, k_real_zero_storage, k_real_zero_storage, owner_index);
            }
            action_submit_event(object_index, 19);
        }
    }
}

// ---- claim 001: easy four ----

// @address 0x1f0ac0 ?qsort_elements@@YAXPAXIIP6A_NPBX11@Z1@Z


// @address 0xbab88 ?scenario_location_from_point@@YAXPAUs_location@@PBTreal_point3d@@@Z
// @address 0xc8960 ?ai_test_line_of_fire@@YA_NJJPBTreal_point3d@@PBTreal_vector3d@@PAJ@Z
bool ai_test_line_of_fire(long p0, long p1, const real_point3d *p2, const real_vector3d *p3, long *p4)
{
    line_of_fire_pill pills[32];
    bool success = true;
    long value = -1;
    short count = ai_find_line_of_fire_friend_pills(p0, 32, pills);
    short pill_index;
    for (pill_index = 0; pill_index < count; pill_index++)
    {
        bool hit = false;
        if (pills[pill_index].object == p1)
            continue;
        if (pills[pill_index].spherical)
            hit = fast_vector_intersects_sphere(p2, p3, &pills[pill_index].position, pills[pill_index].radius);
        else
            hit = vector_intersects_pill3d(p2, p3, &pills[pill_index].position, &pills[pill_index].axis, pills[pill_index].radius);
        if (hit)
        {
            success = false;
            value = pills[pill_index].owner;
            break;
        }
    }
    if (p4)
        *p4 = value;
    return success;
}

// @address 0xc8a60 ?ai_find_location@@YA_NPBTreal_point3d@@PAUs_location@@@Z
bool ai_find_location(const real_point3d *point, s_location *location)
{
    real_point3d p;
    p.x = point->x;
    p.y = point->y;
    p.z = point->z + 0.1f;
    scenario_location_from_point(location, &p);
    return location->cluster_reference.bsp_index != -1;
}

// @address 0x9c9f8 ?game_team_is_enemy@@YA_NW4e_game_team@@0@Z
// @address 0x515070 ?actor_stimulus_vehicle_eviction@@YAXJJ@Z
// @address 0x4f8e28 ?ai_try_vehicle_eviction@@YA_NJJ_N@Z
bool ai_try_vehicle_eviction(long actor_index, long unit_index, bool evict)
{
    bool result = false;
    actor_datum *actor = (actor_datum *)(actor_data->data + ((unsigned short)actor_index) * sizeof(actor_datum));
    if (unit_index != -1) {
        biped_datum *unit = ai_unit_get_and_verify_type(unit_index, 8195);
        if (unit->unit.player_index != -1 && !game_team_is_enemy(unit->unit.team, (e_game_team)actor->team)) {
            if (actor->root_vehicle_index != -1) {
                object_datum *vehicle = (object_datum *)ai_unit_get_and_verify_type(actor->root_vehicle_index, 1);
                if (((1 << vehicle->object_type) & 2) != 0) {
                    vehicle_definition *definition =
                        (vehicle_definition *)tag_get(0, vehicle->definition_index);
                    if (((_rotl(definition->flags, 23) & 1) == 0)) {
                        result = true;
                        if (evict)
                            actor_stimulus_vehicle_eviction(actor_index, unit->unit.player_index);
                    }
                }
            }
        }
    }
    return result;
}

// @address 0xe9a90 ?character_definition_vitality_properties_get@@YAPAUcharacter_vitality_properties@@J@Z

static __forceinline void u_ai_actor_handle_stun(long actor_index, float stun, const real_vector3d *direction)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    character_vitality_properties *vitality = character_definition_vitality_properties_get(actor->character_index);
    if (vitality != 0 && stun > vitality->stun_threshold) {
        actor->stunned = true;
        actor->stun_scale = stun;
        actor->stun_direction = *direction;
        if (normalize3d(&actor->stun_direction) == 0.0f)
            actor->stun_direction = k_real_math_identity_matrix_storage.forward;
    }
}
// @address 0x4f9fe0 ?ai_handle_stun@@YAXJMPBTreal_vector3d@@@Z
void ai_handle_stun(long unit_index, float stun, const real_vector3d *direction)
{
    if (ai_globals->gate) {
        biped_datum *unit = ai_unit_get_and_verify_type(unit_index, 0x2003);
        if (unit->unit.actor_index != -1)
            u_ai_actor_handle_stun(unit->unit.actor_index, stun, direction);
    }
}
// @address 0xfcf30 ?internal_object_get_markers_by_string_id@@YAFJJPAUobject_marker@@F_N@Z
// @address 0xadf48 ?unit_get_body_position@@YA_NJPATreal_point3d@@@Z
// @address 0x9fcb8 ?object_get_center_of_mass@@YAPATreal_point3d@@JPAT1@@Z
// @address 0xb8070 ?ai_get_center_of_mass@@YAXJPATreal_point3d@@@Z
void ai_get_center_of_mass(long object_index, real_point3d *point)
{
    object_datum *object = datum_get<s_object_header>(object_header_data, object_index)->datum;
    long bits = 1 << object->identifier_type;
    if ((bits & 0x1003) != 0) {
        unit_get_body_position(object_index, point);
        return;
    }
    else if ((bits & 0x800) != 0) {
        object_marker marker;
        internal_object_get_markers_by_string_id(object_index, 177, &marker, 1, false);
        *point = marker.matrix.position;
        return;
    }
    if (object->parent_object_index == -1 && object->havok_component_index != -1) {
        object_get_center_of_mass(object_index, point);
        return;
    }
    *point = object->bounding_center;
}

static __forceinline void u_ai7_actor_iterator_new(actor_iterator *iterator, bool active_only)
{
    if (ai_globals->gate) {
        iterator->m_iterator.absolute_index = -1;
        iterator->m_iterator.index = -1;
        iterator->active_only = active_only;
        iterator->m_iterator.data = actor_data;
    }
}
static __forceinline void u_ai7_squad_iterator_new(clump_actor_cursor *iterator)
{
    if (ai_globals->gate) {
        iterator->iterator.absolute_index = -1;
        iterator->iterator.index = -1;
        iterator->iterator.data = squad_data;
    }
}
static __forceinline void *u_ai7_location_invalidate(void *location)
{
    *(long *)location = -1;
    return location;
}
static __forceinline void u_ai7_squad_actor_iterator_new(squad_actor_iterator *iterator, long squad_index, bool active_only)
{
    if (ai_globals->gate) {
        iterator->squad_index = squad_index;
        iterator->index = -1;
        iterator->active_only = active_only;
        if (squad_index == -1)
            iterator->next_index = ai_globals->first_squadless_actor;
        else
            iterator->next_index = ((squad_datum *)squad_data->data)[(unsigned short)squad_index].first_actor_index;
    }
}

// @address 0x4f7a38 ?ai_dispose_from_old_structure_bsp@@YAXK@Z
void ai_dispose_from_old_structure_bsp(unsigned long mask)
{
    if (ai_globals->gate) {
        flocks_dispose_from_bsp_mask(mask);
        union dispose_iterators iterators;
        u_ai7_actor_iterator_new(&iterators.actors, false);
        actor_datum *actor = actor_iterator_next(&iterators.actors);
        while (actor != 0) {
            if (actor->position.location.cluster_reference.bsp_index != -1
                && (((1 << actor->position.location.cluster_reference.bsp_index) & mask)) != 0) {
                actor_set_active(iterators.actors.index, false);
                u_ai7_location_invalidate(&actor->position.location);
            }
            actor = actor_iterator_next(&iterators.actors);
        }
        u_ai7_squad_iterator_new(&iterators.squads);
        squad_datum *squad = squad_iterator_next(&iterators.squads);
        if (squad != 0) {
            do {
                if ((squad->flags & 128) != 0) {
                    squad_actor_iterator squad_actors;
                    long squad_index = iterators.squads.index;
                    u_ai7_squad_actor_iterator_new(&squad_actors, squad_index, true);
                    if (::squad_actor_iterator_next(&squad_actors) == 0)
                        squad_deactivate(squad_index);
                }
                squad = squad_iterator_next(&iterators.squads);
            } while (squad != 0);
        }
    }
}
// @address 0x4f77d8 ?ai_dispose_from_old_map@@YAXXZ
void ai_dispose_from_old_map()
{
    if (ai_globals->gate) {
        squad_data->valid = false;
        g_ai_squad_group_data->valid = false;
        clump_data->valid = false;
        joint_state_data()->valid = false;
        command_script_data->valid = false;
        dynamic_firing_set_data->valid = false;
        prop_data->valid = false;
        prop_ref_data->valid = false;
        tracking_data->valid = false;
        actor_data->valid = false;
        swarms_dispose_from_old_map();
        g_ai_dialogue_records->valid = false;
        flocks_dispose_from_old_map();
        ai_globals->gate = false;
    }
}

/* ---------------------------------------------------------------- team status (0x4f9908) */

// @address 0x9ca90 ?game_team_is_ally@@YA_NW4e_game_team@@0@Z
// @address 0xb3c10 ?actor_compute_prop_salience@@YAXJJW4e_prop_stimulus@@@Z

static __forceinline void u_ai10_actor_iterator_new(actor_iterator *iterator, bool active_only)
{
    if (ai_globals->gate) {
        iterator->m_iterator.absolute_index = -1;
        iterator->m_iterator.index = -1;
        iterator->active_only = active_only;
        iterator->m_iterator.data = actor_data;
    }
}
static __forceinline void u_ai10_actor_prop_ref_iterator_new(actor_prop_ref_iterator *iterator, long actor_index)
{
    iterator->index = -1;
    iterator->next_index = datum_get<actor_datum>(actor_data, actor_index)->first_prop_ref_index;
}
static __forceinline prop_ref_datum *u_ai10_actor_prop_ref_iterator_next(actor_prop_ref_iterator *iterator)
{
    prop_ref_datum *result = 0;
    if (iterator->next_index != -1) {
        iterator->index = iterator->next_index;
        result = datum_get<prop_ref_datum>(prop_ref_data, iterator->next_index);
        iterator->next_index = result->next_prop_ref_index;
    }
    return result;
}

// @address 0x4f9908 ?ai_update_team_status@@YAXXZ
void ai_update_team_status()
{
    actor_iterator iterator;
    actor_datum *actor;
    u_ai10_actor_iterator_new(&iterator, true);
    while ((actor = actor_iterator_next(&iterator)) != 0) {
        actor_prop_ref_iterator prop_iterator;
        prop_ref_datum *prop_ref;
        u_ai10_actor_prop_ref_iterator_new(&prop_iterator, iterator.index);
        while ((prop_ref = u_ai10_actor_prop_ref_iterator_next(&prop_iterator)) != 0) {
            biped_datum *object = (biped_datum *)ai_unit_get_and_verify_type(prop_ref->object_index, -1);
            if ((1 << object->object.object_type) & 0x1003) {
                biped_datum *unit = object;
                prop_datum *prop = datum_get<prop_datum>(prop_data, prop_ref->prop_index);
                prop->team = (short)unit->unit.team;
                prop->valid = game_team_is_enemy((e_game_team)actor->team, (e_game_team)prop->team);
                prop->allegiance_changed = game_team_is_ally((e_game_team)actor->team, (e_game_team)prop->team);
                actor_compute_prop_salience(iterator.index, prop_iterator.index, _prop_stimulus_none);
            }
        }
    }
}

/* clumps (clump.obj): clump_data tls +0xa0, 0x108 each */
static __forceinline void u_ai11_clump_iterator_new(clump_actor_cursor *iterator)
{
    if (ai_globals->gate)
        { iterator->iterator.absolute_index = -1; iterator->iterator.index = -1; iterator->iterator.data = clump_data; }
}
static __forceinline clump_datum *u_ai11_clump_iterator_next(clump_actor_cursor *iterator)
{
    clump_datum *result = 0;
    if (ai_globals->gate) {
        iterator->datum = (clump_datum *)data_iterator_next(&iterator->iterator);
        if (iterator->datum)
            result = (clump_datum *)iterator->datum;
        iterator->index = iterator->iterator.index;
    }
    return result;
}
static __forceinline void u_ai11_clump_actor_iterator_new(clump_actor_iterator *iterator, long clump_index)
{
    iterator->next_actor_index = datum_get<clump_datum>(clump_data, clump_index)->first_actor_index;
}
static __forceinline actor_datum *u_ai11_clump_actor_iterator_next(clump_actor_iterator *iterator)
{
    actor_datum *actor = 0;
    if (iterator->next_actor_index != -1) {
        actor = datum_get<actor_datum>(actor_data, iterator->next_actor_index);
        iterator->actor_index = iterator->next_actor_index;
        iterator->next_actor_index = actor->next_clump_actor_index;
    }
    return actor;
}


// @address 0x4f86a8 ?ai_find_line_of_fire_friend_pills@@YAFJFPAUline_of_fire_pill@@@Z
short ai_find_line_of_fire_friend_pills(long object_index, short maximum_count, line_of_fire_pill *shapes)
{
    short count = 0;
    actor_datum *actor = u_ai_actor_get(object_index);
    if (actor->clump_index != -1) {
        clump_actor_iterator actor_iterator;
        actor_datum *member;
        u_ai11_clump_actor_iterator_new(&actor_iterator, actor->clump_index);
        while ((member = (actor_datum *)u_ai11_clump_actor_iterator_next(&actor_iterator)) != 0) {
            if (actor_iterator.actor_index == object_index)
                continue;
            if (count >= 32)
                continue;
            long unit_index = member->unit_index;
            if (unit_index == -1)
                continue;
            if (member->root_vehicle_index != -1)
                continue;
            s_object_header *header = datum_get<s_object_header>(object_header_data, unit_index);
            if (header->object_type != 0)
                continue;
            ai_generate_line_of_fire_pill(unit_index, -1, &shapes[count]);
            count++;
        }
    }
    {
        actor_prop_ref_iterator prop_iterator;
        prop_ref_datum *prop_ref;
        u_ai10_actor_prop_ref_iterator_new(&prop_iterator, object_index);
        while ((prop_ref = u_ai10_actor_prop_ref_iterator_next(&prop_iterator)) != 0) {
            prop_state *state;
            if (prop_ref->owner_index != -1)
                state = &datum_get<tracking_datum>(tracking_data, prop_ref->owner_index)->state;
            else
                state = &datum_get<prop_datum>(prop_data, prop_ref->prop_index)->state;
            prop_datum *prop = datum_get<prop_datum>(prop_data, prop_ref->prop_index);
            if (prop->valid)
                continue;
            if (state->b64)
                continue;
            if (prop_ref->knowledge_state != 1)
                continue;
            if (state->index44 != -1)
                continue;
            long target_object = prop_ref->object_index;
            s_object_header *target_header = datum_get<s_object_header>(object_header_data, target_object);
            biped_datum *target = (biped_datum *)target_header->datum;
            if (((1 << target->object.object_type) & 1) == 0)
                continue;
            if (count >= 32)
                continue;
            ai_generate_line_of_fire_pill(target_object, prop_iterator.index, &shapes[count]);
            count++;
        }
    }
    if (game_globals->options.game_mode == 1) {
        long team = actor->team;
        bool check_friendly = true;
        if (team == 1)
            check_friendly = false;
        else if (team != -1 && team <= 15u) {
            long bit = (team << 4) + 1;
            check_friendly = (g_game_allegiance_globals->enemy_bits[bit >> 5] & (1 << (bit & 31))) == 0;
        }
        if (check_friendly)
            return count;
    }
    {
        c_data_iterator<player_datum> player_iterator;
        player_datum *player;
        player_iterator.begin(player_data);
        while (player_iterator.next()) {
            player = player_iterator.get_datum();
            if (game_team_is_enemy((e_game_team)actor->team, (e_game_team)player->configuration.host.team_index))
                continue;
            if (player->unit_index == -1)
                continue;
            s_object_header *unit_header = datum_get<s_object_header>(object_header_data, player->unit_index);
            if (unit_header->object_type != 0)
                continue;
            biped_datum *unit = (biped_datum *)unit_header->datum;
            if (unit->object.parent_object_index != -1
                && ai_get_root_vehicle(unit->object.parent_object_index) == actor->root_vehicle_index)
                continue;
            long prop = prop_ref_get_by_object_index(object_index, player->unit_index, false, _clump_importance_none);
            ai_generate_line_of_fire_pill(player->unit_index, prop, &shapes[count]);
            count++;
        }
    }
    return count;
}
/* props: tls +0x90, 196 each */

static __forceinline void u_ai11_clump_prop_iterator_new(actor_prop_ref_iterator *iterator, long clump_index)
{
    iterator->next_index = datum_get<clump_datum>(clump_data, clump_index)->first_prop_index;
}
static __forceinline prop_datum *u_ai11_clump_prop_iterator_next(actor_prop_ref_iterator *iterator)
{
    prop_datum *prop = 0;
    if (iterator->next_index != -1) {
        prop = datum_get<prop_datum>(prop_data, iterator->next_index);
        iterator->index = iterator->next_index;
        iterator->next_index = prop->next_prop;
    }
    return prop;
}
static __forceinline void u_ai11_clump_prop_ref_iterator_new(actor_prop_ref_iterator *iterator, long prop_index)
{
    iterator->next_index = datum_get<prop_datum>(prop_data, prop_index)->first_prop_ref_index;
}
static __forceinline prop_ref_datum *u_ai11_clump_prop_ref_iterator_next(actor_prop_ref_iterator *iterator)
{
    prop_ref_datum *ref = 0;
    if (iterator->next_index != -1) {
        ref = datum_get<prop_ref_datum>(prop_ref_data, iterator->next_index);
        iterator->index = iterator->next_index;
        iterator->next_index = ref->prop_next_ref_index;
    }
    return ref;
}



// @address 0x4f96c8 ?ai_handle_allegiance_status_changed@@YAXW4e_game_team@@0_N11@Z
void ai_handle_allegiance_status_changed(e_game_team team_a, e_game_team team_b, bool broken, bool permanently, bool update)
{
    if (ai_globals->gate) {
        if (update)
            game_allegiance_update_allied_teams(team_a, team_b, broken, permanently);
        clump_actor_cursor iterator;
        clump_datum *clump;
        u_ai11_clump_iterator_new(&iterator);
        while ((clump = u_ai11_clump_iterator_next(&iterator)) != 0) {
            short other_team = -1;
            if (clump->team == team_a)
                other_team = (short)team_b;
            else if (clump->team == team_b)
                other_team = (short)team_a;
            if (other_team != -1) {
                unsigned short clump_index = (unsigned short)iterator.index;
                clump_actor_iterator actor_iterator;
                actor_datum *actor;
                u_ai11_clump_actor_iterator_new(&actor_iterator, clump_index);
                while ((actor = u_ai11_clump_actor_iterator_next(&actor_iterator)) != 0) {
                    if (actor->attached_vehicle_index != -1)
                        ai_vehicle_handle_allegiance_change(actor_iterator.actor_index);
                }
                actor_prop_ref_iterator prop_iterator;
                prop_datum *prop;
                u_ai11_clump_prop_iterator_new(&prop_iterator, clump_index);
                while ((prop = u_ai11_clump_prop_iterator_next(&prop_iterator)) != 0) {
                    if (prop->team == other_team) {
                        if (!permanently)
                            prop->allegiance_changed = true;
                        prop->valid = broken;
                        prop->flags |= 64;
                        if (!broken && !permanently) {
                            c_dialogue_event event(94, prop->object_index, -1, -1, -1, 0);
                            ai_dialogue_event(&event);
                        }
                        if (prop->valid) {
                            actor_prop_ref_iterator ref_iterator;
                            prop_ref_datum *ref;
                            u_ai11_clump_prop_ref_iterator_new(&ref_iterator, prop_iterator.index);
                            while ((ref = u_ai11_clump_prop_ref_iterator_next(&ref_iterator)) != 0)
                                actor_compute_prop_salience(ref->actor_index, ref_iterator.index, _prop_stimulus_none);
                        }
                    }
                }
            }
        }
    }
}

// @address 0x7c3774 g_ai_dialogue_records_allocator
// @address 0x7c3768 g_team_dialogue_state_layout
// @address 0x7c3798 g_swarm_nondeterministic_counter_layout
// @address 0x7c3780 g_swarm_data_allocator
// @address 0x7c378c g_swarm_spawner_allocator
// @address 0x7c5440 g_ai_clumps_allocator
// @address 0x7c544c g_ai_joint_state_allocator
// @address 0x7c5468 g_ai_flocks_allocator
// @address 0x897e20 g_behavior_posture_variant_counts
// @address 0x23238 k_posture_variant_indices



// @address 0x4f78a0 ?ai_initialize_for_new_structure_bsp@@YAXK@Z
void ai_initialize_for_new_structure_bsp(unsigned long mask)
{
    if (ai_globals->gate) {
        actor_iterator iterator;
        u_ai7_actor_iterator_new(&iterator, false);
        actor_datum *actor = actor_iterator_next(&iterator);
        while (actor != 0) {
            if (!actor->active && (unsigned char)actor->position.location.cluster_reference.bsp_index == 255) {
                long actor_index = iterator.index;
                if (actor_switch_bsp(actor_index) && actor->squad_index != -1 && (datum_get<squad_datum>(squad_data,
                    actor->squad_index)->flags & 128) != 0)
                    actor_set_active(actor_index, true);
            }
            actor = actor_iterator_next(&iterator);
        }
        ai_refresh_reference_frames();
        struct scenario *scenario = global_scenario;
        for (short index = 0; index < scenario->structure_bsps.count; ++index) {
        }
        for (short index = 0; index < scenario->flocks.count; ++index) {
            const s_flock_instance *flock = &((s_flock_instance *)scenario->flocks.address)[index];
            short bsp_index = flock->bsp_index;
            if (bsp_index >= 0 && bsp_index < scenario->structure_bsps.count && ((1 << bsp_index) & mask) != 0 && (flock->flags & 1) == 0)
                flock_new_from_scenario(index);
        }
    }
}

// @address 0x4f71d8 ?ai_initialize@@YAXXZ
void ai_initialize()
{
    ai_globals = (ai_globals_type *)g_ai_globals_layout_allocation.reserve_memory("ai globals", 0, 0x680, 0);
    memset(ai_globals, 0, sizeof(*ai_globals));
    tls_ai_players_set((ai_player_state *)g_ai_players_allocator.reserve_memory("ai player state globals", 0, 0xb0, 0));
    ai_players_reset();
    actors_initialize();
    g_swarm_data = data_new("swarm", 0x20, 0x34, 0, (c_allocation_base *)&g_swarm_data_allocator);
    g_swarm_spawners =
        (s_swarm_spawner *)((t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
            &__tls_set_g_swarm_spawners_allocator> *)&g_swarm_spawner_allocator)
            ->reserve_memory("swarm_spawner", 0, 600, 0);
    g_swarm_nondeterministic_counter = (unsigned short *)((t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0),
        static_cast<void (*)(void *)>(0), &__tls_set_g_spawner_globals_allocator> *)&g_swarm_nondeterministic_counter_layout)
        ->reserve_memory("spawner_globals", 0, 2, 0);
    prop_data = data_new("prop", 0x100, 0xc4, 0, (c_allocation_base *)&g_ai_props_allocator);
    prop_ref_data = data_new("prop_ref", 0x400, 0x3c, 0, (c_allocation_base *)&g_ai_prop_ref_allocator);
    tracking_data = data_new("tracking", 100, 0x100, 0, (c_allocation_base *)&g_ai_prop_view_allocator);
    for (long index = 0; index < 7; ++index)
        style_all.bits[index] = -1;
    squad_data = data_new("squad", 0x14f, 0xb0, 0, (c_allocation_base *)&g_ai_squads_allocator);
    g_ai_squad_group_data = data_new("squad group", 0x82, 0x24, 0, (c_allocation_base *)&g_ai_squad_group_allocator);
    g_objectives_data = data_new("objectives", 100, 10, 0, (c_allocation_base *)&g_ai_objectives_allocator);
    g_tasks_records = (s_task_record *)g_ai_tasks_layout_allocation.reserve_memory("task records", 0, 0x445c0, 0);
    clump_data = data_new("clump", 0x14, 0x108, 0, (c_allocation_base *)&g_ai_clumps_allocator);
    joint_state_data() = data_new("joint state", 0x14, 0xcc, 0, (c_allocation_base *)&g_ai_joint_state_allocator);
    dynamic_firing_set_data = data_new("dynamic firing points", 0xf, 0x584, 0,
        (c_allocation_base *)&g_ai_dynamic_firing_set_allocator);
    command_script_data = data_new("command scripts", 0x28, 0x188, 0, (c_allocation_base *)&g_ai_command_script_allocator);
    for (short outer = 0; outer < 35; outer = (short)(outer + 1)) {
        long *row = k_posture_variant_indices[outer];
        short *count = &g_behavior_posture_variant_counts[outer];
        *count = 0;
        short inner = 0;
        do {
            if (row[inner] == 0)
                break;
            inner = (short)(inner + 1);
            *count = (short)(*count + 1);
        } while (inner < 10);
    }
    g_team_dialogue_state = (team_dialogue_state *)((t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0),
        static_cast<void (*)(void *)>(0), &__tls_set_g_team_dialogue_state_allocator> *)&g_team_dialogue_state_layout)
        ->reserve_memory("vocalization timers", 0, 0xfb8, 0);
    g_ai_dialogue_records = data_new("vocalization records", 0xf, 0x5c, 0, (c_allocation_base *)&g_ai_dialogue_records_allocator);
    ai_initialize_avoidance_rays();
    behaviors_initialize();
    g_flocks_data = data_new("flocks", 10, 76, 0, (c_allocation_base *)&g_ai_flocks_allocator);
}

// @address 0x4f7608 ?ai_initialize_for_new_map@@YAXXZ
void ai_initialize_for_new_map()
{
    ai_globals_type *r31_globals;
    char *r9_base;
    short spawner_index;
    short dialogue_team_index;
    short reset_team_index;
    ai_globals_initialize_for_new_map();
    data_make_valid(actor_data);
    memset(g_ai_reference_frame_data, 0, 150 * sizeof(*g_ai_reference_frame_data));
    hash_table_reset(g_actor_firing_position_hash);
    data_make_valid(g_swarm_data);
    spawner_index = 0;
    r9_base = (char *)g_swarm_spawners;
    do {
        s_swarm_spawner *entry = &((s_swarm_spawner *)r9_base)[spawner_index];
        spawner_index = (short)(spawner_index + 1);
        entry->creature_count = 0;
        entry->starting_location_index = -1;
    } while (spawner_index < 5);
    *(unsigned short *)g_swarm_nondeterministic_counter = 0;
    data_make_valid(prop_data);
    data_make_valid(prop_ref_data);
    data_make_valid(tracking_data);
    squads_initialize_for_new_map();
    objectives_initialize_for_new_map();
    data_make_valid(clump_data);
    data_make_valid(joint_state_data());
    data_make_valid(dynamic_firing_set_data);
    data_make_valid(command_script_data);
    // match: 0x4f7608: storing through global_script_context by name instead of this pointer changes
    // hs_runtime_update, task_exhaust and task_update (36/106/261 bytes)
    ai_script_context *script_context = &global_script_context;
    script_context->ai_index = 0;
    script_context->object_index = 0;
    dialogue_team_index = 0;
    r9_base = (char *)g_team_dialogue_state;
    do {
        memset(&g_team_dialogue_state[dialogue_team_index], 0, sizeof(*g_team_dialogue_state));
        dialogue_team_index = (short)(dialogue_team_index + 1);
    } while (dialogue_team_index < 2);
    data_make_valid(g_ai_dialogue_records);
    reset_team_index = 0;
    r9_base = (char *)g_team_dialogue_state;
    do {
        team_dialogue_state *record = &((team_dialogue_state *)r9_base)[reset_team_index];
        reset_team_index = (short)(reset_team_index + 1);
        record->first = -1;
        record->count = 0;
        record->obsessed_object = -1;
    } while (reset_team_index < 2);
    data_make_valid(g_flocks_data);
    r31_globals = ai_globals;
    r31_globals->sound_tail = 0;
    r31_globals->sound_head = 0;
    memset(r31_globals->sound_slots, 0, sizeof(r31_globals->sound_slots));
    r31_globals->gate = 1;
}

/* ---------------------------------------------------------------- squad task scan (0x4f9a68) */

static __forceinline s_equipment_definition *ai_9a68_unit_definition_get(long object_index)
{
    biped_datum *unit = ai_unit_get_and_verify_type(object_index, 1);
    return (s_equipment_definition *)tag_get(0, unit->object.definition_index);
}
// @address 0x4f9a68 ai_break_squad_tasks_around_object
extern "C" void ai_break_squad_tasks_around_object(long object_index)
{
    squad_datum *squad = 0;
    if (ai_globals->gate) {
        biped_datum *unit = ai_unit_get_and_verify_type(object_index, 1);
        s_damage_owner owner = unit->object.damage_owner;
        signed char team = owner.team;
        s_equipment_definition *definition = ai_9a68_unit_definition_get(object_index);
        const s_equipment_orbit_definition *orbit =
            (const s_equipment_orbit_definition *)definition->equipment_types[4].address;
        struct { real_point3d point; float w; } center;
        object_get_center_of_mass(object_index, &center.point);
        actor_iterator iterator;
        u_ai10_actor_iterator_new(&iterator, false);
        actor_datum *actor = actor_iterator_next(&iterator);
        if (actor != 0) {
            float center_z = center.point.z;
            float center_y = center.point.y;
            float center_x = center.point.x;
            do {
                if (actor->squad_index != -1) {
                    squad = datum_get<squad_datum>(squad_data, actor->squad_index);
                    if (::squad_valid_task((squad_datum *)squad)) {
                        bool invalid = false;
                        if (::squad_valid_task((squad_datum *)squad)) {
                            unsigned short objective_index = (unsigned short)squad->objective_index;
                            char *objective_ptr = (char *)g_objectives_data->data + objective_index * 10;
                            unsigned short first_task = ((s_objective_record *)objective_ptr)->first_task;
                            short task_index = (short)(first_task + (unsigned short)squad->task_index);
                            s_task_record *task = (s_task_record *)((char *)g_tasks_records + task_index * 56);
                            invalid = (_rotl(task->flags.get_unsafe(), 30) & 1) != 0;
                        }
                        if (!invalid && game_team_is_enemy((e_game_team)actor->team, (e_game_team)team)) {
                            float dx = actor->position.body.x - center_x;
                            float dz = actor->position.body.z - center_z;
                            float dy = actor->position.body.y - center_y;
                            if (dy * dy + (dz * dz + dx * dx) < orbit->radius) {
                                squad_datum *task_squad = datum_get<squad_datum>(squad_data, actor->squad_index);
                                objective_task_break(task_squad->objective_index, task_squad->task_index);
                            }
                        }
                    }
                }
                actor = actor_iterator_next(&iterator);
            } while (actor != 0);
        }
    }
}


static __forceinline effect_datum *ai_effect_get(long index)
{
    return datum_get<effect_datum>(effect_data, index);
}
static __forceinline event_datum *ai_effect_event_get(long index)
{
    return datum_get<event_datum>(g_event_data, index);
}
static __forceinline void ai_effect_message_initialize(s_effect_message *message, long effect_index, long event_index,
    unsigned char limit_type)
{
    const effect_datum *effect = ai_effect_get(effect_index);
    const event_datum *event = ai_effect_event_get(event_index);
    message->effect_index = effect_index;
    message->definition_index = effect->definition_index;
    message->event_index = event_index;
    message->event_block_index = event->event_block_index;
    message->limit_type = limit_type;
    message->event_counter = event->event_counter;
    message->location_index = 0;
}
// @address 0x236d90 ai_handle_effect_creation
extern "C" void ai_handle_effect_creation(long effect_index, long event_index, const effect_part_definition *part,
    const real_matrix4x3 *matrix)
{
    const effect_datum *effect = ai_effect_get(effect_index);
    const event_datum *event = ai_effect_event_get(event_index);
    if (!event->flags.test(_event_stale_bit)) {
        real_vector3d velocity;
        real_vector3d direction;
        effect_random_translational_velocity("event-request-contrail-system", false, effect, &matrix->forward, &direction, &velocity,
            part->minimum_velocity, part->maximum_velocity, part->velocity_angle, part->scale_a_mask, part->scale_b_mask);
        s_effect_message message;
        ai_effect_message_initialize(&message, effect_index, event_index, part->limit_type);
        message.type = 4;
        memcpy(&message.light_volume.matrix, matrix, sizeof(real_matrix4x3));
        message.light_volume.velocity = velocity;
        message.light_volume.random_seed = (unsigned long)(effect_real_random_range("event-request-contrail-system", false,
            effect, 0.0f, 1.0f) * 4294967296.0f);
        message.location_index = 0;
        effect_send_render_message(&message);
    }
}


static __forceinline bool ai_damage_mission_critical(long unit_index)
{
    bool result = false;
    for (short index = 0; index < 8; ++index) {
        if (ai_globals->mission_critical[index].unit_index == unit_index) {
            result = true;
            break;
        }
    }
    return result;
}
// @address 0xf4f78 ?ai_adjust_damage@@YA_NJJPAUs_damage_data@@PAM@Z
bool ai_adjust_damage(long damaging_unit_index, long damaged_unit_index, s_damage_data *damage_data, float *damage)
{
    actor_datum *actor = 0;
    bool adjusted = false;
    bool has_player_passenger = false;
    bool player_damage = false;
    if (damaging_unit_index != -1) {
        const s_model_damage_info *model = object_get_damage_info(damaged_unit_index);
        biped_datum *damaging_unit = (biped_datum *)object_try_and_get_and_verify_type(damaging_unit_index, 0x1003);
        if (damaging_unit) {
            if (damaging_unit->unit.actor_index != -1) {
                actor = datum_get<actor_datum>(actor_data, damaging_unit->unit.actor_index);
                if (actor->root_vehicle_index != -1) {
                    c_multi_unit_passenger_iterator iterator;
                    iterator.begin(actor->root_vehicle_index);
                    while (iterator.next()) {
                        biped_datum *passenger = ai_unit_get_and_verify_type(iterator.passenger_index, 0x1003);
                        if (passenger->unit.player_index != -1) {
                            has_player_passenger = true;
                            break;
                        }
                    }
                }
            } else if (damaging_unit->unit.player_index != -1) {
                player_damage = true;
            }
        }
        if (model && (model->flags & 0x800) && !has_player_passenger && !player_damage) {
            *damage = k_real_zero_storage;
            adjusted = true;
        } else if (actor && (damage_data->flags & 8)) {
            do {
                s_object_header *header = datum_get<s_object_header>(object_header_data, damaged_unit_index);
                if (((1 << header->object_type) & 0x1003) != 0) {
                    biped_datum *damaged_unit = (biped_datum *)header->datum;
                    if (game_globals->options.game_mode == 1 && damaged_unit->unit.player_index != -1 &&
                        ai_damage_mission_critical(actor->unit_index) &&
                        ::game_team_is_traitor((e_game_team)actor->team, _game_team_player)) {
                        damage_data->flags |= 4;
                        adjusted = true;
                        break;
                    }
                    if (damaged_unit->unit.actor_index != -1 && global_game_globals) {
                        const s_game_globals *globals = global_game_globals;
                        if (globals->ai_globals.count() > 0) {
                            const s_ai_globals_definition *settings = (const s_ai_globals_definition *)globals->ai_globals.address;
                            if (ai_globals) {
                                float scale;
                                if (actor->root_vehicle_index == -1)
                                    scale = settings->infantry_scale;
                                else if (has_player_passenger)
                                    scale = settings->occupied_vehicle_scale;
                                else
                                    scale = settings->vehicle_scale;
                                if (!(scale < k_real_zero_storage) && !(scale <= k_real_zero_storage)) {
                                    *damage *= scale;
                                    adjusted = true;
                                }
                            }
                        }
                    }
                }
                if (actor->damage_scale <= k_real_zero_storage)
                    break;
                *damage = actor->damage_scale * *damage;
                adjusted = true;
            } while (false);
        }
    }
    return adjusted;
}

static __forceinline squad_datum *ai_inactive_squad_next(clump_actor_cursor *iterator, const ai_globals_type *globals, long &squad_index)
{
    squad_datum *squad = 0;
    if (globals->gate) {
        iterator->datum = (squad_datum *)data_iterator_next(&iterator->iterator);
        if (iterator->datum)
            squad = (squad_datum *)iterator->datum;
        iterator->index = squad_index = (unsigned short)iterator->iterator.index;
    }
    return squad;
}
static __forceinline void ai_inactive_add(s_ai_releasable_storage *storage, bool actor, const long &index, const long &release_index)
{
    storage->entries[storage->count].is_actor = actor;
    storage->entries[storage->count].index = index;
    storage->entries[storage->count].release_index = release_index;
    ++storage->count;
}
// @address 0x4f8308 ?ai_find_inactive_squads@@YAXJPAEJ@Z
void ai_find_inactive_squads(long unused, unsigned char *working_memory, long working_memory_size)
{
    s_ai_releasable_storage *storage = (s_ai_releasable_storage *)working_memory;
    storage->count = 0;
    storage->released_count = 0;
    union {
        squad_actor_iterator actor;
        clump_actor_cursor squad;
    } iterator;
    const ai_globals_type *globals = ai_globals;
    if (globals->gate) {
        long next_index = globals->first_squadless_actor;
        iterator.actor.squad_index = -1;
        iterator.actor.index = -1;
        iterator.actor.next_index = next_index;
        iterator.actor.active_only = false;
    }
    actor_datum *actor = ::squad_actor_iterator_next(&iterator.actor);
    while (actor && (unsigned short)storage->count < 256) {
        if (!actor->active && actor->deactivation_ticks != -1) {
            long actor_index = iterator.actor.index;
            ai_inactive_add(storage, true, actor_index, actor->deactivation_ticks);
        }
        actor = ::squad_actor_iterator_next(&iterator.actor);
    }
    if (globals->gate) {
        iterator.squad.iterator.absolute_index = -1;
        iterator.squad.iterator.index = -1;
        iterator.squad.iterator.data = squad_data;
    }
    squad_datum *squad;
    long squad_index;
    while ((squad = ai_inactive_squad_next(&iterator.squad, globals, squad_index)) != 0 && (unsigned short)storage->count < 256) {
        if (!(squad->flags & 128) && squad->count0 > 0 && squad->last_activation_time != -1)
            ai_inactive_add(storage, false, squad_index, squad->last_activation_time);
    }
    if (storage->count > 0)
        qsort_elements(storage->entries, storage->count, sizeof(s_ai_releasable_entry),
            (bool (*)(const void *, const void *, const void *))compare_releasable_inactive_squads, 0);
}


static __forceinline actor_datum *ai_death_actor_get(long index)
{
    return datum_get<actor_datum>(actor_data, index);
}
static __forceinline unit_dialogue_vocalization_record *ai_death_record_get(long index)
{
    return datum_get<unit_dialogue_vocalization_record>(g_ai_dialogue_records, index);
}

static __forceinline void ai_death_set_state(long actor_index)
{
    actor_datum *actor = ai_death_actor_get(actor_index);
    actor->combat_status = 3;
    actor->combat_status_time = 0;
}
static __forceinline bool ai_death_is_mission_critical(long unit_index, actor_datum *actor, biped_datum *unit)
{
    bool result = false;
    if (ai_damage_mission_critical(unit_index)) {
        result = true;
    } else {
        clump_actor_cursor iterator;
        u_ai_clump_iterator_new(&iterator);
        clump_datum *clump;
        while ((clump = (clump_datum *)clump_actor_cursor_next(&iterator)) != 0) {
            if (unit->unit.team == clump->team || !game_team_is_enemy(unit->unit.team, (e_game_team)clump->team)) {
                float radius = clump->radius + 30.0f;
                if (u_ai_distance_squared(&clump->position, &actor->position.body) < radius * radius) {
                    result = true;
                    break;
                }
            }
        }
    }
    return result;
}
static __forceinline void ai_death_notify_clump(actor_datum *actor, long responsible)
{
    if (actor->clump_index != -1) {
        clump_datum *clump = datum_get<clump_datum>(clump_data, actor->clump_index);
        clump_actor_iterator iterator;
        clump_actor_iterator_new(&iterator, clump);
        actor_datum *member;
        while ((member = clump_actor_iterator_next(&iterator)) != 0) {
            if (member != actor && member->combat_status < 7) {
                unsigned long random = random_seed_next(TLS_RANDOM_SEED);
                float random_real = (float)(random >> 16) * 1.5259022e-05f;
                float delay = random_real * 2.0f + 3.0f;
                if (member->attention_level <= 2) {
                    short &ticks = member->attention_ticks;
                    ticks = (short)(long)(delay * (float)game_time_globals->ticks_per_second + 0.5f);
                    member->attention_direction.type = 7;
                    member->attention_direction.object_index = responsible;
                    member->attention_level = 2;
                }
                if (member->combat_status < 3 && member->command_index == -1)
                    ai_death_set_state(iterator.actor_index);
            }
        }
    }
}
static __forceinline void ai_death_remove_dialogue(long unit_index)
{
    biped_datum *unit = ai_unit_get_and_verify_type(unit_index, -1);
    team_dialogue_state *team = team_dialogue_state_get((short)unit->unit.team);
    long index = team->first;
    while (index != -1) {
        unit_dialogue_vocalization_record *record = ai_death_record_get(index);
        long next = record->next;
        if (record->unit_index == unit_index && queue_remove_record(team, index))
            record_delete(index);
        index = next;
    }
}
static __forceinline void ai_death_obsess(long unit_index)
{
    unsigned char enabled = game_globals->game_lost;
    team_dialogue_state *teams = (team_dialogue_state *)g_team_dialogue_state;
    for (short team_index = 0; team_index < 2; ++team_index) {
        team_dialogue_state *team = &teams[team_index];
        long index = team->first;
        while (index != -1) {
            unit_dialogue_vocalization_record *record = ai_death_record_get(index);
            index = record->next;
            record->marked_for_deletion = true;
        }
        multi_deletion_remove_marked(team);
        if (enabled)
            team->obsessed_object = unit_index;
    }
}
// @address 0x4f8fc8 ?ai_handle_death@@YAXJJFM@Z
void ai_handle_death(long unit_index, long damage_object, short damage_type, float damage)
{
    biped_datum *unit = (biped_datum *)object_get(unit_index);
    long actor_index = unit->unit.actor_index;
    if (actor_index == -1 && unit->unit.player_index == -1)
        return;
    long responsible = ai_get_unit_responsible_for_damage(damage_object, damage_type != 9);
    bool suppress_dialogue = false;
    if (actor_index != -1 && responsible != -1) {
        actor_datum *actor = ai_death_actor_get(actor_index);
        biped_datum *killer = (biped_datum *)object_get(responsible);
        if (damage_type == 6 && game_team_is_enemy(unit->unit.team, killer->unit.team) && actor->combat_status < 6)
            suppress_dialogue = true;
        if (killer->unit.actor_index != -1) {
            actor_datum *killer_actor = ai_death_actor_get(killer->unit.actor_index);
            if (killer_actor->clump_index != -1) {
                short type = ai_death_actor_get(unit->unit.actor_index)->type;
                clump_datum *clump = datum_get<clump_datum>(clump_data, killer_actor->clump_index);
                if (type == 16)
                    clump->death_flags |= 1;
                else if ((1 << type) & 0x780e00)
                    clump->death_flags |= 2;
            }
        }
        e_game_team team = unit->unit.team;
        if (team != killer->unit.team && game_team_is_ally(team, killer->unit.team) &&
            !((1 << actor->type) & 0x1782e00)) {
            unsigned char incident = false;
            unsigned char report = true;
            if (actor->clump_index != -1) {
                clump_datum *clump = datum_get<clump_datum>(clump_data, actor->clump_index);
                long close = clump->combat_state < 3 ? 1 : 0;
                incident = (unsigned char)close;
            }
            switch (damage_type) {
            case 4: case 9:
                if (!incident) report = false;
                break;
            case 3:
                if (!incident) report = false;
                incident = false;
                break;
            }
            if (report) {
                bool critical = ai_death_is_mission_critical(unit_index, actor, unit);
                if (critical) {
                    game_allegiance_incident(killer->unit.team, unit->unit.team, incident != 0);
                }
                if (critical)
                    ai_death_notify_clump(actor, responsible);
            }
        }
    }
    ai_death_remove_dialogue(unit_index);
    if (unit->unit.player_index != -1)
        ai_death_obsess(unit_index);
    if (!suppress_dialogue) {
        c_dialogue_event event(1, unit_index, responsible, -1, damage_type, 0);
        ai_dialogue_event(&event);
        if (unit->unit.actor_index != -1) {
            ai_information_data information = { 0 };
            long prop = prop_ref_get_by_object_index(unit->unit.actor_index, responsible, false, _clump_importance_none);
            if (prop != -1) {
                prop_ref_datum *reference = datum_get<prop_ref_datum>(prop_ref_data, prop);
                if ((long)(reference->flags_1e & 15) == 4)
                    prop = -1;
                else if (damage_type >= 3) {
                    if (damage_type <= 4)
                        prop = -1;
                    else if (damage_type == 9)
                        prop = -1;
                }
            }
            information.values[0] = 5;
            information.values[1] = prop;
            ai_sound_data sound;
            sound.type = 0;
            sound.volume = 1;
            sound.information_data = information;
            handle_sound(unit->unit.actor_index, unit_index, 0, true, &sound);
        }
    }
}
static __forceinline unit_datum *ai_effect_unit_get(long index)
{
    return (unit_datum *)datum_get<s_object_header>(object_header_data, index)->datum;
}

// @address 0x4fa238 ?ai_handle_unit_effect@@YAXJW4e_sound_type@@W4e_ai_sound_volume@@@Z
void ai_handle_unit_effect(long unit_index, e_sound_type effect_type, e_ai_sound_volume volume)
{
    if (ai_globals->gate && unit_index != -1 && volume > 0) {
        long time = game_time_globals->time;
        unit_datum *unit = ai_effect_unit_get(unit_index);
        if (unit->unit.sound_type >= 1 && time <= unit->unit.sound_time + (long)((float)game_time_globals->ticks_per_second + 0.5f))
            return;
        ai_sound_data sound;
        sound.volume = (short)volume;
        sound.type = 1;
        for (long i = 0; i < 6; ++i)
            sound.information_data.values[i] = 0;
        unit->unit.sound_type = 1;
        unit->unit.sound_time = time;
        if (unit->object.object_type == 1) {
            long child_index = unit->object.first_child_object_index;
            while (child_index != -1) {
                unit_datum *child = ai_effect_unit_get(child_index);
                if (child->object.object_type == 0)
                    handle_sound(child->unit.actor_index, child_index, 2, true, &sound);
                child_index = child->object.next_object_index;
            }
        } else if (unit->object.object_type == 0) {
            handle_sound(unit->unit.actor_index, unit_index, 2, true, &sound);
        }
    }
}


static __forceinline object_datum *ai_projectile_get(long projectile_index)
{
    return datum_get<s_object_header>(object_header_data, projectile_index)->datum;
}

static __forceinline projectile_definition *ai_projectile_definition_get(long projectile_index)
{
    object_datum *projectile = ai_projectile_get(projectile_index);
    return (projectile_definition *)tag_get(0, projectile->definition_index);
}

static __forceinline void ai_actor_handle_projectile_attach(long actor_index, long projectile_index)
{
    projectile_definition *definition = ai_projectile_definition_get(projectile_index);
    if (definition->ai_radius > 0.0f) {
        prop_perceive(actor_index, projectile_index, 5, _prop_stimulus_bump, 1, -1, -1, -1);
        actor_dialogue_event(actor_index, 87, projectile_index, 0);
    }
    if (definition->targeting_flags_upper & 1)
        stimulus_register(actor_index, _behavior_stimulus_stuck_with_grenade, 30, 3);
}

// @address 0x4fa0d8 ?ai_handle_projectile_attach@@YAXJJ@Z
void ai_handle_projectile_attach(long parent_index, long child_index)
{
    if (ai_globals->gate) {
        projectile_definition *definition = ai_projectile_definition_get(child_index);
        biped_datum *unit = ai_unit_get_and_verify_type(parent_index, 0x2003);
        if (unit->unit.actor_index != -1)
            ai_actor_handle_projectile_attach(unit->unit.actor_index, child_index);
        if (definition->ai_radius > 0.0f) {
            s_damage_owner owner = ai_projectile_get(child_index)->damage_owner;
            long owner_index = object_try_and_get_and_verify_type(owner.object_index, -1) ? owner.object_index : -1;
            c_dialogue_event event(15, parent_index, owner_index, -1, -1, 0);
            ai_dialogue_event(&event);
        }
    }
}

static __forceinline bool ai_scenario_squad_test_flag(const squad_definition *squad,
    long bit) { return (squad->flags & (1UL << bit)) != 0; }

static __forceinline squad_datum *u_ai7_squad_iterator_next(clump_actor_cursor *iterator)
{
    squad_datum *result = 0;
    if (ai_globals->gate) {
        squad_datum *squad = (squad_datum *)data_iterator_next(&iterator->iterator);
        iterator->datum = squad;
        if (squad)
            result = squad;
        iterator->index = (unsigned short)iterator->iterator.index;
    }
    return result;
}

// @address 0x54a620 ai_create_initial_squads
extern "C" void ai_create_initial_squads()
{
    clump_actor_cursor iterator;
    u_ai7_squad_iterator_new(&iterator);
    while (u_ai7_squad_iterator_next(&iterator)) {
        long squad_index = iterator.index;
        struct scenario *scenario = global_scenario;
        if (ai_scenario_squad_test_flag(&((squad_definition *)scenario->squads.address)[datum_index_to_absolute_index(squad_index)], 4))
            squad_create(squad_index, -1, -1, -1, false, false, false);
    }
}
// match: ai.cpp owns C++ linkage; the shared collision constants header uses C linkage.


static __forceinline bool ai_los_bsp_active(long index)
{
 return (unsigned long)index <= 15 && (g_active_structure_bsp_mask & (1UL << index)) != 0;
}
// @address 0xb01f0 ?ai_test_line_of_sight@@YAFPBTreal_point3d@@Us_cluster_reference@@01F_NJJ22PAJPA_N@Z
short ai_test_line_of_sight(const real_point3d *start, s_cluster_reference start_cluster, const real_point3d *end,
    s_cluster_reference end_cluster, short mode, bool transparent, long ignore_a, long ignore_b, bool skip_objects, bool alternate,
    long *hit_object, bool *transparent_hit)
{
 bool passed_transparent = false;
 bool extra_clear = false;
 long object_index = -1;
 if (hit_object) *hit_object = -1;
 short result;
 bool clear;
 float fraction;
 if (start_cluster.bsp_index != -1 && end_cluster.bsp_index != -1) {
  if (!ai_los_bsp_active(start_cluster.bsp_index) || !ai_los_bsp_active(end_cluster.bsp_index) || !scenario_test_pvs(start_cluster,end_cluster)) {
   result = 4;
   extra_clear = true;
   clear = false;
   fraction = 0.0f;
  }
 }
 if (!extra_clear) {
  collision_result collision;
  s_collision_test_flags flags = alternate ? k_ai_alternate_collision_flags : k_ai_line_of_sight_default_flags;
  if (skip_objects) flags.object_flags &= ~4u;
  if (!collision_test_line(flags, start, end, ignore_a, ignore_b, &collision)) clear = true;
  else { fraction = collision.fraction; object_index = collision.object_index; clear = false; }
  if (transparent) passed_transparent = !clear && (collision.flags & 1);
  else {
   flags.collision_flags |= 0x2000;
   if (!clear && (collision.flags & 1)) {
    bool hit = collision_test_line(flags, start, end, ignore_a, ignore_b, &collision);
    if (!hit) { clear=true; passed_transparent=true; }
    else { fraction=collision.fraction; object_index=collision.object_index; }
   }
  }
  if (mode) {
   real_vector3d side;
   ai_set_vector3d(&side, start->y-end->y, end->x-start->x, 0.0f);
   if (normalize3d(&side) == 0.0f) side = *global_forward3d;
   flags.collision_flags |= 0x10000;
   if ((mode==1 && clear) || (mode==2 && !clear)) {
    real_point3d left, right;
    u_ai_point_from_line(start, &side, 0.25f, &left);
    u_ai_point_from_line(start, &side, -0.25f, &right);
    if (clear) {
     extra_clear = collision_test_line(flags,&left,end,ignore_a,ignore_b,&collision);
     if (!extra_clear) extra_clear = collision_test_line(flags,&right,end,ignore_a,ignore_b,&collision);
     if (extra_clear) { object_index=collision.object_index; fraction=collision.fraction; }
    } else {
     extra_clear = !collision_test_line(flags,&left,end,ignore_a,ignore_b,&collision);
     if (!extra_clear) extra_clear = !collision_test_line(flags,&right,end,ignore_a,ignore_b,&collision);
    }
   } else if (mode==3 && clear) {
    real_point3d left, right, down;
    u_ai_point_from_line(end, &side, 0.1f, &left);
    u_ai_point_from_line(end, &side, -0.1f, &right);
    u_ai_point_from_line(end, global_down3d, 0.1f, &down);
    extra_clear=collision_test_line(flags,&left,start,ignore_a,ignore_b,&collision);
    if (!extra_clear) {
     extra_clear=collision_test_line(flags,&right,start,ignore_a,ignore_b,&collision);
     if (!extra_clear) extra_clear=collision_test_line(flags,&down,start,ignore_a,ignore_b,&collision);
    }
    if (extra_clear) { object_index=collision.object_index; fraction=collision.fraction; }
   }
  }
  if (extra_clear) result=1;
  else if (clear) result=0;
  else {
   float length=u_ai_distance(start,end);
   if (length>=1.0f && length*fraction<1.0f) result=2;
   else if (length>=1.0f && !alternate && (1.0f-fraction)*length<4.0f) result=3;
   else result=4;
  }
 }
 if (hit_object && !alternate && result>=1) {
  if (u_ai_distance(start,end)*(1.0f-fraction)<4.0f) *hit_object=object_index;
 }
 if (transparent_hit) *transparent_hit=passed_transparent;
 return result;
}
// @address 0x4f8a38 ai_test_short_line_of_sight
extern "C" bool ai_test_short_line_of_sight(long actor_index, long ignore_object_index, const real_point3d *start, const real_point3d *end)
{
    bool result = false;
    real_vector3d direction;
    direction.i = end->x - start->x;
    direction.j = end->y - start->y;
    direction.k = end->z - start->z;
    float distance = normalize3d(&direction);
    if (distance > 0.25f) {
        float test_distance = distance - 0.25f;
        actor_datum *actor = u_ai_actor_get(actor_index);
        long parent_index = -1;
        if (test_distance > 3.0f)
            test_distance = 3.0f;
        real_point3d test_point;
        test_point.x = test_distance * direction.i + start->x;
        test_point.y = test_distance * direction.j + start->y;
        test_point.z = test_distance * direction.k + start->z;
        s_cluster_reference start_cluster = ai_scenario_find_cluster_index(start, global_up3d);
        s_cluster_reference test_cluster = ai_scenario_find_cluster_index(&test_point, global_up3d);
        if (actor->unit_index != -1)
            parent_index = u_ai_root_object(actor->unit_index);
        result = ai_test_line_of_sight(start, start_cluster, &test_point, test_cluster, 0, true, ignore_object_index,
            parent_index, false, false, 0, 0) == 0;
    }
    return result;
}


// @rdata 0x820221fc
static const float k_ai_sense_ray_weight = 1.0f;
// @rdata 0x820226cc
static const float k_ai_avoidance_ray_weight = 0.7f;

// @rdata 0x820226d0..0x82022770
static const float k_ai_sense_distances[9] = { 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f };
static const float k_ai_sense_elevations[9] = { 0.0f, 0.3f, 0.3f, 0.3f, 0.3f, 1.0f, 1.0f, 1.0f, 1.0f };
static const float k_ai_sense_angles[10] = { 0.0f, 0.0f, 1.57079637f, 3.14159274f, 4.71238899f, 0.0f, 1.57079637f,
    3.14159274f, 4.71238899f, 0.7f };
static const float k_ai_avoidance_angles[8] = { 0.0f, 0.785398185f, 1.57079637f, 2.3561945f, 3.14159274f, 3.926991f,
    4.71238899f, 5.49778748f };
static const float k_ai_avoidance_distances[4] = { 0.7f, 1.0f, 3.402823466e+38f, 1.57079637f };
// @rdata-end

// @address 0x7c5464 sense_elevation_scale
float sense_elevation_scale = 0.052359879f;
// @address 0x7c5f84 avoidance_elevations
float avoidance_elevations[2] = { 0.52359879f, 0.959931076f };
// @address 0x8a1b68 avoidance_rays
vector_avoidance_ray avoidance_rays[8][2];
// @address 0x8a1d28 avoidance_directions
real_vector3d avoidance_directions[8];
// @address 0x8a1d88 sense_rays
vector_avoidance_ray sense_rays[9];

namespace {
static __forceinline float ai_avoidance_sine(float angle)
{
    return (float)sin((double)angle);
}

static __forceinline float ai_avoidance_cosine(float angle)
{
    return (float)cos((double)angle);
}

}

// @address 0x55ac80 ai_initialize_avoidance_rays
extern "C" void ai_initialize_avoidance_rays(void)
{
    for (short i = 0; i < 9; i++)
    {
        float angle_sine = ai_avoidance_sine(k_ai_sense_angles[i]);
        float angle_cosine = ai_avoidance_cosine(k_ai_sense_angles[i]);
        float elevation_sine = ai_avoidance_sine(k_ai_sense_elevations[i] * sense_elevation_scale);
        float elevation_cosine = ai_avoidance_cosine(k_ai_sense_elevations[i] * sense_elevation_scale);
        sense_rays[i].weight = k_ai_sense_ray_weight;
        sense_rays[i].offset.i = 0.0f;
        sense_rays[i].offset.j = k_ai_avoidance_ray_weight * k_ai_sense_distances[i] * angle_cosine;
        sense_rays[i].offset.k = k_ai_avoidance_ray_weight * k_ai_sense_distances[i] * angle_sine;
        sense_rays[i].direction.i = elevation_cosine;
        sense_rays[i].direction.j = elevation_sine * angle_cosine;
        sense_rays[i].direction.k = elevation_sine * angle_sine;
    }

    for (short i = 0; i < 2; i++)
    {
        float elevation_sine = ai_avoidance_sine(avoidance_elevations[i]);
        float elevation_cosine = ai_avoidance_cosine(avoidance_elevations[i]);
        for (short j = 0; j < 8; j++)
        {
            float angle_sine = ai_avoidance_sine(k_ai_avoidance_angles[j]);
            float angle_cosine = ai_avoidance_cosine(k_ai_avoidance_angles[j]);
            avoidance_directions[j].i = 0.0f;
            avoidance_directions[j].j = angle_cosine;
            avoidance_directions[j].k = angle_sine;
            avoidance_rays[j][i].weight = k_ai_avoidance_ray_weight;
            scale_vector3d(&avoidance_directions[j], k_ai_avoidance_distances[i], &avoidance_rays[j][i].offset);
            scale_vector3d(&avoidance_directions[j], elevation_sine, &avoidance_rays[j][i].direction);
            avoidance_rays[j][i].direction.i = elevation_cosine;
        }
    }
}

// sound ring slot (28 bytes): pending-AI-sound view of ai_sound_data-sized entries at ai_globals+64


static __forceinline void ai_sound_discard_head(short tail,
    unsigned short *head) { long index = (short)*head; if (tail == index) *head = (index + 1) & 31; }
static __forceinline real_vector3d *ai_sound_vector_from_points(const real_point3d *a, const real_point3d *b, real_vector3d *result)
{ result->i = b->x - a->x; result->j = b->y - a->y; result->k = b->z - a->z; return result; }
static __forceinline float ai_sound_square(const real_vector3d *value)
{
    float xx = value->i * value->i;
    return xx + value->k * value->k + value->j * value->j;
}

// @address 0xfac38 ai_sound_new_at_position
extern "C" void ai_sound_new_at_position(long object_index, const real_point3d *position, short sound_type, short volume)
{
    long time = game_time_globals->time;
    if (volume <= 0)
        return;
    ai_globals_type *ring = ai_globals;
    unsigned char notify = true;
    short reusable_index = -1;
    s_ai_sound_slot *slot = 0;
    unsigned short raw_index = ring->sound_head;
    long current_index = (short)raw_index;
    float tick_rate = (float)game_time_globals->ticks_per_second;
    long notify_cutoff = time - (long)(tick_rate + 0.5f);
    float rounded_ticks = tick_rate;
    rounded_ticks += 0.5f;
    long average_cutoff = time - (long)rounded_ticks;
    long expire_cutoff = average_cutoff - (long)(tick_rate * 3.0f + 0.5f);
    if (current_index != (short)ring->sound_tail)
    {
        do {
            s_ai_sound_slot *entry = &ring->sound_slots[current_index];
            bool matches = false;
            if (sound_type == ring->sound_slots[current_index].type)
            {
                real_vector3d displacement;
                if (ai_sound_square(ai_sound_vector_from_points(&entry->position, position, &displacement)) < 1.0f)
                    matches = true;
            }
            if (ring->sound_slots[current_index].time <= expire_cutoff)
            {
                ring->sound_slots[current_index].type = -1;
                if (current_index == (short)ring->sound_head)
                    ring->sound_head = (current_index + 1) & 31;
                else
                    reusable_index = (short)raw_index;
            }
            else if (matches)
            {
                slot = &ring->sound_slots[(short)raw_index];
                short count = slot->count + 1;
                slot->count = count;
                long emit = notify;
                if (slot->time >= notify_cutoff) emit = 0;
                notify = (unsigned char)emit;
                if (slot->time < average_cutoff)
                    slot->position = *position;
                else
                {
                    float previous_x = slot->position.x;
                    float previous_y = slot->position.y;
                    float previous_z = slot->position.z;
                    float contribution = 1.0f / (float)count;
                    const float &previous_contribution = 1.0f - contribution;
                    slot->position.x = position->x * contribution + previous_x * previous_contribution;
                    slot->position.y = position->y * contribution + previous_y * previous_contribution;
                    slot->position.z = position->z * contribution + previous_z * previous_contribution;

                }
                break;
            }
            raw_index = (current_index + 1) & 31;
            current_index = (short)raw_index;
        } while ((short)raw_index != (short)ring->sound_tail);
    }
    if (!slot)
    {
        short new_index;
        if (reusable_index == -1)
        {
            new_index = (short)ring->sound_tail;
            short tail = (new_index + 1) & 31;
            ring->sound_tail = tail;
            ai_sound_discard_head(tail, &ring->sound_head);
        }
        else
            new_index = reusable_index;
        slot = &ring->sound_slots[new_index];
        slot->type = sound_type;
        slot->volume = volume;
        slot->count = notify;
        slot->position = *position;
        slot->time = time;
    }
    if ((unsigned char)notify)
        handle_sound(-1, object_index, 2, true, (ai_sound_data *)slot);
}


static __forceinline bool u_ai_game_is_predicted()
{
    unsigned char simulation = game_globals->options.simulation_type;
    bool result = false;
    if (simulation == 4)
        result = true;
    return result;
}


static __forceinline void u_ai_joint_iterator_new(clump_actor_cursor *iterator)
{
    iterator->iterator.absolute_index = -1;
    iterator->iterator.index = -1;
    iterator->iterator.data = joint_state_data();
}
static __forceinline void *u_ai_joint_iterator_next(clump_actor_cursor *iterator)
{
    void *result = 0;
    iterator->datum = data_iterator_next(&iterator->iterator);
    if (iterator->datum != 0)
        result = iterator->datum;
    iterator->index = iterator->iterator.index;
    return result;
}

static __forceinline void u_ai_freeze_iterator_new(actor_iterator *iterator)
{
    if (ai_globals->gate) {
        iterator->active_only = true;
        iterator->m_iterator.absolute_index = -1;
        iterator->m_iterator.index = -1;
        iterator->m_iterator.data = actor_data;
    }
}

// @address 0xa8a60 ?ai_update@@YAXXZ
void ai_update(void)
{
    u_ai_update_iterators storage;
    bool update = ai_globals->gate && !u_ai_game_is_predicted();
    if (update) {
        if (ai_globals->active) {
            ai_player_update();
            ai_globals_update();
            squads_update();
            objectives_update();
            clumps_update();
            u_ai_joint_iterator_new(&storage.joint);
            while (u_ai_joint_iterator_next(&storage.joint) != 0)
                joint_state_update(storage.joint.index);
            swarm_spawners_update();
            actors_update();
            ai_dialogue_queue_update();
            flocks_update();
            ai_globals->initialized = true;
        } else if (ai_globals->initialized) {
            u_ai_freeze_iterator_new(&storage.freeze);
            if (actor_iterator_next(&storage.freeze) != 0) {
                do {
                    actor_freeze(storage.freeze.index);
                } while (actor_iterator_next(&storage.freeze) != 0);
            }
            ai_globals->initialized = false;
        }
    } else {
        bool predicted = u_ai_game_is_predicted();
        if (predicted)
            flocks_update();
    }
}

static __forceinline real_vector3d *ai_pill_cross_product3d(const real_vector3d *a, const real_vector3d *b, real_vector3d *result)
{
    float k = a->i * b->j - a->j * b->i;
    float j = a->k * b->i - a->i * b->k;
    result->i = a->j * b->k - a->k * b->j;
    result->j = j;
    result->k = k;
    return result;
}


// @address 0xc8ad8 ?vector_intersects_pill3d@@YA_NPBTreal_point3d@@PBTreal_vector3d@@01M@Z
inline bool vector_intersects_pill3d(
    const real_point3d *point0, const real_vector3d *vector0, const real_point3d *point1, const real_vector3d *vector1, float radius)
{
    real_vector3d difference;
    real_vector3d cross;
    real_point3d closest0, closest1;
    vector_from_points3d(point0, point1, &difference);
    ai_pill_cross_product3d(vector0, vector1, &cross);
    float cross_squared = magnitude_squared3d(&cross);
    float t0, t1;
    if (!(fabs(cross_squared) < 0.0001f))
    {
        scale_vector3d(&cross, 1.0f / cross_squared, &cross);
        t0 = triple_product3d(&difference, vector1, &cross);
        t1 = triple_product3d(&difference, vector0, &cross);
        bool outside0 = t0 < 0.0f || t0 > 1.0f;
        bool outside1 = t1 < 0.0f || t1 > 1.0f;
        if (outside0 || outside1)
        {
            if (outside0)
                point_from_line3d(point0, vector0, t0 < 0.0f ? 0.0f : 1.0f, &closest0);
            if (outside1)
                point_from_line3d(point1, vector1, t1 < 0.0f ? 0.0f : 1.0f, &closest1);
            return (outside0 && fast_vector_intersects_sphere(point1, vector1, &closest0, radius))
                || (outside1 && fast_vector_intersects_sphere(point0, vector0, &closest1, radius));
        }
    }
    else
    {
        float dot = dot_product3d(vector0, vector1);
        float length_squared = magnitude_squared3d(vector0);
        if (length_squared > 0.0001f)
        {
            float inverse = 1.0f / length_squared;
            float start = dot_product3d(&difference, vector0) * inverse;
            float end = inverse * dot + start;
            t0 = (PIN(start, 0.0f, 1.0f) + PIN(end, 0.0f, 1.0f)) * 0.5f;
        }
        else
            t0 = 0.0f;
        length_squared = magnitude_squared3d(vector1);
        if (length_squared > 0.0001f)
        {
            float inverse = 1.0f / length_squared;
            float start = -(dot_product3d(&difference, vector1) * inverse);
            float end = inverse * dot + start;
            t1 = (PIN(start, 0.0f, 1.0f) + PIN(end, 0.0f, 1.0f)) * 0.5f;
        }
        else
            t1 = 0.0f;
    }
    point_from_line3d(point0, vector0, t0, &closest0);
    point_from_line3d(point1, vector1, t1, &closest1);
    return distance_squared3d(&closest0, &closest1) <= radius * radius;
}


static __forceinline bool ai_ballistic_collision_test_line(s_collision_test_flags flags, const real_point3d *point0,
    const real_point3d *point1, long first_ignore_object_index, long second_ignore_object_index, collision_result *collision)
{
    real_vector3d vector;
    vector_from_points3d(point0, point1, &vector);
    return collision_test_vector(flags, false, point0, &vector, first_ignore_object_index, second_ignore_object_index, -1, collision);
}

// @address 0x4f8bd8 ?ai_test_ballistic_line_of_fire@@YA_NJPBTreal_point3d@@MPBTreal_vector3d@@MJ_N@Z
bool ai_test_ballistic_line_of_fire(long actor_index, const real_point3d *origin, float speed, const real_vector3d *velocity, float gravity,
    long target_index, bool in_vehicle)
{
    bool clear = true;
    line_of_fire_pill pills[32];
    short pill_count = ai_find_line_of_fire_friend_pills(actor_index, 32, pills);
    s_collision_test_flags flags = k_ai_alternate_collision_flags;
    if (in_vehicle)
        flags.object_flags &= ~4u;
    speed *= 0.9f;
    float start_time = 0.0f;
    real_point3d current_point = *origin;
    real_point3d next_point;
    real_vector3d current_velocity = *velocity;
    float end_time = (speed < 0.2f) ? speed : 0.2f;
    do
    {
        point_from_line3d(&current_point, &current_velocity, end_time - start_time, &next_point);
        next_point.z += ((end_time - start_time) * (end_time - start_time)) * gravity * 0.5f;
        collision_result collision;
        clear = !ai_ballistic_collision_test_line(flags, &current_point, &next_point, target_index, -1, &collision);
        if (clear)
        {
            real_vector3d segment;
            vector_from_points3d(&current_point, &next_point, &segment);
            for (short pill_index = 0; pill_index < pill_count; pill_index++)
            {
                if (vector_intersects_pill3d(&current_point, &segment, &pills[pill_index].position, &pills[pill_index].axis,
                    pills[pill_index].radius))
                {
                    clear = false;
                    break;
                }
            }
            if (clear)
            {
                current_velocity.k += gravity * (end_time - start_time);
                start_time = end_time;
                current_point = next_point;
                end_time += 0.2f;
                end_time = (end_time > speed) ? speed : end_time;
            }
        }
    } while (clear && start_time < speed);
    return clear;
}

// ---- 0x111030 ai_handle_deleted_object ----


// @address 0x111030 ?ai_handle_deleted_object@@YAXJ@Z
void ai_handle_deleted_object(long deleted_object_index)
{
    ai_globals_type *globals = ai_globals;
    if (!globals->gate)
        return;
    vehicle_datum *unit = (vehicle_datum *)ai_unit_get_and_verify_type(deleted_object_index, -1);
    short object_type = unit->object.object_type;
    for (short i = 0; i < 8; ++i) {
        if (ai_globals->mission_critical[i].unit_index == deleted_object_index) {
            ai_globals->mission_critical[i].unit_index = -1;
            ai_globals->mission_critical[i].object_index = -1;
            ai_globals->mission_critical[i].time = 0.0f;
            ai_globals->mission_critical[i].bring_forward_ticks = 0;
            ai_globals->mission_critical[i].flags = 0;
            break;
        }
    }
    long type_mask = 1 << object_type;
    if (((type_mask & 0x1803) != 0) || ((unit->object.ai_flags & 2) != 0)) {
        clump_handle_deleted_object(deleted_object_index);
        if ((type_mask & 0x1003) != 0) {
            vehicle_datum *unit_object = (vehicle_datum *)ai_unit_get_and_verify_type(deleted_object_index, 0x2003);
            if (unit_object->unit.simulation_actor_datum_index != -1)
                simulation_action_actor_died(deleted_object_index);
            if (unit_object->unit.actor_index != -1)
                actor_delete(unit_object->unit.actor_index, false);
            if (unit_object->object.object_type == 1) {
                long vehicle_index = unit_object->vehicle.squad_index;
                struct scenario *scenario = global_scenario;
                if (((vehicle_index & 0xffff) >= 0) && ((vehicle_index & 0xffff) < scenario->squads.count))
                    squad_remove_vehicle(vehicle_index, deleted_object_index);
            }
            u_ai_data_iterator<tracking_datum> prop_iterator;
            prop_iterator.begin(tracking_data);
            while (prop_iterator.next()) {
                tracking_datum *prop = prop_iterator.get_datum();
                if (prop->state.object_index == deleted_object_index) {
                    prop->state.object_index = -1;
                    prop->state.b6b = 0;
                    prop->state.b6a = 0;
                }
            }
            ai_globals_type *deletion = ai_globals;
            if (deletion->deleted_object_count > 0) {
                short i = 0;
                do {
                    if (deletion->deleted_object_indices[i] == deleted_object_index) {
                        deletion->deleted_object_count--;
                        if (deletion->deleted_object_count > 0)
                            deletion->deleted_object_indices[i] = deletion->deleted_object_indices[deletion->deleted_object_count];
                    }
                    i++;
                } while (i < deletion->deleted_object_count);
            }
            clump_actor_cursor squad_iterator;
            ai_globals_type *loop_gate = (ai_globals_type *)deletion;
            if (ai_globals->gate) {
                squad_iterator.iterator.absolute_index = -1;
                squad_iterator.iterator.index = -1;
                squad_iterator.iterator.data = squad_data;
            }
            squad_datum *squad = 0;
            do {
                squad = 0;
                if (!loop_gate->gate)
                    break;
                squad_iterator.datum = data_iterator_next(&squad_iterator.iterator);
                if (squad_iterator.datum != 0)
                    squad = (squad_datum *)squad_iterator.datum;
                squad_iterator.index = (unsigned short)squad_iterator.iterator.index;
                if (squad == 0)
                    break;
                if (squad->follow_object_index != deleted_object_index)
                    continue;
                squad->follow_object_index = -1;
            } while (squad != 0);
        } else if (object_type == 11) {
            ai_handle_creature_delete(deleted_object_index);
        }
        ai_dialogue_queue_handle_object_deleted(deleted_object_index);
    }
    if ((type_mask & 0x14c3) != 0) {
        if (object_can_have_reference_frames(deleted_object_index)) {
            s_attachment_state *sets = (s_attachment_state *)g_ai_reference_frame_data;
            for (short i = 0; i < 150; ++i) {
                if (sets[i].object_index == deleted_object_index) {
                    sets[i].object_index = -1;
                    sets[i].flags = 0;
                }
            }
        }
        clump_actor_cursor track_iterator;
        track_iterator.iterator.absolute_index = -1;
        track_iterator.iterator.index = -1;
        track_iterator.iterator.data = (s_data_array *)dynamic_firing_set_data;
        dynamic_firing_set_datum *track = 0;
        do {
            track = 0;
            track_iterator.datum = data_iterator_next(&track_iterator.iterator);
            if (track_iterator.datum != 0)
                track = (dynamic_firing_set_datum *)track_iterator.datum;
            track_iterator.index = track_iterator.iterator.index;
            if (track == 0)
                break;
            if (track->object_index != deleted_object_index)
                continue;
            track->object_index = -1;
        } while (true);
    }
    if (object_type != 5 || ((unit->object.ai_flags & 2) != 0)) {
        actor_iterator actor_it;
        if (ai_globals->gate) {
            actor_it.m_iterator.absolute_index = -1;
            actor_it.m_iterator.index = -1;
            actor_it.active_only = false;
            actor_it.m_iterator.data = actor_data;
        }
        if (actor_iterator_next(&actor_it) != 0) {
            do {
                actor_handle_deleted_object(actor_it.index, deleted_object_index);
            } while (actor_iterator_next(&actor_it) != 0);
        }
        swarm_spawners_handle_deleted_object(deleted_object_index);
    }
    {
        ai_player_state *states = tls_ai_players();
        for (short i = 0; i < 4; ++i) {
            if (states[i].unit_index == deleted_object_index) {
                states[i].unit_index = -1;
                states[i].squad_index = -1;
                states[i].flags = 0;
            }
        }
    }
}

namespace {
static __forceinline long ai_globals_object_get_ultimate_parent(long object_index)
{
    long ultimate;
    do {
        ultimate = object_index;
        object_index = ai_unit_get_and_verify_type(object_index, -1)->object.parent_object_index;
    } while (object_index != -1);
    return ultimate;
}
static __forceinline biped_datum *ai_globals_unit_get(unsigned long unit_index)
{
    return (biped_datum *)datum_get<s_object_header>(object_header_data, unit_index)->datum;
}
static __forceinline void ai_globals_unit_get_head_position(long unit_index, real_point3d *position)
{
    object_marker marker;
    internal_object_get_markers_by_string_id(unit_index, 157, &marker, 1, false);
    *position = marker.matrix.position;
}
}
// @address 0x4f7cb0 ?ai_globals_update@@YAXXZ
void ai_globals_update()
{
    object_marker marker;
    if (ai_globals->dialogue_suspend_level > 0)
        ai_globals->dialogue_suspend_level--;
    for (short index = 0; index < 8; index++) {
        long unit_index = ai_globals->mission_critical[index].unit_index;
        if (unit_index == -1)
            continue;
        bool forced = false;
        long character_definition_index = ai_globals->mission_critical[index].source_index;
        biped_datum *unit = ai_unit_get_and_verify_type(unit_index, 1);
        s_object_header *header = datum_get<s_object_header>(object_header_data, ai_globals_object_get_ultimate_parent(unit_index));
        if (!TEST_BIT(header->flags, 0)) {
            bool deactivated = true;
            if (unit->unit.actor_index != -1 && u_ai_actor_get(unit->unit.actor_index)->deactivation_ticks == -1)
                deactivated = false;
            if (deactivated) {
                forced = true;
                SET_BIT(ai_globals->mission_critical[index].flags, 0, true);
                SET_BIT(ai_globals->mission_critical[index].flags, 1, true);
                ai_globals->mission_critical[index].bring_forward_ticks = -1;
                ai_globals->mission_critical[index].bring_forward_value = 0.0f;
            }
        }
        if ((bool)((unit->object.damage_flags >> 2) & 1)) {
            bool resurrect = false;
            character_vitality_properties *vitality = character_definition_vitality_properties_get(character_definition_index);
            if (TEST_BIT(ai_globals->mission_critical[index].flags, 0))
                resurrect = true;
            else if (vitality && TEST_BIT(vitality->flags, 0) && ai_can_resurrect_unit(unit_index))
                resurrect = true;
            if (resurrect) {
                short resurrect_type = 2;
                if (internal_object_get_markers_by_string_id(unit_index, 177, &marker, 1, false) > 0) {
                    real_vector3d up = { 0.0f, 0.0f, 1.0f };
                    if (u_ai_dot(&marker.matrix.forward, &up) > 0.0f)
                        resurrect_type = 1;
                }
                if (actor_resurrect(unit_index, (short)unit->unit.team, resurrect_type, unit->biped.squad_index) != -1)
                    SET_BIT(ai_globals->mission_critical[index].flags, 0, false);
            }
        } else {
            SET_BIT(ai_globals->mission_critical[index].flags, 0, false);
        }
        if (TEST_BIT(ai_globals->mission_critical[index].flags, 1) && unit->unit.actor_index != -1) {
            bool bring_forward = true;
            if (!forced) {
                float minimum_distance = ai_globals->mission_critical[index].bring_forward_value;
                c_data_iterator<player_datum> iterator;
                iterator.begin(player_data);
                while (iterator.next()) {
                    player_datum *player = iterator.get_datum();
                    if (player->unit_index != -1) {
                        biped_datum *player_unit =
                            (biped_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(player->unit_index)].datum;
                        real_point3d head_position;
                        ai_globals_unit_get_head_position(player->unit_index, &head_position);
                        real_vector3d direction;
                        u_ai_vector_from_points(&head_position, &unit->object.bounding_center, &direction);
                        float distance = normalize3d(&direction);
                        if (distance < minimum_distance) {
                            SET_BIT(ai_globals->mission_critical[index].flags, 1, false);
                            ai_globals->mission_critical[index].bring_forward_ticks = 0;
                            bring_forward = false;
                            break;
                        }
                        if (u_ai_dot(&player_unit->unit.desired_aiming_vector,
                            &direction) > (float)cos((float)atan2(unit->object.bounding_radius, distance) + 0.7853982f)) {
                            bring_forward = false;
                            break;
                        }
                    }
                }
            }
            if (bring_forward && actor_bring_forward_to_firing_position(unit->unit.actor_index)) {
                SET_BIT(ai_globals->mission_critical[index].flags, 1, false);
                ai_globals->mission_critical[index].bring_forward_ticks = 0;
            }
            if (ai_globals->mission_critical[index].bring_forward_ticks > 0) {
                ai_globals->mission_critical[index].bring_forward_ticks--;
                if (ai_globals->mission_critical[index].bring_forward_ticks == 0)
                    SET_BIT(ai_globals->mission_critical[index].flags, 1, false);
            }
        }
    }
}




static inline s_collision_test_flags collisions_test_flags_new(unsigned long collision_flags, unsigned long object_flags)
{
    s_collision_test_flags result = { collision_flags, object_flags };
    return result;
}

// @address 0x7089b0 ??__Ek_ai_alternate_collision_flags@@YAXXZ
s_collision_test_flags k_ai_alternate_collision_flags = collisions_test_flags_new(
    k_ai_line_of_sight_default_flags.collision_flags | 0x10000,
    k_ai_line_of_sight_default_flags.object_flags);
#if defined(_XBOX) && !defined(__clang__)
s_ai_debug_throttled_global g_ai_debug_throttled_global = { 5, &tls_actor_globals()->throttled };
#else
s_ai_debug_throttled_global g_ai_debug_throttled_global{ &tls_actor_globals()->throttled };
#endif

t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_ai_globals_allocator> g_ai_globals_layout_allocation;
ai_player_allocation_manager g_ai_players_allocator;

