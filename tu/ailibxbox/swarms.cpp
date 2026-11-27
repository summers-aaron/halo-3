// ailibxbox/swarms.obj: from the records 0xad6a0 0x2263d8 0x226568 0x524580 0x524598 0x5245b8 0x524680 0x524810 0x524c80 0x524d80 0x524e58 0x524f48 0x525030 0x5251d8 0x525328 0x525488 0x526198 0x526308 0x526458 0x5267d8 0x5268f0 0x526b00 0x526b80 0x526d28 0x526e48 0x526f60 0x527060 0x5271e8 0x527330 0x527560 0x5276a0 0x5277c8 0x527898 0x527990 0x527b10 0x527d58 0x527fe8 0x5280a0 0x5281e0 0x528238 0x5282a8 0x528348 0x528368 0x528408 0x528420 0x528440 0x5284e0 0x5284f8 0x528518 0x70d088 0x70d0a0
#include "tag_files/tag_groups.h"
#include "math/real_math_constants.h"
// @address 0x3aa90 k_real_math_origin3d
#include "physics/character_physics_globals.h"
// @address 0x3aa90 k_real_math_zero_vector3d_storage
#include "scenario/scenario_kill_trigger_volumes_state_types.h"
#include "ai/ai_tls_data.h"
#include "ai/ai_information_data.h"
#include "memory/tls_globals.h"
#include "creatures/creature_datum.h"
#include "units/unit_types.h"
#include "physics/character_physics_mode_ground_datum.h"
#include "math/real_math.h"
#include "math/point_math.h"
#include "math/real_constant_pool.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_types.h"
#include "ai/sector_ref.h"
#include "objects/object_iterator.h"
#include "units/motor_enums.h"
#include "ai/prop_ref_datum.h"
#include <string.h>
#include "ai/actors.h"
#include "cache/restricted_memory_regions.h"
#include "motor/action_system.h"
#include "objects/objects.h"
#include "ai/ai.h"
#include "ai/ai_flocks.h"
#include "ai/clump.h"
#include "ai/props.h"
#include "ai/squads.h"
#include "ai/swarms.h"
#include "creatures/creatures.h"
#include "items/projectiles.h"
#include "motor/motor_system.h"
#include "objects/lights.h"
#include "physics/havok.h"
#include "ai/character_definitions.h"
#include "memory/data.h"
#include "motor/motor_animation.h"
#include "units/motor_task.h"
#include "math/real_math_types.h"
#include "cache/restricted_memory.h"
#include "ai/prop_state.h"
#include "ai/prop_datum.h"
#include "ai/actor_datum.h"
#include "ai/swarm_creature_iterator_types.h"
#include "ai/prop_stimulus_types.h"
#include "cache/cache_files.h"
#include "ai/swarm_spawner_types.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_registration.h"
#include "ai/actor_moving_globals.h"
#include "cache/restricted_memory_regions_inline.h"
#include "scenario/scenario_kill_trigger_volumes.h"
#include "ai/swarm_spawner_definition.h"
#include "ai/actor_type_flood_types.h"
#include "ai/squad_definitions.h"
#include "objects/object_placement.h"
#include "math/matrix_math.h"
#include "objects/object_type_definition.h"
#include "scenario/scenario_tag.h"
#include "scenario/scenario.h"
#include "ai/ai_reference_frame.h"
#include <math.h>
#include "scenario/scenario_objects.h"
#include "cseries/crt_xdk.h"
#include "units/action_request.h"
#include "ai/dialogue_event.h"
#include "ai/ai_dialogue.h"
#include "cseries/cseries.h"
#include "objects/object_header.h"
#include "objects/object_markers_inline.h"
#include "math/random_math_range.h"
#include "game/game_time_inline.h"
extern "C" {
#include "ai/swarm_spawner_globals.h"
#include "cseries/string_literal_pool.h"
}
// @address 0x11df724 g_swarm_data
// @address 0x11df7bc g_swarm_spawners
// @address 0x11df880 g_swarm_nondeterministic_counter
// @flags /TP /O2 /Oc
// @address 0x2263d8 ?__tls_set_g_spawner_globals_allocator@@YAXPAX@Z
// @address 0x527990 swarm_spawner_new_from_defintion
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z
// @address 0x5245a8 ?__tls_set_g_light_render_data_allocator@@YAXPAX@Z

__declspec(thread) s_data_array *g_swarm_data;
__declspec(thread) s_swarm_spawner *g_swarm_spawners;
__declspec(thread) unsigned short *g_swarm_nondeterministic_counter;

struct s_scenario_object_block {
    long count;
    TAG_POINTER(char) address;
};

void __tls_set_g_spawner_globals_allocator(void *allocator)
{
    g_scenario_kill_trigger_volumes_state = (s_kill_trigger_volume_runtime_state *)allocator;
}

namespace {
static __forceinline swarm_datum *swarm_get(long index)
{
    return datum_get<swarm_datum>(g_swarm_data, index);
}
}

namespace {
static __forceinline swarm_creature_state *creature_link(creature_datum *creature)
{
    swarm_creature_state *link;
    if (creature->creature.link_type == 0)
        link = (swarm_creature_state *)((char *)creature + creature->creature.creature_reference.offset);
    else
        link = 0;
    return link;
}
static __forceinline creature_datum *swarms_creature_iterator_next_inline(swarm_creature_iterator *iterator)
{
    creature_datum *creature = 0;
    long creature_index = iterator->next_object_index;
    if (creature_index != -1) {
        iterator->current_object_index = creature_index;
        creature = (creature_datum *)object_get(creature_index);
        swarm_creature_state *link = creature_link(creature);
        if (link != 0) iterator->next_object_index = link->next_swarm_creature_index;
        else iterator->next_object_index = -1;
    }
    return creature;
}
static __forceinline void creature_control(long creature_index, const s_creature_control_data *control)
{
    creature_datum *creature = (creature_datum *)object_get(creature_index);
    creature->creature.control = *control;
}
}
void swarms_dispose_from_old_map()
{
    c_object_iterator<creature_datum> iterator;
    iterator.begin(0x800, 0);
    while (iterator.next()) {
        swarm_creature_state *link = creature_link(iterator.get_datum());
        if (link != 0) {
            link->member_swarm_index = -1;
            link->swarm_index = -1;
            link->next_swarm_creature_index = -1;
        }
    }
    g_swarm_data->valid = false;
}
void actor_detach_swarm(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    if (actor->swarm_index != -1) {
        swarm_datum *swarm = swarm_get(actor->swarm_index);
        long swarm_index = actor->swarm_index;
        actor->swarm_index = -1;
        swarm->actor_index = -1;
        swarm_creature_iterator iterator;
        iterator.current_object_index = -1;
        iterator.next_object_index = swarm_get(swarm_index)->first_object_index;
        creature_datum *creature;
        while ((creature = swarms_creature_iterator_next_inline(&iterator)) != 0) {
            swarm_creature_state *link = creature_link(creature);
            if (link != 0)
                link->swarm_index = -1;
        }
    }
}

void swarm_spawner_end(short spawner_index)
{
    s_swarm_spawner *spawner = &g_swarm_spawners[spawner_index];
    if (spawner->swarm_index != -1) {
        swarm_datum *swarm = swarm_get(spawner->swarm_index);
        swarm->spawning = false;
    }
    spawner->starting_location_index = -1;
    spawner->swarm_index = -1;
    spawner->creature_count = 0;
}

void swarm_spawners_handle_deleted_object(long object_index)
{
    for (short spawner_index = 0; spawner_index < 5; spawner_index++) {
        if (g_swarm_spawners[spawner_index].starting_location_index == object_index) {
            s_swarm_spawner *spawner = &g_swarm_spawners[spawner_index];
            if (spawner->swarm_index != -1) {
                swarm_datum *swarm = swarm_get(spawner->swarm_index);
                swarm->spawning = false;
            }
            spawner->starting_location_index = -1;
            spawner->swarm_index = -1;
            spawner->creature_count = 0;
        }
    }
}
// @address 0x528238 ?swarm_creature_iterator_next@@YAPAUcreature_datum@@PAUswarm_creature_iterator@@@Z
// match: 0x528238: pointer __restrict qualifier prevents alias reload
creature_datum *swarm_creature_iterator_next(swarm_creature_iterator *__restrict iterator)
{
  creature_datum *creature = 0;
  long object_index = iterator->next_object_index;
  if (object_index != -1) {
    iterator->current_object_index = object_index;
    creature = (creature_datum *)((s_object_header *)object_header_data->data)[(unsigned short)object_index].datum;
    swarm_creature_state *link;
    if (creature->creature.link_type == 0)
      link = (swarm_creature_state *)((char *)creature + creature->creature.creature_reference.offset);
    else
      link = 0;
    if (link != 0)
      iterator->next_object_index = link->next_swarm_creature_index;
    else
      iterator->next_object_index = -1;
  }
  return creature;
}

