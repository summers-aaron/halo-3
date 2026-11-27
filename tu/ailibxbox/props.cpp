// ailibxbox/props.obj: from the records 0xaf7d0 0xaf9f0 0xafa48 0xafaf0 0xb0f40 0xb15b0 0xb2c90 0xb3958 0xb47c8 0xbf508 0xc00c0 0xc3c40 0xc7fd0 0xc8040 0xcc240 0xd1a18 0xd23e0 0xd2f90 0x120020 0x120098 0x232ca0 0x24b9c8 0x24eb00 0x24eba0 0x514268 0x51bfb0 0x552d48 0x552d78 0x552e50 0x552fe0 0x5530f0 0x553698 0x553738 0x5537b8 0x5538a0 0x553a90 0x553c58 0x553fa8 0x554048 0x554068 0x554108 0x554128 0x5541c8 0x55c938 0x55ca40
// @address 0x11df668 prop_ref_data
// @address 0x11df690 prop_data
// @address 0x11df768 tracking_data
#include "hs/hs_thread_deterministic_data.h"
#include "ai/style_vector.h"
// @flags /TP /O2 /Oc
#include "ai/clump_actor_cursor_types.h"
#include "ai/character_behavior_properties.h"
#include "creatures/creature_datum.h"
#include "units/biped_types.h"
#include "game/game_time_types.h"
#include "ai/actor_globals.h"
#include "ai/actor_queries.h"
#include "effects/contrails.h"
#include "ai/cl_engine.h"
#include "cseries/cseries.h"
#include "memory/tls_data_arrays.h"
#include "math/real_math.h"
#include "math/real_math_constants.h"
#include "multithreading/thread_tls.h"
#include "objects/object_reference_types.h"
#include "memory/data_types.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "objects/object_placement.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_datum.h"
#include "ai/prop_state.h"
#include "objects/object_datum.h"
#include "game/game_types.h"
#include "ai/actors.h"
#include "cache/restricted_memory_regions.h"
#include "objects/objects.h"
#include "units/units.h"
#include "units/unit_types.h"
#include "units/vehicle_definitions.h"
#include "physics/collision_types.h"
#include "ai/actor_perception.h"
#include "ai/actor_stimulus.h"
#include "ai/ai_player.h"
#include "ai/ai_vehicles.h"
#include "ai/behavior.h"
#include "ai/clump.h"
#include "ai/props.h"
#include "ai/flock_runtime_types.h"
#include "creatures/creature_definitions.h"
#include "physics/water_physics.h"
#include "ai/character_definitions.h"
#include "ai/pathfinding_utilities.h"
#include "memory/data.h"
#include "game/game_allegiance.h"
#include "ai/clump_datum.h"
#include "game/player_types.h"
#include "ai/prop_dialogue_event_types.h"
#include "ai/prop_stimulus_types.h"
#include "ai/prop_dialogue.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "scenario/scenario_access.h"
#include "game/materials.h"
#include "physics/water_physics_types.h"
#include "tag_files/tag_types.h"
#include "ai/behavior_info.h"
#include "ai/ai_player_types.h"
#include "cache/restricted_memory.h"
#include "ai/actor_datum.h"
#include "ai/swarms.h"
#include "objects/object_header_access.h"
#include "tag_files/tag_groups.h"
#include "units/unit_seat.h"
#include "units/unit_seat_flags.h"
#include "cache/restricted_memory_region_members.h"
#include "ai/actor_iterator_noalias.h"
#include "ai/actor_posture.h"
#include "ai/sector_geometry.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "ai/prop_type_info.h"
#include "ai/actor_iterator.h"
#include "math/real_constant_pool.h"
#include "effects/effect_data.h"
#include "cache/restricted_memory_layout.h"

__declspec(thread) s_data_array *prop_ref_data;
__declspec(thread) s_data_array *prop_data;
__declspec(thread) s_data_array *tracking_data;

// @address 0xaf7d0 ?prop_should_refresh_status@@YA_NPAUprop_ref_datum@@@Z
// @address 0xaf9f0 ?prop_state_get@@YAPAUprop_state@@PBUprop_ref_datum@@@Z
// @address 0xafa48 ?allow_orphanhood@@YA_NPAUprop_ref_datum@@@Z
// @address 0xafaf0 ?prop_view_get@@YAPAUprop_view@@J@Z
// @address 0xb0f40 ?prop_get_visibility@@YAFJJ_NFPAUactor_position_data@@1PA_N@Z
// @address 0xb47c8 ?pref_unreachable@@YA_NJ@Z
// @address 0xc00c0 ?prop_get_target_object_index@@YAJJ@Z
// @address 0xc3c40 ?pref_unassailable@@YA_NJ@Z
// @address 0xd2f90 ?pref_inspected_orphan@@YA_NPAUprop_ref_datum@@@Z
// @address 0x120020 ?prop_remove_tracking@@YAXJJ@Z
// @address 0x120098 ?prop_state_new@@YAXPAUprop_state@@@Z
// @address 0x232ca0 ?__tls_set_g_prop_data_allocator@@YAXPAX@Z
// @address 0x24b9c8 ?__tls_set_g_prop_ref_data_allocator@@YAXPAX@Z
// @address 0x552d30 ?__tls_set_g_command_script_allocator@@YAXPAX@Z
// @address 0x552d48 ?__tls_set_g_tracking_data_allocator@@YAXPAX@Z
// @address 0x552d60 ?__tls_set_g_contrail_system_allocator@@YAXPAX@Z
// @address 0x553738 ?prop_object_get_actor_index@@YAJJ@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0xd23e0 ?behavior_search_next@@YAFJPAVc_behavior_state@@0@Z
// @address 0x24eba0 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_prop_ref_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x552d78 ?prop_ref_reset_search@@YAXJ_N@Z
// @address 0xbf508 prop_state_get_by_index
// @address 0x553fa8 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_command_script_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x554048 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_command_script_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x554068 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_tracking_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x554108 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_tracking_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x554128 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_contrail_system_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x5541c8 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_contrail_system_allocator@@YAXPAX@Z@@UAAXPAX@Z
// @address 0x24eb00 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_prop_ref_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x55c938 prop_avoidance_add_object_repulsion
// @address 0x552fe0 prop_get_battle_vector
// @address 0xc7fd0 ?prop_unacknowledge@@YAXJJ@Z

















#include "ai/prop_view.h"
// 0xd23e0
struct search_state : c_behavior_state { short duration; long time; };
typedef char search_duration_offset[(offsetof(search_state, duration) == 0x24) ? 1 : -1];
typedef char search_time_offset[(offsetof(search_state, time) == 0x28) ? 1 : -1];


// 0x552d78



#include "ai/actor_prop_ref_iterator.h"

struct prop_prop_ref_iterator { long index; long next_index; };

// 0xc7fd0 prop_unacknowledge









// 0x514268 prop_orphan


/* Data arrays live in thread-local storage; the elements start at data. */
// @address 0x72d2b8 k_prop_type_info




bool prop_should_refresh_status(prop_ref_datum *prop_ref)
{
  prop_datum *prop = datum_get<prop_datum>(tls_ai_prop_data(), prop_ref->prop_index);
  const prop_type *type = &k_prop_type_info[(int)prop_ref->type];
  bool result = false;
  if (!(prop->status == 1 && type->state_update_mode == 0)) {
    if (prop->status != 1 || type->tracking_mode >= 2 || prop->flag != 0)
      result = true;
  }
  return result;
}

prop_state *prop_state_get(const prop_ref_datum *prop_ref)
{
  if (prop_ref->owner_index != -1)
    return &datum_get<tracking_datum>(tls_prop_data(), prop_ref->owner_index)->state;
  return &datum_get<prop_datum>(tls_ai_prop_data(), prop_ref->prop_index)->state;
}


/* Decides from an object's owner whether it counts as active; two tests on the owning player or on the object itself. */
bool allow_orphanhood(prop_ref_datum *prop_ref)
{
  prop_datum *object = datum_get<prop_datum>(tls_ai_prop_data(), prop_ref->prop_index);
  bool result;
  if (prop_ref->owner_index != -1) {
    long owner = object->owner_index;
    result = true;
    if (owner != -1) {
      actor_datum *player = datum_get<actor_datum>(tls_actor_data(), owner);
      if (player->type == 15 || (player->passenger != 0 && player->controlling_other_unit == 0))
        result = false;
    }
  } else
    result = (object->flag != 0) && (object->valid == 0);
  return result;
}

