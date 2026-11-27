// ailibxbox/squads.obj: from the records 0xa9858 0xa9a98 0xb4980 0xb57f0 0xcb800 0xcba08 0xcbdb0 0xcc120 0x11ab20 0x11ab80 0x302fc0 0x509868 0x54a078 0x54a0a8 0x54a2f0 0x54a408 0x54a6e8 0x54a780 0x54a8f8 0x54aa10 0x54aae8 0x54ac70 0x54ad28 0x54ada8 0x54ae80 0x54aff0 0x54b578 0x54bc78 0x54bd20 0x54bdd0 0x54c4e0 0x54c5d8 0x54c840 0x54ca40 0x54cf10 0x54d098 0x54d148 0x54d1c8 0x54d328 0x54d410 0x54d6a8 0x54d740 0x54d880 0x54d910 0x54da78 0x54dc60 0x54dcc0 0x54dd40 0x54e000 0x54e4f8 0x54e638 0x54e810 0x54e888 0x54e998 0x54ea08 0x54eaa8 0x54eac8 0x54eb68
// @address 0x11df684 squad_data
// @address 0x11df744 g_ai_squad_group_data
#include "ai/ai_tls_data.h"
// @flags /TP /O2 /Oc
#include "ai/squad_data.h"
#include "objects/object_header.h"
#include "ai/actor_type_flood_types.h"
#include "units/vehicle_datum.h"
#include "ai/swarms.h"
#include "ai/prop_datum.h"
#include "ai/prop_ref_datum.h"
#include "scenario/scenario_access.h"
#include "ai/actor_queries.h"
#include "ai/actor_moving_globals.h"
#include "ai/actor_firing_position_queries.h"
#include "ai/firing_position_definition.h"
#include "ai/firing_position_accessors.h"
#include "units/vehicle_definitions.h"
#include "math/real_math_functions.h"
#include "ai/ai_player_types.h"
#include "ai/ai_player_storage.h"
#include "math/real_constant_pool_access.h"
#include "memory/tls_data_arrays.h"
#include "memory/tls_globals.h"
#include "math/real_math_globals.h"
#include "objects/object_reference_types.h"
#include "objects/object_damage_flags.h"
#include "ai/firing_position_ref.h"
#include "ai/firing_position_ref_inline.h"
#include "ai/squads.h"
#include "cache/cache_files.h"
#include "scenario/scenario.h"
#include "ai/squad_iterator_types.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "math/point_math.h"
#include "game/game_types.h"
#include "objects/object_iterator.h"
#include "units/motor_enums.h"
#include "ai/actor_globals.h"
#include "ai/squad_types.h"
#include "ai/ai_point3d.h"
#include "ai/actor_datum.h"
#include "game/player_types.h"
#include "scenario/scenario_tag.h"
#include "game/game_globals_storage.h"
#include "ai/behavior_info.h"
#include "ai/pureform_types.h"
#include "cache/restricted_memory.h"
#include "cache/restricted_allocation_manager_inline.h"
#include "game/game_globals.h"
#include "units/unit_seat_source.h"
#include "units/unit_definitions.h"
// @address 0x54ac70 ?emitter_index_get_runtime_object_index@@YAJF@Z
// @address 0xb57f0 ?squad_get_task@@YAPAUs_task@@PAUsquad_datum@@@Z
// @address 0xcb800 squad_get_leader
// @address 0x11ab80 ?squad_actor_iterator_next@@YAPAUactor_datum@@PAUsquad_actor_iterator@@@Z
// @address 0x54a078 __tls_set_g_squad_data_allocator
// @address 0xcc120 squad_test_area_type
// @address 0x302fc0 ?set_range@?$c_static_flags_no_init@$0CA@@@QAAXJ@Z
// @address 0x54d328 ?squad_consider_major_upgrade@@YA_NFM@Z
// @address 0x54c4e0 ?squad_group_test_activation@@YAXJ@Z
// @address 0x54c5d8 ?squad_group_update_status@@YAXJ@Z
// @address 0x54d6a8 ?squad_force_activate@@YA_NJ@Z
// @address 0x54cf10 major_upgrade_chance
// @address 0x54a0a8 ?squads_build_hierarchy@@YAXXZ
// @address 0x54bc78 ?squad_remove_vehicle@@YAXJJ@Z
// @address 0x54bd20 squad_vehicle_change_squad
// @address 0x54c840 ?squad_update_style@@YA_NJF@Z
// @address 0x54d098 ?squad_get_actor_character_definition_index@@YAXFFFPAJ@Z
// @address 0x54dc60 ?squadless_attach_actor@@YAXJ@Z
// @address 0x54dcc0 squadless_detach_actor
// @address 0x54e888 squad_group_squad_iterator_next
// @address 0x54e998 squad_ancestor_of_squad_group
#include "cseries/crt_xdk.h"
#include <math.h>
#include <stdlib.h>
// @address 0x54eb68 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_squad_group_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
#include <string.h>
#include "cseries/cseries_wrapped.h"
#include "memory/static_arrays.h"

#include "ai/actors.h"
#include "ai/actor_types.h"
#include "cache/restricted_memory_regions.h"
#include "game/game.h"
#include "motor/action_system.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "units/units.h"
#include "ai/actor_dynamic_firing_position.h"
#include "ai/ai_script.h"
#include "ai/ai_vehicles.h"
#include "ai/cl_engine.h"
#include "ai/clump.h"
#include "ai/objectives.h"
#include "ai/squads.h"
#include "objects/widgets/leaf_system.h"
#include "ai/Zone.h"
#include "ai/character_definitions.h"
#include "memory/data.h"
#include "ai/ai_player.h"
#include "game/game_allegiance.h"
#include "math/real_math_types.h"
#include "ai/handle_array_types.h"
#include "ai/objective_types.h"
#include "ai/squad_array_types.h"
#include "hs/hs.h"
#include "scenario/scenario_light_types.h"

struct s_task;
struct actor_starting_location_definition;


#include "ai/objective_definitions.h"
#include "ai/objective_storage.h"

#include "ai/ai_globals_definition.h"

typedef actor_starting_location_definition location;
typedef actor_starting_location_definition squads_script_location;
typedef actor_starting_location_definition squads_test_start;
#include "ai/squad_definitions.h"
typedef char fire_team_definition_size[(sizeof(fire_team_definition) == 96) ? 1 : -1];
typedef char fire_team_locations_offset[(offsetof(fire_team_definition, locations) == 88) ? 1 : -1];
typedef fire_team_definition s_new_location;
typedef fire_team_definition squad;
typedef fire_team_definition squads_mark_fire_team;
typedef fire_team_definition squads_place_fire_team;
typedef fire_team_definition squads_script_fire_team;
typedef fire_team_definition squads_create_fire_team;


typedef squad_definition zone;
typedef squad_definition squads_mark_squad;
typedef squad_definition squads_place_squad;
typedef squad_definition squads_script_squad;
typedef char squad_definition_size[(sizeof(squad_definition) == 64) ? 1 : -1];


#include "ai/squad_group_datum.h"



#define squad_group_data g_ai_squad_group_data
#define DATUM_GET_ABSOLUTE(array, type, index) ((type *)((char *)(array)->data + ((index) & 0xffff) * sizeof(type)))

extern "C" {
#include "ai/squad_leader.h"
}
#include "ai/squad_hierarchy.h"
#include <float.h>
#include "ai/squad_area_iterator.h"
#include "ai/pureform_types.h"
#include "objects/object_placement.h"
#include "units/vehicle_datum.h"
#include "scenario/scenario_map_variant_types.h"
#include "ai/ai_reference_frame.h"
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_regions_inline.h"
#include "cseries/string_literal_pool.h"
#include "cache/restricted_memory_region_members.h"
#include "units/action_request.h"
#include "scenario/scenario_objects.h"
#include "ai/ai_player_tls.h"
#include "memory/datum_index.h"
#include "game/game_time_inline.h"
#include "ai/objective_access.h"
#include "objects/object_header_access.h"
#include "objects/object_markers_inline.h"
#include "math/random_math_range.h"
#include "tag_files/tag_groups.h"

__declspec(thread) s_data_array *g_ai_squad_group_data;
__declspec(thread) s_data_array *squad_data;
// @address 0x115fc40 style_all
s_style_flags style_all;

const float k_real_max = FLT_MAX;


struct placement_properties { long flags; float low[4]; float normal[4]; float high[4]; };

struct squad_iterator { squad_datum *datum; s_data_iterator iterator; long index; };

struct s_squads_dirty_iterator { squad_datum *datum; s_data_iterator iterator; long index; };

struct s_fire_team_location_pair { short fire_team_index; short location_index; };

struct s_squads_update_iterator { void *datum; s_data_iterator iterator; long index; };

// March 2007 calls this out of line from squad_update_style; it is static there with no map row.
inline void style_flags_inhibit_behaviors(s_style_flags *flags, short inhibit) {
    flags->set_all();
    for (short i = 0; i < 197; ++i)
        if ((short)(global_behavior_info[i].inhibit_flags & inhibit)) flags->set(i, false);
}






/* area iterator view (records 0xcbdb0, 0x54e4f8) */


// @address 0x54a408 ?squad_new@@YAXPAUsquad_definition@@@Z

static __forceinline scenario *squads_rva_area_scenario_get(void)
{
    return (scenario *)global_scenario;
}
static __forceinline zone_definition *squads_rva_area_zone_get(short index)
{
    return &((zone_definition *)squads_rva_area_scenario_get()->zones.address)[(unsigned short)index];
}
static __forceinline squad_datum *squads_rva_area_squad_get(short index)
{
    return &((squad_datum *)squad_data->data)[(unsigned short)index];
}

struct s_task * squad_get_task(struct squad_datum * squad)
{
  if (::squad_valid_task((squad_datum *)(const squad_datum *)squad)) {
    return &((s_task *)((s_objective *)global_scenario->objectives.address)[(short)squad->objective].tasks.address)[(short)squad->task];
  }
  return 0;
}

long squad_get_leader(unsigned long reference_index)
{
    long result = -1;
    squad_datum *reference = &((squad_datum *)((s_data_array *)squad_data)->data)[datum_index_to_absolute_index(reference_index)];
    if (::squad_valid_task((squad_datum *)reference)) {
        short first = ((s_objective_record *)g_objectives_data->data)[(unsigned short)reference->objective].first_task;
        result = g_tasks_records[(short)(first + reference->task)].leader;
    }
    return result;
}

struct area_definition;
// 0xcba08 (dev ?squad_area_iterator_new@@YA_NPAUsquad_area_iterator@@FFF_NW4e_pureform@@@Z)
bool squad_area_iterator_new(squad_area_iterator *iterator, short squad_index, short flags, short area_type, bool enabled_only, e_pureform pureform)
{
    bool result = false;
    iterator->iterator_type = 0;
    iterator->next_index = NONE;
    iterator->zone_set = 0;
    iterator->firing_position = NONE;
    if (squad_index != NONE) {
        unsigned short squad_absolute_index = squad_index;
        squad_datum *squad = DATUM_GET_ABSOLUTE(squad_data, squad_datum, squad_absolute_index);
        s_task *task;
        short follow_squad_index = ::squad_valid_task((squad_datum *)(const squad_datum *)squad) &&
                (task = (s_task *)::squad_get_task(squad))->follow_mode == 2
            ? task->follow_area_index
            : NONE;
        iterator->area_index = NONE;
        iterator->area_type = area_type;
        iterator->squad_index = squad_index;
        if (squad->follow_object_index != NONE) {
            signed char dynamic_area_count = ((squad_datum *)squad)->num_dynamic_areas;
            if (dynamic_area_count == 0) {
                result = true;
            } else if (dynamic_area_count > 0) {
                result = true;
                iterator->zone_index = NONE;
                iterator->next_index = 0;
                iterator->iterator_type = 1;
            }
        } else if (follow_squad_index != NONE) {
            result = squad_area_iterator_new(iterator, follow_squad_index, flags, area_type, enabled_only, pureform);
        } else if ((short)squad->objective != NONE) {
            if (::squad_valid_task((squad_datum *)(const squad_datum *)squad)) {
                scenario *scenario = global_scenario_get();
                s_objective *objective = &((s_objective *)scenario->objectives.address)[(short)squad->objective];
                iterator->zone_set = &((s_task *)objective->tasks.address)[(short)squad->task].areas;
            }
            if (iterator->zone_set != 0 && iterator->zone_set->count > 0) {
                bool test_type = (flags & 4) != 0 && (flags & 1) == 0;
                bool test_enabled = enabled_only;
                bool test_firing = pureform + 1 != 0;
                iterator->zone_index = NONE;
                iterator->next_index = 0;
                iterator->iterator_type = 2;
                iterator->enabled_only = enabled_only;
                iterator->type_flags = flags;
                if (test_type || test_enabled || test_firing) {
                    bool type_found = !test_type;
                    bool enabled_found = !test_enabled;
                    bool firing_found = !test_firing;
                    for (short i = 0; i < iterator->zone_set->count; i++) {
                        squads_area_zone_set_entry *entry = &((squads_area_zone_set_entry *)iterator->zone_set->address)[i];
                        area_definition *area = zone_get_area(entry->zone_index, entry->area_index);
                        if (area) {
                            if (squad_test_area_type(area_type, (area_definition *)area)) {
                                if (test_type && !type_found)
                                    type_found = entry->type - 2 == 0;
                                if (test_enabled && !enabled_found)
                                    enabled_found = entry->flags & 1;
                            }
                            if (!firing_found && (entry->firing_positions & (1 << pureform)) != 0) {
                                firing_found = true;
                                iterator->firing_position = pureform;
                            }
                        }
                        if (type_found && enabled_found && firing_found)
                            break;
                    }
                    if (test_type && !type_found)
                        iterator->type_flags = flags | 1;
                    if (test_enabled && !enabled_found)
                        iterator->enabled_only = false;
                }
                result = true;
            }
        } else {
            scenario *scenario = global_scenario_get();
            squad_definition *definition = (squad_definition *)tag_block_get_element_with_size(
                &scenario->squads, squad_absolute_index, sizeof(squad_definition));
            long zone_index = (unsigned short)definition->zone_index;
            if (zone_index != 0xffff) {
                zone_definition *zone = (zone_definition *)tag_block_get_element_with_size(
                    &scenario->zones, datum_index_to_absolute_index(zone_index), sizeof(zone_definition));
                if (zone->areas.count > 0) {
                    iterator->zone_index = zone_index;
                    iterator->next_index = 0;
                    iterator->iterator_type = 3;
                    result = true;
                }
            }
        }
    }
    return result;
}