template<>
void *t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_spawner_globals_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x528440: assume materializes the alignment_bits argument as li r7,0 for add_member
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_register_member(1, name, type, size, alignment_bits, __tls_set_g_light_render_data_allocator, 0, 0);
    m_thread_id = g_registered_thread_index;
    return restricted_region_get_member_address(1, m_member_index);
}

// @address 0x524598 ?__tls_set_g_swarm_spawners_allocator@@YAXPAX@Z

// @address 0x226568 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_scenario_kill_trigger_volumes_state_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_kill_trigger_volumes_state_allocator>::deallocate(void *allocation)
{
    int value = m_member_index;
    g_scenario_kill_trigger_volumes_state = 0;
    restricted_region_free_member(1, value);
}

// 0x524580
// @address 0x524580 ?__tls_set_g_swarm_data_allocator@@YAXPAX@Z
void __tls_set_g_swarm_data_allocator(void *allocator)
{
    data_set_new_base_address(&g_swarm_data, (s_data_array *)allocator);
}

// 0x524598
void __tls_set_g_swarm_spawners_allocator(void *allocator)
{
    g_swarm_spawners = (s_swarm_spawner *)allocator;
}

// @address 0x524c80 actor_attach_swarm
void actor_attach_swarm(long actor_index, long swarm_index)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    swarm_datum *swarm = swarm_get(swarm_index);
    if (actor->swarm_index != swarm_index) {
        if (actor->swarm_index != -1) actor_detach_swarm(actor_index);
        if (swarm->actor_index != -1) actor_detach_swarm(swarm->actor_index);
        actor->swarm_index = swarm_index;
        swarm->actor_index = actor_index;
        swarm_creature_iterator iterator;
        iterator.next_object_index = swarm_get(swarm_index)->first_object_index;
        iterator.current_object_index = -1;
        creature_datum *creature;
        while ((creature = swarms_creature_iterator_next_inline(&iterator)) != 0) {
            swarm_creature_state *link = creature_link(creature);
            if (link) link->swarm_index = actor_index;
        }
    }
}

// 0x524f48
// @address 0x524f48 ?swarm_add_creature@@YA_NJJ@Z
bool swarm_add_creature(long swarm_index, long creature_index)
{
    bool added = false;
    swarm_datum *swarm = swarm_get(swarm_index);
    creature_datum *creature = (creature_datum *)object_get(creature_index);
    if (creature->creature.link_type == 0) {
        swarm_creature_state *link = creature_link(creature);
        if (link != 0 && link->member_swarm_index == -1) {
            link->next_swarm_creature_index = swarm->first_object_index;
            swarm->first_object_index = creature_index;
            link->member_swarm_index = swarm_index;
            link->swarm_index = swarm->actor_index;
            creature_set_actively_controlled(creature_index, true);
            unsigned char nondeterministic = (creature->creature.flags >> 9) & 1;
            if (nondeterministic != 0) {
                unsigned short *count = (unsigned short *)g_swarm_nondeterministic_counter;
                *count = *count + 1;
            }
            added = true;
            swarm->creature_count++;
        }
    }
    return added;
}

// @address 0x5267d8 ?swarm_creature_control@@YAXJ@Z
void swarm_creature_control(long creature_index)
{
    creature_datum *creature = (creature_datum *)object_get(creature_index);
    swarm_creature_state *state = creature->creature.link_type == 0
        ? (swarm_creature_state *)((char *)object_get(creature_index) + creature->creature.creature_reference.offset) : 0;
    if (state) {
        s_creature_control_data control;
        creature_control_clear(&control);
        if (state->jump_requested) control.flags |= 16; else control.flags &= ~16;
        control.facing = state->desired_forward;
        control.up = state->desired_up;
        control.movement = state->movement;
        if (state->new_action) {
            control.flags |= 1;
            control.action = state->action;
            control.animation = state->animation;
            state->new_action = false;
        }
        ((creature_datum *)object_get(creature_index))->creature.control = control;
    }
}

// @address 0x528348 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_swarm_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_swarm_data_allocator>::deallocate(void *allocation)
{
    g_swarm_data = 0;
    restricted_region_free_member(1, m_member_index);
}

// @address 0x5284f8 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_spawner_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_spawner_globals_allocator>::deallocate(void *allocation)
{
    g_swarm_nondeterministic_counter = 0;
    restricted_region_free_member(1, m_member_index);
}

// @address 0x528368 ?reserve_memory@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_swarm_spawners_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
template<>
void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_swarm_spawners_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    int member_index = restricted_memory_register_member(&g_restricted_regions[1], name, type, size, 0,
        __tls_set_g_swarm_spawners_allocator, 0, 0);
    unsigned int offset = g_restricted_regions[1].get_member_offset(member_index);
    void *address = g_restricted_address[1] + offset;
    int owner = g_registered_thread_index;
    m_thread_id = owner;
    m_member_index = member_index;
    g_swarm_spawners = (s_swarm_spawner *)address;
    return g_restricted_address[1] + g_restricted_regions[1].get_member_offset(member_index);
}

// @address 0x528408 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_swarm_spawners_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_swarm_spawners_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x528420 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_swarm_spawners_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_swarm_spawners_allocator>::deallocate(void *allocation)
{
    int value = m_member_index;
    g_swarm_spawners = 0;
    restricted_region_free_member(1, value);
}

// @address 0x5284e0 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_spawner_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_spawner_globals_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// 0x527990

// match: 0x527990: __restrict on definition preserves swarm_spawner_new_from_defintion's loads
extern "C" void swarm_spawner_new_from_defintion(swarms_spawner_definition *__restrict definition, s_swarm_spawner *spawner)
{
    spawner->definition_index = definition->reference.definition_index;
        spawner->starting_location_index = definition->reference.starting_location_index; spawner->marker_name = definition->reference.marker_name;
    if (definition->reference.marker_name)
    {
        memcpy(&spawner->matrix, &k_real_math_identity_matrix_storage, sizeof(real_matrix4x3));
        spawner->node_index = -1;
    }
    else
    {
        memcpy(&spawner->matrix, definition->matrix, sizeof(real_matrix4x3));
        spawner->node_index = definition->node_index;
    }
    unsigned long *seed = tls_random_seed();
    game_time_globals_definition *game_time = game_time_globals;
    spawner->flags = 0;
    spawner->swarm_index = -1;
    spawner->creature_count = definition->count;
    spawner->minimum_spawn_delay = definition->minimum_delay;
    spawner->maximum_spawn_delay = definition->maximum_delay;
    spawner->spawn_parameters[0] = definition->values[0];
    spawner->spawn_parameters[1] = definition->values[1];
    spawner->spawn_parameters[2] = definition->values[2];
    spawner->spawn_parameters[3] = definition->values[3];
    spawner->spawn_parameters[4] = definition->values[4];
    spawner->spawn_parameters[5] = definition->values[5];
    spawner->spawn_parameters[6] = definition->values[6];
    spawner->spawn_parameters[7] = definition->values[7];
    spawner->spawn_ticks = (short)game_seconds_to_ticks_round(_real_random_range(seed, definition->minimum_delay, definition->maximum_delay));
    if (definition->count <= 1) spawner->flags |= 2;
}

// @address 0x5251d8 ?swarm_target_get_info@@YA_NFJPAJPATreal_point3d@@1@Z
// match: 0x5251d8: pointer __restrict qualifier prevents alias reload
bool swarm_target_get_info(short type, long index, long * __restrict object_index, union real_point3d * origin, union real_point3d * __restrict center)
{
 bool result=false;
 *object_index=-1;
 if(index!=-1) {
  switch(type) {
  case 1:
  {
      creature_datum *datum = (creature_datum *)object_get(index);
      *object_index = index;
      object_get_origin(index, origin);
      *center = datum->object.bounding_center;
      result = true;
      break;
  }
  case 2:
  {
      prop_ref_datum *ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), index);
      prop_state *state = (prop_state *)prop_state_get(ref);
      *object_index = ref->object_index;
      *origin = state->position;
      *center = state->head_position;
      result = true;
      break;
  }
  case 3:
  {
      actor_datum *datum = datum_get<actor_datum>(actor_data, index);
      *object_index = datum->unit_index;
      *origin = datum->position.body;
      *center = datum->position.head;
      result = true;
      break;
  }
  default: *object_index=-1; break;
  }
 }
 return result;
}
// @address 0x526b00 creature_state_refresh
extern "C" void creature_state_refresh(long creature_index, swarm_creature_state *state)
{
 creature_datum *creature=(creature_datum *)object_get(creature_index);
 object_get_origin(creature_index,&state->position);
 state->target_object_index=-1;
 if(creature->object.parent_object_index!=-1) state->flags |= 1;
 else state->flags &= ~1;
}