prop_view *prop_view_get(long prop_ref_index)
{
    prop_view *view = 0;
    prop_ref_datum *reference = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
    if (reference->view_index == -1)
        return view;
    tracking_datum *tracking = datum_get<tracking_datum>(tls_prop_data(), reference->view_index);
    if (!tracking)
        return view;
    view = (prop_view *)tracking->view;
    return view;
}




namespace {
static __forceinline prop_state *props_visibility_state_get(const prop_ref_datum *r)
{
  prop_state *state;
  if (r->owner_index != -1)
    state = &datum_get<tracking_datum>(tls_prop_data(), r->owner_index)->state;
  else
    state = &datum_get<prop_datum>(tls_ai_prop_data(), r->prop_index)->state;
  return state;
}
}

short prop_get_visibility(long actor_index, long prop_ref_index, bool enemy_prop, short visibility_type, actor_position_data *sense_position,
    actor_position_data *target_position, bool *visible)
{
  short result = 0;
  actor_datum *act = datum_get<actor_datum>(tls_actor_data(), actor_index);
  prop_ref_datum *r = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_ref_index);
  prop_state *state = props_visibility_state_get(r);
  unit_datum *u = (unit_datum *)object_try_and_get_and_verify_type(r->object_index, 0x1003);
  signed char type = r->type;
  bool enemy = type == 1;
  // NOTE: keep '|| false': without it the 6130 optimizer drops the bool materialization the shipped image has.
  bool blind = actor_is_blind(actor_index) || false;
  if (u && r->stimulus == 255 && state->index44 == -1) {
    if ((enemy_prop && u->unit.active_camouflage > 0.05f) || (!enemy_prop && u->unit.active_camouflage > 0.8f)) {
      if (enemy)
        blind = true;
      else
        blind |= enemy_prop && r->distance > 4.0f;
    }
  }
  if (blind) {
    result = 0;
  } else {
    bool use_frustum = true;
    if (act->vehicle_driver_type == 4)
      use_frustum = false;
    else if (act->type == 15 || act->type == 20)
      use_frustum = false;
    else if (!enemy && type != 3 && type != 7)
      use_frustum = false;
    else if (r->knowledge_state >= 1 && r->knowledge_state <= 2)
      use_frustum = false;
    else if (act->type == 24 && type == 3)
      use_frustum = false;
    result = actor_visibility_at_point(actor_index, sense_position, &target_position->head, &target_position->body, visibility_type,
        use_frustum, enemy_prop, visible);
  }
  return result;
}

bool pref_unreachable(long prop_ref_index)
{
  bool result = false;
  long second_index = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_ref_index)->view_index;
  if (second_index != -1) {
    tracking_datum *datum = datum_get<tracking_datum>(tls_prop_data(), second_index);
    if (datum) {
      prop_view *state = (prop_view *)datum->view;
      if (state)
        result = state->unreachable_time != -1;
    }
  }
  return result;
}

extern "C" void *prop_state_get_by_index(int owner_index)
{
  prop_ref_datum *owner = datum_get<prop_ref_datum>(tls_prop_ref_data(), owner_index);
  if (owner->view_index != -1)
    return &datum_get<tracking_datum>(tls_prop_data(), owner->view_index)->state;
  return &datum_get<prop_datum>(tls_ai_prop_data(), owner->prop_index)->state;
}

long prop_get_target_object_index(long prop_ref_index)
{
  long object_index = datum_get<prop_datum>(tls_ai_prop_data(), prop_ref_index)->object_index;
  s_object_header *header = object_header_get(object_index);
  unit_datum *unit = (unit_datum *)header->datum;
  if (unit->object.object_type == 0 && (1 << header->object_type & 0x1003) != 0 &&
      unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != -1) {
    vehicle_definition *vehicle = TAG_GET('vehi', vehicle_definition, object_get(unit->object.parent_object_index)->definition_index);
    unit_seat *seat = &((unit_seat *)vehicle->seats.address)[unit->unit.parent_seat_index];
    bool boarding_seat = (seat->flags >> _unit_seat_boarding_seat_bit) & 1;
    if (boarding_seat)
      return object_index;
  }
  long last = NONE;
  for (long current = object_index; current != NONE; current = object_get(current)->parent_object_index)
    last = current;
  return last;
}

bool pref_unassailable(long prop_ref_index)
{
  bool result = false;
  long second_index = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_ref_index)->view_index;
  if (second_index != -1) {
    tracking_datum *datum = datum_get<tracking_datum>(tls_prop_data(), second_index);
    if (datum) {
      prop_view *state = (prop_view *)datum->view;
      if (state)
        result = state->unassailable_until != -1;
    }
  }
  return result;
}

namespace {
}
namespace {
static __forceinline float props_track_player_attenuation(long clump_index, float weight)
{
    clump_datum *clump = datum_get<clump_datum>(tls_clump_data(), clump_index);
    float attenuation = 1.0f;
    if (clump->player_distance > 10.0f) {
        attenuation = clump->player_distance < 10.0f ? 1.0f :
            (float)pow(weight / 1.0f, (clump->player_distance - 10.0f) / 10.0f + 1.0f);
        attenuation = (attenuation > 0.0f ? attenuation : 0.0f) < 1.0f ?
            (attenuation > 0.0f ? attenuation : 0.0f) : 1.0f;
    }
    return attenuation;
}
}
/* the prop view, reset to its idle state: every handle -1, the modes 4, the flags 6 */
// @address 0xc8040 prop_view_new
extern "C" void prop_view_new(prop_view *view)
{
    memset(view, 0, sizeof(*view));
    view->tracking_index = -1;
    view->acknowledge_time = -1;
    view->vitality = 0.0f;
    view->unreachable_time = -1;
    view->salience = 0.0f;
    view->unassailable_until = -1;
    view->search_state = 4;
    view->sighting_search_state = 4;
    view->sighting_awareness = 0;
    view->sighting_point = k_real_math_origin3d;
    view->orphaned_time = -1;
    view->sighting_status = 0;
    view->pursuit_type = 0;
    view->sighting_count = 0;
    view->sighting_awareness = 0;
    view->flags = 6;
}

// @address 0xcc240 ?prop_track_weight@@YAMJ@Z
float prop_track_weight(long prop_ref_index)
{
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_ref_index);
    long actor_index = prop_ref->actor_index;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    prop_datum *prop = datum_get<prop_datum>(tls_ai_prop_data(), prop_ref->prop_index);
    float weight;
    if (prop->flag)
        weight = 1.0f;
    else {
        weight = prop_ref->track_weight;
        if (prop_ref_index == actor->target_prop_index)
            weight += 0.2f;
        if (prop_ref->owner_index != -1)
            weight += 0.1f;
        weight *= props_track_player_attenuation(actor->clump_index, weight);
        weight = (weight > 0.0f ? weight : 0.0f) < 1.0f ?
            (weight > 0.0f ? weight : 0.0f) : 1.0f;
    }
    return weight;
}

namespace {
static __forceinline prop_view *prop_view_from_reference(const prop_ref_datum *reference)
{
    prop_view *view = 0;
    if (reference->owner_index != -1) {
        tracking_datum *tracking = datum_get<tracking_datum>(tls_prop_data(), reference->owner_index);
        if (tracking)
            view = (prop_view *)tracking->view;
    }
    return view;
}
}



static __forceinline unsigned long props_search_behavior_is_allowed(long index, Behavior_header *b)
{
    return b->kind != g_action_selection_nonce && (b->flags & global_condition_flags) == global_condition_flags
        ? ((global_style_vector[index >> 5] >> (index & 31)) & 1)
        : 0;
}