short squad_area_iterator_next(squad_area_iterator *iterator)
{
    short result = -1;
    if (iterator->next_index != -1) {
        if (iterator->iterator_type == 1) {
            squad_datum *squad = squads_rva_area_squad_get(iterator->squad_index);
            while (iterator->next_index != -1) {
                short zone_index = squad->dynamic_areas[iterator->next_index].zone_index;
                short area_index = squad->dynamic_areas[iterator->next_index].area_index;
                zone_definition *zone = squads_rva_area_zone_get(zone_index);
                bool usable = true;
                if (area_index < 0 || area_index >= zone->areas.count) {
                    iterator->next_index = -1;
                    break;
                } else {
                    iterator->next_index++;
                    if (iterator->next_index >= squad->num_dynamic_areas)
                        iterator->next_index = -1;
                    if (iterator->area_type == 1) {
                        area_definition *area = &((area_definition *)zone->areas.address)[area_index];
                        usable = ((unsigned long)area->flags & 1) != 0;
                    } else if (iterator->area_type == 2) {
                        area_definition *area = &((area_definition *)zone->areas.address)[area_index];
                        usable = (((unsigned long)area->flags >> 3) & 1) != 0;
                    }
                }
                if (usable) {
                    iterator->zone_index = zone_index;
                    result = area_index;
                    break;
                }
            }
        } else if (iterator->iterator_type == 2) {
            while (iterator->next_index != -1) {
                squads_area_zone_set_entry *entry = 0;
                iterator->zone_set_index = iterator->next_index;
                iterator->next_index++;
                if (iterator->next_index >= iterator->zone_set->count)
                    iterator->next_index = -1;
                entry = &((squads_area_zone_set_entry *)iterator->zone_set->address)[iterator->zone_set_index];
                if (iterator->type_flags & (1 << entry->type)) {
                    short zone_index = entry->zone_index;
                    short area_index = entry->area_index;
                    area_definition *area = (area_definition *)zone_get_area(zone_index, area_index);
                    if (area && squad_test_area_type(iterator->area_type, (area_definition *)area) &&
                        (iterator->firing_position == -1 || (entry->firing_positions & (1 << iterator->firing_position))) &&
                        (!iterator->enabled_only || (entry->flags & 1))) {
                        iterator->zone_index = zone_index;
                        result = area_index;
                        break;
                    }
                }
            }
        } else {
            while (iterator->next_index != -1) {
                short area_index = iterator->next_index;
                zone_definition *zone = squads_rva_area_zone_get(iterator->zone_index);
                area_definition *area = &((area_definition *)zone->areas.address)[area_index];
                iterator->next_index++;
                if (iterator->next_index >= zone->areas.count)
                    iterator->next_index = -1;
                if (area && squad_test_area_type(iterator->area_type, (area_definition *)area)) {
                    result = area_index;
                    break;
                }
            }
        }
    }
    iterator->area_index = result;
    return result;
}


// 0xcc120
extern "C" bool squad_test_area_type(short area_type, area_definition *area)
{
    bool valid = false;
    if (area_type == 3 ||
        area_type == 1 && (area->flags & 1) ||
        area_type == 2 && (area->flags & 8) ||
        area_type == 0 && !(area->flags & 8) && !(area->flags & 1))
        valid = true;
    return valid;
}

static __forceinline void squad_actor_iterator_new_inline(squad_actor_iterator *iterator, long squad_index, bool active_only)
{
    if (ai_globals->enabled) {
        iterator->squad_index = squad_index;
        iterator->index = NONE;
        iterator->active_only = active_only;
        if (squad_index == NONE) {
            iterator->next_index = ai_globals->first_actor_index;
        } else {
            squad_datum *squad = &((squad_datum *)squad_data->data)[datum_index_to_absolute_index(squad_index)];
            iterator->next_index = squad->first_actor_index;
        }
    }
}

// 0x11ab20
void squad_actor_iterator_new(squad_actor_iterator *iterator, long squad_index, bool active_only)
{
    squad_actor_iterator_new_inline(iterator, squad_index, active_only);
}

struct actor_datum * squad_actor_iterator_next(struct squad_actor_iterator * iterator)
{
    actor_datum *result = 0;
    if (ai_globals->enabled) {
        while (iterator->next_index != -1) {
            result = (actor_datum *)(actor_data->data + (datum_index_to_absolute_index(iterator->next_index)) * sizeof(actor_datum));
            iterator->index = iterator->next_index;
            iterator->next_index = result->next_squad_actor_index;
            if (!iterator->active_only || result->active)
                break;
            iterator->index = -1;
            result = 0;
        }
    }
    return result;
}

// 0x302fc0 (dev ?set_range@?$c_static_flags_no_init@$0CA@@@QAAXJ@Z)
template <long k_bit_count> void c_static_flags_no_init<k_bit_count>::set_range(long count)
{
    if (count != 0) {
        long last = ((count + 31) >> 5) - 1;
        if (last != 0)
            XMemSet(m_storage, -1, last * sizeof(unsigned long));
        m_storage[last] |= (unsigned long)((1i64 << (count - last * 32)) - 1);
    }
}
template void c_static_flags_no_init<32>::set_range(long count);

void __tls_set_g_squad_data_allocator(void *object)
{
    data_set_new_base_address((s_data_array **)&squad_data, (s_data_array *)object);
}

void squads_build_hierarchy(void)
{
    scenario *scenario = global_scenario_get();
    for (short i = 0; i < scenario->squad_groups.count; ++i) {
        squad_group_datum *group = datum_get<squad_group_datum>(g_ai_squad_group_data, (unsigned short)i);
        group->next_sibling_index = -1;
        group->first_squad_index = -1;
        group->first_child_index = -1;
        group->parent_index = ((squads_group_definition *)scenario->squad_groups.address)[(unsigned short)i].parent_index;
    }
    for (short i = 0; i < scenario->squads.count; ++i) {
        // match: 0x54a0a8: datum_get<squad_datum> here schedules the squad_data->data load before the mulli 176, swapping r8/r11 in the second loop.
        squad_datum *squad = &((squad_datum *)squad_data->data)[datum_index_to_absolute_index((unsigned short)i)];
        squad->next_squad_index = -1;
        squad->parent_index = ((squad_definition *)scenario->squads.address)[(unsigned short)i].parent_index;
    }
    for (short i = 0; i < scenario->squad_groups.count; ++i) {
        squad_group_datum *group = datum_get<squad_group_datum>(g_ai_squad_group_data, (unsigned short)i);
        if (group->parent_index != -1) {
            squad_group_datum *parent = datum_get<squad_group_datum>(g_ai_squad_group_data, group->parent_index);
            long child_index = parent->next_sibling_index;
            if (child_index == -1) parent->next_sibling_index = i;
            else {
                squad_group_datum *child;
                do {
                    child = datum_get<squad_group_datum>(g_ai_squad_group_data, child_index);
                    child_index = child->first_child_index;
                } while (child_index != -1);
                child->first_child_index = i;
            }
        }
    }
    for (short i = 0; i < scenario->squads.count; ++i) {
        long child_index_to_add = i;
        squad_datum *squad = datum_get<squad_datum>(squad_data, (unsigned short)i);
        if (squad->parent_index != -1) {
            squad_group_datum *parent = datum_get<squad_group_datum>(g_ai_squad_group_data, squad->parent_index);
            long child_index = parent->first_squad_index;
            if (child_index == -1) parent->first_squad_index = child_index_to_add;
            else {
                squad_datum *child;
                do {
                    child = datum_get<squad_datum>(squad_data, child_index);
                    child_index = child->next_squad_index;
                } while (child_index != -1);
                child->next_squad_index = (short)child_index_to_add;
            }
        }
    }
}

static __forceinline void squads_initialize_group_new(squads_group_definition *definition)
{
    long absolute_index = datum_new((s_data_array *)g_ai_squad_group_data) & 0xffff;
    squad_group_datum *group = DATUM_GET_ABSOLUTE(g_ai_squad_group_data, squad_group_datum, absolute_index);
    group->next_sibling_index = -1;
    group->first_squad_index = -1;
    group->first_child_index = -1;
    group->initial_strength = -1;
    group->active_count = 0;
    group->alerted = false;
    group->previous_strength = -1;
    group->reserved_count = -1;
    group->status = 0;
    group->placed = 0;
    group->parent_index = definition->parent_index;
    group->first_child_index = -1;
}


static __forceinline short squad_new_actor_type_default_team(short actor_type)
{
    return g_actor_type_default_team[actor_type];
}

void squad_new(squad_definition *definition)
{
    long squad_index = datum_new((s_data_array *)squad_data) & 0xffff;
    squad_datum *squad = DATUM_GET_ABSOLUTE(squad_data, squad_datum, squad_index);
    squad->flags = 0;
    squad->flags = (definition->flags >> 1) & 1;
    if ((long)definition->flags & 4)
        squad->flags |= 2;
    else
        squad->flags &= ~2;
    squad->first_actor_index = -1;
    squad->inactive_ticks = 0;
    squad->status_level = 0;
    squad->count = 0;
    squad->temporary = 0;
    squad->highest_rank = 0;
    float initial_count = 0.0f;
    squad->active_count = initial_count;
    squad->original_count = initial_count;
    squad->first_vehicle_index = -1;
    squad->last_activation_time = -1;
    squad->follow_object_index = -1;
    squad->vehicle_index = -1;
    squad->objective = -1;
    squad->task = -1;
    squad->next_index = -1;
    squad->last_task_change_time = -1;
    squad->objective2_index = -1;
    squad->task2_index = -1;
    squad->parent_index = definition->parent_index;
    squad->next_squad_index = -1;
    squad->style.set_all();
    squad->style_index = -1;
    signed char team = (signed char)definition->team;
    squad->team = team;
    if (team == 0) {
        scenario *scenario_data = global_scenario;
        for (short i = 0; i < definition->locations_count; i++) {
            s_new_location *location = &definition->locations[i];
            short character_index = location->character_index;
            if (character_index < 0 || character_index >= scenario_data->characters.count)
                continue;
            s_tag_reference *reference = &((s_tag_reference *)scenario_data->characters.address)[character_index];
            long character_tag = reference->index;
            if (character_tag == -1)
                continue;
            character_definition *character = (character_definition *)tag_get('char', character_tag);
            long unit_index = character->unit.index;
            if (unit_index == -1)
                unit_index = character->creature.index;
            if (unit_index != -1)
                squad->team = (unsigned char)((unit_definition *)tag_get('unit', unit_index))->default_team;
            if (squad->team == 0) {
                character_general_properties *properties = (character_general_properties *)character_definition_general_properties_get(reference->index);
                if (properties != 0) {
                    short actor_type = properties->type;
                    if (actor_type >= 0 && actor_type < 25)
                        squad->team = (unsigned char)squad_new_actor_type_default_team(actor_type);
                }
            }
            if (squad->team != 0)
                break;
        }
    }
}

void squads_initialize_for_new_map(void)
{
    scenario *scenario_data = global_scenario_get();
    data_make_valid((s_data_array *)squad_data);
    data_make_valid((s_data_array *)g_ai_squad_group_data);
    for (short index = 0; index < scenario_data->squads.count; index++) {
        squad_definition *definition = (squad_definition *)tag_block_get_element_with_size(&scenario_data->squads, index, sizeof(squad_definition));
        squad_new(definition);
    }
    for (short index = 0; index < scenario_data->squad_groups.count; index++) {
        squads_group_definition *definition = (squads_group_definition *)tag_block_get_element_with_size(
            &scenario_data->squad_groups, index, sizeof(squads_group_definition));
        squads_initialize_group_new(definition);
    }
    squads_build_hierarchy();
}