namespace {
static __forceinline creature_datum *swarm_object_get(long object_index)
{
    return (creature_datum *)((s_object_header *)object_header_data->data)[(unsigned short)object_index].datum;
}
static __forceinline actor_datum *swarm_actor_get(long actor_index)
{
    unsigned long absolute_index = DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index);
    return &((actor_datum *)actor_data->data)[absolute_index];
}
}

// @address 0xad6a0 ?swarm_spawners_update@@YAXXZ
void swarm_spawners_update()
{
    for (short spawner_index = 0; spawner_index < 5; spawner_index++) {
        s_swarm_spawner *spawner = &g_swarm_spawners[spawner_index];
        if ((spawner->flags & 1) && spawner->creature_count > 0) {
            spawner->spawn_ticks--;
            if (spawner->spawn_ticks <= 0) {
                swarm_spawner_spawn_creature(spawner_index);
                float delay = _real_random_range(tls_random_seed(), spawner->minimum_spawn_delay, spawner->maximum_spawn_delay);
                spawner->spawn_ticks = (short)game_seconds_to_ticks_round(delay);
            }
            spawner->creature_count--;
            if (spawner->creature_count <= 0)
                swarm_spawner_end(spawner_index);
        }
    }
}

// @address 0x524e58 ?swarm_delete@@YAXJ@Z
void swarm_delete(long swarm_index)
{
    swarm_datum *swarm = swarm_get(swarm_index);
    if (swarm->creature_count > 0) {
        while (swarm->first_object_index != -1)
            swarm_remove_creature(swarm_index, swarm->first_object_index, true, false);
    }
    if (swarm->actor_index != -1) {
        actor_datum *actor = swarm_actor_get(swarm->actor_index);
        if (actor->swarm_index == swarm_index)
            actor->swarm_index = -1;
    }
    if (swarm->spawning) {
        for (short spawner_index = 0; spawner_index < 5; spawner_index++) {
            s_swarm_spawner *spawner = &g_swarm_spawners[spawner_index];
            if (spawner->swarm_index == swarm_index)
                spawner->swarm_index = -1;
        }
    }
    ::datum_delete(g_swarm_data, swarm_index);
}

// @address 0x525030 ?swarm_remove_creature@@YA_NJJ_N0@Z
bool swarm_remove_creature(long swarm_index, long creature_index, bool delete_creature, bool delete_empty_swarm)
{
    bool removed = false;
    swarm_datum *swarm = swarm_get(swarm_index);
    long *object_index = &swarm->first_object_index;
    while (*object_index != -1) {
        creature_datum *creature = swarm_object_get(*object_index);
        swarm_creature_state *link = creature_link(creature);
        if (!link)
            break;
        if (*object_index == creature_index) {
            *object_index = link->next_swarm_creature_index;
            creature_set_actively_controlled(creature_index, false);
            removed = true;
            break;
        }
        object_index = &link->next_swarm_creature_index;
    }
    if (removed) {
        creature_datum *creature = swarm_object_get(creature_index);
        swarm_creature_state *link = creature_link(creature);
        if (delete_creature) {
            ::clump_handle_deleted_object(creature_index);
            object_delete(creature_index);
        }
        if (link) {
            link->member_swarm_index = -1;
            link->swarm_index = -1;
            link->next_swarm_creature_index = -1;
        }
        swarm->creature_count--;
        unsigned char nondeterministic = (creature->creature.flags >> 9) & 1;
        if (nondeterministic != 0) {
            short *count = (short *)g_swarm_nondeterministic_counter;
            *count = *count - 1;
        }
        if (swarm->creature_count == 0 && delete_empty_swarm) {
            if (swarm->actor_index != -1)
                ::actor_delete(swarm->actor_index, false);
            swarm_delete(swarm_index);
        }
    }
    return removed;
}

namespace {
static __forceinline bool swarm_actor_infection_immune(long actor_index)
{
    character_general_properties *properties = character_definition_general_properties_get(swarm_actor_get(actor_index)->character_definition_index);
    bool immune = false;
    if (properties && (properties->flags & 0x80))
        immune = true;
    return immune;
}
}

namespace {
}
// @address 0x526e48 ?swarm_perception_creature_from_swarm@@YAJJPAUactor_position_data@@J@Z
long swarm_perception_creature_from_swarm(long swarm_index, actor_position_data *position, long preferred_creature_index)
{
    long closest_creature_index = -1;
    float closest_distance = 3.402823466e+38f;
    swarm_creature_iterator iterator;
    swarm_creature_iterator_new(swarm_index, &iterator);
    creature_datum *creature;
    while ((creature = swarms_creature_iterator_next_inline(&iterator)) != 0) {
        swarm_creature_state *link = creature_link(creature);
        float distance = distance_squared3d(&link->position, &position->body);
        if (link->flags & 1)
            distance *= 2.25f;
        else if (iterator.current_object_index == preferred_creature_index)
            distance *= 0.36f;
        if (distance < closest_distance) {
            closest_distance = distance;
            closest_creature_index = iterator.current_object_index;
        }
    }
    return closest_creature_index;
}
// @address 0x526d28 ?swarm_perception_find_sense_position@@YA_NJPBTreal_point3d@@PAUactor_position_data@@@Z
bool swarm_perception_find_sense_position(long swarm_index, const real_point3d *position, actor_position_data *out)
{
    swarm_datum *swarm = swarm_get(swarm_index);
    float best_distance = 3.402823466e+38f;
    long best = -1;
    bool result = false;
    swarm_creature_iterator iterator;
    swarm_creature_iterator_new(swarm_index, &iterator);
    creature_datum *creature;
    while ((creature = swarms_creature_iterator_next_inline(&iterator)) != 0) {
        swarm_creature_state *link = creature_link(creature);
        float distance = distance_squared3d(&link->position, position);
        if (distance < best_distance) {
            best_distance = distance;
            best = iterator.current_object_index;
        }
    }
    if (best != -1) {
        actor_input_sample_position(swarm->actor_index, best, out);
        result = true;
    }
    return result;
}

namespace {

}
// @address 0x526f60 ?swarm_handle_creature_death@@YAXJJF@Z
void swarm_handle_creature_death(long creature_index, long damage_owner, short damage_type)
{
    creature_datum *creature = swarm_object_get(creature_index);
    swarm_creature_state *link = creature_link(creature);
    if (link != 0 && link->swarm_index != -1 && link->member_swarm_index != -1) {
        swarm_datum *swarm = swarm_get(link->member_swarm_index);
        long responsible_unit = ai_get_unit_responsible_for_damage(damage_owner, damage_type != 9);
        float damage = 1.0f / swarm->creature_count;
        swarm->death_count++;
        swarm->last_death_time = game_time_get();
        actor_handle_damage(link->swarm_index, damage_type, 0, responsible_unit, damage, &k_real_math_zero_vector3d_storage);
    }
}

// @address 0x5271e8 swarm_can_attach_to_unit
// match: 0x5271e8: pointer __restrict qualifier prevents alias reload
extern "C" bool swarm_can_attach_to_unit(long unit_index, bool *__restrict infectable)
{
    bool result = true;
    if (infectable) {
        *infectable = false;
        s_object_header *header = object_header_get(unit_index);
        if ((1 << header->object_type) & 1) {
            unit_datum *unit = (unit_datum *)header->datum;
            if (unit->object.body_vitality < 0.4f && unit->unit.player_index == -1) {
                if (unit->unit.actor_index == -1 || !swarm_actor_infection_immune(unit->unit.actor_index))
                    *infectable = true;
            }
        }
    }
    if (action_executing(unit_index, _action_get_infected) || action_executing(unit_index, _action_hostile_morph) || action_executing(unit_index,
        _action_morph))
        result = false;
    return result;
}

// @address 0xfcf30 ?internal_object_get_markers_by_string_id@@YAFJJPAUobject_marker@@F_N@Z
// @address 0xade58 ?matrix4x3_transform_point@@YAPATreal_point3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0xed598 ?matrix4x3_transform_vector@@YAPATreal_vector3d@@PBUreal_matrix4x3@@PBT1@PAT1@@Z
// @address 0xc1a90 ?euler_angles2d_from_vector3d@@YAPATreal_euler_angles2d@@PAT1@PBTreal_vector3d@@@Z

static __forceinline void swarms_starting_location_point_set(c_ai_point3d *point, const real_point3d *source)
{
    point->point = *source;
    point->location[0] = -1;
    point->location[1] = -1;
}

static __forceinline bool swarm_creature_flag_test(unsigned short flags, long bit)
{
    return ((flags >> bit) & 1) != 0;
}
// 0x526198