short behavior_search_next(long index, c_behavior_state *behavior_state, c_behavior_state *unused)
{
    search_state *state = static_cast<search_state *>(behavior_state);
    short result = -1;
    character_search_properties *properties = character_definition_search_multi_properties_get(index);
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), index);
    if (!actor->search_active)
        result = -1;
    else if (actor->target_prop_index != -1)
    {
        bool keep_searching = properties && (properties->flags & 4) != 0;
        prop_ref_datum *ref = (prop_ref_datum *)datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
        if (ref->knowledge_state >= 3)
        {
            prop_view *view = (prop_view *)prop_view_from_reference(ref);
            if (view && (view->flags & 16) == 0)
            {
                if (state->duration + state->time < tls_game_time_globals()->time && !keep_searching)
                    actor_prop_abandon_search(index, actor->target_prop_index, 1);
                else
                    result = -2;
            }
            if ((view->flags & 16) != 0 && props_search_behavior_is_allowed(63, global_behavior_info[63].header))
                result = 63;
        }
    }
    return result;
}

// 0xd2f90
bool pref_inspected_orphan(prop_ref_datum *reference)
{
    bool result = false;
    if (reference->knowledge_state >= 3) {
        prop_view *view = prop_view_from_reference(reference);
        if (view)
            result = (_rotl(view->flags, 26) & 1) != 0;
    }
    return result;
}


// 0x120020
void prop_remove_tracking(long actor_index, long prop_ref_index)
{
    prop_ref_datum *reference = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
    ::datum_delete(tls_prop_data(), reference->owner_index);
    reference->owner_index = -1;
    if (reference->knowledge_state < 1 || reference->knowledge_state > 2)
        reference->knowledge_state = 0;
}

void prop_state_new(prop_state *state)
{
    state->aiming_position = k_real_math_origin3d;
    state->b6b = false;
    state->b67 = false;
    state->b64 = false;
    state->b69 = false;
    state->b66 = false;
    state->index0 = -1;
    state->b65 = true;
    state->b68 = false;
    state->b6d = false;
    state->b6a = false;
    state->index44 = -1;
    state->b6c = false;
    state->pathfinding_point.set(&k_real_math_origin3d, -1, -1);
    state->pathfinding_sector.clear();
    state->b6e = false;
    state->index5c = -1;
    state->zero60 = 0;
}

void __tls_set_g_prop_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&effect_data, (s_data_array *)object);
}

void __tls_set_g_prop_ref_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&g_hs_thread_deterministic_data, (s_data_array *)object);
}

// 0x552d48
void __tls_set_g_tracking_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&prop_ref_data, (s_data_array *)object);
}


static __forceinline void props_view_reset_search(prop_view *view)
{
    short flags = (short)view->flags;
    view->pursuit_type = 0;
    view->sighting_count = 0;
    view->sighting_awareness = 0;
    flags &= ~8;
    flags &= ~16;
    flags &= ~32;
    flags |= 2;
    flags |= 4;
    flags &= ~64;
    flags &= ~1;
    view->flags = flags;
}
void prop_ref_reset_search(long index, bool clear_prop)
{
    prop_ref_datum *ref = datum_get<prop_ref_datum>(prop_ref_data, index);
    prop_view *view = 0;
    if (ref->owner_index != -1)
    {
        tracking_datum *datum = datum_get<tracking_datum>(tls_prop_data(), ref->owner_index);
        if (datum) view = (prop_view *)datum->view;
    }
    if (view) props_view_reset_search(view);
    if (clear_prop)
    {
        prop_datum *prop = datum_get<prop_datum>(tls_ai_prop_data(), ref->prop_index);
        prop->flags &= ~2;
        prop->flags &= ~4;
        prop->flags &= ~8;
    }
    prop_datum *area = datum_get<prop_datum>(tls_ai_prop_data(), ref->prop_index);
    area->next_discarded_area = 0;
    area->discarded_area_count = 0;
}


/* Abandonment dialogue uses a one-second delay. */
static __forceinline void props_dialogue_abandoned(long actor_index, long prop_index, short dialogue_index)
{
    prop_dialogue_event_internal(actor_index, prop_index, prop_dialogue_event_4, dialogue_index, 1.0f);
}

// @address 0x553698 ?actor_prop_abandon_search@@YAXJJF@Z
void actor_prop_abandon_search(long actor_index, long prop_index, short mode)
{
    if (prop_index != -1)
    {
        prop_view *view = prop_view_get(prop_index);
        if (view != 0)
        {
            short flags = (short)(view->flags | 16);
            view->flags = flags;
            if (mode == 3)
                view->flags = flags | 32;
            short dialogue_index = -1;
            if (mode != 1)
            {
                if (mode != 2)
                {
                    if (mode == 3)
                        dialogue_index = 27;
                }
                else
                    dialogue_index = 24;
            }
            else
                dialogue_index = 25;
            props_dialogue_abandoned(actor_index, prop_index, dialogue_index);
        }
    }
}


long prop_object_get_actor_index(long object_index)
{
    long actor_index = -1;
    if (object_index != -1) {
        unit_datum *object = (unit_datum *)datum_get<object_header_datum>(tls_object_header_data(), object_index)->datum;
        if ((1 << object->object.type) & 0x1003) {
            actor_index = object->unit.actor_index;
        } else if (object->object.type == 11) {
            swarm_creature_state *block = 0;
            if (((creature_datum *)object)->creature.link_type == 0)
                block = (swarm_creature_state *)((char *)object + ((creature_datum *)object)->creature.creature_reference.offset);
            if (block != 0)
                actor_index = block->swarm_index;
        }
    }
    return actor_index;
}

// @address 0x226cc k_real_seven_tenths_storage
namespace {
static __forceinline unit_datum *props_battle_object_get(long index)
{
    return (unit_datum *)datum_get<object_header_datum>(tls_object_header_data(), index)->datum;
}
}
extern "C" bool prop_get_battle_vector(long prop_ref_index, real_vector3d *battle_vector, bool *unused)
{
    prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
    unit_datum *object = props_battle_object_get(prop_ref->object_index);
    bool result = false;
    if ((1 << object->object.type) & 0x1003)
    {
        if (object->unit.player_index != -1)
        {
            ai_player_state *player_state = (ai_player_state *)ai_player_state_get(object->unit.player_index);
            if (player_state)
            {
                real_vector3d vector = player_state->position;
                float magnitude = normalize3d(&vector);
                result = magnitude > k_real_seven_tenths_storage;
                if (battle_vector)
                    *battle_vector = vector;
            }
        }
    }
    return result;
}

// @address 0x5537b8 ?prop_ref_get_acknowledged_by_object_index@@YAJJJ@Z
long prop_ref_get_acknowledged_by_object_index(long actor_index, long object_index)
{
    long prop_ref_index = -1;
    long object_actor_index = -1;
    actor_prop_ref_iterator iterator;
    prop_ref_datum *prop_ref;
    object_actor_index = prop_object_get_actor_index(object_index);
    actor_prop_ref_iterator_new(&iterator, actor_index);
    while ((prop_ref = actor_prop_ref_iterator_next(&iterator)) != 0)
    {
        prop_datum *prop = datum_get<prop_datum>(tls_ai_prop_data(), prop_ref->prop_index);
        if (prop_ref->knowledge_state >= 1)
        {
            if (prop_ref->object_index == object_index ||
                (prop->owned_by_actor && prop->actor_index != -1 && prop->actor_index == object_actor_index))
            {
                prop_ref_index = iterator.index;
                break;
            }
        }
    }
    return prop_ref_index;
}

static __forceinline prop_ref_datum *props_prop_ref_get(long prop_ref_index)
{
    return datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
}
static __forceinline prop_datum *props_prop_get(long prop_index)
{
    return &((prop_datum *)prop_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(prop_index)];
}
static __forceinline void prop_prop_ref_iterator_new(prop_prop_ref_iterator *iterator, long prop_index)
{
    iterator->index = -1;
    iterator->next_index = props_prop_get(prop_index)->first_prop_ref_index;
}
static __forceinline prop_ref_datum *prop_prop_ref_iterator_next(prop_prop_ref_iterator *iterator)
{
    prop_ref_datum *result = 0;
    if (iterator->next_index != -1) {
        iterator->index = iterator->next_index;
        result = props_prop_ref_get(iterator->next_index);
        iterator->next_index = result->prop_next_ref_index;
    }
    return result;
}
static __forceinline long props_clump_aquire_prop_ref(long actor_index, long object_index, e_clump_importance importance)
{
    long clump_index = datum_get<actor_datum>(actor_data, actor_index)->clump_index;
    long prop_ref_index = -1;
    if (clump_index != -1)
    {
        long prop_index = clump_get_prop_datum(clump_index, object_index);
        if (prop_index == -1)
            prop_index = clump_aquire_prop(clump_index, object_index, importance);
        if (prop_index != -1)
        {
            prop_prop_ref_iterator iterator;
            prop_ref_datum *prop_ref;
            prop_prop_ref_iterator_new(&iterator, prop_index);
            while ((prop_ref = prop_prop_ref_iterator_next(&iterator)) != 0)
            {
                if (prop_ref->actor_index == actor_index)
                {
                    prop_ref_index = iterator.index;
                    break;
                }
            }
        }
    }
    return prop_ref_index;
}