// @address 0x54a6e8 ?starting_location_new@@YAXPAUactor_starting_location_definition@@@Z
void starting_location_new(struct actor_starting_location_definition * starting_location)
{
    memset(&starting_location->name, 0, sizeof(starting_location->name));
    starting_location->flags = 0;
    starting_location->starting_location_name = 0;
    starting_location->position.set(global_origin3d, -1, -1);
    starting_location->facing.yaw = 0.0f;
    starting_location->facing.pitch = 0.0f;
    starting_location->initial_movement_mode = 0;
    starting_location->character = -1;
    starting_location->primary_weapon = -1;
    starting_location->secondary_weapon = -1;
    starting_location->vehicle_type = -1;
    starting_location->seat_type = -1;
    starting_location->grenade_type = -1;
    starting_location->initial_state = 0;
    starting_location->actor_variant = 0;
    starting_location->vehicle_variant = 0;
    starting_location->initial_movement_distance = 0.0f;
    starting_location->movement_mode = 0;
    starting_location->emitter_palette_index = -1;
    starting_location->giant_index = -1;
    starting_location->biped_index = -1;
    starting_location->script_index = -1;
    starting_location->script_name[0] = 0;
}

// @address 0x54a780 ?squad_placement_find_seat@@YAFJFPAJPAF@Z
short squad_placement_find_seat(long unit_index, short seat_type, long *selected_unit_index, short *occupied_seat_count)
{
    short seat_count = 0;
    short result = -1;
    unit_seat_source seats[64];
    unit_get_all_seats_internal(unit_index, seats, &seat_count, 64, 0);
    short count = seat_count;
    if (count > 0) {
        short i = 0;
        for (;;) {
            unit_seat_source *seat = &seats[i];
            long seat_unit_index = seat->unit_index;
            short seat_index = seat->seat_index;
            if (unit_seat_get_occupant(seat_unit_index, seat_index) != -1) {
                if (occupied_seat_count)
                    ++*occupied_seat_count;
            } else if (result == -1) {
                bool usable = false;
                short type = (short)seat->definition->type;
                if (type != 0) {
                    switch (seat_type) {
                    case 0:
                        if (type != 3 && type != 4) usable = true;
                        break;
                    case 1:
                        if (type == 1) usable = true;
                        break;
                    case 2:
                        if (type == 2) usable = true;
                        break;
                    case 3:
                        if (type == 5) usable = true;
                        break;
                    case 4:
                        if (type == 3) usable = true;
                        break;
                    case 5:
                        if (type == 4) usable = true;
                        break;
                    case 6:
                    case 7:
                        ASSERT(false);
                        break;
                    default:
                        break;
                    }
                }
                if (usable) {
                    result = seat_index;
                    if (selected_unit_index)
                        *selected_unit_index = seat_unit_index;
                    if (!occupied_seat_count)
                        break;
                }
            }
            i = (short)(i + 1);
            if (i >= count)
                break;
        }
    }
    return result;
}



static __forceinline s_scenario_object *squads_identifier_find_scenario_object(const c_object_identifier *identifier, long *scenario_object_index)
{
    return identifier->find_scenario_object_from_scenario(global_scenario_get(), scenario_object_index);
}
// @address 0x54a8f8 ?squad_placement_find_vehicle@@YAJFFJPAF@Z
long squad_placement_find_vehicle(short vehicle_type, short seat_type, long first_vehicle, short * vehicle_seat_index)
{
    long result = -1;
    long vehicle_index = first_vehicle;
    scenario *scenario_data = global_scenario_get();
    long best_vehicle_index = -1;
    short best_preference = 32767;
    short best_seat_index;
    if (seat_type != 3 && seat_type != 6 && first_vehicle != -1) {
        const scenario_object_palette_entry *vehicle_type_definition = (const scenario_object_palette_entry *)tag_block_get_element_with_size(
            &scenario_data->vehicle_palette, vehicle_type, sizeof(scenario_object_palette_entry));
        while (vehicle_index != -1) {
            vehicle_datum *vehicle = ((vehicle_datum *)object_get_and_verify_type(vehicle_index, 2));
            if (vehicle_type_definition->object.index == vehicle->object.definition_index) {
                short preference = 0;
                long selected_vehicle = -1;
                short seat_index = ::squad_placement_find_seat(vehicle_index, seat_type, (long *)(&selected_vehicle), (short *)(&preference));
                if (seat_index != -1 && preference < best_preference) {
                    best_preference = preference;
                    best_vehicle_index = selected_vehicle;
                    best_seat_index = seat_index;
                    if (preference == 0) break;
                }
            }
            vehicle_index = vehicle->vehicle.next_vehicle_index;
        }
        if (best_vehicle_index != -1) {
            result = best_vehicle_index;
            *vehicle_seat_index = best_seat_index;
        }
    }
    return result;
}

namespace {
}

// @address 0x54aa10 ?ai_handle_squad_group_placement@@YAXJPAV?$c_static_flags@$0KA@@@@Z
void ai_handle_squad_group_placement(long group_index, c_static_flags<160> *flags)
{
    squad_group_datum *group = DATUM_GET_ABSOLUTE(squad_group_data, squad_group_datum, datum_index_to_absolute_index(group_index));
    if (group->placed)
        return;
    if (flags->test(group_index))
        return;
    squads_group_definition *definition = (squads_group_definition *)tag_block_get_element_with_size(
        &(global_scenario_get())->squad_groups, datum_index_to_absolute_index(group_index), sizeof(squads_group_definition));
    long parent_index = (short)definition->parent_index;
    group->placed = true;
    if (parent_index != -1) {
        squad_group_datum *parent = DATUM_GET_ABSOLUTE(squad_group_data, squad_group_datum, (unsigned short)parent_index);
        if (!parent->placed)
            ai_handle_squad_group_placement(parent_index, flags);
    }
    flags->set(group_index, true);
}
void ai_handle_squad_placement(long squad_index)
{
    squad_datum *squad = DATUM_GET_ABSOLUTE(squad_data, squad_datum, squad_index);
    squad_definition *definition = (squad_definition *)tag_block_get_element_with_size(
        &(global_scenario_get())->squads, datum_index_to_absolute_index(squad_index), sizeof(squad_definition));
    scenario *scenario_data = global_scenario_get();
    if (!(squad->flags & 4)) {
        c_static_flags<160> flags;
        flags.clear();
        squad->flags |= 4;
        if (definition->parent_index != -1) {
            squad_group_datum *group = DATUM_GET_ABSOLUTE(g_ai_squad_group_data, squad_group_datum, (unsigned short)definition->parent_index);
            if (!group->placed) ai_handle_squad_group_placement(definition->parent_index, &flags);
        }
        if (definition->objective >= 0 && definition->objective < scenario_data->objectives.count) {
            short task_index = -1;
            s_objective *objective = (s_objective *)tag_block_get_element_with_size(
                &scenario_data->objectives, definition->objective, sizeof(s_objective));
            if (definition->task >= 0 && definition->task < objective->tasks.count)
                task_index = definition->task;
            ::squad_set_objective(squad_index, definition->objective, task_index, false);
        } else {
            short group_index = definition->parent_index;
            while (group_index != -1) {
                squads_group_definition *group_definition = (squads_group_definition *)tag_block_get_element_with_size(
                    &scenario_data->squad_groups, (unsigned short)group_index, sizeof(squads_group_definition));
                if (group_definition->objective >= 0 && group_definition->objective < scenario_data->objectives.count) {
                    short task_index = -1;
                    ::squad_set_objective(squad_index, group_definition->objective, task_index, false);
                    break;
                }
                group_index = group_definition->parent_index;
            }
        }
    }
    ::squad_update_status(squad_index);
    ai_update_activation();
    clumps_update_state();
}


long emitter_index_get_runtime_object_index(short emitter_index)
{
    long result = NONE;
    c_object_iterator<vehicle_datum> iterator;
    iterator.begin(1UL << 1, 0);
    while (iterator.next()) {
        vehicle_datum *vehicle = iterator.get_datum();
        long scenario_object_index;
        if (squads_identifier_find_scenario_object(&vehicle->object.object_identifier, &scenario_object_index) && scenario_object_index == emitter_index) {
            result = iterator.get_index();
            break;
        }
    }
    return result;
}

// @address 0x54ad28 ?emitter_is_usable@@YA_NJ@Z
bool emitter_is_usable(long emitter_object_index)
{
    bool result = true;
    object_datum *object = object_get(emitter_object_index);
    if (object->m_damage_flags.test(_object_damage_body_depleted_bit))
        result = false;
    else if (action_executing(emitter_object_index, _action_emit_ai))
        result = false;
    return result;
}

// @address 0x54ada8 ?starting_location_emitter_is_usable@@YA_NFFF@Z
bool starting_location_emitter_is_usable(short squad_index, short member_index, short start_index)
{
    bool usable = false;
    zone *squad_group = &((zone *)global_scenario_get()->squads.address)[(unsigned short)squad_index];
    if (member_index >= 0 && member_index < squad_group->count) {
        squad *member = &squad_group->squads[member_index];
        if (start_index >= 0 && start_index < member->count) {
            location *start = &member->locations[start_index];
            if (start->emitter_index != 0xffff) {
                long emitter_object_index = emitter_index_get_runtime_object_index(start->emitter_index);
                if (emitter_object_index != -1)
                    usable = emitter_is_usable(emitter_object_index);
            } else {
                usable = true;
            }
        }
    }
    return usable;
}


static __forceinline squads_mark_squad *squads_mark_squad_get(short index)
{
    return &((squads_mark_squad *)global_scenario->squads.address)[(unsigned short)index];
}
static __forceinline squads_mark_fire_team *squads_mark_team_get(squads_mark_squad *squad, short index) { return &squad->fire_teams[index]; }
// @address 0x54ae80 ?mark_unusable_starting_locations@@YAXFFPAV?$c_static_flags@$0CA@@@@Z
void mark_unusable_starting_locations(short squad_index, short team_index, c_static_flags<32> *flags)
{
    squads_mark_fire_team *team = squads_mark_team_get(squads_mark_squad_get(squad_index), team_index);
    scenario *scenario = global_scenario;
    for (short i = 0; i < team->count; i = (short)(i + 1)) {
        actor_starting_location_definition *location = &team->locations[i];
        bool unusable = false;
        if (location->flags > 0 && ((1 << game_difficulty_level_get()) & location->flags) == 0) {
            unusable = true;
        } else if ((unsigned short)location->emitter_palette_index != 0xffff) {
            long object_index = emitter_index_get_runtime_object_index(location->emitter_palette_index);
            if (object_index == -1 || !emitter_is_usable(object_index))
                unusable = true;
        } else if (location->biped_index != -1) {
            if (location->biped_index < 0 || location->biped_index >= scenario->bipeds.count) {
                unusable = true;
            } else {
                long object_index = ((s_scenario_biped *)scenario->bipeds.address)
                    [location->biped_index].object_identifier.find_object_index();
                if (object_index == -1)
                    unusable = true;
                else
                    unusable = ((unit_datum *)((s_object_header *)object_header_data->data)
                        [datum_index_to_absolute_index(object_index)].datum)->unit.actor != -1;
            }
        }
        if (unusable)
            flags->set(i, true);
    }
}
void squad_remove_vehicle(long squad_index, long datum_index)
{
  vehicle_datum *element;
  bool found;
  long *link;

  found = false;
  link = &((squad_datum *)squad_data->data)[datum_index_to_absolute_index(squad_index)].first_vehicle_index;
  if (*link != -1) {
    do {
      element = (vehicle_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(*link)].datum;
      if (*link == datum_index) {
        found = true;
        *link = element->vehicle.next_vehicle_index;
        break;
      }
      link = &element->vehicle.next_vehicle_index;
    } while (*link != -1);
  }
  if (found) {
    ((vehicle_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(datum_index)].datum)->vehicle.squad_index = -1;
  }
}

extern "C" void squad_vehicle_change_squad(unsigned int datum_index, int owner_index)
{
  vehicle_datum *datum;
  squad_datum *owner;

  datum = (vehicle_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(datum_index)].datum;
  if (datum->vehicle.squad_index != -1) {
    ::squad_remove_vehicle(datum->vehicle.squad_index, datum_index);
  }
  if (owner_index != -1) {
    owner = &((squad_datum *)squad_data->data)[datum_index_to_absolute_index(owner_index)];
    ((vehicle_datum *)((s_object_header *)object_header_data->data)
        [datum_index_to_absolute_index(datum_index)].datum)->vehicle.next_vehicle_index = owner->first_vehicle_index;
    owner->first_vehicle_index = datum_index;
    if ((owner_index & ~0xffff) == 0) {
      datum->vehicle.squad_index = (unsigned int)(unsigned short)owner->header << 16 | owner_index;
    }
    else {
      datum->vehicle.squad_index = owner_index;
    }
  }
}