// @address 0x525328
void creature_generate_up_vector(long creature_index, swarm_creature_state *state)
{
    bool generated = false;
    creature_datum *creature = (creature_datum *)object_get(creature_index);
    if (creature->creature.physics.m_mode == 1 && !swarm_creature_flag_test(creature->creature.flags, 5)) {
        state->desired_up = *creature->creature.physics.get_mode_ground()->get_ground_plane();
        real_vector3d left;
        cross_product3d(&state->desired_up, &state->desired_forward, &left);
        if (normalize3d(&left) > 0.0f) {
            cross_product3d(&left, &state->desired_up, &state->desired_forward);
            if (normalize3d(&state->desired_forward) != 0.0f) {
                generated = true;
            }
        }
    } else {
        generate_up_vector3d(&state->desired_forward, &state->desired_up);
        generated = true;
    }
    if (!generated)
        object_get_orientation(creature_index, &state->desired_forward, &state->desired_up);
}
// @address 0x526198 ?swarm_creature_infect@@YA_NJJPBUobject_marker@@PBTreal_vector3d@@@Z
bool swarm_creature_infect(long creature_index, long parent_index, struct object_marker const * marker, union real_vector3d const * direction)
{
    creature_datum *creature = (creature_datum *)object_get(creature_index);
    bool result = true;
    bool orientation = false;
    real_vector3d left, forward;
    cross_product3d(&marker->matrix.forward, direction, &left);
    if (normalize3d(&left) > 0.0f) {
        cross_product3d(&left, &marker->matrix.forward, &forward);
        if (normalize3d(&forward) > 0.0f) orientation = true;
    }
    if (orientation) object_set_position_direct(creature_index, &marker->matrix.position, &forward, &marker->matrix.forward, 0, false);
    else object_set_position_direct(creature_index, &marker->matrix.position, 0, 0, 0, false);
    object_attach_to_node_with_type(parent_index, creature_index, marker->node_index);
    swarm_creature_state *state = creature->creature.link_type == 0
        ? (swarm_creature_state *)((char *)creature + creature->creature.creature_reference.offset) : 0;
    if (state) { state->new_action = true; state->action = 138; state->animation = 594; }
    return result;
}

// @address 0x527560 ?swarm_freeze@@YAXJ@Z
void swarm_freeze(long swarm_index)
{
    swarm_creature_iterator iterator;
    iterator.next_object_index = swarm_get(swarm_index)->first_object_index;
    iterator.current_object_index = -1;
    for (;;) {
        creature_datum *creature = swarms_creature_iterator_next_inline(&iterator);
        if (creature == 0)
            break;
        s_creature_control_data control;
        memset(&control, 0, sizeof(control));
        control.action = 138;
        control.animation = 12;
        control.facing = k_real_math_identity_matrix_storage.forward;
        control.up = k_real_math_identity_matrix_storage.up;
        ::object_get_orientation(iterator.current_object_index, &control.facing, &control.up);
        swarm_object_get(iterator.current_object_index)->creature.control = control;
        swarm_object_get(iterator.current_object_index)->creature.flags &= ~1;
    }
}

// @address 0x5277c8 ?swarm_spawner_new@@YAFPAUswarms_spawner_definition@@@Z
short swarm_spawner_new(swarms_spawner_definition *definition)
{
    short spawner_index = -1;
    for (short index = 0; index < 5; index = (short)(index + 1))
    {
        if (g_swarm_spawners[index].creature_count <= 0)
        {
            spawner_index = index;
            break;
        }
    }
    if (spawner_index != -1)
    {
        bool success = false;
        if (*(short *)g_swarm_nondeterministic_counter < 25)
        {
            if (definition != 0) swarm_spawner_new_from_defintion(definition, (s_swarm_spawner *)&g_swarm_spawners[spawner_index]);
            actor_starting_location_definition starting_location;
            success = swarm_spawner_starting_location_new(&starting_location, spawner_index);
        }
        if (success == 0) spawner_index = -1;
    }
    return spawner_index;
}

// Gives a newly spawned creature the spawner's form lifetime (a random value in its range, in ticks).
// @address 0x527898 ?spawner_initialize_creature@@YAXFJ@Z
void spawner_initialize_creature(short spawner_index, long object_index)
{
    s_swarm_spawner *spawner = &g_swarm_spawners[spawner_index];
    creature_datum *creature = swarm_object_get(object_index);
    swarm_spawner_accelerate_creature(spawner_index, object_index);
    if (spawner->form_lifetime_minimum > 0.0f && spawner->form_lifetime_maximum > 0.0f) {
        float lifetime = _real_random_range(tls_random_seed(), spawner->form_lifetime_minimum, spawner->form_lifetime_maximum);
        creature->creature.form_lifetime = (short)game_seconds_to_ticks_round(lifetime);
    }
}

static const float k_swarms_degrees_to_radians = 0.017453292f;

static __forceinline void swarms_perlin_state_initialize(s_perlin_state *state)
{
    state->elapsed = 0;
    state->period = -1;
    state->previous = k_real_math_origin3d;
}
// @address 0x524810 ?swarm_create_creature@@YAJJJJPAUactor_starting_location_definition@@_N1@Z
long swarm_create_creature(long swarm_index, long character_index, long definition_index,
    actor_starting_location_definition *starting_location, bool exact_position, bool nondeterministic)
{
    object_placement_data placement;
    long object_index = -1;
    bool success = false;
    object_placement_data_new(&placement, definition_index, -1, 0);
    ai_point_get_position(&starting_location->position, &placement.position);
    if ((starting_location->initial_movement_mode & 2) != 0) {
        vectors3d_from_euler_angles2d(&placement.forward, &placement.up, &starting_location->facing);
    } else {
        real_vector3d angle;
        vector3d_from_angle(&angle, _real_random_range(tls_random_seed(), -3.1415927f, 3.1415927f));
        if ((starting_location->initial_movement_mode & 1) != 0) {
            float velocity = _real_random_range(tls_random_seed(), 2.0f, 3.0f);
            float pitch = _real_random_range(tls_random_seed(), 15.0f * k_swarms_degrees_to_radians, 1.5707964f);
            placement.forward = angle;
            real_vector3d up;
            up.i = 0.0f;
            up.j = 0.0f;
            up.k = 1.0f;
            real_vector3d axis;
            axis.i = angle.j * up.k - angle.k * up.j;
            axis.j = angle.k * up.i - angle.i * up.k;
            axis.k = angle.i * up.j - angle.j * up.i;
            rotate_vector_about_axis(&angle, &axis, (float)sin(pitch), (float)cos(pitch));
            point_from_line3d(&placement.position, &angle, 0.1f, &placement.position);
            point_from_line3d(&placement.position, &up, 0.1f, &placement.position);
            placement.linear_velocity.i = angle.i * velocity;
            placement.linear_velocity.j = angle.j * velocity;
            placement.linear_velocity.k = angle.k * velocity;
        } else {
            if (!exact_position) {
                real_vector3d displacement = { _real_random_range(tls_random_seed(), -0.5f, 0.5f), _real_random_range(tls_random_seed(), -0.5f, 0.5f), 0.0f };
                point_from_line3d(&placement.position, &displacement, 1.0f, &placement.position);
            }
            placement.forward = angle;
        }
    }
    if (starting_location->actor_variant == 0)
        placement.model_variant_index = actor_select_character_variant(character_index);
    else
        placement.model_variant_index = starting_location->actor_variant;
    placement.destroyed_constraints = 0;
    placement.ai_state_size = 192;
    placement.ai_state_alignment_bits = 2;
    object_index = object_new(&placement);
    if (object_index != -1) {
        creature_datum *creature = swarm_object_get(object_index);
        if (creature->creature.link_type == 0) {
            swarm_creature_state *state = (swarm_creature_state *)((char *)creature + creature->creature.creature_reference.offset);
            if (state != 0) {
                state->flags = 0;
                state->swarm_index = -1;
                state->prop_index = -1;
                state->next_swarm_creature_index = -1;
                state->target_object_index = -1;
                state->member_swarm_index = -1;
                state->position = k_real_math_origin3d;
                state->prop_ref_index = -1;
                state->returning_to_anchor = false;
                state->stuck_ticks = 0;
                state->stuck_position = k_real_math_origin3d;
                swarms_perlin_state_initialize(&state->movement_noise);
                if (nondeterministic)
                    creature->creature.flags |= 0x200;
                if (swarm_add_creature(swarm_index, object_index)) {
                    creature_state_refresh(object_index, state);
                    success = true;
                }
            }
        }
        if (!success) {
            object_delete(object_index);
            object_index = -1;
        }
    }
    return object_index;
}