// match: 0x5538a0: retain the original direct actor lookup; the shared iterator getter
static __forceinline void props_object_index_iterator_new(actor_prop_ref_iterator *iterator, long actor_index)
{
    iterator->index = -1;
    iterator->next_index = datum_get<actor_datum>(tls_actor_data(), actor_index)->first_prop_ref_index;
}

// @address 0x5538a0 ?prop_ref_get_by_object_index@@YAJJJ_NW4e_clump_importance@@@Z
long prop_ref_get_by_object_index(long actor_index, long object_index, bool create, e_clump_importance importance)
{
    long prop_ref_index = -1;
    if (object_index != -1)
    {
        actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
        long object_actor_index = -1;
        object_actor_index = prop_object_get_actor_index(object_index);
        if (object_actor_index != actor_index)
        {
            actor_prop_ref_iterator iterator;
            prop_ref_datum *prop_ref;
            props_object_index_iterator_new(&iterator, actor_index);
            while ((prop_ref = actor_prop_ref_iterator_next(&iterator)) != 0)
            {
                prop_datum *prop = props_prop_get(prop_ref->prop_index);
                if (prop_ref->object_index == object_index ||
                    (prop->owned_by_actor && prop->actor_index != -1 && prop->actor_index == object_actor_index))
                {
                    prop_ref_index = iterator.index;
                    break;
                }
            }
            if (prop_ref_index == -1 && create && actor->active && actor->clump_index != -1)
            {
                prop_ref_index = props_clump_aquire_prop_ref(actor_index, object_index, importance);
                if (prop_ref_index != -1)
                {
                    prop_datum *prop = props_prop_get(props_prop_ref_get(prop_ref_index)->prop_index);
                    prop->expiration_time = tls_game_time_globals()->time + 60 * tls_game_time_globals()->ticks_per_second;
                }
            }
        }
    }
    return prop_ref_index;
}





namespace {

template<void (*set_user_address)(void *)>
static __forceinline long props_restricted_region_add_prop_ref_member(const char *name, const char *type, unsigned int size, long alignment_bits)
{
    long member_index = g_restricted_regions[1].add_member(name, type, size, alignment_bits, set_user_address, 0, 0);
    void *address = restricted_region_get_member_address(1, member_index);
    set_user_address(address);
    return member_index;
}

template<>
__forceinline long props_restricted_region_add_prop_ref_member<&__tls_set_g_command_script_allocator>(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    long member_index = g_restricted_regions[1].add_member(name, type, size, alignment_bits, &__tls_set_g_command_script_allocator, 0, 0);
    void *address = restricted_region_get_member_address(1, member_index);
    data_set_new_base_address(&prop_data, (s_data_array *)address);
    return member_index;
}

template<>
__forceinline long props_restricted_region_add_prop_ref_member<&__tls_set_g_contrail_system_allocator>(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    long member_index = g_restricted_regions[1].add_member(name, type, size, alignment_bits, &__tls_set_g_contrail_system_allocator, 0, 0);
    void *address = restricted_region_get_member_address(1, member_index);
    data_set_new_base_address((s_data_array **)&TLS_PROP_DATA, (s_data_array *)address);
    return member_index;
}

}