// @address 0x54bdd0 ?squad_run_placement_script@@YAXJFFF@Z
static __forceinline scenario *squads_script_scenario_get() { return (scenario *)global_scenario; }
static __forceinline unsigned int squads_script_name_length(const char *s, unsigned int size)
{
    unsigned int length = 0;
    while (length < size && *s++) length++;
    return length;
}
void squad_run_placement_script(long actor_index, short squad_index, short team_index, short location_index)
{
    squads_script_squad *squad = (squads_script_squad *)tag_block_get_element_with_size(
        &squads_script_scenario_get()->squads, (unsigned short)squad_index, sizeof(squads_script_squad));
    squads_script_fire_team *team = (squads_script_fire_team *)tag_block_get_element_with_size(&squad->fire_team_block,
        team_index, sizeof(squads_script_fire_team));
    scenario *sc = squads_script_scenario_get();
    short script_index = -1;
    squads_script_location *location = (squads_script_location *)tag_block_get_element_with_size(
        &team->starting_locations, location_index, sizeof(squads_script_location));
    if (location->script_index >= 0 && location->script_index < sc->scripts.count && squads_script_name_length(location->script_name, 32)) {
        s_hs_script_definition *definition = (s_hs_script_definition *)tag_block_get_element_with_size(
            &sc->scripts, location->script_index, sizeof(s_hs_script_definition));
        if (definition->type == 4) script_index = location->script_index;
    }
    if (script_index == -1 && team->script_index >= 0 && team->script_index < sc->scripts.count && squads_script_name_length(team->script_name, 32)) {
        s_hs_script_definition *definition = (s_hs_script_definition *)tag_block_get_element_with_size(
            &sc->scripts, team->script_index, sizeof(s_hs_script_definition));
        if (definition->type == 4) script_index = team->script_index;
    }
    if (script_index != -1) cs_attach_command_script(actor_index, script_index, -1);
}

// 0x54c4e0 (dev ?squad_group_test_activation@@YAXJ@Z)
static __forceinline void squads_group_activate(long squad_group_index)
{
    squad_group_datum *group = DATUM_GET_ABSOLUTE(squad_group_data, squad_group_datum, squad_group_index);
    group->active = true;
}
void squad_group_test_activation(long squad_group_index)
{
    squad_group_datum *group = DATUM_GET_ABSOLUTE(squad_group_data, squad_group_datum, squad_group_index);
    bool activate = false;
    for (long child_index = group->next_sibling_index; child_index != NONE; ) {
        squad_group_test_activation(child_index);
        squad_group_datum *child = DATUM_GET_ABSOLUTE(squad_group_data, squad_group_datum, child_index);
        activate |= child->active;
        child_index = child->first_child_index;
    }
    for (long squad_index = group->first_squad_index; squad_index != NONE; ) {
        squad_datum *squad = DATUM_GET_ABSOLUTE(squad_data, squad_datum, squad_index);
        activate |= (squad->flags & 0x80) != 0;
        squad_index = squad->next_squad_index;
    }
    if (activate && !group->active)
        squads_group_activate(squad_group_index);
}

// 0x54c5d8 (dev ?squad_group_update_status@@YAXJ@Z)
void squad_group_update_status(long squad_group_index)
{
    squad_group_datum *group = DATUM_GET_ABSOLUTE(squad_group_data, squad_group_datum, squad_group_index);
    bool alerted = false;
    short strength = 0;
    short reserved_count = 0;
    group->status = 0;
    group->active_count = 0;
    float weighted_status = 0.0f;
    for (long child_index = group->next_sibling_index; child_index != NONE; ) {
        squad_group_update_status(child_index);
        squad_group_datum *child = DATUM_GET_ABSOLUTE(squad_group_data, squad_group_datum, child_index);
        strength += child->strength;
        reserved_count += child->reserved_count;
        alerted |= child->alerted;
        if (child->status > group->status) group->status = child->status;
        group->active_count += child->active_count;
        if (child->active_count > 0) weighted_status += child->weighted_status * (float)child->active_count;
        child_index = child->first_child_index;
    }
    for (long squad_index = group->first_squad_index; squad_index != NONE; ) {
        squad_datum *squad = DATUM_GET_ABSOLUTE(squad_data, squad_datum, squad_index);
        strength += squad->strength;
        reserved_count += squad->reserved_count;
        alerted |= (squad->flags & 32) != 0;
        group->active_count += squad->active_count;
        if (squad->active_count > 0.0f) weighted_status += squad->status * squad->active_count;
        if (squad->status_level > group->status) group->status = squad->status_level;
        squad_index = squad->next_squad_index;
    }
    if (group->active_count > 0) group->weighted_status = weighted_status / (float)group->active_count;
    else group->weighted_status = 0.0f;
    if ((group->reserved_count == -1 || group->reserved_count == 0) && reserved_count > 0) {
        if (group->strength == 65535) group->previous_strength = strength;
        else group->previous_strength = group->strength;
    }
    group->strength = strength;
    group->reserved_count = reserved_count;
    group->alerted = alerted;
}

bool squad_update_style(long squad_index, short style_global_index)
{
    squad_datum *squad = DATUM_GET_ABSOLUTE(squad_data, squad_datum, squad_index);
    s_game_globals *game_globals = ((s_game_globals *)global_game_globals);
    bool result = false;
    if (game_globals->ai_globals.count() > 0) {
        long style_index = game_globals_get_style_index(game_globals, style_global_index);
        if (style_index != -1) {
            style_definition *style = (style_definition *)tag_get('styl', style_index);
            if (style) {
                squad->style = style->style;
                squad->style_index = style_index;
                result = true;
            }
        }
        if (!result) {
            squad->style.set_all();
            squad->style_index = -1;
            result = true;
        }
    }
    if (!result) {
        squad->style.set_all();
        squad->style_index = -1;
    }
    s_task *task = ::squad_get_task(squad);
    if (task) {
        s_style_flags flags;
        style_flags_inhibit_behaviors(&flags, task->vehicle_inhibit_flags);
        squad->style.intersect(&flags);
    }
    return result;
}

// 0x54cf10 (dev ?major_upgrade_chance@@YAXJFPA_N0PAM@Z)
static __forceinline placement_properties *squads_character_placement_properties_get(long character_index)
{
    placement_properties *result = 0;
    long index = character_index;
    while (index != NONE) {
        character_definition *definition = (character_definition *)tag_get('char', index);
        if (definition->placement_properties.count > 0) {
            result = (placement_properties *)definition->placement_properties.address;
            break;
        }
        index = definition->parent_character.index;
    }
    return result;
}
static __forceinline unsigned long squads_game_skull_flags()
{
    unsigned long flags;
    if (game_globals->options.game_mode == 1) flags = game_globals->options.active_skulls;
    else flags = 0;
    return flags;
}
extern "C" void major_upgrade_chance(
    // match: 0x54cf10: without __restrict the *is_random/*force_major stb stores and *random_chance stfs reorder (extra stb, moved stfs)
    long character_index, short type, bool *__restrict force_major, bool *__restrict is_random, float *__restrict random_chance)
{
    placement_properties *properties = squads_character_placement_properties_get(character_index);
    if (properties) {
        short difficulty = game_globals->options.game_mode == 1 ? game_globals->options.campaign_difficulty : 1;
        switch (type) {
        case 3:
            *is_random = false;
            *force_major = false;
            break;
        case 4:
            if (properties->high[difficulty] > 0.0f) {
                *is_random = false;
                *force_major = true;
            } else {
                *is_random = true;
                *random_chance = 0.0f;
            }
            break;
        case 1:
            *is_random = true;
            *random_chance = properties->low[difficulty];
            break;
        case 2:
            *is_random = true;
            *random_chance = properties->high[difficulty];
            break;
        default:
            *is_random = true;
            *random_chance = properties->normal[difficulty];
            break;
        }
    } else {
        *is_random = false;
        *force_major = false;
    }
    bool skull_active = ((squads_game_skull_flags() >> 6) & 1) != 0;
    if (skull_active) {
        *is_random = false;
        *force_major = true;
    }
}

void squad_get_actor_character_definition_index(short zone_index, short squad_index, short location_index, long * out) {
 *out=-1;
 if(zone_index!=-1 && squad_index!=-1 && location_index!=-1) {
  scenario *s=global_scenario_get();
  zone *z=&((zone *)global_scenario_get()->squads.address)[(unsigned short)zone_index];
  squad *q=&z->squads[squad_index];
  location *l=&q->locations[location_index];
  short character_index=-1;
  character_index=l->character!=-1 ? l->character : q->character!=-1 ? q->character : -1;
  if(character_index>=0 && character_index<s->characters.count) {
   s_tag_reference *c=&((s_tag_reference *)global_scenario_get()->characters.address)[character_index];
   *out=c->index;
  }
 }
}

// @address 0x54d148 ?squad_set_actor_starting_location@@YAXJFFF@Z
void squad_set_actor_starting_location(long actor_index, short squad_index, short member_index, short start_index)
{
 scenario *root=global_scenario_get();
 squad_definition *set=(squad_definition *)tag_block_get_element_with_size(&root->squads,(unsigned short)squad_index,sizeof(squad_definition));
 fire_team_definition *member=&set->fire_teams[member_index];
 actor_starting_location_definition *start=&member->locations[start_index];
 actor_datum *actor=(actor_datum *)(actor_data->data+(datum_index_to_absolute_index(actor_index))*sizeof(actor_datum));
 actor->starting_location_data=start->starting_location_name;
 actor->cell_index=member_index;
 actor->starting_location_index=start_index;
 if(actor->unit_index!=-1)
  ::simulation_action_object_update(actor->unit_index, 0x4000);
}

// @address 0x54d740 ?squad_activate@@YA_NJ@Z
// @address 0x54d1c8 ?squad_place_actor@@YAJFFFJ_N0@Z


static __forceinline scenario *squads_place_scenario_get() { return (scenario *)global_scenario; }
long squad_place_actor(short squad_index, short fire_team_index, short location_index, long emitter, bool collect, bool force)
{
    long result = -1;
    long character = -1;
    ::squad_get_actor_character_definition_index(squad_index, fire_team_index, location_index, (long *)(&character));
    if (character != -1) {
        long actor_index = -1;
        squads_place_squad *s = &((squads_place_squad *)squads_place_scenario_get()->squads.address)[(unsigned short)squad_index];
        squads_place_fire_team *f = &s->fire_teams[fire_team_index];
        actor_starting_location_definition *starting_location = &f->locations[location_index];
        actor_starting_location_definition override_location;
        actor_starting_location_definition *placement = 0;
        object_marker marker;
        if (emitter != -1 && object_get_markers_by_string_id(emitter, 658, &marker, 1) > 0) {
            memcpy(&override_location, starting_location, sizeof(override_location));
            override_location.position.set(&marker.matrix.position, -1, -1);
            euler_angles2d_from_vector3d(&override_location.facing, &marker.matrix.forward);
            override_location.roll = 0.0f;
            placement = &override_location;
        }
        if (!placement) placement = starting_location;
        if (collect) ::objects_garbage_collection();
        short flags = 1;
        if (force) flags |= 2; else flags &= ~2;
        actor_index = actor_place(character, squad_index, fire_team_index, placement, flags);
        if (actor_index != -1) { ::squad_set_actor_starting_location(actor_index, squad_index, fire_team_index, location_index); result = actor_index; }
    }
    return result;
}

// 0x54d328 (dev ?squad_consider_major_upgrade@@YA_NFM@Z)
bool squad_consider_major_upgrade(short squad_index, float chance)
{
    squad_datum *squad = &((squad_datum *)squad_data->data)[(unsigned short)squad_index];
    float global_error = -ai_globals->major_upgrade_bias / 3.0f;
    float squad_error = -squad->upgrade_bias / 1.0f;
    float error = fabs(global_error) > fabs(squad_error) ? global_error : squad_error;
    float random = _real_random_range(tls_random_seed(), 0.0f, 1.0f);
    bool upgrade = random < chance + error;
    float delta = (float)upgrade - chance;
    squad->upgrade_bias += delta;
    ai_globals->major_upgrade_bias += delta;
    return upgrade;
}
static __forceinline short squads_fire_team_location_priority(const actor_starting_location_definition *location)
{
    short result = location->priority;
    if (location->initial_movement_mode & 4)
        result += 100;
    return result;
}
static __forceinline short squads_fire_team_random_range(short minimum, short maximum)
{
    unsigned long *seed = TLS_RANDOM_SEED;
    *seed = 1664525UL * (*seed) + 1013904223UL;
    short result = (short)(((*seed >> 16) * (maximum - minimum)) >> 16);
    return minimum + result;
}
// @address 0x54d410 ?fire_team_pick_actor_starting_location@@YAFFFPAV?$c_static_flags@$0CA@@@0@Z
short fire_team_pick_actor_starting_location(short squad_index, short team_index, c_static_flags<32> *available, c_static_flags<32> *used)
{
    zone *squad_definition = &((zone *)global_scenario->squads.address)[(unsigned short)squad_index];
    squad *team = &squad_definition->squads[team_index];
    short priority = -1;
    short highest_count = 0;
    short available_count = 0;
    short used_count = 0;
    short result = -1;
    for (short i = 0; i < team->count; i++) {
        if (used->test(i))
            used_count++;
        else if (available->test(i)) {
            actor_starting_location_definition *location = (actor_starting_location_definition *)&team->locations[i];
            short p = squads_fire_team_location_priority(location);
            available_count++;
            if (p > priority) {
                priority = p;
                highest_count = 1;
            } else if (p == priority)
                highest_count++;
        }
    }
    if (used_count == team->count)
        result = -1;
    else if (available_count == 0 && team->count > 0) {
        available->set_range(team->count);
        return fire_team_pick_actor_starting_location(squad_index, team_index, available, used);
    } else if (highest_count != 0) {
        short pick;
        if (highest_count > 1)
            pick = squads_fire_team_random_range(0, highest_count);
        else
            pick = 0;
        short seen = 0;
        for (short i = 0; i < team->count; i++) {
            if (available->test(i) && !used->test(i)) {
                actor_starting_location_definition *location = (actor_starting_location_definition *)&team->locations[i];
                if (squads_fire_team_location_priority(location) == priority) {
                    if (seen == pick) {
                        result = i;
                        available->set(result, false);
                        break;
                    } else
                        seen++;
                }
            }
        }
    }
    return result;
}
// 0x54d6a8 (dev ?squad_force_activate@@YA_NJ@Z)
bool squad_force_activate(long squad_index)
{
    squad_datum *squad = DATUM_GET_ABSOLUTE(squad_data, squad_datum, squad_index);
    squad->activation_ticks = (short)game_seconds_to_ticks_round(5.0f);
    squad->flags &= ~0x80;
    return squad_activate(squad_index);
}
bool squad_activate(long squad_index)
{
 squad_datum *squad=DATUM_GET_ABSOLUTE(squad_data,squad_datum,squad_index);
 if(!(squad->flags&0x80))
 {
  bool any_active=false;
  squad->flags|=0x80;
  squad_actor_iterator iterator;
  squad_actor_iterator_new(&iterator,squad_index,false);
  actor_datum *actor=::squad_actor_iterator_next(&iterator);
  while(actor)
  {
   any_active|=(actor->active||actor_set_active(iterator.index,true));
   actor=::squad_actor_iterator_next(&iterator);
  }
  if(any_active)
   squad->last_activation_time=game_time_globals->time;
  else
   squad->flags&=~0x80;
  unsigned short objective_index=squad->objective;
  short objective=(short)objective_index;
  if(objective>=0 && objective<((scenario *)global_scenario_get())->objectives.count)
   ((s_objective_record *)g_objectives_data->data)[objective_index].changed=1;
 }
 return (squad->flags&0x80)!=0;
}