static __forceinline s_scenario_object_block *swarms_identifier_scenario_datums(scenario *scenario, long object_type, long *datum_size)
{
    // match: 0x527b10: range assumption keeps table-index scheduling in swarm_spawner_new_swarm's inline instance
    __assume(object_type >= 0 && object_type < 14);
    object_type_definition *const *slot = &object_type_definitions[object_type];
    object_type_definition *definition = *slot;
    long offset = (short)definition->scenario_datums_offset;
    long size = (short)definition->scenario_datum_size;
    if (datum_size) *datum_size = size;
    return (s_scenario_object_block *)((char *)scenario + offset);
}

// Searches the given scenario (unlike c_object_identifier::find_scenario_object_from_scenario, which
// uses the global one); kept local because this build emits bl for cross-unit calls.
static __forceinline s_scenario_object *swarms_find_scenario_object_in_scenario(
    const c_object_identifier *identifier, scenario *scenario, long *scenario_datum_index)
{
    s_scenario_object *result = 0;
    long source = identifier->m_source;
    if (source == 1 || source == 0) {
        if (object_type_definitions[identifier->m_type]->scenario_datums_offset != -1) {
            long datum_size = 0;
            s_scenario_object_block *datums = swarms_identifier_scenario_datums(scenario, identifier->m_type, &datum_size);
            for (long index = 0; index < datums->count; index++) {
                s_scenario_object *scenario_object = (s_scenario_object *)(datums->address + index * datum_size);
                if (scenario_object->object_identifier.is_equal(identifier)) {
                    result = scenario_object;
                    if (scenario_datum_index)
                        *scenario_datum_index = index;
                    break;
                }
            }
        }
    }
    return result;
}

// @address 0x527b10 ?swarm_spawner_new_swarm@@YAXF@Z
void swarm_spawner_new_swarm(short index)
{
    actor_starting_location_definition starting_location;
    if (!swarm_spawner_starting_location_new(&starting_location, index))
        return;
    s_swarm_spawner *spawner = &g_swarm_spawners[index];
    short squad_index = -1;
    if ((spawner->flags & 2) != 0)
    {
        starting_location.initial_movement_mode = 1;
        starting_location.initial_state = 3;
    }
    if (spawner->starting_location_index != -1)
    {
        unit_datum *object = (unit_datum *)((s_object_header *)object_header_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(spawner->starting_location_index)].datum;
        if (((1 << object->object.object_identifier.m_type) & 0x40) != 0)
        {
            scenario *scenario = global_scenario;
            s_scenario_object *scenario_object = swarms_find_scenario_object_in_scenario(&object->object.object_identifier, scenario, 0);
            if (scenario_object)
            {
                short scenario_squad_index = static_cast<s_scenario_scenery *>(scenario_object)->squad_index;
                if (scenario_squad_index >= 0 && scenario_squad_index < scenario->squads.count)
                    squad_index = scenario_squad_index;
            }
        }
        else if ((((1 << object->object.object_identifier.m_type) & 0x1003) != 0) && object->unit.actor_index != -1)
            squad_index = (short)swarm_actor_get(object->unit.actor_index)->squad_index;
    }
    long actor_index = actor_place(spawner->definition_index, squad_index, -1, &starting_location, 4);
    if (actor_index != -1)
    {
        actor_datum *actor = swarm_actor_get(actor_index);
        actor_set_active(actor_index, true);
        long swarm_index = actor->swarm_index;
        if (swarm_index != -1)
        {
            swarm_datum *swarm = swarm_get(swarm_index);
            swarm->spawning = true;
            spawner->swarm_index = swarm_index;
            spawner_initialize_creature(index, swarm->first_object_index);
        }
    }
    if ((spawner->flags & 2) != 0)
        swarm_spawner_end(index);
}

// @address 0x527d58 ?swarm_spawner_starting_location_new@@YA_NPAUactor_starting_location_definition@@F@Z
static __forceinline float swarms_spawner_random_range(unsigned long *seed, float minimum, float maximum)
{
    random_seed_next(seed);
    float fraction = (float)(*seed >> 16) * (1.0f / 65535.0f);
    float result = fraction * (maximum - minimum) + minimum;
    return result;
}

bool swarm_spawner_starting_location_new(actor_starting_location_definition *starting_location, short spawner_index)
{
    bool success = true;
    s_swarm_spawner *spawner = &g_swarm_spawners[spawner_index];
    real_point3d position = k_real_math_origin3d;
    real_vector3d facing;
    if (spawner->starting_location_index != -1) {
        if (spawner->marker_name != 0) {
            object_marker marker;
            if (object_get_markers_by_string_id(spawner->starting_location_index, spawner->marker_name, &marker, 1) > 0) {
                position = marker.matrix.position;
                facing = marker.matrix.forward;
            } else {
                success = false;
            }
        } else {
            const object_datum *object = object_get(spawner->starting_location_index);
            long node_count = object->node_matrices.size / sizeof(real_matrix4x3);
            real_matrix4x3 *node_matrices = (real_matrix4x3 *)object_header_block_get(object, &object->node_matrices);
            if (spawner->node_index >= 0 && spawner->node_index < node_count) {
                real_matrix4x3 *node_matrix = &node_matrices[spawner->node_index];
                matrix4x3_transform_point(node_matrix, &spawner->position, &position);
                matrix4x3_transform_vector(node_matrix, &spawner->facing, &facing);
            } else {
                success = false;
            }
        }
    } else {
        position = spawner->position;
        facing = spawner->facing;
    }
    if (success) {
        ::starting_location_new(starting_location);
        swarms_starting_location_point_set(&starting_location->position, &position);
        euler_angles2d_from_vector3d(&starting_location->facing, &facing);
        starting_location->facing.yaw = swarms_spawner_random_range(
                                            tls_random_seed(), spawner->yaw_minimum, spawner->yaw_maximum) +
            starting_location->facing.yaw;
        starting_location->facing.pitch = swarms_spawner_random_range(
                                              tls_random_seed(), spawner->pitch_minimum, spawner->pitch_maximum) +
            starting_location->facing.pitch;
        starting_location->roll = 0.0f;
        starting_location->initial_movement_mode = 2;
        starting_location->initial_state = 1;
    }
    return success;
}

// @address 0x524680 ?swarm_place@@YAJJPAUactor_starting_location_definition@@F@Z
long swarm_place(long character_index, actor_starting_location_definition *starting_location, short flags)
{
    character_definition *character = (character_definition *)tag_get('char', character_index);
    character_general_properties *properties = character_definition_general_properties_get(character_index);
    long swarm_index = -1;
    if ((properties->flags & 1) != 0 && character->creature.index != -1) {
        swarm_index = datum_new(g_swarm_data);
        if (swarm_index != -1) {
            long creature_index = character->creature.index;
            swarm_datum *swarm = swarm_get(swarm_index);
            swarm->actor_index = -1;
            swarm->first_object_index = -1;
            swarm->creature_count = 0;
            swarm->initial_member_count = 0;
            swarm->position = k_real_math_origin3d;
            swarm->death_count = 0;
            swarm->anchor_position = k_real_math_origin3d;
            swarm->anchor_valid = 0;
            swarm->spawning = false;
            short count = 10;
            if (starting_location->initial_state > 0)
                count = starting_location->initial_state;
            for (short i = 0; i < count; i++)
                swarm_create_creature(swarm_index, character_index, creature_index, starting_location, i == 0, (flags & 4) != 0);
            if (swarm->creature_count <= 0) {
                swarm_delete(swarm_index);
                swarm_index = -1;
            } else {
                swarm->initial_member_count = swarm->creature_count;
            }
        }
    }
    return swarm_index;
}

// @address 0x527fe8 swarm_spawner_spawn_creature
void swarm_spawner_spawn_creature(short spawner_index)
{
    s_swarm_spawner *spawner = &g_swarm_spawners[spawner_index];
    if (spawner->swarm_index == -1) {
        swarm_spawner_new_swarm(spawner_index);
    } else {
        actor_starting_location_definition starting_location;
        if (swarm_spawner_starting_location_new(&starting_location, spawner_index)) {
            character_definition *character = (character_definition *)tag_get('char', spawner->definition_index);
            long object_index = swarm_create_creature(spawner->swarm_index, spawner->definition_index, character->creature.index, &starting_location,
                false, true);
            if (object_index != -1)
                spawner_initialize_creature(spawner_index, object_index);
        }
    }
}