template<>
inline void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_command_script_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = props_restricted_region_add_prop_ref_member<&__tls_set_g_command_script_allocator>(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

template<>
inline void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tracking_data_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = props_restricted_region_add_prop_ref_member<&__tls_set_g_tracking_data_allocator>(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

template<>
inline void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_system_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = props_restricted_region_add_prop_ref_member<&__tls_set_g_contrail_system_allocator>(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

template<>
inline void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_prop_ref_data_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    m_member_index = props_restricted_region_add_prop_ref_member<&__tls_set_g_prop_ref_data_allocator>(name, type, size, alignment_bits);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(1, m_member_index);
}

// 0x553fa8
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_command_script_allocator>::allocate(unsigned
    int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x554048
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_command_script_allocator>::deallocate(void *memory)
{
    prop_data = 0;
    restricted_region_free_member(1, m_member_index);
}

// 0x554068
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tracking_data_allocator>::allocate(unsigned
    int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x554108
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tracking_data_allocator>::deallocate(void *memory)
{
    prop_ref_data = 0;
    restricted_region_free_member(1, m_member_index);
}

// 0x554128
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_system_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x5541c8
template<>
void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_system_allocator>::deallocate(void *memory)
{
    TLS_PROP_DATA = 0;
    restricted_region_free_member(1, m_member_index);
}

// 0x24eb00
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_prop_ref_data_allocator>::allocate(unsigned
    int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x24eba0
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_prop_ref_data_allocator>::deallocate(void *memory)
{
    g_hs_thread_deterministic_data = 0;
    restricted_region_free_member(1, m_member_index);
}

// @address 0x9b308 ?normalize3d@@YAMPATreal_vector3d@@@Z

// @address 0x2251c k_real_zero_storage
// @address 0x3aa90 k_real_math_origin3d
// @rdata 0x820221fc
static const float props_repulsion_one = 1.0f;

static __forceinline real_vector3d *props_battle_direction(const real_point3d *from, const real_point3d *to, real_vector3d *direction)
{
    direction->i = to->x - from->x;
    direction->j = to->y - from->y;
    direction->k = to->z - from->z;
    return direction;
}
extern "C" void prop_avoidance_add_object_repulsion(
    unsigned char inner_only, unsigned long object_index, float radius, const real_point3d *position, real_vector3d *vector,
    // match: 0x55c938: pointer __restrict qualifier prevents alias reload
    float *__restrict aggregate)
{
    real_point3d object_position;
    object_get_origin(object_index, &object_position);
    real_vector3d direction;
    props_battle_direction(position, &object_position, &direction);
    float distance = normalize3d(&direction);
    float strength;
    if (distance < radius) {
        strength = props_repulsion_one;
    } else {
        if (inner_only != 0)
            return;
        strength = radius * 2.0f;
        if (distance >= strength)
            return;
        strength = (strength - distance) / radius;
        if (strength <= 0.0f)
            return;
    }
    float scale = -strength;
    float y = vector->j;
    float z = vector->k;
    vector->i = direction.i * scale + vector->i;
    vector->j = direction.j * scale + y;
    vector->k = direction.k * scale + z;
    if (strength > *aggregate)
        *aggregate = strength;
}


void prop_unacknowledge(long actor_index, long prop_ref_index)
{
    prop_ref_datum *datum = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_ref_index);
    if (datum->view_index != -1)
        prop_remove_tracking(actor_index, prop_ref_index);
    datum->knowledge_state = 0;
    datum->accumulated_awareness = 0.0f;
    actor_compute_prop_salience(actor_index, prop_ref_index, _prop_stimulus_none);
}

// @address 0x553a90 ?actor_flush_prop_tracks@@YAFJM@Z
static __forceinline bool props3_prop_essential(unsigned short prop_ref_index)
{
    prop_ref_datum *reference = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_ref_index);
    prop_datum *prop = datum_get<prop_datum>(tls_ai_prop_data(), reference->prop_index);
    return prop->flag != 0;
}

static __forceinline void props3_prop_remove_tracking(long actor_index, unsigned short prop_ref_index)
{
    prop_ref_datum *reference = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_ref_index);
    ::datum_delete(tls_prop_data(), reference->owner_index);
    reference->owner_index = -1;
    if (reference->knowledge_state < 1 || reference->knowledge_state > 2)
        reference->knowledge_state = 0;
}

short actor_flush_prop_tracks(long actor_index, float max_distance)
{
    actor_datum *actor = actor_prop_ref_iterator_actor_get(tls_actor_data(), actor_index);
    short count = 0;
    float track_weight;
    actor_prop_ref_iterator iterator;
    actor_prop_ref_iterator_new(&iterator, actor_index);
    while (max_distance == 0.0f || actor->track_threshold < max_distance)
    {
        float best_weight = 0.0f;
        for (;;)
        {
            prop_ref_datum *prop_ref = actor_prop_ref_iterator_next(&iterator);
            if (prop_ref == 0)
                break;
            long current_index = iterator.index;
            if (prop_ref->owner_index == -1)
                continue;
            track_weight = prop_track_weight(current_index);
            bool flush;
            if (max_distance == 0.0f)
                flush = true;
            else if (props3_prop_essential(current_index))
                flush = false;
            else
                flush = track_weight < actor->track_threshold;
            if (!flush)
                continue;
            if (track_weight > best_weight)
                best_weight = track_weight;
            props3_prop_remove_tracking(actor_index, current_index);
            count = (short)(count + 1);
        }
        if (max_distance == 0.0f)
            break;
        float distance = actor->track_threshold;
        if (count <= 0)
        {
            actor->track_threshold = distance + 0.15f;
            continue;
        }
        float best = best_weight + 0.15f;
        if (distance <= best)
            distance = best;
        actor->track_threshold = distance;
        break;
    }
    return count;
}

// @address 0x552e50 ?prop_track@@YAJJJ@Z

long prop_track(long player_index, long unit_index)
{
    float distance = prop_track_weight(unit_index);
    long best = -1;
    float best_distance = distance;
    short count = 0;
    long result = -1;
    bool ok = false;
    long index = datum_get<actor_datum>(actor_data, player_index)->first_prop_ref_index;
    long current;
    for (;;) {
        for (;;) {
            // NOTE: keep goto done: the equivalent single loop reshuffles register allocation in the shipped image.
            if (index == -1)
                goto done;
            current = index;
            prop_ref_datum *datum = datum_get<prop_ref_datum>(prop_ref_data, current);
            index = datum->next_prop_ref_index;
            if (datum->owner_index != -1)
                break;
        }
        float d = prop_track_weight(current);
        if (d < best_distance) {
            best_distance = d;
            best = current;
        }
        count = count + 1;
    }
done:
    if (count < 4) {
        ok = true;
    } else if (best != -1) {
        prop_remove_tracking(player_index, best);
        ok = true;
    }
    if (ok) {
        result = datum_new(TLS_PROP_DATA);
        if (result == -1 && prop_track_reclaim_inactive_tracks(player_index, distance))
            result = datum_new(TLS_PROP_DATA);
        if (result != -1) {
            tracking_datum *object = datum_get<tracking_datum>(TLS_PROP_DATA, result);
            datum_get<prop_ref_datum>(prop_ref_data, unit_index)->owner_index = result;
            prop_state_new(&object->state);
            prop_view_new((prop_view *)object->view);
        }
    }
    return result;
}

static __forceinline void props_actor_iterator_new(actor_iterator *const &iterator, bool active_only)
{
    if (ai_globals->enabled) {
        iterator->m_iterator.absolute_index = -1;
        iterator->m_iterator.index = -1;
        iterator->active_only = active_only;
        iterator->m_iterator.data = actor_data;
    }
}
// NOTE: 15.0f stays a shared-pool load: spelling the seconds parameter lets the 6130 optimizer hoist it out of the reclaim loop.
static __forceinline long props_seconds_to_ticks(float seconds, float rounding)
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x553c58: the float loads through the constant-pool base register the function already holds
    return (long)((float)game_time_globals->ticks_per_second * (&k_real_zero_storage)[424] + rounding);
#else
    return (long)((float)game_time_globals->ticks_per_second * 15.0f + rounding);
#endif
}
// @address 0x553c58 prop_track_reclaim_inactive_tracks
extern "C" bool prop_track_reclaim_inactive_tracks(long actor_index, float distance)
{
    short freed = 0;
    long time = game_time_globals->time;
    if (ai_globals->last_cleanup_time < time) {
        actor_iterator iterator;
        props_actor_iterator_new(&iterator, false);
        actor_datum *actor = actor_iterator_next(&iterator);
        // NOTE: 0.5f stays a shared-pool load: a literal breaks the shared pool-base addressing the shipped image uses.
#if defined(_XBOX) && !defined(__clang__)
        // match: 0x553c58: the float loads through the constant-pool base register the function already holds
        const float rounding = (&k_real_zero_storage)[553];
#else
        const float rounding = 0.5f;
#endif
        const float zero = k_real_zero_storage;
        while (actor != 0) {
            if (!actor->active && !structure_bsp_is_active(actor->position.location.cluster_reference.bsp_index) &&
                time - actor->deactivation_ticks > props_seconds_to_ticks(15.0f, rounding))
                freed += actor_flush_prop_tracks(iterator.index, zero);
            actor = actor_iterator_next(&iterator);
        }
        if (freed < 5) {
            props_actor_iterator_new(&iterator, false);
            while ((actor = actor_iterator_next(&iterator)) != 0) {
                if (!actor->active && (!structure_bsp_is_active(actor->position.location.cluster_reference.bsp_index) ||
                    time - actor->deactivation_ticks > props_seconds_to_ticks(15.0f, rounding)))
                    freed += actor_flush_prop_tracks(iterator.index, zero);
            }
        }
        if (freed < 5) {
            props_actor_iterator_new(&iterator, false);
            while ((actor = actor_iterator_next(&iterator)) != 0) {
                if (!actor->active)
                    freed += actor_flush_prop_tracks(iterator.index, zero);
            }
        }
        if (freed < 5) {
            props_actor_iterator_new(&iterator, true);
            while (actor_iterator_next(&iterator)) {
                if (iterator.index != actor_index)
                    freed += actor_flush_prop_tracks(iterator.index, distance);
                if (freed >= 5)
                    break;
            }
        }
    }
    return freed > 0;
}

static __forceinline prop_state *props_position_state_get(const prop_ref_datum *reference)
{
    prop_state *state;
    if (reference->owner_index != -1)
        state = (prop_state *)&datum_get<tracking_datum>(tls_prop_data(), reference->owner_index)->state;
    else
        state = (prop_state *)&datum_get<prop_datum>(tls_ai_prop_data(), reference->prop_index)->state;
    return state;
}
static __forceinline prop_view *props_position_view_get(const prop_ref_datum *reference)
{
    prop_view *view = 0;
    if (reference->owner_index != -1) {
        tracking_datum *tracking = datum_get<tracking_datum>(tls_prop_data(), reference->owner_index);
        if (tracking)
            view = (prop_view *)tracking->view;
    }
    return view;
}
static __forceinline float props_distance_squared(const real_point3d *const &a, const real_point3d *b)
{
    float dx = a->x - b->x;
    float dy = a->y - b->y;
    float dz = a->z - b->z;
    return dx * dx + (dz * dz + dy * dy);
}
// @address 0xb3958 actor_prop_update_new_position
extern "C" void actor_prop_update_new_position(long actor_index, long prop_ref_index, bool update_salience)
{
    prop_ref_datum *reference = datum_get<prop_ref_datum>(prop_ref_data, prop_ref_index);
    prop_state *state = props_position_state_get(reference);
    prop_view *view = props_position_view_get(reference);
    float threshold;
    if (reference->distance < 10.0f)
        threshold = 1.5f;
    else
        threshold = (reference->distance - 10.0f) * 0.5f + 1.5f;
    if (!(view->flags & 0x80) ||
        !(props_distance_squared(&view->sighting_position, &state->position) <= threshold * threshold)) {
        ::prop_ref_reset_search(prop_ref_index, false);
        if (view->flags & 0x80) {
            real_vector3d *direction = &view->sighting_direction;
            props_battle_direction(&view->sighting_position, &state->position, direction);
            normalize3d(direction);
        } else {
            view->sighting_point = k_real_math_origin3d;
        }
        view->sighting_position = state->position;
        view->flags |= 0x80;
        actor_perception_unassailable(actor_index, prop_ref_index, false, true);
    }
}

// @address 0xb15b0 ?prop_get_audibility@@YAFJJPAUactor_position_data@@PATreal_point3d@@PAUs_location@@F@Z
namespace {
static __forceinline s_object_header *props_audibility_object_header_get(long object_index)
{
    return datum_get<s_object_header>(tls_object_header_data(), object_index);
}
static __forceinline unit_datum *props_audibility_object_get(long object_index)
{
    return (unit_datum *)props_audibility_object_header_get(object_index)->datum;
}
}

short prop_get_audibility(long actor_index, long object_index, actor_position_data *position_data, real_point3d *point, s_location *location, short flags)
{
    short audibility = 0;
    long unit_index = ((1 << props_audibility_object_header_get(object_index)->object_type) & 0x1003) ? object_index : -1;
    if (actor_is_deaf(actor_index))
    {
        audibility = 0;
    }
    else if (unit_index != -1)
    {
        unit_datum *unit = props_audibility_object_get(unit_index);
        long source_index = unit_index;
        if (unit->unit.actor_index != -1 && datum_get<actor_datum>(tls_actor_data(), unit->unit.actor_index)->type == 15)
        {
            source_index = -1;
        }
        else if (unit->object.parent_object_index != -1 && props_audibility_object_get(unit->object.parent_object_index)->object.type == 1)
        {
            source_index = unit->object.parent_object_index;
        }
        if (source_index != -1)
        {
            creature_definition *definition = TAG_GET('crea', creature_definition, props_audibility_object_get(source_index)->object.definition_index);
            audibility = actor_audibility_at_point(actor_index, position_data, point, location, definition->motion_sensor_blip_size, 1.0f, flags);
        }
    }
    return audibility;
}


static __forceinline void props_prop_iterator_new(clump_actor_cursor *iterator)
{
    iterator->iterator.absolute_index = -1;
    iterator->iterator.index = -1;
    iterator->iterator.data = tls_ai_prop_data();
}
static __forceinline prop_datum *props_prop_iterator_next(clump_actor_cursor *iterator)
{
    prop_datum *result = 0;
    iterator->datum = (prop_datum *)data_iterator_next(&iterator->iterator);
    if (iterator->datum)
        result = (prop_datum *)iterator->datum;
    iterator->index = iterator->iterator.index;
    return result;
}
// @address 0x51bfb0 prop_perceive_target_for_trackers
extern "C" void prop_perceive_target_for_trackers(long object_index, long target_object_index)
{
    if (tls_actor_globals()->enabled) {
        clump_actor_cursor iterator;
        props_prop_iterator_new(&iterator);
        prop_datum *prop;
        while ((prop = props_prop_iterator_next(&iterator)) != 0) {
            if (prop->object_index == object_index) {
                long prop_ref_index = prop->first_prop_ref_index;
                while (prop_ref_index != -1) {
                    prop_ref_datum *prop_ref = (prop_ref_datum *)inl_datum_try_and_get(tls_prop_ref_data(), prop_ref_index);
                    if (!prop_ref)
                        break;
                    long next_prop_ref_index = prop_ref->prop_next_ref_index;
                    if (prop_ref->knowledge_state >= 1 && prop_ref->knowledge_state <= 2) {
                        long actor_index = prop_ref->actor_index;
                        if (datum_get<actor_datum>(actor_data, actor_index)->active) {
                            long target_prop_ref_index = prop_ref_get_by_object_index(actor_index, target_object_index, true, _clump_importance_normal);
                            if (target_prop_ref_index != -1)
                                prop_perceive(actor_index, target_object_index, 2, _prop_stimulus_none, 0, target_prop_ref_index, -1, -1);
                        }
                    }
                    prop_ref_index = next_prop_ref_index;
                }
            }
        }
    }
}

// ---------------------------------------------------------------------------
// prop_state_refresh




namespace {
static __forceinline bool props_refresh_flag_test(unsigned long flags, long bit)
{
    return (flags & (1UL << bit)) != 0;
}
static __forceinline object_datum *props_refresh_object_get(long object_index)
{
    return datum_get<object_header_datum>(tls_object_header_data(), object_index)->datum;
}
static __forceinline void *props_refresh_object_header_block_get(long object_index, const object_header_block_reference *reference)
{
    return (char *)props_refresh_object_get(object_index) + reference->offset;
}
static __forceinline long props_refresh_object_get_ultimate_parent(long object_index)
{
    long ultimate_parent_index = -1;
    while (object_index != -1) {
        ultimate_parent_index = object_index;
        object_index = props_refresh_object_get(object_index)->parent_object_index;
    }
    return ultimate_parent_index;
}
static __forceinline void props_refresh_unit_get_aiming_vector(long unit_index, real_vector3d *aiming_vector)
{
    *aiming_vector = ((unit_datum *)props_refresh_object_get(unit_index))->unit.aiming_vector;
}
static __forceinline void props_refresh_creature_get_center_of_mass(long creature_index, real_point3d *center)
{
    object_marker marker;
    internal_object_get_markers_by_string_id(creature_index, 177, &marker, 1, false);
    *center = marker.matrix.position;
}
static __forceinline vehicle_definition *vehicle_definition_get(long vehicle_index)
{
    return TAG_GET('vehi', vehicle_definition, props_refresh_object_get(vehicle_index)->definition_index);
}
static __forceinline bool props_refresh_vehicle_causes_collision_damage(long vehicle_index)
{
    return !props_refresh_flag_test(vehicle_definition_get(vehicle_index)->flags, 10);
}
static __forceinline bool props_refresh_actor_is_noncombat(long actor_index)
{
    return datum_get<actor_datum>(tls_actor_data(), actor_index)->behavior_mode < 4;
}
}

// @address 0xb2c90 ?prop_state_refresh@@YA_NPAUprop_state@@JFPAUactor_position_data@@@Z
bool prop_state_refresh(prop_state *state, long object_index, short refresh_type, actor_position_data *position_data)
{
    long time = tls_game_time_globals()->time;
    bool refreshed = true;
    if (state->index0 == time)
        refreshed = false;
    if (refreshed)
    {
        object_datum *object = props_refresh_object_get(object_index);
        unit_datum *unit = 0;
        creature_datum *creature = 0;
        long actor_index = -1;
        if ((1 << object->type) & 0x1003)
            unit = (unit_datum *)object;
        else if (object->type == 11)
            creature = (creature_datum *)object;
        if (unit && unit->unit.actor_index != -1)
            actor_index = unit->unit.actor_index;
        else if (creature && creature->creature.link_type == 0)
        {
            swarm_creature_state *creature_actor = (swarm_creature_state *)props_refresh_object_header_block_get(
                object_index, &creature->creature.creature_reference);
            if (creature_actor && creature_actor->swarm_index != -1)
                actor_index = creature_actor->swarm_index;
        }

        actor_position_data target_data;
        if (!position_data)
        {
            ::actor_perception_find_target_data(object_index, &target_data);
            position_data = &target_data;
        }
        state->head_position = position_data->head;
        state->body_position = position_data->body;
        state->location = position_data->location;
        state->velocity = position_data->velocity;

        if (unit)
        {
            unit_get_body_position(object_index, &state->center);
            props_refresh_unit_get_aiming_vector(object_index, &state->aiming);
            state->b6c = props_refresh_flag_test(unit->unit.unit_flags, 8);
        }
        else
        {
            state->aiming_position = k_real_math_origin3d;
            if (creature)
                props_refresh_creature_get_center_of_mass(object_index, &state->center);
            else
                state->center = state->body_position;
        }
        s_water_physics_test_sphere_result water_result;
        state->b6d = water_physics_test_sphere(true, &state->center, 0.0f, &water_result);

        state->index44 = -1;
        state->b6a = false;
        state->b6b = false;
        state->b68 = refresh_type == 1;
        if (object->parent_object_index != -1)
        {
            unit_datum *parent = (unit_datum *)props_refresh_object_get(object->parent_object_index);
            if (unit && parent->object.type == 1)
            {
                state->index44 = ai_get_root_vehicle(unit->object.parent_object_index);
                state->b6a = parent->unit.gunner_object_index == object_index || unit->object.type == 15;
                state->b6b = parent->unit.driver_object_index == object_index && props_refresh_vehicle_causes_collision_damage(state->index44);
            }
        }

        if (object_pathfinding_immediate(object_index))
            prop_find_pathfinding_location_internal(g_updating_actor_index, object_index, state);
        else
            state->b6e = false;

        object_datum *ultimate_parent = props_refresh_object_get(props_refresh_object_get_ultimate_parent(object_index));
        switch (ultimate_parent->type)
        {
        case 0:
            state->b69 = ((biped_datum *)ultimate_parent)->biped.physics.get_mode() == c_character_physics_component::_mode_flying;
            break;
        case 1:
            if (props_refresh_flag_test(TAG_GET('vehi', vehicle_definition, ultimate_parent->definition_index)->flags, 6))
                state->b69 = true;
            else
                state->b69 = false;
            break;
        case _object_type_creature:
            state->b69 = ((creature_datum *)ultimate_parent)->creature.physics.get_mode() == c_character_physics_component::_mode_flying;
            break;
        default:
            state->b69 = false;
            break;
        }

        bool damaged = props_refresh_flag_test(object->damage_flags, 2);
        if (damaged && !state->b64 && state->index0 != -1)
            state->index5c = tls_game_time_globals()->time;
        state->b64 = damaged;

        long posture = 0;
        bool noncombat;
        bool in_combat;
        bool fighting;
        if (actor_index == -1)
        {
            noncombat = false;
            in_combat = false;
            fighting = !state->b64;
        }
        else
        {
            noncombat = props_refresh_actor_is_noncombat(actor_index);
            in_combat = actor_in_combat(actor_index);
            fighting = actor_executing_layer(actor_index, 25) != -1;
            posture = actor_get_posture(actor_index);
        }
        state->b65 = noncombat;
        state->b66 = in_combat;
        state->b67 = fighting;
        state->zero60 = posture;
        state->index0 = time;
    }
    return refreshed;
}

// @address 0xd1a18 ?prop_acknowledge@@YA_NJJ_N@Z
bool prop_acknowledge(long actor_index, long prop_ref_index, bool p2)
{
    bool result = true;
    prop_ref_datum *pref = props_prop_ref_get(prop_ref_index);
    prop_datum *prop = props_prop_get(pref->prop_index);
    bool no_knowledge = !pref->knowledge_state;
    pref->type = (signed char)prop->type;
    bool in_range = pref->knowledge_state >= 1 ? pref->knowledge_state <= 2 : false;
    if (!p2 && k_prop_type_info[pref->type].tracking_mode != 2)
        p2 = true;
    pref->knowledge_state = p2 ? 1 : 3;
    if (!in_range)
    {
        if (prop->status <= 0 && k_prop_type_info[prop->type].kind == 1)
        {
            prop->status = 1;
            prop_state_refresh(&prop->state, prop->object_index, -1, 0);
        }
        actor_position_data target_data;
        actor_perception_find_target_data(pref->object_index, &target_data);
        actor_position_data sense_data;
        if (actor_perception_find_sense_position(actor_index, &target_data.body, &sense_data))
        {
            actor_prop_state_refresh(actor_index, prop_ref_index, &sense_data, &target_data, true);
            if (k_prop_type_info[pref->type].tracking_mode == 2 && pref->owner_index == -1 &&
                prop_track_weight(prop_ref_index) > datum_get<actor_datum>(actor_data, actor_index)->track_threshold &&
                prop_track(actor_index, prop_ref_index) != -1)
                actor_prop_state_refresh(actor_index, prop_ref_index, &sense_data, &target_data, true);
        }
        if (!p2)
        {
            if (allow_orphanhood(pref))
                actor_stimulus_prop_sighted(actor_index, prop_ref_index, true);
            else
            {
                result = false;
                prop_unacknowledge(actor_index, prop_ref_index);
            }
        }
        if (result)
            actor_stimulus_prop_acknowledged(actor_index, prop_ref_index, no_knowledge);
    }
    return result;
}

// @rdata 0x820221fc
static const float props_avoidance_one = 1.0f;
// @rdata 0x8202251c
static const float props_avoidance_zero = 0.0f;
// @rdata 0x820242d4
static const float props_avoidance_three = 3.0f;
// @rdata 0x820242d8
static const float props_avoidance_two = 2.0f;
static __forceinline float props_avoidance_interpolate(const boid_flock_definition *properties, float aggregate, float one)
{
 const float weight = aggregate > one ? one : aggregate;
 return (properties->far_weight - properties->fallback_weight) * weight + properties->fallback_weight;
}
static __forceinline float props_avoidance_double(float value) { return value * props_avoidance_two; }
static __forceinline void props_avoidance_add(real_vector3d *vector, const real_vector3d *direction, float scale)
{
 float x = vector->i, y = vector->j, z = vector->k;
 float dx = direction->i;
 float dy = direction->j;
 float dz = direction->k;
 vector->i = dx * scale + x;
 vector->j = dy * scale + y;
 vector->k = dz * scale + z;
}
// @address 0x55ca40 prop_compute_avoidance_vector
extern "C" void prop_compute_avoidance_vector(flock_datum *group, s_flock_instance *flags, boid_flock_definition *properties,
    // match: 0x55ca40: pointer __restrict qualifier prevents alias reload
    boid_state *actor, const real_point3d *position, float *__restrict strength, real_vector3d *vector, bool unused, float *result)
{
 float aggregate = props_avoidance_zero;
 float distance = props_avoidance_three;
 memcpy(vector, &k_real_math_origin3d, sizeof(*vector));
 if (properties->danger_radius > props_avoidance_zero) distance = properties->danger_radius;
 if (static_cast<short>(group->danger_mask) > 0) {
  c_data_iterator<player_datum> iterator;
  iterator.m_iterator.absolute_index = -1;
  iterator.m_iterator.index = -1;
  iterator.m_iterator.data = tls_player_data();
  if (iterator.next()) {
   do {
    if (iterator.get_datum()->unit_index != -1) {
     float range = distance;
     prop_avoidance_add_object_repulsion(actor->needs_update, iterator.get_datum()->unit_index, range, position, vector, &aggregate);
    }
   } while (iterator.next());
  }
 }
 if (flags->flags & 0x20UL) {
  actor_iterator iterator;
  props_actor_iterator_new(&iterator, true);
  actor_datum *current = actor_iterator_next(&iterator);
  if (current) {
   do {
    if (current->unit_index != -1) {
     float range = distance;
     prop_avoidance_add_object_repulsion(actor->needs_update, current->unit_index, range, position, vector, &aggregate);
    }
    current = actor_iterator_next(&iterator);
   } while (current);
  }
 }
 float one = props_avoidance_one;
 float weight;
 if (group->death_timer > 0) {
  real_vector3d direction;
  props_battle_direction(position, &group->last_death_position, &direction);
  float length = normalize3d(&direction);
  float radius = group->death_scale * distance;
  if (length < radius) weight = one;
  else if (length < props_avoidance_double(radius)) weight = (props_avoidance_double(radius) - length) / radius;
  else weight = props_avoidance_zero;
  if (weight > props_avoidance_zero) {
   float magnitude = group->death_scale * weight;
   props_avoidance_add(vector, &direction, -magnitude);
   if (magnitude > aggregate) aggregate = magnitude;
  }
 }
 if (aggregate > props_avoidance_zero) {
  float saved_aggregate = aggregate;
  normalize3d(vector);
  float scale = properties->danger_scale * saved_aggregate;
  float x = vector->i, y = vector->j, z = vector->k;
  *strength = aggregate;
  vector->i = x * scale; vector->j = y * scale; vector->k = z * scale;
  if (saved_aggregate > one)
   weight = one;
  else
   weight = saved_aggregate;
  *result = (properties->far_weight-properties->fallback_weight)*weight+properties->fallback_weight;
 }
}

// @address 0x72d348 ?g_stimulus_clump_importance@@3PAW4e_clump_importance@@A
e_clump_importance g_stimulus_clump_importance[] = {
    _clump_importance_none,
    _clump_importance_none,
    _clump_importance_critical,
    _clump_importance_none
};

static __forceinline void prop_dialogue_heard(long actor_index, long prop_ref_index, short dialogue_index)
{
    prop_dialogue_event_internal(actor_index, prop_ref_index, prop_dialogue_event_2, dialogue_index, 0.3f);
}
static __forceinline void actor_stimulus_prop_sound(long actor_index, long prop_ref_index, bool new_acknowledge)
{
    if (datum_get<actor_datum>(actor_data, actor_index)->clump_index != -1)
        prop_dialogue_heard(actor_index, prop_ref_index, -1);
}

// @address 0x5530f0 ?prop_perceive@@YA_NJJFW4e_prop_stimulus@@FJJJ@Z
bool prop_perceive(long actor_index, long object_index, short type, e_prop_stimulus stimulus, short stimulus_decay_ticks,
    long prop_ref_index, long swarm_index, long source_prop_ref_index)
{
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->active) {
        prop_ref_datum *source_prop_ref = 0;
        bool new_acknowledge = false;
        if (source_prop_ref_index != -1)
            source_prop_ref = props_prop_ref_get(source_prop_ref_index);
        if (prop_ref_index == -1) {
            e_clump_importance importance = stimulus == _prop_stimulus_none ? _clump_importance_none : g_stimulus_clump_importance[stimulus];
            bool create = importance > 0;
            prop_ref_index = prop_ref_get_by_object_index(actor_index, object_index, create, importance);
        }
        if (prop_ref_index != -1) {
            prop_ref_datum *prop_ref = props_prop_ref_get(prop_ref_index);
            prop_ref_datum *perception = prop_ref;
            prop_state *state = prop_state_get(prop_ref);
            prop_view *view = prop_view_from_reference(prop_ref);
            result = true;
            if (stimulus > perception->stimulus_type) {
                perception->stimulus_type = (signed char)stimulus;
                prop_ref->stimulus_decay_ticks = (unsigned char)stimulus_decay_ticks;
            }
            if (!(prop_ref->knowledge_state >= 1 && prop_ref->knowledge_state <= 2)) {
                if (prop_ref->knowledge_state < 1) {
                    result = prop_acknowledge(actor_index, prop_ref_index, false);
                    view = prop_view_from_reference(prop_ref);
                    state = prop_state_get(prop_ref);
                    new_acknowledge = true;
                }
                if (result) {
                    actor_position_data target_data;
                    actor_position_data sense_data;
                    if (prop_ref->knowledge_state >= 3) {
                        actor_perception_find_target_data(prop_ref->object_index, &target_data);
                        if (actor_perception_find_sense_position(actor_index, &state->position, &sense_data)) {
                            if (source_prop_ref_index != -1) {
                                prop_state *source_state = prop_state_get(source_prop_ref);
                                if (state != source_state && source_state->flags > state->flags) {
                                    memcpy(state, source_state, sizeof(prop_state));
                                    if (view) {
                                        prop_view *source_view = prop_view_from_reference(source_prop_ref);
                                        if (source_view)
                                            view->sighting_point = source_view->sighting_point;
                                    }
                                    prop_ref->distance = distance3d(&sense_data.body, &state->position);
                                }
                            } else if (state->flags < game_time_globals->time) {
                                actor_prop_state_refresh(actor_index, prop_ref_index, &sense_data, &target_data, true);
                            }
                            if (!new_acknowledge && state->flags == game_time_globals->time) {
                                prop_ref->awareness = actor_visibility_at_point(actor_index, &sense_data, &target_data.head, &target_data.body,
                                    prop_ref->visibility_type, false, props_prop_get(prop_ref->prop_index)->player, 0);
                                prop_ref->maximum_awareness = MAX(prop_ref->maximum_awareness, prop_ref->awareness);
                                if (prop_ref->awareness >= 1)
                                    prop_acknowledge(actor_index, prop_ref_index, true);
                            }
                            if (view) {
                                view->sighting_ticks = (short)(long)((float)game_time_globals->ticks_per_second * 30.0f + 0.5f);
                                if (!new_acknowledge)
                                    actor_prop_update_new_position(actor_index, prop_ref_index, true);
                            }
                        }
                    }
                    switch (type) {
                    case 2:
                        actor_perception_find_target_data(prop_ref->object_index, &target_data);
                        if (actor_perception_find_sense_position(actor_index, &state->position, &sense_data)) {
                            prop_ref->visibility_type = 0;
                            prop_ref->awareness = actor_visibility_at_point(actor_index, &sense_data, &target_data.head, &target_data.body,
                                prop_ref->visibility_type, false, false, 0);
                            prop_ref->maximum_awareness = MAX(prop_ref->maximum_awareness, prop_ref->awareness);
                            if (prop_ref->awareness >= 1 && !(prop_ref->knowledge_state >= 1 && prop_ref->knowledge_state <= 2))
                                prop_acknowledge(actor_index, prop_ref_index, true);
                        }
                        break;
                    case 3:
                        actor_stimulus_prop_sound(actor_index, prop_ref_index, new_acknowledge);
                        break;
                    }
                } else if (new_acknowledge) {
                    prop_unacknowledge(actor_index, prop_ref_index);
                }
            }
            actor_compute_prop_salience(actor_index, prop_ref_index, _prop_stimulus_none);
        }
    }
    return result;
}