// 0x54d880 (dev ?squad_deactivate@@YAXJ@Z)
void squad_deactivate(long squad_index)
{
    squad_datum *squad = DATUM_GET_ABSOLUTE(squad_data, squad_datum, squad_index);
    squad->flags &= ~0x80;
    squad_actor_iterator iterator;
    squad_actor_iterator_new(&iterator, squad_index, true);
    actor_datum *actor = ::squad_actor_iterator_next(&iterator);
    while (actor) {
        if (actor->active)
            actor_set_active(iterator.index, false);
        actor = ::squad_actor_iterator_next(&iterator);
    }
}

// @address 0x54d910 ?squad_attach_actor@@YAXJF_N@Z
void squad_attach_actor(long actor_index, short squad_index, bool preserve_team)
{
    if (ai_globals->enabled) {
        actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
        squad_datum *squad = &((squad_datum *)squad_data->data)[datum_index_to_absolute_index(squad_index)];
        if (actor->active && !(squad->flags & 0x80))
            squad_activate(squad_index);
        actor->next_squad_actor_index = squad->first_actor_index;
        squad->first_actor_index = actor_index;
        actor->squad_index = squad_index;
        actor_set_active(actor_index, (_rotl(squad->flags, 25) & 1) != 0);
        unsigned char team = squad->team;
        if (actor->team != (signed char)team && !preserve_team)
            actor_set_team(actor_index, (signed char)team);
        if (actor->unit_index != -1)
            squad->active_count = actor->weight + squad->active_count;
        squad->original_count = squad->original_count + real_constant_pool_one();
        squad->flags |= 0x100;
        short rank = ::actor_get_rank(actor_index);
        if (squad->highest_rank < rank)
            squad->highest_rank = rank;
        if (::squad_valid_task((squad_datum *)(const squad_datum *)squad))
            objective_update_leadership(squad->objective, squad->task, 0, actor_index);
    }
}
// @address 0x54da78 ?squad_detach_actor@@YAXJ_N@Z
void squad_detach_actor(long actor_index, bool died)
{
    if (ai_globals->enabled) {
        actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
        long squad_index = actor->squad_index;
        if (squad_index != -1) {
            squad_datum *squad = &((squad_datum *)squad_data->data)[datum_index_to_absolute_index(squad_index)];
            short rank = ::actor_get_rank(actor_index);
            long *actor_index_reference = &squad->first_actor_index;
            long link_index = *actor_index_reference;
            while (link_index != actor_index) {
                actor_index_reference = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(link_index)].next_squad_actor_index;
                link_index = *actor_index_reference;
            }
            *actor_index_reference = actor->next_squad_actor_index;
            if (!died) {
                if (actor->unit_index != -1 && actor->weight > real_constant_pool_zero())
                    squad->active_count -= actor->weight;
                squad->original_count -= real_constant_pool_one();
            }
            if (::squad_valid_task((squad_datum *)(const squad_datum *)squad))
                objective_update_leadership(squad->objective, squad->task, (died != 0) + 1, actor_index);
            actor->next_squad_actor_index = -1;
            actor->squad_index = -1;
            squad->flags |= 0x100;
            if (rank >= squad->highest_rank) {
                short refresh_index = (short)squad_index;
                squad_datum *refresh_squad = &((squad_datum *)squad_data->data)[(unsigned short)refresh_index];
                refresh_squad->highest_rank = 0;
                squad_actor_iterator iterator;
                squad_actor_iterator_new(&iterator, refresh_index, true);
                while (::squad_actor_iterator_next(&iterator)) {
                    short actor_rank = 0;
                    actor_datum *iterator_actor = &((actor_datum *)actor_data->data)[(unsigned short)iterator.index];
                    character_general_properties *properties = character_definition_general_properties_get(iterator_actor->character_index);
                    if (properties)
                        actor_rank = properties->rank;
                    if (actor_rank > refresh_squad->highest_rank)
                        refresh_squad->highest_rank = actor_rank;
                }
            }
        }
    }
}
void squadless_attach_actor(long actor_index)
{
  if (ai_globals->enabled) {
    actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
    actor->next_squad_actor_index = ai_globals->first_actor_index;
    ai_globals->first_actor_index = actor_index;
    actor->squadless = true;
    actor->attach_time = game_time_globals->time;
    actor_flush_position_indices(actor_index, false);
  }
}

extern "C" void squadless_detach_actor(int actor_index)
{
  if (ai_globals->enabled) {
    long *link = (long *)&ai_globals->first_actor_index;
    actor_datum *actors = (actor_datum *)actor_data->data;
    actor_datum *actor = &actors[datum_index_to_absolute_index(actor_index)];
    int index = *link;

    while (index != actor_index) {
      link = &actors[datum_index_to_absolute_index(index)].next_squad_actor_index;
      index = *link;
    }
    *link = actor->next_squad_actor_index;
    actor->squadless = false;
    actor->next_squad_actor_index = -1;
    actor->force_active = false;
  }
}

static __forceinline float squads_adoption_distance3d(const c_ai_point3d *a, const real_point3d *b)
{
    return sqrtf(ai_distance_squared3d(a, b));
}
float squad_adoption_distance_to_actor(long squad_index, long actor_index, short area_type)
{
    actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
    squad_datum *squad = DATUM_GET_ABSOLUTE(squad_data, squad_datum, squad_index);
    float result = FLT_MAX;
    if ((squad->flags & 128) &&
        !game_team_is_enemy((e_game_team)actor->team,
        (e_game_team)(signed char)((squad_datum *)squad_data->data)[datum_index_to_absolute_index(squad_index)].team)) {
        squad_area_iterator iterator;
        squad_area_iterator_new(&iterator, (short)squad_index, 7, area_type, false, _pureform_none);
        while (squad_area_iterator_next(&iterator) != -1) {
            zone_definition *zone = squads_rva_area_zone_get(iterator.zone_index);
            area_definition *area = &((area_definition *)zone->areas.address)[iterator.area_index];
            float distance = squads_adoption_distance3d((const c_ai_point3d *)&area->point, &actor->position.body) - area->radius;
            distance = distance > 0.0f ? distance : 0.0f;
            if (distance < result) result = distance;
        }
    }
    return result;
}

// @address 0x54e810 squad_iterator_next
extern "C" squad_datum *squad_iterator_next(squad_iterator *iterator)
{
 squad_datum *result=0;
 if(ai_globals->enabled) {
  squad_datum *datum=(squad_datum *)data_iterator_next(&iterator->iterator);
  iterator->datum=datum;
  if(datum)
   result=datum;
  iterator->index=(unsigned short)iterator->iterator.index;
 }
 return result;
}

squad_datum *squad_group_squad_iterator_next(squad_group_squad_iterator *iterator)
{
    if (!iterator->squad_group)
        return 0;

    if (iterator->next_squad_index == NONE)
    {
        if (iterator->squad_group == iterator->root_squad_group)
        {
            iterator->squad_group = 0;
            return 0;
        }

        long squad_group_index = iterator->squad_group->first_child_index;
        if (squad_group_index == NONE)
        {
            squad_group_index = iterator->squad_group->parent_index;
            iterator->squad_group = DATUM_GET_ABSOLUTE(squad_group_data, squad_group_datum, squad_group_index);
            iterator->next_squad_index = iterator->squad_group->first_squad_index;
            return squad_group_squad_iterator_next(iterator);
        }
        else
        {
            while (squad_group_index != NONE)
            {
                iterator->squad_group = DATUM_GET_ABSOLUTE(squad_group_data, squad_group_datum, squad_group_index);
                squad_group_index = iterator->squad_group->next_sibling_index;
            }
            iterator->next_squad_index = iterator->squad_group->first_squad_index;
            return squad_group_squad_iterator_next(iterator);
        }
    }

    squad_datum *squad = DATUM_GET_ABSOLUTE(squad_data, squad_datum, iterator->next_squad_index);
    iterator->previous_squad_index = iterator->squad_index;
    iterator->squad_index = iterator->next_squad_index;
    iterator->next_squad_index = squad->next_squad_index;
    return squad;
}

bool squad_ancestor_of_squad_group(unsigned int owner_index, int datum_index)
{
    squad_datum *owner = (squad_datum *)squad_data->data + (datum_index_to_absolute_index(owner_index));
    bool result = false;
    for (int entry_index = owner->parent_index; entry_index != -1; entry_index = 
        ((squad_group_datum *)g_ai_squad_group_data->data + (datum_index_to_absolute_index(entry_index)))->parent_index) {
        if (entry_index == datum_index) {
            result = true;
            break;
        }
    }
    return result;
}




// 0x54ea08
template <>
void *t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    __tls_set_g_squad_data_allocator>::allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x54eaa8 ?deallocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_squad_data_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<> void t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_squad_data_allocator>::deallocate(void *allocation)
{
    squad_data = 0;
    restricted_region_free_member(1, m_member_index);
}

// @address 0x54eac8 ?allocate@?$t_restricted_allocation_manager@$00$0A@$0A@$1?__tls_set_g_squad_group_data_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z

template <>
void *t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    __tls_set_g_squad_group_data_allocator>::allocate(unsigned int size, const char *name)
{
    m_member_index = restricted_region_add_member<t_restricted_allocator_publisher<__tls_set_g_leaf_system_data_allocator> >(
        1, name, "", size, 0, __tls_set_g_leaf_system_data_allocator);
    m_thread_id = g_registered_thread_index;
    return restricted_region_get_member_address(1, m_member_index);
}

// 0x54eb68
template <>
void t_restricted_allocation_manager<1,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),
    &__tls_set_g_squad_group_data_allocator>::deallocate(void *memory)
{
    g_ai_squad_group_data = 0;
    restricted_region_free_member(1, m_member_index);
}

// 0xa9a98 (dev ?squads_update_dirty_status@@YAXXZ)
static __forceinline void squads_dirty_iterator_begin(s_squads_dirty_iterator *it)
{
    if (ai_globals->enabled) {
        it->iterator.data = (s_data_array *)squad_data;
        it->iterator.absolute_index = -1;
        it->iterator.index = -1;
    }
}
static __forceinline squad_datum *squads_dirty_iterator_next(s_squads_dirty_iterator *it)
{
    squad_datum *result = 0;
    if (ai_globals->enabled) {
        it->datum = (squad_datum *)data_iterator_next(&it->iterator);
        if (it->datum) result = it->datum;
        it->index = (unsigned short)it->iterator.index;
    }
    return result;
}
// @address 0xa9a98 ?squads_update_dirty_status@@YAXXZ
void squads_update_dirty_status(void)
{
    s_squads_dirty_iterator iterator;
    squads_dirty_iterator_begin(&iterator);
    squad_datum *squad;
    while ((squad = squads_dirty_iterator_next(&iterator)) != 0) {
        if (squad->flags & 0x100)
            ::squad_update_status(iterator.index);
    }
}