// @address 0x5280a0 ?swarm_spawner_accelerate_creature@@YAXFJ@Z
void swarm_spawner_accelerate_creature(short spawner_index, long object_index)
{
    s_swarm_spawner *spawner = &g_swarm_spawners[spawner_index];
    float vz = 0.0f;
    real_vector3d velocity;
    ::object_get_orientation(object_index, &velocity, 0);
    float minimum = spawner->random_velocity_minimum;
    float maximum = spawner->random_velocity_maximum;
    float speed = _real_random_range(tls_random_seed(), minimum, maximum);
    float vx = velocity.i * speed;
    float vy = velocity.j * speed;
    vz = velocity.k * speed;
    velocity.i = vx;
    velocity.j = vy;
    velocity.k = vz;
    object_set_velocities_internal(object_index, &velocity, 0, false);
    havok_object_set_velocities(object_index, &velocity, 0, true);
    if (!(vy * vy + (vx * vx + vz * vz) <= 9.999999e-9f))
    {
        object_set_at_rest(object_index, false);
        object_wake(object_index);
        object_set_requires_motion(object_index);
    }
}

// A swarm creature finished its action: remove it (or let its flock handle the death).
// @address 0x528518 ?ai_handle_creature_delete@@YAXJ@Z
void ai_handle_creature_delete(long creature_index)
{
    creature_datum *creature = swarm_object_get(creature_index);
    unsigned long link_type = creature->creature.link_type;
    if (link_type >= 1) {
        if (link_type != 1)
            return;
        flock_handle_creature_delete(creature_index);
        return;
    }
    if (creature->object.identifier_type != 11)
        return;
    swarm_creature_state *link = creature_link(swarm_object_get(creature_index));
    if (link == 0 || link->member_swarm_index == -1)
        return;
    if (link->prop_ref_index != -1) {
        prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), link->prop_ref_index);
        prop_datum *prop = datum_get<prop_datum>(prop_data, prop_ref->prop_index);
        prop->flags &= ~32;
        link->resurrect_type = -1;
    }
    swarm_remove_creature(link->member_swarm_index, creature_index, false, true);
}

// @address 0x527060 ?swarm_creature_handle_action_finished@@YAXJJJ@Z
void swarm_creature_handle_action_finished(long object_index, long, long)
{
    creature_datum *creature = swarm_object_get(object_index);
    swarm_creature_state *link = creature_link(creature);
    if (link == 0)
        return;
    long parent_index = creature->object.parent_object_index;
    if (parent_index == -1)
        return;
    s_object_header *parent_header = object_header_get(parent_index);
    if (parent_header->object_type != 0)
        return;
    object_datum *unit = (object_datum *)parent_header->datum;
    short squad_index = -1;
    actor_datum *actor = 0;
    if (link->swarm_index != -1)
    {
        actor = swarm_actor_get(link->swarm_index);
        squad_index = (short)actor->squad_index;
    }
    if (!(bool)((unit->damage_flags >> 2) & 1))
        return;
    long actor_index = actor_resurrect(parent_index, creature->creature.team, link->resurrect_type, squad_index);
    if (actor)
    {
        if (actor->clump_index != -1 && actor_index != -1)
        {
            clump_add_actor(actor->clump_index, actor_index);
            if (actor->target_prop_index != -1)
            {
                prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), actor->target_prop_index);
                prop_perceive(actor_index, prop_ref->object_index, -1, _prop_stimulus_none, 0, -1, link->swarm_index, actor->target_prop_index);
            }
        }
        if (actor->disposable && actor_index != -1)
            swarm_actor_get(actor_index)->disposable = true;
    }
    if (link->member_swarm_index != -1)
        swarm_remove_creature(link->member_swarm_index, object_index, true, true);
}