// @address 0x514268 ?prop_orphan@@YAXJJ_N@Z
void prop_orphan(long actor_index, long prop_ref_index, bool new_prop)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    prop_ref_datum *prop_ref = props_prop_ref_get(prop_ref_index);
    long object_index = prop_ref->object_index;
    e_object_type object_type = (e_object_type)object_header_get(object_index)->object_type;
    object_datum *object = object_get(object_index);
    if (object_type == _object_type_projectile || object_type == _object_type_equipment)
    {
        if (prop_ref->awareness > 0 && (object->damage_owner.team == NONE ||
            !game_team_is_friendly((e_game_team)actor->team, (e_game_team)object->damage_owner.team)))
        {
            prop_dialogue_event_internal(actor_index, prop_ref_index, prop_dialogue_event_5, NONE, 0.3f);
        }
        long owner_object_index = object->damage_owner.object_index;
        if (owner_object_index != NONE && object_try_and_get_and_verify_type(owner_object_index, NONE) &&
            game_team_is_enemy((e_game_team)actor->team, (e_game_team)object->damage_owner.team))
        {
            long owner_prop_ref_index = prop_ref_get_by_object_index(actor_index, owner_object_index, false, _clump_importance_none);
            if (owner_prop_ref_index != NONE)
            {
                bool perceive = false;
                prop_ref_datum *owner_prop_ref = props_prop_ref_get(owner_prop_ref_index);
                prop_view *view = prop_view_from_reference(owner_prop_ref);
                if (owner_prop_ref->status < 1)
                    perceive = true;
                else if (view)
                    perceive = view->perception_time + 3 * tls_game_time_globals()->ticks_per_second < tls_game_time_globals()->time;
                if (perceive && prop_perceive(actor_index, owner_object_index, 1, _prop_stimulus_none, 0, owner_prop_ref_index, NONE, NONE))
                {
                    prop_state *state = prop_state_get(owner_prop_ref);
                    prop_view *owner_view = prop_view_from_reference(owner_prop_ref);
                    if (state)
                    {
                        real_point3d origin;
                        object_get_origin(object_index, &origin);
                        state->target_position = origin;
                        state->position = origin;
                        long index = object_index;
                        long root_index = NONE;
                        while (index != NONE)
                        {
                            root_index = index;
                            index = object_get(index)->parent_object_index;
                        }
                        state->location = object_get(root_index)->location;
                        c_ai_point3d ai_point;
                        collision_result collision;
                        collision.global_material.m_index = NONE;
                        state->pathfinding_sector = pathfinding_find_ground_sector(&origin, &k_real_math_identity_matrix_storage.up,
                            &ai_point, &collision, 0.5f);
                        state->pathfinding_point = ai_point;
                    }
                    if (owner_view && object->parent_object_index == NONE)
                    {
                        scale_vector3d(&object->linear_velocity, -1.0f, &owner_view->sighting_direction);
                        owner_view->sighting_direction.k = 0.0f;
                        normalize3d(&owner_view->sighting_direction);
                    }
                }
            }
        }
    }
}

t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_command_script_allocator> g_ai_props_allocator;
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tracking_data_allocator> g_ai_prop_ref_allocator;
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_system_allocator> g_ai_prop_view_allocator;