// @address 0x54a090 ?__tls_set_g_leaf_system_data_allocator@@YAXPAX@Z
// @address 0xa0538 ?action_executing@@YA_NJW4e_action@@@Z
// @address 0x5006f0 ?actor_set_active@@YA_NJ_N@Z
// @address 0x1ec830 ?data_set_new_base_address@@YAXPAPAUs_data_array@@PAU1@@Z
// @address 0xc1a90 ?euler_angles2d_from_vector3d@@YAPATreal_euler_angles2d@@PAT1@PBTreal_vector3d@@@Z
// @address 0x319108 ?find_scenario_object_from_scenario@c_object_identifier@@QBAPAUs_scenario_object@@PAUscenario@@PAJ@Z
// @address 0xfcf30 ?internal_object_get_markers_by_string_id@@YAFJJPAUobject_marker@@F_N@Z
// @address 0x1d6bb0 ?restricted_region_free_member@@YAXJJ@Z

// @address 0xcbdb0 ?squad_area_iterator_next@@YAFPAUsquad_area_iterator@@@Z
// @address 0x54a2f0 ?squads_initialize_for_new_map@@YAXXZ
// @address 0x54aae8 ?ai_handle_squad_placement@@YAXJ@Z
// @address 0x54e4f8 ?squad_adoption_distance_to_actor@@YAMJJF@Z
// @address 0x1ec580 ?data_delete_all@@YAXPAUs_data_array@@@Z
// @address 0x9c9f8 ?game_team_is_enemy@@YA_NW4e_game_team@@0@Z
// @address 0xcba08 ?squad_area_iterator_new@@YA_NPAUsquad_area_iterator@@FFF_NW4e_pureform@@@Z
// @address 0xcc0c0 ?zone_get_area@@YAPAUarea_definition@@FF@Z
// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z
// @address 0xaae70 ?datum_new@@YAJPAUs_data_array@@@Z
// @address 0x4fbb30 ?cs_attach_command_script@@YAJJFJ@Z
// @address 0x501c08 ?actor_flush_position_indices@@YAXJ_N@Z
// @address 0x500580 ?actor_set_team@@YAXJF@Z
// @address 0xadcf0 ?character_definition_general_properties_get@@YAPAUcharacter_general_properties@@J@Z
// @address 0x320050 ?find_object_index@c_object_identifier@@QBAJXZ
// @address 0xaaca8 ?game_difficulty_level_get@@YAFXZ
// @address 0x5103b8 ?objective_update_leadership@@YAXFFFJ@Z


static __forceinline actor_datum *squads_status_actor_get(long index)
{
    return &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(index)];
}


// @address 0xb4980 ?actor_update_squad_status@@YAXJ@Z
void actor_update_squad_status(long index)
{
    actor_datum *a = squads_status_actor_get(index);
    if (a->seeking_squad) {
        if (a->squad_seek_ticks > 0) --a->squad_seek_ticks;
        if (a->squad_seek_ticks == 0) {
            long squad = squad_actor_adopt_squad(index, actor_get_area_type(index));
            if (squad == -1 && a->original_squad_index != -1 && a->squad_seek_attempts >= 3)
                squad = a->original_squad_index;
            if (squad != -1) {
                if (squad != a->squad_index) {
                    actor_change_squad(index, (short)squad);
                    actor_attach_to_vehicle(index, -1, false);
                    if (a->support_object_index != -1)
                        actor_dynamic_firing_set_support_object(index, -1);
                }
                a->seeking_squad = false;
            } else {
                a->squad_seek_ticks = (short)game_seconds_to_ticks_integer(3);
                ++a->squad_seek_attempts;
            }
        }
    }
}

// 0x509868

// @address 0x5063c0 ?ai_index_squad_iterator_next@@YAPAUsquad_datum@@PAUai_index_squad_iterator@@@Z
// @address 0xadb80 ?actor_runtime_within_structure_bsp@@YA_NJ@Z
// @address 0x500ab8 actor_switch_bsp
// @address 0x110a98 ?object_detach@@YAXJ@Z
// @address 0x318a80 ?object_set_position_direct@@YAXJPBTreal_point3d@@PBTreal_vector3d@@1PBUs_location@@_N@Z
// @address 0x50ccc8 ?ai_reference_frame_transform_point@@YA_NFPBTreal_point3d@@PAT1@@Z