// @address 0x5282a8 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_swarm_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<> void *t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_swarm_data_allocator>::allocate(unsigned int size, const char *name)
{
  int member_index = restricted_memory_register_member(&g_restricted_regions[1], name, "", size, 0,
                                  __tls_set_g_swarm_data_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[1].get_member_offset(member_index);
  data_set_new_base_address(&g_swarm_data, (s_data_array *)(g_restricted_address[1] + offset));
  m_member_index = member_index;
  m_thread_id = g_registered_thread_index;
  return g_restricted_address[1] + g_restricted_regions[1].get_member_offset(member_index);
}

// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0xaf9f0 ?prop_state_get@@YAPAUprop_state@@PBUprop_ref_datum@@@Z
// @address 0xa0538 ?action_executing@@YA_NJW4e_action@@@Z
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0x4f8f38 ?ai_get_unit_responsible_for_damage@@YAJJ_N@Z
// @address 0xd3ce0 ?creature_set_actively_controlled@@YAXJ_N@Z
// @address 0xadcf0 ?character_definition_general_properties_get@@YAPAUcharacter_general_properties@@J@Z
// @address 0x55de38 ?flock_handle_creature_delete@@YAXJ@Z
// @address 0xe2c88 ?havok_object_set_velocities@@YAXJPBTreal_vector3d@@0_N@Z
// @address 0xe2c20 ?object_wake@@YAXJ@Z
// @address 0xe3070 ?object_set_requires_motion@@YAXJ@Z

namespace {

}

// @address 0x5276a0 ?swarm_creature_moving_jump@@YA_NJMMPBTreal_point3d@@PATreal_vector3d@@@Z
bool swarm_creature_moving_jump(long creature_index, float speed, float maximum_speed, const real_point3d *destination, real_vector3d *velocity)
{
    real_point3d origin;
    float minimum_speed;
    float jump_speed;
    real_vector3d direction;
    object_get_origin(creature_index, &origin);
    bool success = projectile_aim_ballistic(0.9f, 1.0f, &origin, destination, &minimum_speed, 0, 0, false, &direction, &jump_speed, 0, 0, 0, 0);
    if (!success)
    {
        if (minimum_speed > 0.0f && minimum_speed < 2.7f)
        {
            float retry_speed = minimum_speed + 0.01f;
            success = projectile_aim_ballistic(retry_speed, 1.0f, &origin, destination, &minimum_speed, 0, 0, false, &direction, &jump_speed, 0, 0, 0, 0);
        }
    }
    if (success)
        scale_vector3d(&direction, jump_speed, velocity);
    return success;
}
// @address 0x526b80 ?swarm_input_update@@YAXJ@Z
void swarm_input_update(long swarm_index)
{
    swarm_datum *swarm = swarm_get(swarm_index);
    if (swarm->actor_index != -1) {
        actor_datum *actor =
            datum_get<actor_datum>(actor_data, swarm->actor_index);
        long creature_index = swarm->first_object_index;
        swarm->position = k_real_math_origin3d;
        swarm->creature_count = 0;
        while (creature_index != -1) {
            creature_datum *creature = (creature_datum *)object_get(creature_index);
            swarm_creature_state *state;
            if (creature->creature.link_type == 0)
                state = (swarm_creature_state *)((char *)creature + creature->creature.creature_reference.offset);
            else
                state = 0;
            if (state != 0) {
                creature_state_refresh(creature_index, state);
                add_points3d(&state->position, &swarm->position, &swarm->position);
                ++swarm->creature_count;
                creature_index = state->next_swarm_creature_index;
            } else {
                break;
            }
        }
        if (swarm->creature_count > 0)
            scale_point3d(&swarm->position, 1.0f / (float)swarm->creature_count, &swarm->position);
        memset(&actor->position, 0, offsetof(actor_datum, player_weight) + sizeof(actor->player_weight) - offsetof(actor_datum, position));
        actor->root_vehicle_index = -1;
        actor->controlled_unit_index = -1;
        actor->pathfinding_sector.clear();
        actor->sector_valid = false;
        actor_input_sample_position(swarm->actor_index, swarm->first_object_index, &actor->position);
    }
}

// @address 0x527330 ?ai_swarm_creature_handle_attach@@YA_NJJ@Z
bool ai_swarm_creature_handle_attach(long creature_index, long unit_index)
{
    creature_datum *creature = (creature_datum *)object_get(creature_index);
    bool infectable = false;
    bool result = false;
    if (swarm_can_attach_to_unit(unit_index, &infectable) && infectable) {
        action_request attach;
        memset(&attach, 0, sizeof(attach));
        attach.action = _action_get_infected;
        attach.infection.object_index = creature_index;
        if (action_submit(unit_index, &attach)) {
            float minimum = 3.0f;
            float maximum = 5.0f;
            if (creature->creature.link_type == 0) {
                swarm_creature_state *link = (swarm_creature_state *)((char *)creature + creature->creature.creature_reference.offset);
                if (link->swarm_index != -1) {
                    character_swarm_properties *properties = character_definition_swarm_multi_properties_get(link->swarm_index);
                    if (properties->infection_time_minimum > 0.0f)
                        minimum = properties->infection_time_minimum;
                    if (properties->infection_time_maximum > 0.0f)
                        maximum = properties->infection_time_maximum;
                }
            }
            action_request infect;
            memset(&infect, 0, sizeof(infect));
            const float &infection_time = _real_random_range(tls_random_seed(), minimum, maximum);
            infect.action = _action_infect;
            infect.infection.object_index = unit_index;
            infect.infection.duration = (short)game_seconds_to_ticks_round(infection_time);
            if (action_submit(creature_index, &infect)) {
                c_dialogue_event event(99, unit_index, creature_index, -1, -1, 0);
                ai_dialogue_event(&event);
                result = true;
            } else {
                c_motor_task *task = motor_task_find_by_action(unit_index, _action_get_infected);
                if (task)
                    motor_system_interrupted(unit_index, task, true);
            }
        }
    }
    return result;
}

// @address 0x526308 ?swarm_action_execute_command_script@@YA_NJJPATreal_vector3d@@0@Z
bool swarm_action_execute_command_script(long swarm_index, long creature_index, real_vector3d *facing, real_vector3d *up)
{
    creature_datum *creature = (creature_datum *)object_get(creature_index);
    bool result = false;
    swarm_creature_state *state = creature_link(creature);
    s_atom_control *command = &state->command_script_control;
    if (state->command_script_control.status == 2) {
        if (command->flags & (1 << 2)) {
            *facing = command->direction;
            *up = *global_forward3d;
            result = true;
        } else if (command->flags & (1 << 9)) {
            real_point3d origin;
            real_vector3d direction;
            object_get_origin(creature_index, &origin);
            ai_vector_from_points3d(&origin, &command->point, &direction);
            if (normalize3d(&direction) > 0.0f) {
                *facing = direction;
                *up = *global_forward3d;
                result = true;
            }
        }
    }
    if (!result)
        *up = *global_zero_vector3d;
    return result;
}

static __forceinline float swarms_movement_magnitude(const real_vector3d *v)
{
    return sqrtf(v->k*v->k + (v->i*v->i + v->j*v->j));
}
static __forceinline float swarms_movement_distance_squared(const real_point3d *a, const real_point3d *b)
{
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float dz = b->z - a->z;
    return dx*dx + (dz*dz + dy*dy);
}
// @address 0x526458 ?swarm_movement_update@@YAXJJ@Z
void swarm_movement_update(long actor_index, long creature_index)
{
    creature_datum *creature = (creature_datum *)object_get(creature_index);
    swarm_creature_state *state = creature_link(creature);
    if (state) {
        character_swarm_properties *properties = character_definition_swarm_multi_properties_get(actor_index);
        if (properties) {
            union { real_point3d point; real_vector3d vector; } offset;
            memcpy(&offset, global_zero_vector3d, sizeof(offset));
            perlin_state_update(&state->movement_noise, 1.0f, properties->noise_period_minimum, properties->noise_period_maximum, true, &offset.point);
            actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
            float threshold = actor->combat_status <= 1 ? properties->noise_threshold_idle : properties->noise_threshold_active;
            if (fabsf(offset.point.x) < threshold) offset.point.x = 0.0f;
            if (fabsf(offset.point.y) < threshold) offset.point.y = 0.0f;
            if (state->movement_mode == 0) {
                if (normalize3d(&offset.vector) > 0.0f) {
                    state->desired_forward = offset.vector;
                    creature_generate_up_vector(creature_index, state);
                    state->movement.i = properties->acceleration;
                    state->movement.j = 0.0f;
                    state->movement.k = 0.0f;
                }
            } else {
                scale_vector3d(&offset.vector, properties->acceleration, &offset.vector);
                add_vectors3d(&state->movement, &offset.vector, &state->movement);
            }
            state->movement.i = PIN(state->movement.i, -1.0f, 1.0f);
            state->movement.j = PIN(state->movement.j, -1.0f, 1.0f);
            state->movement.k = PIN(state->movement.k, -1.0f, 1.0f);
            if (swarms_movement_magnitude(&state->movement) > 0.0f) {
                float distance = properties->stuck_distance > 0.0f ? properties->stuck_distance : 0.05f;
                float time = properties->stuck_time > 0.0f ? properties->stuck_time : 2.0f;
                if (swarms_movement_distance_squared(&state->position, &state->stuck_position) < distance*distance) {
                    ++state->stuck_ticks;
                } else {
                    state->stuck_ticks = 0;
                    state->stuck_position = state->position;
                }
                if (state->stuck_ticks > game_seconds_to_ticks_round(time) && creature_index != -1) {
                    object_datum *object = object_get(creature_index);
                    object->damage_flags |= 1 << 6;
                    object_wake(creature_index);
                }
            }
        }
    }
}

// @address 0x5268f0 ?swarm_update@@YAXJ@Z
void swarm_update(long actor_index)
{
    actor_datum *actor = swarm_actor_get(actor_index);
    long time = game_time_get();
    swarm_datum *swarm = swarm_get(actor->swarm_index);
    if (game_ticks_to_seconds(time - swarm->last_death_time) > 2.0f)
        swarm->death_count = 0;
    swarm_creature_iterator iterator;
    swarm_creature_iterator_new(actor->swarm_index, &iterator);
    while (swarms_creature_iterator_next_inline(&iterator))
        swarm_creature_update(actor_index, iterator.current_object_index);
    swarm_creature_iterator_new(actor->swarm_index, &iterator);
    while (swarms_creature_iterator_next_inline(&iterator))
        swarm_movement_update(actor_index, iterator.current_object_index);
    swarm_creature_iterator_new(actor->swarm_index, &iterator);
    while (swarms_creature_iterator_next_inline(&iterator))
        swarm_creature_control(iterator.current_object_index);
}

static __forceinline creature_datum *swarms_update_object_get(long index)
{ return (creature_datum *)object_get(index); }
static __forceinline float swarms_update_magnitude_squared2d(const real_vector3d *v)
{ return v->i*v->i + v->j*v->j; }

static __forceinline bool swarms_update_creature_is_busy(long creature_index)
{ return swarm_creature_flag_test(swarms_update_object_get(creature_index)->creature.flags, 1); }
// @rdata 0x820242d4
static const float swarms_update_maximum_delay = 3.0f;
static __forceinline unsigned long swarms_update_target_is_acting(long object_index)
{
    return object_index != -1 && ((1 << object_header_get(object_index)->object_type) & 0x1803) != 0 &&
        (action_executing(object_index, _action_get_infected) || action_executing(object_index, _action_morph) ||
            action_executing(object_index, _action_hostile_morph));
}
static __forceinline void swarms_update_attached_creature(const long &creature_index, creature_datum *creature, swarm_creature_state *state,
    long target_object, bool target_info, const real_point3d &target_point)
{

        if (swarms_update_creature_is_busy(creature_index))
            return;
        if (!target_info)
            return;
        long long ticks = state->tick_count;
        float seconds = game_time_globals->tick_length * (float)ticks;
        if (seconds > 1.0f || action_executing(target_object, _action_get_infected) || action_executing(target_object, _action_hostile_morph) ||
            action_executing(target_object, _action_morph))
        {
            long current_creature = -1;
            current_creature = creature_index;
            object_detach(current_creature);
            creature->creature.impact_delay_ticks = (char)game_seconds_to_ticks_round(1.0f);
            real_point3d origin;
            object_get_origin(current_creature, &origin);
            real_vector3d dir;
            dir.i = origin.x - target_point.x;
            dir.j = origin.y - target_point.y;
            dir.k = 0.0f;
            if (normalize3d(&dir) > 0.0f) {
                real_point3d jump_point;
                point_from_line3d(&origin, &dir, 1.0f, &jump_point);
                if (swarm_creature_moving_jump(current_creature, 0.9f, 2.7f, &jump_point, &state->jump_velocity))
                    state->jump_requested = true;
            }
        }

}
// NOTE: keep the gotos below: the structured equivalent re-nests this function and changes codegen.
static __forceinline void swarms_update_infection_target(long creature_index, creature_datum *creature, swarm_creature_state *state,
    long target_object, bool target_info, real_vector3d *facing, real_vector3d *up)
{

        if (!target_info)
            return;
        bool found = false;
        long long tick_now = game_time_globals->ticks_per_second;
        short marker_kind;
        long marker_id;
        object_marker marker;
        float dot;
        if (state->tick_count > (int)((float)tick_now * 10.0f + 0.5f))
            goto invalid_target;
        marker_kind = state->resurrect_type;
        switch (marker_kind) {
        case 3:
            marker_id = 0x250;
            break;
        case 4:
            marker_id = 0x251;
            break;
        default:
            goto no_marker;
        }
        if (object_get_markers_by_string_id(target_object, marker_id, &marker, 1) > 0)
            found = true;
    no_marker:
        if (found) {
            long current_creature = -1;
            current_creature = creature_index;
            real_point3d origin;
            object_get_origin(current_creature, &origin);
            real_vector3d dir;
            dir.i = marker.matrix.position.x - origin.x;
            dir.j = marker.matrix.position.y - origin.y;
            dir.k = marker.matrix.position.z - origin.z;
            float dist = normalize3d(&dir);
            if (dist > 0.0f) {
            *facing = dir;
            *up = *global_forward3d;
            if (dist < 0.2f) {
                swarm_creature_infect(current_creature, target_object, &marker, &dir);
            } else if (creature->creature.physics.m_mode == 1 && !swarm_creature_flag_test(creature->creature.flags, 5)) {
                const real_vector3d *ground_plane = creature->creature.physics.get_mode_ground()->get_ground_plane();
                dot = (ground_plane->j * dir.j + ground_plane->i * dir.i) + ground_plane->k * dir.k;
                if (dot < -0.99f || (ground_plane->k > 0.70710677f && dot > 0.99f))
                    goto invalid_target;
            }
            }
        }
        goto check_target;
    invalid_target:
        found = false;
    check_target:
        if (!found) {
            state->prop_ref_index = -1;
        }

}
// @address 0x525488 ?swarm_creature_update@@YAXJJ@Z
static __forceinline float swarms_update_random_range(unsigned long *seed, float minimum, float maximum)
{
    random_seed_next(seed);
    return minimum + ((float)(*seed >> 16) * (1.0f / 65535.0f)) * (maximum - minimum);
}

void swarm_creature_update(long actor_index, long creature_index)
{
    actor_datum *actor = swarm_actor_get(actor_index);
    creature_datum *creature = swarms_update_object_get(creature_index);
    character_swarm_properties *properties = character_definition_swarm_multi_properties_get(actor_index);
    if (creature->creature.link_type != 0)
        return;
    swarm_creature_state *state = (swarm_creature_state *)((char *)creature + creature->creature.creature_reference.offset);
    if (state == 0)
        return;
    if (properties == 0)
        return;
    swarm_datum *swarm = swarm_get(actor->swarm_index);
    if (!state->jump_requested && !swarm_creature_flag_test(creature->creature.flags, 5) && state->delay_ticks > 0)
        state->delay_ticks--;
    unsigned char target_acting = false;
    short mode = (signed char)actor->secondary_control_mode;
    short target_type = (signed char)actor->secondary_control_flags;
    long target = actor->secondary_control_target;
    long target_object;
    real_point3d target_point;
    real_point3d target_point2;
    object_marker marker;
    if (TEST_BIT(state->flags, 0)) {
        mode = 4;
        target_type = 1;
        target = creature->object.parent_object_index;
        } else if (state->prop_ref_index != -1) {
        mode = 5;
        target_type = 2;
        target = state->prop_ref_index;
        }
    bool target_info = swarm_target_get_info(target_type, target, &target_object, &target_point, &target_point2);
    if (target_info)
        target_acting = (unsigned char)swarms_update_target_is_acting(target_object);
    bool target_available = target_info;
    state->desired_forward = creature->object.forward;
    state->desired_up = creature->object.up;
    state->movement = *global_zero_vector3d;
    if (mode != state->movement_mode)
        state->tick_count = 0;
    else
        state->tick_count++;
    switch (mode) {
    case 0: {
        if (swarm->anchor_valid == 0) {
            swarm->anchor_position = swarm->position;
            swarm->anchor_valid = 1;
        }
        float dist2 = distance_squared3d(&state->position, &swarm->anchor_position);
        if (state->returning_to_anchor != 0 || dist2 > 9.0f) {
            if (dist2 < 1.0f) {
                state->returning_to_anchor = false;
            } else {
                real_vector3d direction;
                vector_from_points3d(&state->position, &swarm->anchor_position, &direction);
                if (normalize3d(&direction) > 0.0f) {
                    state->desired_forward = direction;
                    state->movement = *global_forward3d;
                }
                state->returning_to_anchor = true;
            }
        }
        break;
    }
    case 1:
    case 2: {
        bool advance = false;
        if (target_available) {
            if (!target_acting) {
                real_vector3d toward;
                vector_from_points3d(&state->position, &target_point, &toward);
                float planar_dist2 = swarms_update_magnitude_squared2d(&toward);
                if (normalize3d(&toward) > 0.0f && !state->jump_requested && !creature->creature.m_flags.test(_creature_flag_airborne)) {
                    if (state->delay_ticks <= 0) {
                        state->desired_forward = toward;
                        state->movement = *global_forward3d;
                        float dot = dot_product3d(&creature->object.forward, &state->desired_forward);
                        if (dot > 0.95f) {
                            real_vector3d offset;
                            real_point3d jump_point;
                            vector_from_points3d(&target_point, &target_point2, &offset);
                            point_from_line3d(&target_point, &offset, 0.75f, &jump_point);
                            if (swarm_creature_moving_jump(creature_index, 0.9f, 2.7f, &jump_point, &state->jump_velocity)) {
                                float delay = swarms_update_random_range(tls_random_seed(), 1.0f, swarms_update_maximum_delay);
                                state->jump_requested = true;
                                state->delay_ticks = (short)game_seconds_to_ticks_round(delay);
                            }
                        } else if (planar_dist2 < 0.09f) {
                            state->delay_ticks = (short)game_seconds_to_ticks_round(swarms_update_random_range(tls_random_seed(), 1.0f,
                                swarms_update_maximum_delay));
                        }
                    } else {
                        advance = true;
                    }
                }
            } else {
                advance = true;
            }
        }
        if (!advance)
            break;
    }
    case 3: {
        if (!target_available)
            break;
        real_vector3d toward;
        vector_from_points3d(&state->position, &target_point, &toward);
        float dist = normalize3d(&toward);
        if (dist <= 0.0f)
            break;
        float near_dist = properties->hound_distance_minimum;
        float far_dist = properties->hound_distance_maximum;
        if (target_acting) {
            near_dist *= 0.5f;
            far_dist *= 0.5f;
        }
        state->desired_forward = toward;
        if (dist > far_dist) {
            state->movement = *global_forward3d;
            if (state->movement_noise.target.x < 0.0f)
                state->movement_noise.target.x = 0.0f;
            if (state->movement_noise.previous.x >= 0.0f)
                break;
            state->movement_noise.previous.x = 0.0f;
            break;
        }
        if (dist >= near_dist)
            break;
        state->movement = *global_backward3d;
        if (state->movement_noise.target.x > 0.0f)
            state->movement_noise.target.x = 0.0f;
        if (state->movement_noise.previous.x <= 0.0f)
            break;
        state->movement_noise.previous.x = 0.0f;
        break;
    }
    case 4: {
        swarms_update_attached_creature(creature_index, creature, state, target_object, target_available, target_point);
        break;
    }
    case 5: {
        swarms_update_infection_target(creature_index, creature, state, target_object, target_available, (&state->desired_forward), (&state->movement));
        break;
    }
    case 6: {
        if (!target_available)
            break;
        real_vector3d toward;
        vector_from_points3d(&target_point, &state->position, &toward);
        float dist = normalize3d(&toward);
        if (dist <= 0.0f)
            break;
        if (dist < properties->scatter_distance) {
        state->desired_forward = toward;
        state->movement = *global_forward3d;
        real_vector3d anchor;
        vector_from_points3d(&swarm->position, &state->position, &anchor);
        if (normalize3d(&anchor) <= 0.0f)
            break;
        float seconds = game_ticks_to_seconds(state->tick_count);
        seconds = (seconds > 0.0f ? seconds : 0.0f) < swarms_update_maximum_delay
            ? (seconds > 0.0f ? seconds : 0.0f) : swarms_update_maximum_delay;
        scale_vector3d(&anchor, 2.0f * (1.0f - seconds / swarms_update_maximum_delay), &anchor);
        add_vectors3d(&anchor, &state->desired_forward, &state->desired_forward);
        if (normalize3d((&state->desired_forward)) == 0.0f)
            state->desired_forward = toward;
        } else {

            scale_vector3d(&toward, -1.0f, &state->desired_forward);
            state->movement = *global_zero_vector3d;
        }
        break;
    }
    case 7: {
        swarm_action_execute_command_script(actor_index, creature_index, (&state->desired_forward), (&state->movement));
    }
    }
    state->movement_mode = mode;
    if (mode != 0)
        swarm->anchor_valid = 0;
    creature_generate_up_vector(creature_index, state);
}

extern "C" {
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_swarm_data_allocator> c_swarm_data_allocator;
c_swarm_data_allocator g_swarm_data_allocator;
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_swarm_spawners_allocator> c_swarm_spawner_allocator;
c_swarm_spawner_allocator g_swarm_spawner_allocator;
typedef t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_spawner_globals_allocator> c_swarm_nondeterministic_counter_allocator;
c_swarm_nondeterministic_counter_allocator g_swarm_nondeterministic_counter_layout;
}