// @address 0x509868 ?squads_test_activation@@YAXJ@Z
void squads_test_activation(long ai_index)
{
    unsigned long ai_type = (unsigned long)ai_index >> 29;
    if (ai_type != 1 && ai_type != 2)
        return;
    ai_index_squad_iterator squad_iterator;
    ::ai_index_squad_iterator_new(ai_index, &squad_iterator);
    while (ai_index_squad_iterator_next(&squad_iterator) != 0) {
        short total = 0;
        zone *zone_info = &((zone *)global_scenario->squads.address)[datum_index_to_absolute_index(squad_iterator.squad_index)];
        long squad_index = squad_iterator.squad_index;
        short j = 0;
        long squad_ref = datum_index_to_absolute_index(squad_index);
        long zone_squad_count = zone_info->count;
        s_fire_team_location_pair pairs[320];
        long squad_total = zone_squad_count;
        if (zone_squad_count > 0) {
            squad *zone_squads = zone_info->squads;
            do {
                short k = 0;
                long seat_count = zone_squads[j].count;
                if (seat_count > 0) {
                    do {
                        pairs[total].fire_team_index = j;
                        pairs[total].location_index = k;
                        total++;
                        k++;
                    } while (k < seat_count);
                }
                j++;
            } while (j < squad_total);
        }
        if (total <= 0)
            continue;
        unsigned long used[10];
        memset(used, 0, sizeof(used));
        short placed = 0;
        short actor_count = 0;
        struct { real_point3d point; long point_pad; squad_actor_iterator iterator; } scratch;
        if (ai_globals->enabled) {
            scratch.iterator.squad_index = squad_index;
            scratch.iterator.index = -1;
            scratch.iterator.active_only = false;
            if (squad_index == -1)
                scratch.iterator.next_index = ai_globals->first_actor_index;
            else
                scratch.iterator.next_index = (&((squad_datum *)squad_data->data)[squad_ref])->first_actor_index;
        }
        long actor_list[258];
        actor_datum *actor = ::squad_actor_iterator_next(&scratch.iterator);
        while (actor != 0) {
            long listed_index = scratch.iterator.index;
            if (!actor_runtime_within_structure_bsp(listed_index) && actor->unit_index != -1) {
                actor_list[actor_count] = listed_index;
                actor_count++;
            }
            actor = ::squad_actor_iterator_next(&scratch.iterator);
        }
        if (actor_count > 0) {
            short i = 0;
            do {
                long actor_index = actor_list[i];
                actor_datum *listed_actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
                float best;
                short best_pair;
                short attempt = 0;
                do {
                    best = k_real_max;
                    best_pair = -1;
                    for (short v = 0; v < total; v++) {
                        if ((used[v >> 5] & (unsigned long)(1 << (v & 31))) == 0) {
                            squads_test_start *spot = (squads_test_start *)&zone_info->squads[pairs[v].fire_team_index].locations[pairs[v].location_index];
                            float distance = ai_distance_squared3d(&spot->point, &listed_actor->position.body);
                            if (distance < best) {
                                best = distance;
                                best_pair = v;
                            }
                        }
                    }
                    if (best_pair != -1)
                        break;
                    attempt++;
                    memset(used, 0, sizeof(used));
                } while (attempt < 2);
                if (best_pair != -1) {
                    long object_index = listed_actor->unit_index;
                    squads_test_start *spot = (squads_test_start *)&zone_info->squads[pairs[best_pair].fire_team_index]
                        .locations[pairs[best_pair].location_index];
                    const c_ai_point3d *point = &spot->point;
                    s_object_header *header = &((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(object_index)];
                    object_datum *object = (object_datum *)header->datum;
                    if ((unsigned short)point->attachment_index == 0xffff ||
                        !ai_reference_frame_transform_point(point->attachment_index, &point->point, &scratch.point))
                        scratch.point = point->point;
                    if (object->parent_object_index != -1)
                        object_detach(object_index);
                    object_set_position_direct(listed_actor->unit_index, &scratch.point, 0, 0, 0, false);
                    used[best_pair >> 5] |= (unsigned long)(1 << (best_pair & 31));
                    placed++;
                    actor_switch_bsp(actor_index);
                }
                i++;
            } while (i < actor_count);
        }
        squad_index = squad_iterator.squad_index;
        if (placed > 0)
            ai_handle_squad_placement(squad_index);
    }
}

static __forceinline void squads_adopt_iterator_begin(squad_iterator *iterator)
{
    if (ai_globals->enabled)
    {
        iterator->iterator.data = (s_data_array *)squad_data;
        iterator->iterator.absolute_index = NONE;
        iterator->iterator.index = NONE;
    }
}

static __forceinline squad_datum *squads_adopt_iterator_next(squad_iterator *iterator)
{
    squad_datum *result = 0;
    if (ai_globals->enabled)
    {
        iterator->datum = (squad_datum *)data_iterator_next(&iterator->iterator);
        if (iterator->datum)
            result = iterator->datum;
        iterator->index = (unsigned short)iterator->iterator.index;
    }
    return result;
}

// @address 0x54e638 ?squad_actor_adopt_squad@@YAJJF@Z
long squad_actor_adopt_squad(long actor_index, short area_type)
{
    actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
    long best_squad = -1;
    float best_distance = 15.0f;
    squad_iterator squads;
    squads_adopt_iterator_begin(&squads);
    squad_datum *squad;
    while ((squad = squads_adopt_iterator_next(&squads)) != 0) {
        if (!(squad->flags & 128)) continue;
        if (objective_task_is_vehicle_task(squad->objective, squad->task) && actor->attached_vehicle_index == -1) continue;
        if ((signed char)squad->team != actor->team) continue;
        float distance = squad_adoption_distance_to_actor(squads.index, actor_index, area_type);
        if (distance >= k_real_max) continue;
        if (squads.index == actor->original_squad_index) distance *= 0.5f;
        if (distance >= best_distance) continue;
        squad_actor_iterator members;
        bool same_type = false;
        squad_actor_iterator_new(&members, squads.index, false);
        actor_datum *member;
        while ((member = ::squad_actor_iterator_next(&members)) != 0) {
            if (squads.index != actor_index && member->type == actor->type) {
                same_type = true;
                break;
            }
        }
        if (!same_type) distance *= 2.0f;
        if (distance < best_distance) { best_squad = squads.index; best_distance = distance; }
    }
    return best_squad;
}


static __forceinline long squads_load_root_object(long index)
{
 while(index != -1) {
  const vehicle_datum *object = (const vehicle_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(index)].datum;
  bool attached = ((object->object.flags >> 26) & 1) != 0;
  if(!attached) break;
  index = object->object.parent;
 }
 return index;
}
// @address 0x54dd40 ?squad_load_in_squad@@YAXJJ@Z
void squad_load_in_squad(long squad_index, long destination_index)
{
    if (squad_index == -1 || destination_index == -1) return;
    squad_datum *source = &((squad_datum *)squad_data->data)[datum_index_to_absolute_index(squad_index)];
    squad_datum *destination = &((squad_datum *)squad_data->data)[datum_index_to_absolute_index(destination_index)];
    for (long vehicle_index = source->first_vehicle_index; vehicle_index != -1;) {
        vehicle_datum *vehicle = (vehicle_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(vehicle_index)].datum;
        vehicle_definition *definition = (vehicle_definition *)global_tag_instances[datum_index_to_absolute_index(vehicle->object.definition)].definition;
        for (long target = destination->first_vehicle_index; target != -1;) {
            long selected = -1;
            vehicle_datum *target_object = (vehicle_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(target)].datum;
            short seat_type = (definition->havok_vehicle_physics.size != 0) + 4;
            short seat = ::squad_placement_find_seat(target, seat_type, (long *)(&selected), (short *)(0));
            if (seat != -1) {
                squads_action_request request;
                request.type = _action_vehicle_enter;
                request.seat.parent_index = selected;
                request.seat.seat_index = seat;
                request.flag_a = true;
                request.flag_b = false;
                if (action_submit(vehicle_index, &request)) break;
            }
            target = target_object->vehicle.next_vehicle_index;
        }
        vehicle_index = vehicle->vehicle.next_vehicle_index;
    }
    squad_actor_iterator iterator;
    squad_actor_iterator_new(&iterator, squad_index, true);
    actor_datum *actor;
    while ((actor = ::squad_actor_iterator_next(&iterator)) != 0) {
        vehicle_datum *unit = (vehicle_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(actor->unit_index)].datum;
        if (unit->object.parent != -1) {
            long parent = squads_load_root_object(unit->object.parent);
            vehicle_datum *root = (vehicle_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(parent)].datum;
            if (root->object.type == 1 &&
                datum_index_to_absolute_index(root->vehicle.squad_index) != datum_index_to_absolute_index(squad_index))
                action_submit_event(actor->unit_index,
        20);
        }
        if (unit->object.parent == -1) {
            for (long target = destination->first_vehicle_index; target != -1;) {
                long selected = -1;
                vehicle_datum *target_object = (vehicle_datum *)((s_object_header *)object_header_data->data)[datum_index_to_absolute_index(target)].datum;
                short seat = ::squad_placement_find_seat(target, 0, (long *)(&selected), (short *)(0));
                if (seat != -1) {
                    squads_action_request request;
                    request.type = _action_vehicle_enter;
                    request.seat.parent_index = selected;
                    request.seat.seat_index = seat;
                    request.flag_a = true;
                    request.flag_b = false;
                    if (action_submit(actor->unit_index, &request)) break;
                }
                target = target_object->vehicle.next_vehicle_index;
            }
        }
    }
}

// @address 0x54ca40 ?squad_update_status@@YAXJ@Z
void squad_update_status(long squad_index)
{
    squad_datum *squad = DATUM_GET_ABSOLUTE(squad_data, squad_datum, squad_index);
    bool enemy = false;
    squad->flags = (short)(squad->flags & -33);
    squad->reserved_count = 0;
    squad->current_non_swarm_count = 0;
    squad->current_swarm_count = 0;
    squad->strength = 0;
    squad->status = 0.0f;
    squad->status_level = 0;
    squad_actor_iterator iterator;
    squad_actor_iterator_new(&iterator, squad_index, false);
    actor_datum *actor;
    while ((actor = squad_actor_iterator_next(&iterator)) != 0) {
        float strength = 0.0f;
        short actor_count = 0;
        if (actor->unit_index != -1) {
            object_datum *unit = object_get(actor->unit_index);
            strength = unit->body_vitality * unit->maximum_body_vitality;
            actor_count = 1;
            squad->status += strength;
            squad->current_non_swarm_count++;
        } else if (actor->swarm_index != -1) {
            swarm_datum *swarm = (swarm_datum *)((char *)g_swarm_data->data + ((actor->swarm_index & 0xffff)) * 52);
            actor_count = swarm->creature_count;
            squad->current_swarm_count = (short)(squad->current_swarm_count + actor_count);
        }
        squad->strength = (short)(squad->strength + actor_count);
        squad->reserved_count = (short)(squad->reserved_count + actor_count * (short)actor_in_combat(iterator.index));
        if (actor->target_prop_index != -1) {
            prop_ref_datum *prop_ref = (prop_ref_datum *)((char *)prop_ref_data->data + ((actor->target_prop_index & 0xffff)) * 60);
            if (!game_team_is_ally((e_game_team)actor->team,
                    (e_game_team)((prop_datum *)((char *)prop_data->data + ((prop_ref->prop_index & 0xffff)) * 196))->team))
                enemy = true;
            if (actor->active) {
                if (actor->combat_status > squad->status_level)
                    squad->status_level = (signed char)actor->combat_status;
                if (actor->combat_status >= 7)
                    squad->flags = (short)(squad->flags | 32);
            }
        }
    }
    if (enemy)
        squad->flags = (short)(squad->flags & -65);
    if (squad->active_count > 0.0f) {
        float v = squad->status / squad->active_count;
        if (!(v >= 0.0f))
            v = 0.0f;
        squad->status = v;
    }
    squad->flags = (short)(squad->flags & -257);
    squad->centroid = *global_origin3d;
    if (squad->strength > 0) {
        short count = 0;
        squad_actor_iterator iterator2;
        squad_actor_iterator_new(&iterator2, squad_index, true);
        while ((actor = squad_actor_iterator_next(&iterator2)) != 0) {
            add_points3d(&actor->position.body, &squad->centroid, &squad->centroid);
            count++;
        }
        if (count > 0) {
            scale_point3d(&squad->centroid, 1.0f / (float)count, &squad->centroid);
        } else {
            squad_actor_iterator_new(&iterator2, squad_index, false);
            squad->centroid = *global_origin3d;
            while ((actor = squad_actor_iterator_next(&iterator2)) != 0) {
                add_points3d(&actor->position.body, &squad->centroid, &squad->centroid);
                count++;
            }
            if (count > 0) {
                scale_point3d(&squad->centroid, 1.0f / (float)count, &squad->centroid);
            }
        }
    }
    if (::squad_valid_task(squad) && objective_get_task(squad->objective, squad->task)->follow_mode != 0) {
        s_tag_block *areas = 0;
        float distance = 1.0f;
        s_task *task = (s_task *)::squad_get_task(squad);
        if (task->follow_mode == 1) {
            areas = &task->areas;
            distance = task->distance;
        }
        if (areas != 0) {
            if (squad->vehicle_index == -1 || game_ticks_to_seconds((float)(game_time_get() - squad->vehicle_index)) > 2.0f)
                calculate_dynamic_areas(squad_index, areas, distance);
        } else {
            squad->follow_object_index = -1;
        }
    } else {
        squad->follow_object_index = -1;
    }

}

// @address 0x54e000 ?calculate_dynamic_areas@@YAXJPAUs_tag_block@@M@Z
void calculate_dynamic_areas(long squad_index, s_tag_block *tag_block, float falloff)
{
    squad_datum *squad = DATUM_GET_ABSOLUTE(squad_data, squad_datum, squad_index);
    squad_datum *areas = (squad_datum *)squad;
    bool best_active = false;
    long best_object = -1;
    float best_distance = k_real_max;
    for (short i = 0; i < 4; i++) {
        const long &player_index = tls_ai_player_states()[i].id;
        if (player_index == -1)
            continue;
        player_datum *player = (player_datum *)(player_data->data + (datum_index_to_absolute_index(player_index)) * sizeof(player_datum));
        long unit_index = player->unit_index;
        if (unit_index == -1)
            continue;
        real_point3d position;
        object_get_origin(unit_index, &position);
        float dx = squad->centroid.x - position.x;
        float dy = squad->centroid.y - position.y;
        float dz = squad->centroid.z - position.z;
        float distance = sqrtf(dx * dx + dy * dy + dz * dz);
        if ((squad->flags & 0x800) != 0) {
            bool active = ai_player_has_no_nearby_vehicle(i);
            if ((active && !best_active) || (distance < best_distance && active == best_active)) {
                best_active = active;
                best_object = unit_index;
            }
        } else if (distance < best_distance) {
            best_object = unit_index;
        }
    }
    squad->follow_object_index = best_object;
    if (best_object != -1) {
        zone_area_ref fallback;
        // match: 0x54e000: direct copy initialization lets c2 reuse the first sentinel load at the final comparison.
        memcpy(&fallback, &ZONE_AREA_REF_NONE, sizeof(fallback));
        float nearest_area_distance = k_real_max;
        areas->num_dynamic_areas = 0;
        real_point3d position;
        object_get_origin(best_object, &position);
        if (tag_block->count > 0) {
            for (short i = 0; i < tag_block->count; i++) {
                long area_count = areas->num_dynamic_areas;
                if (area_count >= 10)
                    break;
                squads_area_zone_set_entry *entry = &((squads_area_zone_set_entry *)tag_block->address)[i];
                short zone_index = entry->zone_index;
                scenario *scenario = global_scenario_get();
                short area_index = entry->area_index;
                if (zone_index >= 0 && zone_index < scenario->zones.count) {
                    zone_definition *zone = &((zone_definition *)scenario->zones.address)[(unsigned short)zone_index];
                    if (area_index >= 0 && area_index < zone->areas.count) {
                        area_definition *area = &((area_definition *)zone->areas.address)[area_index];
                        if (area) {
                            float distance = sqrtf(ai_distance_squared3d((const c_ai_point3d *)&area->point, &position));
                            if (distance < area->radius + falloff) {
                                areas->dynamic_areas[area_count].zone_index = zone_index;
                                areas->dynamic_areas[areas->num_dynamic_areas].area_index = entry->area_index;
                                areas->num_dynamic_areas++;
                            } else if (distance < nearest_area_distance) {
                                nearest_area_distance = distance;
                                fallback.area_index = area_index;
                                fallback.zone_index = zone_index;
                            }
                        }
                    }
                }
            }
        } else {
            scenario *scenario = global_scenario_get();
            squad_definition *squad_def = &((squad_definition *)scenario->squads.address)[datum_index_to_absolute_index(squad_index)];
            if (squad_def->zone_index >= 0 && squad_def->zone_index < scenario->zones.count) {
                zone_definition *zone = &((zone_definition *)scenario->zones.address)[(unsigned short)squad_def->zone_index];
                    for (short i = 0; i < zone->areas.count; i++) {
                        long area_count = areas->num_dynamic_areas;
                        if (area_count >= 10)
                            break;
                        area_definition *area = &((area_definition *)zone->areas.address)[i];
                        float distance = sqrtf(ai_distance_squared3d((const c_ai_point3d *)&area->point, &position));
                        if (distance < area->radius + falloff) {
                            areas->dynamic_areas[area_count].zone_index = squad_def->zone_index;
                            areas->dynamic_areas[areas->num_dynamic_areas].area_index = i;
                            areas->num_dynamic_areas++;
                        } else if (distance < nearest_area_distance) {
                            nearest_area_distance = distance;
                            fallback.area_index = i;
                            fallback.zone_index = squad_def->zone_index;
                        }
                    }
                }
        }
        if (areas->num_dynamic_areas == 0 && fallback.value != ZONE_AREA_REF_NONE.value) {
            areas->dynamic_areas[0] = fallback;
            areas->num_dynamic_areas = 1;
        }
        squad_actor_iterator iterator;
        if (ai_globals->enabled)
            squad_actor_iterator_new_inline(&iterator, squad_index, false);
        actor_datum *actor = (actor_datum *)squad_actor_iterator_next(&iterator);
        if (actor) {
            do {
                firing_position_ref current = actor->firing_position_reference;
                bool keep = false;
                if (!fp_ref_equals(current, FP_REF_NONE)) {
                    if ((current.block & 0x8000) == 0 && !fp_ref_equals(current, FP_REF_CURRENT)) {
                        firing_position_definition *definition = firing_position_ref_get_definition(current);
                        if (definition) {
                            for (short i = 0; i < areas->num_dynamic_areas; i++) {
                                if (areas->dynamic_areas[i].zone_index == current.block &&
                                    areas->dynamic_areas[i].area_index == definition->area) {
                                    keep = true;
                                    break;
                                }
                            }
                        } else {
                            keep = false;
                        }
                    }
                }
                if (!keep) {
                    actor_set_current_firing_position(iterator.index, FP_REF_NONE.value);
                    actor->search_active = 0;
                }
                actor = (actor_datum *)squad_actor_iterator_next(&iterator);
            } while (actor);
        }
    }
    squad->vehicle_index = game_time_get();
}

static __forceinline long squads_update_seconds_to_ticks_round(float seconds)
{
    return (long)((float)game_time_globals->ticks_per_second * seconds + 0.5f);
}
static __forceinline long squads_update_half_second_to_ticks_round(void)
{
    float prod = (float)game_time_globals->ticks_per_second * 0.5f;
    return (long)(prod + 0.5f);
}
static __forceinline void squads_update_groups_begin(s_squads_update_iterator *iterator)
{
    if (ai_globals->enabled) {
        iterator->iterator.data = (s_data_array *)g_ai_squad_group_data;
        iterator->iterator.absolute_index = -1;
        iterator->iterator.index = -1;
    }
}
static __forceinline void *squads_update_groups_next(s_squads_update_iterator *iterator)
{
    void *result = 0;
    if (ai_globals->enabled) {
        iterator->datum = data_iterator_next(&iterator->iterator);
        if (iterator->datum) result = iterator->datum;
        iterator->index = iterator->iterator.index;
    }
    return result;
}
static __forceinline void squads_update_squads_begin(s_squads_update_iterator *iterator)
{
    if (ai_globals->enabled) {
        iterator->iterator.data = (s_data_array *)squad_data;
        iterator->iterator.absolute_index = -1;
        iterator->iterator.index = -1;
    }
}
static __forceinline void *squads_update_squads_next(s_squads_update_iterator *iterator)
{
    void *result = 0;
    if (ai_globals->enabled) {
        iterator->datum = data_iterator_next(&iterator->iterator);
        if (iterator->datum) result = iterator->datum;
        iterator->index = (unsigned short)iterator->iterator.index;
    }
    return result;
}
static __forceinline void squad_groups_test_activation(s_squads_update_iterator *iterator)
{
    squads_update_groups_begin(iterator);
    squad_group_datum *group;
    while ((group = (squad_group_datum *)squads_update_groups_next(iterator)) != 0) {
        if (group->parent_index == -1)
            squad_group_test_activation(iterator->index);
    }
}
static __forceinline void squad_groups_update_status(s_squads_update_iterator *iterator)
{
    squads_update_groups_begin(iterator);
    squad_group_datum *group;
    while ((group = (squad_group_datum *)squads_update_groups_next(iterator)) != 0) {
        if (group->parent_index == -1)
            squad_group_update_status(iterator->index);
    }
}
// @address 0xa9858 ?squads_update@@YAXXZ
void squads_update(void)
{
    long time = game_time_get();
    long activation_interval = (long)((float)game_time_globals->ticks_per_second + 0.5f);
    long group_interval = squads_update_seconds_to_ticks_round(2.0f);
    long squad_interval = squads_update_half_second_to_ticks_round();
    s_squads_update_iterator iterator;
    if (time % activation_interval == 0) {
        squads_update_dirty_status();
        ai_update_activation();
        squad_groups_test_activation(&iterator);
    }
    if (time % group_interval == 10) {
        squad_groups_update_status(&iterator);
    }
    squads_update_squads_begin(&iterator);
    squad_datum *squad;
    while ((squad = (squad_datum *)squads_update_squads_next(&iterator)) != 0) {
        if (((unsigned short)iterator.index + time) % squad_interval == 0 && (squad->flags & 0x80))
            squad_update_status(iterator.index);
    }
    squads_update_dirty_status();
}



static __forceinline long squads_create_root_object(long object_index)
{
    long root = -1;
    while (object_index != -1) {
        root = object_index;
        object_index = ((vehicle_datum *)object_get_and_verify_type(object_index, 0xffffffff))->object.parent_object_index;
    }
    return root;
}
static __forceinline void squads_create_add_vehicle(long squad_index, long vehicle_index)
{
    squad_datum *squad = &((squad_datum *)squad_data->data)[datum_index_to_absolute_index(squad_index)];
    vehicle_datum *vehicle = ((vehicle_datum *)object_get_and_verify_type(vehicle_index, 2));
    vehicle->vehicle.next_vehicle_index = squad->first_vehicle_index;
    squad->first_vehicle_index = vehicle_index;
}

// @address 0x54b578 ?fire_team_create@@YAFJFFF_N00PA_N@Z
short fire_team_create(long squad_index, short fire_team_index, short location_index, short count,
    bool force, bool collect, bool created_by_script, bool *all_actors_from_emitters)
{
    squad_datum *squad = &((squad_datum *)squad_data->data)[datum_index_to_absolute_index(squad_index)];
    squad_definition *definition = &((squad_definition *)global_scenario_get()->squads.address)[datum_index_to_absolute_index(squad_index)];
    squads_create_fire_team *fire_team = &definition->fire_teams[fire_team_index];
    long first_vehicle_index = -1;
    c_static_flags<32> available;
    c_static_flags<32> unusable;
    short objective_index = -1;
    short task_index = -1;
    short placed_count = 0;
    if (all_actors_from_emitters)
        *all_actors_from_emitters = true;
    if (location_index != -1)
        count = 1;
    if (squad->flags & 4) {
        objective_index = squad->objective_index;
        task_index = squad->task_index;
    }
    if (count == -1)
        count = fire_team->initial_count;
    available.set_range(fire_team->location_count);
    unusable.clear();
    if (location_index == -1)
        mark_unusable_starting_locations((short)squad_index, fire_team_index, &unusable);
    else if (!force && !starting_location_emitter_is_usable((short)squad_index, fire_team_index, location_index))
        count = 0;
    for (short i = 0; i < count; i++) {
        short starting_location_index = location_index != -1 ? location_index :
            fire_team_pick_actor_starting_location((short)squad_index, fire_team_index, &available, &unusable);
        if (starting_location_index >= 0 && starting_location_index < fire_team->location_count) {
            actor_starting_location_definition *location = &fire_team->locations[starting_location_index];
            long emitter_index = -1;
            bool emitted = false;
            placed_count++;
            if (location->emitter_palette_index != -1) {
                emitter_index = emitter_index_get_runtime_object_index(location->emitter_palette_index);
                if (!force && emitter_index != -1) {
                    squads_action_request request;
                    request.type = _action_emit_ai;
                    request.emit_ai.squad_index = (short)squad_index;
                    request.emit_ai.spawn_index = fire_team_index;
                    request.emit_ai.location_index = starting_location_index;
                    if (action_submit(emitter_index, &request))
                        emitted = true;
                }
            }
            if (!emitted) {
                long vehicle_index = -1;
                short vehicle_seat_index = -1;
                scenario *scenario_data = global_scenario_get();
                if (all_actors_from_emitters)
                    *all_actors_from_emitters = false;
                long vehicle_type = location->vehicle_type;
                if (vehicle_type == -1)
                    vehicle_type = fire_team->vehicle_type;
                if (vehicle_type >= 0 && vehicle_type < scenario_data->vehicle_palette.count &&
                    vehicle_type != -1 && location->seat_type != 7) {
                    vehicle_index = squad_placement_find_vehicle((short)vehicle_type, location->seat_type, first_vehicle_index, &vehicle_seat_index);
                    if (vehicle_index == -1) {
                        const long &vehicle_offset = vehicle_type * 48;
                        scenario_object_palette_entry *vehicle_definition =
                            // match: 0x54b578: zero-bit rotate preserves 32-bit GPR value
                            (scenario_object_palette_entry *)((char *)scenario_data->vehicle_palette.address + _rotl(vehicle_offset, 0));
                        object_placement_data placement;
                        object_placement_data_new(&placement, vehicle_definition->object.index, -1, 0);
                        ai_point_get_position(&location->position, &placement.position);
                        vectors3d_from_euler_angles2d(&placement.forward, &placement.up, &location->facing);
                        placement.model_variant_index = location->vehicle_variant ? location->vehicle_variant :
                            fire_team->vehicle_variant ? fire_team->vehicle_variant : 0;
                        placement.multiplayer_cinematic_object = created_by_script;
                        vehicle_index = object_new(&placement);
                        vehicle_seat_index = -1;
                        if (vehicle_index != -1) {
                            vehicle_datum *vehicle = ((vehicle_datum *)object_get_and_verify_type(vehicle_index, 2));
                            vehicle->vehicle.squad_index = squad_index;
                            vehicle->vehicle.next_vehicle_index = first_vehicle_index;
                            vehicle->vehicle.starting_location_name = location->starting_location_name;
                            first_vehicle_index = vehicle_index;
                            if (squad->team == 0) {
                                short team = ((unit_definition *)tag_get('unit', vehicle->object.definition_index))->default_team;
                                if (team != 0)
                                    squad->team = (signed char)team;
                            } else {
                                vehicle->unit.team = static_cast<e_game_team>(squad->team);
                            }
                            simulation_action_object_create(vehicle_index);
                            if (location->seat_type != 6) {
                                long selected_vehicle = -1;
                                vehicle_seat_index = squad_placement_find_seat(vehicle_index, location->seat_type, &selected_vehicle, 0);
                                if (vehicle_seat_index != -1)
                                    vehicle_index = selected_vehicle;
                            }
                            if (location->initial_movement_mode & (1 << 5))
                                vehicle->unit.unit_flags |= 0x40000000;
                        }
                    }
                    if (vehicle_index != -1) {
                        vehicle_datum *vehicle = ((vehicle_datum *)object_get_and_verify_type(vehicle_index, 2));
                        if (location->initial_movement_mode & (1 << 4)) {
                            long root_index = squads_create_root_object(vehicle_index);
                            if (root_index != -1 && ((1 << ((s_object_header *)object_header_data->data)
                                [datum_index_to_absolute_index(root_index)].object_type) & 0x1003))
                                ((vehicle_datum *)object_get_and_verify_type(root_index, 0x1003))->unit.flags400 |= 2;
                            else
                                vehicle->unit.flags400 |= 2;
                        }
                        if ((long)definition->flags & (1 << 5))
                            vehicle->unit.unit_flags |= 0x1000;
                    }
                }
                if (location->seat_type != 6) {
                    long actor_index = squad_place_actor(
                        (short)squad_index, fire_team_index, starting_location_index, emitter_index, collect, created_by_script);
                    if (actor_index != -1) {
                        squad->count++;
                        if (objective_index != -1) {
                            if (task_index != -1) {
                                s_objective *objective = &(
                                    (s_objective *)global_scenario_get()->objectives.address)[objective_index];
                                s_task *task = &((s_task *)objective->tasks.address)[task_index];
                                actor_handle_task_change(actor_index, task);
                            } else {
                                actor_handle_change_internal(actor_index, false, false);
                            }
                        }
                        if (vehicle_index != -1 && vehicle_seat_index != -1) {
                            actor_datum *actor = &((actor_datum *)actor_data->data)[datum_index_to_absolute_index(actor_index)];
                            squads_action_request request;
                            request.type = _action_vehicle_enter;
                            request.seat.parent_index = vehicle_index;
                            request.seat.seat_index = vehicle_seat_index;
                            request.flag_a = true;
                            request.flag_b = false;
                            if (action_submit(actor->unit_index, &request)) {
                                vehicle_datum *vehicle = ((vehicle_datum *)object_get_and_verify_type(vehicle_index, 2));
                                long root_vehicle_index = ai_get_root_vehicle(vehicle_index);
                                vehicle->unit.team = static_cast<e_game_team>(actor->team);
                                actor_attach_to_vehicle(actor_index, root_vehicle_index, true);
                                actor_attach_to_seat(actor_index, vehicle_index, vehicle_seat_index, 0.0f, true);
                            }
                        }
                        squad_run_placement_script(actor_index, (short)squad_index, fire_team_index, starting_location_index);
                    }
                }
            }
        }
    }
    if (first_vehicle_index != -1) {
        long vehicle_index = first_vehicle_index;
        while (vehicle_index != -1) {
            long index = vehicle_index;
            vehicle_datum *vehicle = ((vehicle_datum *)object_get_and_verify_type(index, 2));
            vehicle_index = vehicle->vehicle.next_vehicle_index;
            squads_create_add_vehicle(squad_index, index);
        }
    }
    return placed_count;
}

// @address 0x54aff0
void squad_create(long squad_index, short fire_team_index, short location_index, short count,
    bool force, bool collect, bool created_by_script)
{
    if (ai_globals->enabled) {
        squad_definition *definition = &((squad_definition *)global_scenario_get()->squads.address)[datum_index_to_absolute_index(squad_index)];
        bool all_actors_from_emitters = true;
        if (fire_team_index != -1 && location_index != -1) {
            if (fire_team_index >= 0 && fire_team_index < definition->locations_count)
                fire_team_create(squad_index, fire_team_index, location_index, count, force, collect, created_by_script, &all_actors_from_emitters);
        } else {
            short placement_counts[10];
            unsigned long inhibited_fire_teams[1];
            memset(inhibited_fire_teams, 0, sizeof(inhibited_fire_teams));
            for (short index = 0; index < definition->locations_count; index++) {
                squads_create_fire_team *fire_team = &definition->fire_teams[index];
                placement_counts[index] = 0;
                if (fire_team->difficulty_mask > 0 && (fire_team->difficulty_mask & (1 << game_difficulty_level_get())) == 0)
                    inhibited_fire_teams[index >> 5] |= 1 << (index & 31);
            }
            if (count != -1) {
                short placed_count = 0;
                short total_count = 0;
                short normal_count = 0;
                short always_count = 0;
                short normal_counts[10];
                short always_counts[10];
                short total_counts[10];
                for (short index = 0; index < definition->locations_count; index++) {
                    squads_create_fire_team *fire_team = &definition->fire_teams[index];
                    placement_counts[index] = 0;
                    always_counts[index] = 0;
                    total_counts[index] = 0;
                    normal_counts[index] = 0;
                    if ((inhibited_fire_teams[index >> 5] & (1 << (index & 31))) == 0) {
                        for (short start_index = 0; start_index < fire_team->location_count; start_index++) {
                            actor_starting_location_definition *location = &fire_team->locations[start_index];
                            if (location->flags == 0 || (location->flags & (1 << game_difficulty_level_get())) != 0) {
                                total_counts[index]++;
                                if (location->initial_movement_mode & 4)
                                    always_counts[index]++;
                            }
                        }
                        total_count += total_counts[index];
                        always_count += always_counts[index];
                        normal_counts[index] = total_counts[index] - always_counts[index];
                    }
                }
                normal_count = total_count - always_count;
                while (placed_count < count) {
                    if (always_count > 0) {
                        short choice = squads_fire_team_random_range(0, always_count);
                        short accumulated = 0;
                        for (short index = 0; index < definition->locations_count; index++) {
                            accumulated += always_counts[index];
                            if (accumulated > choice) {
                                placed_count++;
                                placement_counts[index]++;
                                always_count--;
                                always_counts[index]--;
                                break;
                            }
                        }
                    } else if (normal_count > 0) {
                        short choice = squads_fire_team_random_range(0, normal_count);
                        short accumulated = 0;
                        for (short index = 0; index < definition->locations_count; index++) {
                            accumulated += normal_counts[index];
                            if (accumulated > choice) {
                                placed_count++;
                                placement_counts[index]++;
                                normal_count--;
                                normal_counts[index]--;
                                break;
                            }
                        }
                    } else if (total_count > 0) {
                        short choice = squads_fire_team_random_range(0, total_count);
                        short accumulated = 0;
                        for (short index = 0; index < definition->locations_count; index++) {
                            accumulated += total_counts[index];
                            if (accumulated > choice) {
                                placed_count++;
                                placement_counts[index]++;
                                break;
                            }
                        }
                    } else {
                        break;
                    }
                }
            }
            for (short index = 0; index < definition->locations_count; index++) {
                if ((inhibited_fire_teams[index >> 5] & (1 << (index & 31))) == 0 && (count == -1 || placement_counts[index] > 0)) {
                    bool fire_team_from_emitters;
                    short placed_count = fire_team_create(squad_index, index, -1, count == -1 ? -1 : placement_counts[index], force,
                        collect, created_by_script, &fire_team_from_emitters);
                    all_actors_from_emitters &= fire_team_from_emitters;
                    if (count != -1) {
                        count -= placed_count;
                        if (count <= 0)
                            break;
                    }
                }
            }
        }
        if (!all_actors_from_emitters)
            ai_handle_squad_placement(squad_index);
    }
}

t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0), &__tls_set_g_squad_data_allocator> g_ai_squads_allocator;
t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_squad_group_data_allocator> g_ai_squad_group_allocator;
