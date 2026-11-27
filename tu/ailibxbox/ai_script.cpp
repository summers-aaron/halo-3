// ailibxbox/ai_script.obj: from the records 0xd4c50 0xd4f60 0xd5040 0x2c8058 0x2c8340 0x505c88 0x505dd8 0x505eb0 0x505fd0 0x506100 0x5061d8 0x5063c0 0x506518 0x5065e0 0x5066f0 0x506978 0x506ae8 0x506bf8 0x506c60 0x506d90 0x507178 0x507260 0x5073d0 0x507798 0x507890 0x507988 0x507b18 0x507cb8 0x507ec0 0x507f98 0x5080b8 0x508220 0x5083b8 0x508510 0x5090e0 0x5091d0 0x5092f0 0x509680 0x5097f0 0x509c90 0x509fe8 0x50a1e8 0x50a360 0x50a460 0x50a540 0x50a858 0x50aa60 0x50ac38 0x50aef8 0x50afa8 0x50b100 0x50b180 0x54bf90 0x508660
#include "animations/animation_manager.h"
#include "math/real_constant_pool.h"
#include "units/unit_definitions.h"
#include "structures/structure_bsp_tag.h"
#include "ai/Zone.h"
#include "ai/squad_definitions.h"
#include "ai/objective_definitions.h"
#include "ai/command_script_data.h"
#include "ai/mission_dialogue_types.h"
#include "objects/object_list_header_types.h"
#include "memory/tls_globals.h"
#include "ai/prop_datum.h"
#include "ai/ai_tls_data.h"
#include "ai/squad_data.h"
#include "units/action_request.h"
#include "hs/hs_types.h"
#include "scenario/scenario_tag.h"
#include "memory/data_types.h"
#include "math/real_math_types.h"
#include "game/game_time_types.h"
#include "objects/object_reference_types.h"
#include "objects/object_datum.h"
#include "math/real_math_functions.h"
#include "objects/object_header.h"
#include "units/unit_types.h"
#include "creatures/creature_datum.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "game/game_globals_storage.h"
#include "ai/actor_globals.h"
#include "ai/squad_types.h"
#include "units/vehicle_datum.h"
#include "ai/actor_datum.h"
#include "ai/actor_moving.h"
#include "ai/clump_datum.h"
#include "ai/prop_ref_datum.h"
#include "ai/ai_point3d.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ppcintrinsics.h>
#include "tag_files/tag_types.h"
#include "ai/actors.h"
#include "motor/action_system.h"
#include "objects/objects.h"
#include "units/units.h"
#include "ai/actor_perception.h"
#include "ai/ai.h"
#include "ai/ai_flocks.h"
#include "ai/ai_script.h"
#include "math/real_math_globals.h"
#include "ai/ai_vehicles.h"
#include "ai/cl_engine.h"
#include "ai/clump.h"
#include "ai/actor_stimulus.h"
#include "ai/objectives.h"
#include "ai/behavior_stimulus_types.h"
#include "memory/tls_data_arrays.h"
#include "ai/squads.h"
#include "ai/swarms.h"
#include "hs/hs_runtime.h"
#include "hs/hs_unit_seats.h"
#include "hs/object_lists.h"
#include "math/integer_math.h"
#include "objects/damage.h"
#include "units/vehicles.h"
#include "ai/actor_type_flood.h"
#include "ai/ai_mission_dialogue.h"
#include "ai/mission_dialogue_variant.h"
#include "ai/character_definitions.h"
#include "memory/data.h"
#include "sound/game_sound_definitions.h"
#include "game/game_allegiance.h"
#include "scenario/scenario_structure_bsp_reference.h"
#include "units/unit_seat_source.h"
#include "objects/object_marker_globals_types.h"
#include "ai/handle_array_types.h"
#include "ai/squad_array_types.h"
#include "cache/cache_files.h"
#include "hs/hs_function.h"
#include "ai/squad_iterator_types.h"
#include "ai/ai_reference_frame.h"
#include "scenario/scenario.h"
#include "ai/squad_group_datum.h"
#include "ai/character_behavior_properties.h"
#include "hs/hs_function_definition.h"
#include "ai/cs_scenario_definitions.h"
#include "ai/objective_storage.h"
#include "units/biped_types.h"
#include "ai/actor_prop_ref_iterator.h"
#include "ai/squad_area_iterator.h"
#include "units/unit_seat_helpers.h"
#include "objects/object_type_access.h"
#include "interface/user_interface_utilities.h"
#include "objects/object_marker_access.h"
#include "ai/pureform_types.h"
#include "ai/actor_type_pureform.h"
#include "ai/actor_squad.h"
#include "sound/scripted_sound_constants.h"
#include "sound/game_sound.h"
#include "memory/data_absolute_index.h"
#include "math/random_math_range.h"
#include "game/game_time_inline.h"
#include "memory/static_arrays_flags.h"
#include "hs/object_list_inlines.h"
#include "cseries/maximum.h"
#include "cseries/bit_operations.h"
#include "scenario/cluster_reference_helpers.h"
#include "ai/clump_actor_cursor_types.h"
#include "structures/cluster_bit_vectors.h"
#include "hs/hs_value_types.h"
// @flags /TP /O2 /Oc

// @address 0x897e18 global_script_context
ai_script_context global_script_context;


struct teleport_swarm_creature_iterator { long swarm_index; long creature_index; long next_creature_index; };

struct script_swarm_iterator { long swarm_index, index, next_object_index; };

// match: 0x54bf90: retain the private short-index test boundary; canonical flags change an add operand tie.
static __forceinline bool game_cluster_bit_vector_test(const s_game_cluster_bit_vectors *pvs, long bsp_index, short cluster_index)
{
    return (pvs->bit_vectors[bsp_index].get_bits_direct()[cluster_index >> 5] & (1UL << (cluster_index & 31))) != 0;
}


static inline void ai_script_action_request_clear(action_request *request)
{
    memset((__int64 *)request, 0, sizeof(*request));
}

static __forceinline long ai_script_actor_index_from_ai(long ai_index)
{
    long actor_index = -1;
    short absolute_index = (short)ai_index;
    if (datum_try_and_get_absolute(actor_data, absolute_index)) {
        long index = datum_absolute_index_to_index(actor_data, absolute_index);
        if (((index & 0x1fffffff) | 0x60000000) == ai_index)
            actor_index = index;
    }
    return actor_index;
}

static __forceinline void ai_script_squad_group_squad_iterator_new(squad_group_squad_iterator *iterator, long index)
{
    squad_group_datum *group = datum_get<squad_group_datum>(g_ai_squad_group_data, index);
    iterator->previous_squad_index = -1;
    iterator->squad_index = -1;
    iterator->squad_group = group;
    iterator->root_squad_group = group;
    for (long parent = group->next_sibling_index; parent != -1; parent = iterator->squad_group->next_sibling_index)
        iterator->squad_group = datum_get<squad_group_datum>(g_ai_squad_group_data, parent);
    iterator->next_squad_index = iterator->squad_group->first_squad_index;
}

namespace {

static __forceinline unit_datum *script_object_get(long index) { return (unit_datum *)object_get_and_verify_type(index, ~0UL); }
static __forceinline bool ai_script_unit_in_seat(long unit_index, long seat)
{
    bool result = false;
    unit_datum *unit = script_object_get(unit_index);
    if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE) {
        result = hs_match_unit_seat_internal(unit->object.parent_object_index, unit->unit.parent_seat_index, seat);
    }
    return result;
}
static __forceinline long script_root_object(long index) {
    while (index != NONE) {
        unit_datum *object = script_object_get(index);
        if (((bool)(_rotl(object->object.flags, 6) & 1)) == false) break;
        index = object->object.parent_object_index;
    }
    return index;
}
static __forceinline long script_ultimate_parent(long index) {
    long result = NONE;
    while (index != NONE) { result = index; index = script_object_get(index)->object.parent_object_index; }
    return result;
}
static __forceinline void script_swarm_new(long index, script_swarm_iterator *iterator) {
    swarm_datum *swarm = datum_get<swarm_datum>(g_swarm_data, index);
    iterator->swarm_index = index;
    iterator->index = NONE;
    iterator->next_object_index = swarm->first_object_index;
}
static __forceinline unit_datum *script_swarm_next(script_swarm_iterator *iterator) {
    unit_datum *result = 0;
    if (iterator->next_object_index != NONE) {
        result = script_object_get(iterator->next_object_index);
        iterator->index = iterator->next_object_index;
        swarm_creature_state *link = ((creature_datum *)result)->creature.link_type == 0
            ? (swarm_creature_state *)((char *)result + ((creature_datum *)result)->creature.creature_reference.offset)
            : 0;
        if (link) iterator->next_object_index = link->next_swarm_creature_index;
        else iterator->next_object_index = NONE;
    }
    return result;
}
static __forceinline bool script_squad_verify(long squad_index, bool enable) { bool result = true; return result; }
static __forceinline void script_squad_set_deaf(long index, bool deaf) {
    if (ai_globals->enabled) {
        squad_datum *squad = &(*datum_get<squad_datum>(squad_data, index));
        if (deaf) squad->flags |= 2; else squad->flags &= ~2;
    }
}
static __forceinline void script_squad_set_blind(long index, bool blind) {
    if (ai_globals->enabled) {
        squad_datum *squad = &(*datum_get<squad_datum>(squad_data, index));
        if (blind) squad->flags |= 1; else squad->flags &= ~1;
    }
}
static __forceinline short script_grenade_type(long index) { return script_object_get(index)->unit.current_grenade_index; }
static __forceinline character_grenade_properties *script_grenade_properties(long index) {
    character_grenade_properties *result = 0;
    actor_datum *actor = datum_get<actor_datum>(actor_data, index);
    if (actor->unit_index != NONE) {
        short type = script_grenade_type(actor->unit_index);
        if (type != NONE) result = character_grenade_properties_get(actor->character_index, type);
    }
    return result;
}
static __forceinline float ai_script_swarm_random_delay(unsigned long *seed, float minimum)
{
    random_seed_next(seed);
    float unit = (float)(*seed >> 16) * 1.5259022e-05f;
    return unit * 5.0f + minimum;
}
static __forceinline short script_grenade_count(long index, short type)
{
    unit_datum *unit = script_object_get(index);
    if (type != NONE)
        return unit->unit.grenade_counts[type];
    return 0;
}
static __forceinline void script_add_grenades(long index, short type, short count) {
    unit_datum *unit = script_object_get(index);
    unit->unit.grenade_counts[type] += count;
    unit_datum *desired_unit = script_object_get(index);
    desired_unit->unit.desired_grenade_index = (signed char)type;
    desired_unit->unit.current_grenade_index = (signed char)type;
}
}

long ai_index_get_actor(long ai_index)
{
    long actor_index = NONE;
    if (ai_globals->enabled) {
        if ((unsigned long)ai_index >> 29 == 0) {
            actor_index = NONE;
        } else if ((unsigned long)ai_index >> 29 == 3) {
            actor_index = ai_script_actor_index_from_ai(ai_index);
        } else if ((unsigned long)ai_index >> 29 == 4) {
            short squad_index = (short)((ai_index >> 16) & 0x1fff);
            if (squad_index >= 0 && squad_index < global_scenario_get()->squads.count) {
                squad_definition *squad = (squad_definition *)tag_block_get_element_with_size(
                    &global_scenario_get()->squads, (unsigned short)squad_index, sizeof(squad_definition));
                short fire_team_index = (short)((ai_index >> 8) & 0xff);
                if (fire_team_index >= 0 && fire_team_index < squad->fire_team_block.count) {
                    fire_team_definition *fire_team = (fire_team_definition *)tag_block_get_element_with_size(
                        &squad->fire_team_block, fire_team_index, sizeof(fire_team_definition));
                    short starting_location_index = (short)(ai_index & 0xff);
                    if (starting_location_index >= 0 && starting_location_index < fire_team->starting_locations.count) {
                        actor_starting_location_definition *starting_location = (actor_starting_location_definition *)tag_block_get_element_with_size(
                            &fire_team->starting_locations, starting_location_index, sizeof(actor_starting_location_definition));
                        squad_actor_iterator iterator;
                        actor_datum *actor;
                        squad_actor_iterator_new(&iterator, squad_index, false);
                        while ((actor = ::squad_actor_iterator_next(&iterator)) != 0) {
                            if (actor->starting_location_data == starting_location->starting_location_name) {
                                actor_index = iterator.index;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return actor_index;
}

// @address 0xd4f60 ?ai_index_actor_iterator_new@@YAXJPAUai_index_actor_iterator@@@Z
void ai_index_actor_iterator_new(long ai_index, ai_index_actor_iterator *iterator)
{
    bool found = false;
    iterator->single = false;
    iterator->done = false;
    if (ai_globals->enabled) {
        switch ((unsigned long)ai_index >> 29) {
        case 0:
            iterator->done = true;
            break;
        case 1:
        case 2:
        case 5:
            ::ai_index_squad_iterator_new(ai_index, &iterator->squad_iterator);
            iterator->single = false;
            iterator->actor_index = NONE;
            if (ai_index_squad_iterator_next(&iterator->squad_iterator) != 0) {
                squad_actor_iterator_new(&iterator->actor_iterator, iterator->squad_iterator.squad_index, false);
                found = true;
            }
            break;
        case 3:
        case 4:
            iterator->actor_index = ai_index_get_actor(ai_index);
            iterator->single = true;
            found = true;
            break;
        default:
            break;
        }
    }
    if (!found)
        iterator->done = true;
}

actor_datum *ai_index_actor_iterator_next(ai_index_actor_iterator *iterator)
{
    actor_datum *result = 0;
    if (iterator->done) {
        iterator->actor_index = NONE;
    } else if (iterator->single) {
        if (iterator->actor_index != NONE)
            result = datum_get<actor_datum>(actor_data, iterator->actor_index);
        else
            result = 0;
        iterator->done = true;
    } else {
        while (!result) {
            result = ::squad_actor_iterator_next(&iterator->actor_iterator);
            if (result) {
                iterator->actor_index = iterator->actor_iterator.index;
            } else if (ai_index_squad_iterator_next(&iterator->squad_iterator)) {
                squad_actor_iterator_new(&iterator->actor_iterator, iterator->squad_iterator.squad_index, false);
            } else {
                iterator->done = true;
                iterator->actor_index = NONE;
                break;
            }
        }
    }
    return result;
}

// @address 0x2c8058 ?ai_scripting_flock_delete@@YAXFJ_N@Z
void ai_scripting_flock_delete(short type, long object_index, bool parameter)
{
    const hs_function_definition *definition = hs_function_table[type];
    long *result = hs_arguments_evaluate(object_index, definition->formal_parameter_count, definition->formal_parameters, parameter);
    if (result) {
        long handle = *result;
        if (handle != -1) {
            long instance = flock_get_by_name(handle);
            if (instance != -1)
                flock_reset_boid_update_timers(instance);
        }
        hs_return(object_index, 0);
    }
}

namespace {

}

__forceinline short ai_script_play_line_on_object_for_team(long object, long line, short team)
{
    short result = 0;
    bool found = false;
    game_player_options players[16];
    long count;
    s_machine_identifier machine;
    user_interface_build_local_machine_and_players(&machine, &count, players);
    for (long i = 0; i < count; i++) {
        if (players[i].configuration.host.team == team) {
            found = true;
            break;
        }
    }

    return result;
}

// @address 0x2c8340 ?ai_play_line_on_object_for_team@@YAXFJ_N@Z
void ai_play_line_on_object_for_team(short function_index, long thread_index, bool initialize)
{
    union { float real; long integer; } result;
    result.integer = 0;
    const hs_function_definition *definition = hs_function_table[function_index];
    hs_value *arguments = (hs_value *)hs_arguments_evaluate(thread_index, definition->formal_parameter_count, definition->formal_parameters, initialize);
    if (arguments) {
        result.real = (float)ai_script_play_line_on_object_for_team(arguments[0].long_value, arguments[1].long_value, (short)arguments[2].short_value);
        hs_return(thread_index, result.integer);
    }
}

// @address 0x505c88 ?object_index_from_ai@@YAJJW4e_hs_type@@@Z
long object_index_from_ai(long ai_index, e_hs_type type) {
    long result = NONE;
    ai_index_actor_iterator iterator;
    ai_index_actor_iterator_new(ai_index, &iterator);
    actor_datum *actor = ai_index_actor_iterator_next(&iterator);
    if (actor && actor->unit_index != NONE && (type == _hs_type_object || type == _hs_type_unit || type == _hs_type_vehicle)) {
        bool need_a_vehicle = type == _hs_type_vehicle;
        short object_type = (short)object_get_type(actor->unit_index);
        if (!need_a_vehicle || ((1 << object_type) & 2)) result = actor->unit_index;
        else {
            long root = script_root_object(actor->unit_index);
            if (root != actor->unit_index && ((1 << object_get_type(root)) & 2)) result = root;
            else {
                long parent = script_ultimate_parent(actor->unit_index);
                if (parent != actor->unit_index && ((1 << object_get_type(parent)) & 2)) result = parent;
            }
        }
    }
    return result;
}

// @address 0x505dd8 ai_index_test_squad
extern "C" bool ai_index_test_squad(long a, long b)
{
    bool result = false;
    switch ((unsigned long)b >> 29) {
    case 1:
        result = a == (b & 0xffff);
        break;
    case 2:
        result = squad_ancestor_of_squad_group(a, b & 0xffff);
        break;
    case 3:
    case 4:
        result = false;
        break;
    case 5: {
        squad_datum *record = &(*datum_get<squad_datum>(squad_data, a));
        short id = (b >> 16) & 0x1fff;
        result = record->objective_index == (short)b && (id == 0x1fff || record->task_index == id);
        break;
    }
    }
    return result;
}

long ai_index_get_vehicle(long ai_index)
{
    long result=-1;
    if(ai_globals->enabled && (unsigned long)ai_index>>29==4)
    {
        short squad_index=(short)((ai_index>>16)&0x1fff);
        if(squad_index>=0 && squad_index<global_scenario_get()->squads.count)
        {
            squad_datum *squad_vehicle_data=datum_get<squad_datum>(squad_data, squad_index);
            squad_definition *definition = (squad_definition *)tag_block_get_element_with_size(
                &global_scenario_get()->squads, (unsigned short)squad_index, sizeof(squad_definition));
            short team_index=(short)((ai_index>>8)&0xff);
            if(team_index>=0 && team_index<definition->fire_team_block.count)
            {
                fire_team_definition *team=(fire_team_definition*)tag_block_get_element_with_size(&definition->fire_team_block,
                    team_index, sizeof(fire_team_definition));
                short location_index=(short)(ai_index&0xff);
                if(location_index>=0 && location_index<team->starting_locations.count)
                {
                    actor_starting_location_definition *start=(actor_starting_location_definition*)tag_block_get_element_with_size(&team->starting_locations,
                        location_index, sizeof(actor_starting_location_definition));
                    long vehicle=squad_vehicle_data->first_vehicle_index;
                    while(vehicle!=-1)
                    {
                        vehicle_datum *object=(vehicle_datum *)object_get_and_verify_type(vehicle,2);
                        if(object->vehicle.starting_location_name==start->starting_location_name){result=vehicle; break;}
                        vehicle=object->vehicle.next_vehicle_index;
                    }
                }
            }
        }
    }
    return result;
}

// @address 0x505fd0 ai_vehicle_has_player
extern "C" bool ai_vehicle_has_player(long vehicle_index)
{
    long root_index = script_root_object(vehicle_index);
    bool result = false;
    c_data_iterator<player_datum> iterator;
    iterator.begin(player_data);
    while (iterator.next()) {
        player_datum *player = iterator.get_datum();
        if ((player->flags & 1) && !(player->flags & 2) && player->unit_index != NONE) {
            long parent_index = script_object_get(player->unit_index)->object.parent_object_index;
            if (parent_index != NONE && script_root_object(parent_index) == root_index) {
                result = true;
                break;
            }
        }
    }
    return result;
}

static __forceinline s_object_marker_globals *ai_script_object_marker_globals_get()
{
    return &OBJECT_MARKER_GLOBALS[g_registered_thread_index - 1];
}
static __forceinline void ai_script_object_marker_begin()
{
    s_object_marker_globals *globals = ai_script_object_marker_globals_get();
    globals->mark++;
    globals->active = true;
}
static __forceinline void ai_script_object_marker_end()
{
    s_object_marker_globals *globals = ai_script_object_marker_globals_get();
    globals->active = false;
}
static __forceinline bool ai_script_object_mark(long object_index)
{
    s_object_marker_globals *globals = ai_script_object_marker_globals_get();
    long absolute_index = (unsigned short)object_index;
    long *slot = &globals->marks[absolute_index];
    long previous_mark = *slot;
    long mark = globals->mark;
    *slot = mark;
    return previous_mark != mark;
}

// @address 0x506100 ai_vehicle_count
extern "C" short ai_vehicle_count(long ai_index)
{
    short count = 0;
    if (ai_index != NONE) {
        ai_index_actor_iterator iterator;
        ai_index_actor_iterator_new(ai_index, &iterator);
        ai_script_object_marker_begin();
        actor_datum *actor;
        while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
            if (actor->root_vehicle_index != NONE && ai_script_object_mark(actor->root_vehicle_index))
                count++;
        }
        ai_script_object_marker_end();
    }
    return count;
}

// @address 0x5061d8 ?ai_index_squad_iterator_new@@YAXJPAUai_index_squad_iterator@@@Z
void ai_index_squad_iterator_new(long ai_index, struct ai_index_squad_iterator * iterator)
{
    iterator->objective_index = -1;
    iterator->task_index = -1;
    iterator->objective_squads_started = false;
    iterator->squad_group_index = -1;
    iterator->next_squad_index = -1;
    if (ai_globals->enabled) {
        switch ((unsigned long)ai_index >> 29) {
        case 2: {
            long squad_group_index = ai_index & 0xffff;
            if (squad_group_index >= 0 && squad_group_index < global_scenario->squad_groups.count) {
                iterator->squad_group_index = squad_group_index;
                ai_script_squad_group_squad_iterator_new(&iterator->squad_group_iterator, iterator->squad_group_index);
            }
            break;
        }
        case 1: {
            short squad_index = (short)ai_index;
            if (squad_index >= 0 && squad_index < global_scenario->squads.count) {
                iterator->squad_group_index = -1;
                iterator->next_squad_index = squad_index;
            }
            break;
        }
        case 5: {
            short objective_index = (short)ai_index;
            short task_index = (ai_index >> 16) & 0x1fff;
            if (objective_index >= 0 && objective_index < global_scenario->objectives.count) {
                s_objective_record *objective = (s_objective_record *)((char *)g_objectives_data->data +
                    (unsigned short)objective_index * sizeof(s_objective_record));
                if (task_index == 0x1fff ||
                    (task_index >= 0 &&
                        task_index <
                            ((s_objective *)tag_block_get_element_with_size(
                                 &global_scenario->objectives, objective_index, sizeof(s_objective)))
                                ->tasks.count))
                {
                    iterator->squad_group_index = -1;
                    iterator->next_squad_index = objective->first_squad_index;
                    iterator->task_index = task_index == 0x1fff ? -1 : task_index;
                    iterator->objective_index = objective_index;
                    iterator->objective_squads_started = false;
                }
            }
            break;
        }
        }
    }
}

squad_datum *ai_index_squad_iterator_next(ai_index_squad_iterator *iterator)
{
    squad_datum *current_squad = 0;
    iterator->squad_index = NONE;
    if (iterator->squad_group_index != NONE)
    {
        current_squad = squad_group_squad_iterator_next(&iterator->squad_group_iterator);
        iterator->squad_index = iterator->squad_group_iterator.squad_index;
    }
    else
    {
        while (iterator->next_squad_index != NONE)
        {
            long squad_index = iterator->next_squad_index;
            iterator->squad_index = squad_index;
            current_squad = datum_get<squad_datum>(squad_data, squad_index);
            if (iterator->objective_index != NONE)
            {
                iterator->next_squad_index = current_squad->next_index;
                if (iterator->next_squad_index == NONE && !iterator->objective_squads_started)
                {
                    s_objective_record *objective = datum_get<s_objective_record>(g_objectives_data, iterator->objective_index);
                    iterator->next_squad_index = objective->first_unassigned_squad_index;
                    iterator->objective_squads_started = true;
                }
            }
            else
            {
                iterator->next_squad_index = NONE;
            }

            if (current_squad)
            {
                if (iterator->task_index == NONE)
                    break;
                if ((current_squad->temporary) && current_squad->task2_index == iterator->task_index)
                    break;
                if (!(current_squad->temporary) && (short)current_squad->task == iterator->task_index)
                    break;
            }
            iterator->squad_index = NONE;
            current_squad = 0;
        }
    }
    return current_squad;
}

// @address 0x506518 ?ai_handle_script_verification@@YA_N_N@Z
bool ai_handle_script_verification(bool enable) {
    ::scenario *scenario = (::scenario *)global_scenario_get();
    bool result = true;
    for (short squad_index = 0; squad_index < scenario->squads.count; ++squad_index)
        result &= script_squad_verify(squad_index, enable);
    if (ai_globals->enabled) {
        c_data_iterator<command_script_datum> iterator;
        iterator.begin(command_script_data);
        while (iterator.next()) {
            command_script_datum *script = iterator.get_datum();
            script->thread_index = NONE;
        }
    }
    return result;
}

// @address 0x5065e0 ?object_list_from_ai_reference@@YAJJ@Z
long object_list_from_ai_reference(long reference) {
    long result = NONE;
    if (reference != NONE) {
        result = object_list_new();
        if (result != NONE) {
            ai_index_actor_iterator iterator;
            ai_index_actor_iterator_new(reference, &iterator);
            actor_datum *actor;
            while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
                actor_datum *state = actor;
                if (state->unit_index != NONE) object_list_add(result, state->unit_index);
                if (state->swarm_index != NONE) {
                    script_swarm_iterator creatures;
                    script_swarm_new(state->swarm_index, &creatures);
                    while (script_swarm_next(&creatures)) object_list_add(result, creatures.index);
                }
            }
        }
    }
    return result;
}

// @address 0x5066f0 ?ai_scripting_attach_unit@@YAXJJ@Z
void ai_scripting_attach_unit(long unit_index, long ai_index)
{
    scenario *scenario_definition = global_scenario_get();
    if (ai_globals->enabled && unit_index != -1 && ai_index != -1)
    {
        long squad_index = (unsigned short)ai_index;
        if (squad_index >= 0 && squad_index < scenario_definition->squads.count)
        {
            squad_definition *squad = (squad_definition *)tag_block_get_element_with_size(
                &global_scenario_get()->squads, (unsigned short)squad_index, sizeof(squad_definition));
            if (squad->fire_team_block.count > 0)
            {
                fire_team_definition *location = (fire_team_definition *)tag_block_get_element_with_size(
                    &squad->fire_team_block, 0, sizeof(fire_team_definition));
                if (location->character != -1)
                {
                    s_tag_reference *character = (s_tag_reference *)tag_block_get_element_with_size(
                        &scenario_definition->characters, location->character,
                        sizeof(s_tag_reference));
                    if (character->index != -1)
                    {
                        ::actor_create(unit_index, character->index, squad_index, (squad->flags & 8) != 0);
                        squads_update_dirty_status();
                    }
                }
            }
        }
    }
}

// @address 0x506978 ?ai_scripting_cannot_die@@YAXJ_N@Z
// match: 0x506978: boolean helper preserves the shift-to-bit test before the flag setter reload.
static __forceinline bool ai_script_damage_flag_test(const object_datum *object, long bit)
{
    return ((object->damage_flags >> bit) & 1) != 0;
}

static __forceinline void ai_script_object_scripting_cannot_die(long object_index, bool cannot_die)
{
    if (object_index != NONE) {
        unit_datum *object = script_object_get(object_index);
        if (!ai_script_damage_flag_test(&object->object, _object_damage_body_depleted_bit))
            object->object.m_damage_flags.set(_object_damage_vitality_pinned_bit, cannot_die);
    }
}
void ai_scripting_cannot_die(long ai_index, bool cannot_die) {
    ai_index_actor_iterator iterator;
    ai_index_actor_iterator_new(ai_index, &iterator);
    actor_datum *actor;
    while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
        actor_datum *state = actor;
        if (state->unit_index != NONE) ai_script_object_scripting_cannot_die(state->unit_index, cannot_die);
        else if (state->swarm_index != NONE) {
            script_swarm_iterator creatures;
            script_swarm_new(state->swarm_index, &creatures);
            while (script_swarm_next(&creatures)) ai_script_object_scripting_cannot_die(creatures.index, cannot_die);
        }
    }
}

// @address 0x506ae8 ?ai_scripting_vitality_pinned@@YA_NJ@Z
bool ai_scripting_vitality_pinned(long ai_index) {
    bool result = true;
    ai_index_actor_iterator iterator;
    ai_index_actor_iterator_new(ai_index, &iterator);
    actor_datum *actor;
    while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
        actor_datum *state = actor;
        if (state->unit_index != NONE) result = object_scripting_vitality_pinned(state->unit_index);
        else if (state->swarm_index != NONE) {
            script_swarm_iterator creatures;
            script_swarm_new(state->swarm_index, &creatures);
            while (script_swarm_next(&creatures)) {
                result = object_scripting_vitality_pinned(creatures.index);
                if (!result) break;
            }
        }
        if (!result) break;
    }
    return result;
}

// @address 0x506bf8 ?ai_scripting_get_object@@YAJJ@Z
long ai_scripting_get_object(long ai_index) {
    long result = NONE;
    if (ai_index != NONE) {
        ai_index_actor_iterator iterator;
        ai_index_actor_iterator_new(ai_index, &iterator);
        actor_datum *actor = ai_index_actor_iterator_next(&iterator);
        if (actor) result = actor->unit_index;
    }
    return result;
}

// @address 0x506c60 ?ai_scripting_get_turret_ai@@YAJJF@Z
long ai_scripting_get_turret_ai(long ai_index, short turret_index) {
    long result = 0;
    if (ai_index != NONE) {
        ai_index_actor_iterator iterator;
        ai_index_actor_iterator_new(ai_index, &iterator);
        actor_datum *actor = ai_index_actor_iterator_next(&iterator);
        if (actor) {
            actor_datum *state = actor;
            long unit_index = NONE;
            if (state->root_vehicle_index != NONE) unit_index = state->root_vehicle_index;
            else if (state->unit_index != NONE) unit_index = state->unit_index;
            if (unit_index != NONE) {
                unit_datum *unit = script_object_get(unit_index);
                long child_index = unit->object.first_child_object_index;
                short current_turret = 0;
                while (child_index != NONE) {
                    unit_datum *child = script_object_get(child_index);
                    if (((1 << child->object.type) & 0x1003) != 0) {
                        unit_datum *child_unit = child;
                        if (child_unit->unit.parent_seat_index == -1) {
                            unit_definition *definition =
                                (unit_definition *)tag_get(NONE, child_unit->object.definition_index);
                            if (definition->spawned_actor.index != NONE) {
                                if (current_turret == turret_index) {
                                    if (child_unit->unit.actor_index != NONE) result = (child_unit->unit.actor_index & 0x1fffffff) | 0x60000000;
                                    break;
                                }
                                ++current_turret;
                            }
                        }
                    }
                    child_index = child->object.next_object_index;
                }
            }
        }
    }
    return result;
}

struct s_attached_point;

// math/real_math.h cannot be included here (its file-scope constants grow .rdata),
// so this unit keeps its own spelling of the shared dot_product3d body.
static __forceinline float dot_product3d_inline(const real_vector3d *a, const real_vector3d *b)
{
    return (a->i * b->i + a->j * b->j) + a->k * b->k;
}

// @address 0x507178 ?ai_scripting_nearest_point@@YAJJJ@Z
long ai_scripting_nearest_point(long object_index, long point_reference)
{
    long result = NONE;
    if (object_index != NONE) {
        scenario *scenario = global_scenario;
        if (scenario->script_data.count > 0) {
            cs_script_data *script_data = cs_scenario_get_script_data(scenario);
            long point_set_index = (short)(point_reference >> 16);
            if (point_set_index >= 0 && point_set_index < script_data->point_sets.count) {
                cs_point_set *point_set = cs_get_point_set(point_set_index);
                float nearest_distance = 3.402823466e+38f;
                short nearest_index = NONE;
                real_point3d origin;
                object_get_origin(object_index, &origin);
                long point_count = point_set->points.count;
                short point_index = 0;
                if (point_count > 0) {
                    cs_point *points = (cs_point *)point_set->points.address;
                    do {
                        float distance = ai_distance_squared3d(&points[point_index].position, &origin);
                        if (distance < nearest_distance) {
                            nearest_index = point_index;
                            nearest_distance = distance;
                        }
                        point_index++;
                    } while (point_index < point_count);
                }
                if (nearest_index != NONE)
                    result = (point_set_index << 16) | (unsigned short)nearest_index;
            }
        }
    }
    return result;
}

// @address 0x506d90 ?ai_scripting_random_point@@YAJJMMM@Z
long ai_scripting_random_point(long point_reference, float minimum_distance, float maximum_distance, float angle)
{
    scenario *scenario = global_scenario;
    long result = NONE;
    if (scenario->script_data.count > 0) {
        cs_script_data *script_data = cs_scenario_get_script_data(scenario);
        long point_set_index = (short)(point_reference >> 16);
        if (point_set_index >= 0 && point_set_index < script_data->point_sets.count) {
            minimum_distance = minimum_distance > 0.5f ? minimum_distance : 0.5f;
            maximum_distance = maximum_distance > minimum_distance + 1.0f ? maximum_distance : minimum_distance + 1.0f;
            float ideal_distance = (minimum_distance + maximum_distance) / 2.0f;
            float near_range = ideal_distance - minimum_distance;
            float far_range = maximum_distance - ideal_distance;
            angle = (angle > 10.0f ? angle : 10.0f) < 180.0f ? (angle > 10.0f ? angle : 10.0f) : 180.0f;
            angle = angle * 0.017453292f;
            float half_angle = angle / 2.0f;
            float cosine_half_angle = (float)cos(half_angle);
            bool valid[32];
            memset(valid, 0, sizeof(valid));
            cs_point_set *point_set = (cs_point_set *)tag_block_get_element_with_size(
                &((cs_script_data *)tag_block_get_element_with_size(&(global_scenario)->script_data, 0, sizeof(cs_script_data)))
                    ->point_sets,
                point_set_index, sizeof(cs_point_set));
            float scores[NUMBEROF(valid)];
            short point_index;
            for (point_index = 0; point_index < point_set->points.count; point_index++)
                scores[point_index] = -3.402823466e+38f;
            c_data_iterator<player_datum> iterator;
            iterator.begin(player_data);
            while (iterator.next()) {
                player_datum *player = iterator.get_datum();
                if ((player->flags & 1) && !(player->flags & 2) && player->unit_index != NONE) {
                    const long unit_index = player->unit_index;
                    real_point3d origin;
                    real_vector3d forward;
                    object_get_origin(unit_index, &origin);
                    ::object_get_orientation(unit_index, &forward, 0);
                    bool facing = normalize3d(&forward) > 0.0f;
                    long point_count = point_set->points.count;
                    short index;
                    if (point_count > 0) {
                    index = 0;
                    cs_point *points = (cs_point *)point_set->points.address;
                    do {
                        cs_point *point = &points[index];
                        float distance_score = 0.0f;
                        float facing_score = 0.0f;
                        bool point_valid = true;
                        float distance = ai_distance3d(&point->position, &origin);
                        if (distance < minimum_distance) {
                            distance_score = distance / minimum_distance - 2.0f;
                            point_valid = false;
                        } else if (distance > maximum_distance) {
                            distance_score = maximum_distance / distance - 1.0f;
                            point_valid = false;
                        } else if (distance <= ideal_distance) {
                            distance_score = 1.0f - (ideal_distance - distance) / near_range;
                        } else {
                            distance_score = 1.0f - (distance - ideal_distance) / far_range;
                        }
                        if (facing) {
                            real_vector3d direction;
                            ai_vector_from_points3d(&origin, &point->position, &direction);
// match: 0x506d90: the inner point loop calls normalize3d at 0x507014. Inlining adds a square root and division.
#pragma inline_depth(0)
                            if (normalize3d(&direction) > 0.0f) {
// match: 0x508660: restoring the default depth keeps ai_assign_actors_to_seats from shrinking by 64 bytes.
#pragma inline_depth()
                                float dot = dot_product3d_inline(&direction, &forward);
                                if (dot >= cosine_half_angle) {
                                    facing_score = 1.0f;
                                } else if (dot <= 0.0f) {
                                    facing_score = dot;
                                    point_valid = false;
                                } else {
                                    facing_score = dot / cosine_half_angle;
                                    point_valid = false;
                                }
                            }
                        }
                        float score = distance_score;
                        score += facing_score;
                        scores[index] = scores[index] > score ? scores[index] : score;
                        valid[index] = valid[index] | point_valid;
                        index++;
                    } while (index < point_count);
                    }
                }
            }
            short best_index = NONE;
            float best_score = -3.402823466e+38f;
            for (point_index = 0; point_index < point_set->points.count; point_index++) {
                if (valid[point_index] && scores[point_index] > best_score) {
                    best_score = scores[point_index];
                    best_index = point_index;
                }
            }
            if (best_index == NONE) {
                for (point_index = 0; point_index < point_set->points.count; point_index++) {
                    if (scores[point_index] > best_score) {
                        best_score = scores[point_index];
                        best_index = point_index;
                    }
                }
            }
            result = (point_set_index << 16) | (unsigned short)best_index;
        }
    }
    return result;
}
static __forceinline vehicle_datum *ai_script_erase_object_try_get(long index)
{
    s_data_array *data = (s_data_array *)object_header_data;
    vehicle_datum *result = 0;
    if (index != -1) {
        long absolute_index = index & 0xffff;
        if (absolute_index < data->first_unallocated) {
            object_header_datum *header = (object_header_datum *)(data->data + data->size * absolute_index);
            long identifier = header->identifier;
            if (identifier != 0 && identifier == (index >> 16) && ((1UL << header->object_type) & 2UL)) result = (vehicle_datum *)header->datum;
        }
    }
    return result;
}

// @address 0x507260 ?ai_scripting_erase@@YAXJ@Z
void ai_scripting_erase(long ai_index)
{
    if (ai_index != -1) {
        ai_index_actor_iterator actors;
        ai_index_actor_iterator_new(ai_index, &actors);
        actor_datum *actor;
        while ((actor = ai_index_actor_iterator_next(&actors)) != 0) {
            if (actors.actor_index == g_updating_actor_index) ((actor_datum *)actor)->deleted = true;
            else actor_erase(actors.actor_index, false);
        }
        ai_index_squad_iterator squads;
        ::ai_index_squad_iterator_new(ai_index, &squads);
        squad_datum *squad;
        while ((squad = ai_index_squad_iterator_next(&squads)) != 0) {
            long remaining = -1;
            long next = squad->first_vehicle_index;
            while (next != -1) {
                long index = next;
                vehicle_datum *vehicle = ai_script_erase_object_try_get(index);
                if (vehicle) {
                    next = vehicle->vehicle.next_vehicle_index;
                    if (!object_is_or_contains_player(index)) { vehicle->vehicle.squad_index = -1; ::object_delete(index); }
                    else { vehicle->vehicle.next_vehicle_index = remaining; remaining = index; }
                } else break;
            }
            squad->first_vehicle_index = remaining;
        }
    }
}

// @rdata 0x82022850
static const float k_ai_script_swarm_minimum_delay = 10.0f;
// @address 0x5073d0 ai_scripting_set_objective
extern "C" void ai_scripting_set_objective(int ai_index, int objective_index)
{
    float rounding = 0.5f;
    float minimum_delay = k_ai_script_swarm_minimum_delay;
    short thirty_ticks = (short)(int)((float)game_time_globals->ticks_per_second * 30.0f + rounding);
    long fifteen_ticks = (long)((float)game_time_globals->ticks_per_second * 15.0f + rounding);
    if (fifteen_ticks == NONE)
        ai_globals->scripted_unknown_time = NONE;
    else
        ai_globals->scripted_unknown_time = game_time_globals->time + fifteen_ticks;
    ai_index_actor_iterator iterator;
    ai_index_actor_iterator_new(ai_index, &iterator);
    actor_datum *actor;
    while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
        actor_datum *view = actor;
        short type = view->type;
        if (((1 << type) & 0x700000) != 0) {
            actor_datum *pureform = datum_get<actor_datum>(actor_data, iterator.actor_index);
            if (pureform->morph.pending_form_priority <= 1) {
                pureform->morph.pending_form = 2;
                pureform->morph.pending_form_priority = 1;
                pureform->morph.immediate_morph = true;
                pureform->morph.force_morph = false;
                pureform->morph.current_form = 2;
            }
            actor_morph_impl(iterator.actor_index, _pureform_stalker, false, true);
        } else if (type == 9) {
            actor_type_flood_handle_eject_parasite(iterator.actor_index);
            actor_kill(iterator.actor_index, false, true);
        } else if (type == 11) {
            object_destroy_internal(view->unit_index, -1, -1, 0, 0);
        }
    }
    ai_index_actor_iterator_new(ai_index, &iterator);
    while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
        actor_datum *view = actor;
        if (view->type == 10) {
            stimulus_register(iterator.actor_index, _behavior_stimulus_flood_disperse, thirty_ticks, 7);
            script_swarm_iterator creatures;
            script_swarm_new(view->swarm_index, &creatures);
            unit_datum *creature;
            while ((creature = script_swarm_next(&creatures)) != 0) {
                ((creature_datum *)creature)->creature.form_lifetime =
                    (short)(int)(ai_script_swarm_random_delay(TLS_RANDOM_SEED, minimum_delay) * (float)game_time_globals->ticks_per_second +
                        rounding);
            }
        }
    }
    if (((objective_index & 0xe0000000) == 0xa0000000) && ai_globals->enabled) {
        short objective = (short)objective_index;
        short task = -1;
        if (objective < 0 || objective >= global_scenario->objectives.count) {
            objective = -1;
        } else {
            task = (short)((objective_index >> 16) & 0x1fff);
            if (task < 0 ||
                task >= ((s_objective *)tag_block_get_element_with_size(
                             &global_scenario->objectives, objective, sizeof(s_objective)))
                            ->tasks.count)
                task = -1;
        }
        ai_index_squad_iterator squads;
        ai_index_squad_iterator_new(ai_index, &squads);
        while (ai_index_squad_iterator_next(&squads))
            squad_set_objective(squads.squad_index, objective, task, false);
    }
}

// @address 0x507798 ?ai_scripting_set_deaf@@YAXJ_N@Z
void ai_scripting_set_deaf(long ai_index, bool deaf) {
    if (ai_index != NONE) {
        unsigned long type = (unsigned long)ai_index >> 29;
        if (type > 0 && (type <= 2 || type == 5)) {
            ai_index_squad_iterator iterator;
            ::ai_index_squad_iterator_new(ai_index, &iterator);
            squad_datum *squad;
            while ((squad = ai_index_squad_iterator_next(&iterator)) != 0) script_squad_set_deaf(iterator.squad_index, deaf);
        } else {
            ai_index_actor_iterator iterator;
            ai_index_actor_iterator_new(ai_index, &iterator);
            actor_datum *actor;
            while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) (actor)->deaf = deaf;
        }
    }
}

// @address 0x507890 ?ai_scripting_set_blind@@YAXJ_N@Z
void ai_scripting_set_blind(long ai_index, bool blind) {
    if (ai_index != NONE) {
        unsigned long type = (unsigned long)ai_index >> 29;
        if (type > 0 && (type <= 2 || type == 5)) {
            ai_index_squad_iterator iterator;
            ::ai_index_squad_iterator_new(ai_index, &iterator);
            squad_datum *squad;
            while ((squad = ai_index_squad_iterator_next(&iterator)) != 0) script_squad_set_blind(iterator.squad_index, blind);
        } else {
            ai_index_actor_iterator iterator;
            ai_index_actor_iterator_new(ai_index, &iterator);
            actor_datum *actor;
            while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) (actor)->blind = blind;
        }
    }
}

static __forceinline long ai_script_see_game_seconds_to_ticks_round(float seconds, float rounding)
{
    return (long)(seconds * (float)game_time_globals->ticks_per_second + rounding);
}

// @address 0x507988 ?ai_magically_see@@YAXJJ@Z
void ai_magically_see(long ai_index, long other_ai_index)
{
    c_static_flags_no_init<20> clumps_seen;
    clumps_seen.clear();
    ai_index_actor_iterator iterator;
    ai_index_actor_iterator_new(ai_index, &iterator);
    actor_datum *actor = ai_index_actor_iterator_next(&iterator);
    if (actor) {
        float rounding = k_real_constant_pool_half[0];
        do {
            long clump_index = actor->clump_index;
            if (clump_index != NONE && !clumps_seen.test((unsigned short)clump_index)) {
                clumps_seen.set((unsigned short)clump_index, true);
                ai_index_actor_iterator other_iterator;
                ai_index_actor_iterator_new(other_ai_index, &other_iterator);
                actor_datum *other;
                while ((other = ai_index_actor_iterator_next(&other_iterator)) != 0) {
                    long unit_index = other->unit_index;
                    if (unit_index != NONE) {
                        long prop_index = clump_get_clump_prop(clump_index, unit_index);
                        if (prop_index == NONE) prop_index = clump_aquire_prop(clump_index, unit_index, _clump_importance_critical);
                        if (prop_index != NONE) {
                            prop_datum *prop = &(*datum_get<prop_datum>(prop_data, prop_index));
                            long ticks = prop->ineffability_ticks;
                            prop->ineffability_ticks = (short)maximum(
#if defined(_XBOX) && !defined(__clang__)
                                // match: 0x507988: the float loads through the constant-pool base register the function already holds
                                ticks, ai_script_see_game_seconds_to_ticks_round(k_real_constant_pool_half[1321], rounding));
#else
                                ticks, ai_script_see_game_seconds_to_ticks_round(5.0f, rounding));
#endif
                        }
                    }
                }
            }
        } while ((actor = ai_index_actor_iterator_next(&iterator)) != 0);
    }
}

// @address 0x507b18 ?ai_magically_see_object@@YAXJJ@Z
void ai_magically_see_object(long ai_index, long object_index)
{
    if ((ai_index & 0xe0000000) >> 29 == 1) {
        c_static_flags_no_init<20> clumps_seen;
        clumps_seen.clear();
        long squad_index = (unsigned short)ai_index;
        squad_actor_iterator iterator;
        squad_actor_iterator_new(&iterator, squad_index, true);
        actor_datum *actor;
        while ((actor = ::squad_actor_iterator_next(&iterator)) != 0) {
            if (actor->clump_index != NONE && !clumps_seen.test((unsigned short)actor->clump_index)) {
                clump_datum *clump = &((clump_datum *)clump_data->data)[(unsigned short)actor->clump_index];
                long prop_index = clump->first_prop_index;
                clumps_seen.set((unsigned short)actor->clump_index, true);
                while (prop_index != NONE) {
                    prop_datum *prop = &(*datum_get<prop_datum>(prop_data, prop_index));
                    if (prop->object_index == object_index) {
                        long ticks = prop->ineffability_ticks;
                        prop->ineffability_ticks = (short)maximum(ticks, game_seconds_to_ticks_round(5.0f));
                        break;
                    }
                    prop_index = prop->next_prop;
                }
            }
        }
    }
}

// @address 0x507cb8 ?ai_scripting_count_internal@@YAJJFPAJPAM@Z
long ai_scripting_count_internal(long ai_index, short count_type, long *out_alive_count, float *out_health)
{
    long count = 0;
    long alive_count = 0;
    float health = 0.0f;
    float weight = 0.0f;
    long type = (unsigned long)ai_index >> 29;
    if (type == 3 || type == 4) {
        long actor_index = ai_index_get_actor(ai_index);
        if (actor_index != NONE) {
            actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
            object_datum *unit = (object_datum *)datum_get<s_object_header>(object_header_data, actor->unit_index)->datum;
            if (count_type != 3 || actor->root_vehicle_index != NONE)
                count = 1;
            alive_count = 1;
            health = unit->body_vitality * unit->maximum_body_vitality;
            weight = actor->weight;
        }
    } else {
        ai_index_squad_iterator iterator;
        ::ai_index_squad_iterator_new(ai_index, &iterator);
        squad_datum *squad;
        while ((squad = ai_index_squad_iterator_next(&iterator)) != 0) {
            switch (count_type) {
            case 0:
                count += squad->count0;
                break;
            case 1:
                count += squad->count1;
                break;
            case 2:
                count += squad->count2;
                break;
            case 3: {
                squad_actor_iterator actor_iterator;
                squad_actor_iterator_new(&actor_iterator, iterator.squad_index, false);
                actor_datum *actor;
                while ((actor = ::squad_actor_iterator_next(&actor_iterator)) != 0) {
                    if (actor->root_vehicle_index != NONE)
                        count++;
                }
                break;
            }
            default:
                // match: 0x507cb8: unreachable hint omits dead branch check
                __assume(0);
            }
            alive_count = (long)((float)alive_count + squad->original_count);
            health = health + squad->original_vitality * squad->health;
            weight = weight + squad->original_vitality;
        }
    }
    if (out_alive_count)
        *out_alive_count = alive_count;
    if (out_health) {
        if (weight > 0.0f)
            *out_health = health / weight;
        else
            *out_health = 0.0f;
    }
    return count;
}

// @address 0x507ec0 ai_task_get_count

extern "C" int ai_task_get_count(long reference)
{
    int result = 0;
    if (((unsigned long)reference >> 29) == 5 && ai_globals->enabled) {
        short squad_index = (short)reference;
        if (squad_index >= 0) {
            scenario *scenario = global_scenario;
            if (squad_index < scenario->objectives.count) {
                short task = (short)((reference >> 16) & 0x1fff);
                s_objective *objective = (s_objective *)tag_block_get_element_with_size(
                    &scenario->objectives, squad_index, sizeof(s_objective));
                if (task >= 0 && task < objective->tasks.count) {
                    short index = (short)(datum_get<s_objective_record>(g_objectives_data, squad_index)->first_task + task);
                    result = g_tasks_records[index].deaths;
                }
            }
        }
    }
    return result;
}

// @address 0x507f98 ?ai_fighting_count@@YAFJ@Z
short ai_fighting_count(long ai_index)
{
    unsigned short count = 0;
    if (ai_globals->enabled) {
        switch ((unsigned long)(ai_index & 0xe0000000) >> 29) {
        case 3:
        case 4: {
            long actor_index = ai_index_get_actor(ai_index);
            if (actor_index != NONE && actor_in_combat(actor_index))
                count = 1;
            break;
        }
        case 1: {
            long squad_index = (unsigned short)ai_index;
            if (squad_index >= 0 && squad_index < global_scenario->squads.count)
                count = datum_get<squad_datum>(squad_data, squad_index)->fighting_count;
            break;
        }
        case 2: {
            long squad_group_index = (unsigned short)ai_index;
            if (squad_group_index >= 0 && squad_group_index < global_scenario->squad_groups.count)
                count = (*datum_get<squad_group_datum>(g_ai_squad_group_data, squad_group_index)).fighting_count;
            break;
        }
        default:
            break;
        }
    }
    return count;
}

// @address 0x5080b8 ai_scripting_migrate_infantry
extern "C" void ai_scripting_migrate_infantry(long ai_index, long squad_index)
{
    short count = 0;
    if (ai_globals->enabled) {
        {
            ai_index_actor_iterator actor_iterator;
            ai_index_actor_iterator_new(ai_index, &actor_iterator);
            while (ai_index_actor_iterator_next(&actor_iterator) != 0) {
                long actor_index = actor_iterator.actor_index;
                actor_datum *actor_view = datum_get<actor_datum>(actor_data, actor_index);
                if (actor_view->attached_vehicle_index != NONE && actor_view->vehicle_desire >= 3)
                    continue;
                actor_change_squad(actor_index, (short)squad_index);
                count++;
            }
        }
        ai_globals_type *globals = ai_globals;
        for (short i = 0; i < 8; i++) {
            long unit_index = globals->mission_critical[i].unit_index;
            if (unit_index == NONE)
                continue;
            biped_datum *unit = (biped_datum *)object_get_and_verify_type(unit_index, 1);
            bool alive = (unit->object.damage_flags >> 2) & 1;
            if (!alive)
                continue;
            ai_index_squad_iterator squad_iterator;
            ::ai_index_squad_iterator_new(ai_index, &squad_iterator);
            if (ai_index_squad_iterator_next(&squad_iterator) != 0) {
                short squad_index_cached = unit->biped.squad_index;
                do {
                    if (squad_index_cached == squad_iterator.squad_index) {
                        unit->biped.squad_index = (short)squad_index;
                        count++;
                        break;
                    }
                } while (ai_index_squad_iterator_next(&squad_iterator) != 0);
            }
        }
        if (count > 0 && squad_index != NONE)
            ai_handle_squad_placement(squad_index);
    }
}

// @address 0x508220 ?ai_scripting_migrate_internal@@YAXJJ_N0@Z
void ai_scripting_migrate_internal(long ai_index, long squad_index, bool, bool)
{
    short count = 0;
    {
        ai_index_actor_iterator actor_iterator;
        ai_index_actor_iterator_new(ai_index, &actor_iterator);
        actor_datum *actor;
        while ((actor = ai_index_actor_iterator_next(&actor_iterator)) != 0) {
            actor_change_squad(actor_iterator.actor_index, (short)squad_index);
            count++;
        }
    }
    {
        ai_index_squad_iterator squad_iterator;
        ::ai_index_squad_iterator_new(ai_index, &squad_iterator);
        squad_datum *squad;
        while ((squad = ai_index_squad_iterator_next(&squad_iterator)) != 0) {
            long unit_index = squad->first_vehicle_index;
            while (unit_index != NONE) {
                vehicle_datum *unit = (vehicle_datum *)object_get_and_verify_type(unit_index, 2);
                long next_unit_index = unit->vehicle.next_vehicle_index;
                squad_vehicle_change_squad(unit_index, squad_index);
                unit_index = next_unit_index;
            }
        }
    }
    ai_globals_type *globals = ai_globals;
    for (short i = 0; i < 8; i++) {
        long unit_index = globals->mission_critical[i].unit_index;
        if (unit_index == NONE)
            continue;
        biped_datum *unit = (biped_datum *)object_get_and_verify_type(unit_index, 1);
        bool alive = (unit->object.damage_flags >> 2) & 1;
        if (!alive)
            continue;
        ai_index_squad_iterator iterator;
        ::ai_index_squad_iterator_new(ai_index, &iterator);
        if (ai_index_squad_iterator_next(&iterator) != 0) {
            short squad_index_cached = unit->biped.squad_index;
            do {
                if (squad_index_cached == iterator.squad_index) {
                    unit->biped.squad_index = (short)squad_index;
                    count++;
                    break;
                }
            } while (ai_index_squad_iterator_next(&iterator) != 0);
        }
    }
    if (count > 0 && squad_index != NONE)
        ai_handle_squad_placement(squad_index);
    squads_update_dirty_status();
}

// @address 0x5083b8 ?ai_scripting_allegiance@@YAXFF@Z
static __forceinline short tls_game_difficulty_level_get()
{
    return tls_game_globals()->options.game_mode == 1 ? tls_game_globals()->options.campaign_difficulty : 1;
}

void ai_scripting_allegiance(short team1, short team2)
{
    if (team1 != -1 && team2 != -1) {
        short other_team = -1;
        short threshold = -1;
        short timer = -1;
        bool enabled = false;
        if (team1 == 1) {
            other_team = team2;
        } else if (team2 == 1) {
            other_team = team1;
        }
        if (other_team != -1) {
            float traitor_timer[4] = { 2.0f, 5.0f, 10.0f, 20.0f };
            long difficulty = tls_game_difficulty_level_get();
            enabled = true;
            threshold = 5;
            timer = (short)game_seconds_to_ticks_round(traitor_timer[difficulty]);
        }
        game_allegiance_create((e_game_team)team1,
            enabled && team1 == other_team,
            (e_game_team)team2,
            enabled && team2 == other_team,
            threshold,
            timer);
    }
}

static const long k_max_squad_seats = 320;

// @address 0x508510 ?ai_enter_squad_vehicles@@YAFJ@Z
short ai_enter_squad_vehicles(long ai_index)
{
    short actor_count = 0;
    short seat_count = 0;
    unit_seat_source seats[k_max_squad_seats];
    long actor_indices[k_max_squad_seats];
    {
        ai_index_squad_iterator squad_iterator;
        ::ai_index_squad_iterator_new(ai_index, &squad_iterator);
        squad_datum *squad;
        while ((squad = ai_index_squad_iterator_next(&squad_iterator)) != 0) {
            long unit_index = squad->first_vehicle_index;
            while (unit_index != NONE && seat_count < k_max_squad_seats) {
                vehicle_datum *unit = (vehicle_datum *)object_get_and_verify_type(unit_index, 2);
                seat_count += unit_get_all_seats(unit_index, &seats[seat_count], (short)(k_max_squad_seats - seat_count));
                unit_index = unit->vehicle.next_vehicle_index;
            }
            if (seat_count >= k_max_squad_seats)
                break;
        }
    }
    {
        ai_index_actor_iterator iterator;
        ai_index_actor_iterator_new(ai_index, &iterator);
        actor_datum *actor;
        while ((actor = (actor_datum *)ai_index_actor_iterator_next(&iterator)) != 0) {
            if (actor->unit_index != -1 && actor->active != 0) {
                actor_indices[actor_count++] = iterator.actor_index;
                if (actor_count >= k_max_squad_seats)
                    break;
            }
        }
    }
    return ai_assign_actors_to_seats(actor_indices, actor_count, seats, seat_count, false);
}

const float k_real_max = 3.4028234663852886e+38f;
const float k_real_min = -3.4028234663852886e+38f;

static __forceinline bool ai_script_seat_flag_test(unsigned long flags, long bit)
{
    return (flags & (1 << bit)) != 0;
}
static __forceinline bool ai_script_actor_has_scary_weapon(long actor_index)
{
    long unit_index = ((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_index)].unit_index;
    bool result = false;
    if (unit_index != NONE)
        result = unit_has_scary_weapon(unit_index);
    return result;
}
static __forceinline float ai_script_points_distance_squared3d(const real_point3d *a, const real_point3d *b)
{
    float dx = b->x - a->x;
    float dy = b->y - a->y;
    float dz = b->z - a->z;
    return dx * dx + (dz * dz + dy * dy);
}
static __forceinline void ai_script_action_request_new(action_request *request, long type)
{
    ai_script_action_request_clear(request);
    request->action = type;
}
static __forceinline bool action_submit_by_type(long unit_index, long type)
{
    action_request request;
    ai_script_action_request_clear(&request);
    ai_script_action_request_new(&request, type);
    return action_submit(unit_index, &request);
}
static __forceinline float ai_script_points_distance3d(const real_point3d *a, const real_point3d *b)
{
    return sqrtf(ai_script_points_distance_squared3d(a, b));
}

// @address 0x508660 ?ai_assign_actors_to_seats@@YAFPBJFPBUunit_seat_source@@F_N@Z
short ai_assign_actors_to_seats(const long *actor_indices, short num_actors, const unit_seat_source *seats, short num_seats, bool force)
{
    short scary_actor_count = 0;
    unsigned long invalid_seats[(k_max_squad_seats + 31) >> 5];
    unsigned long valid_seats[k_max_squad_seats][NUMBEROF(invalid_seats)];
    short assigned_count = 0;
    short unassigned_actor_count = 0;
    unsigned long assigned_actors[NUMBEROF(invalid_seats)];

    memset(invalid_seats, 0, sizeof(invalid_seats));
    memset(assigned_actors, 0, sizeof(assigned_actors));
    memset(valid_seats, 0, sizeof(valid_seats));

    if (num_seats > 0 && num_actors > 0) {
        actor_datum *first_actor = (actor_datum *)(actor_data->data + (unsigned short)actor_indices[0] * sizeof(actor_datum));
        e_game_team team = (e_game_team)first_actor->team;
        if (team != _game_team_none) {
            for (short seat_index = 0; seat_index < num_seats; seat_index++) {
                short unit_seat_index = seats[seat_index].seat_index;
                long unit_index = seats[seat_index].unit_index;
                if (!ai_script_seat_flag_test(seats[seat_index].definition->flags, 11)) {
                    long reserved_actor_index = ai_vehicle_get_reservation(unit_index, unit_seat_index, team);
                    if (reserved_actor_index != NONE) {
                        actor_datum *reserved_actor = &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(reserved_actor_index)];
                        if (reserved_actor->vehicle_desire == 2) {
                            actor_handle_seat_attach(reserved_actor_index,
                                reserved_actor->destination_unit_index,
                                    reserved_actor->vehicle_seat_index);
                            reserved_actor->vehicle_desire = 3;
                            reserved_actor->reserved_timer = 0;
                        }
                    }
                }
            }

            {
                short assigned_actor_count = 0;
                short total_actor_count = 0;
                for (short actor_index = 0; actor_index < num_actors; actor_index++) {
                    actor_datum *actor = &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_indices[actor_index])];
                    if (actor->attached_vehicle_index != NONE && actor->vehicle_desire == 3) {
                        for (short seat_index = 0; seat_index < num_seats; seat_index++) {
                            if (actor->attached_vehicle_index == seats[seat_index].unit_index) {
                                SET_BIT(assigned_actors[actor_index >> 5], actor_index & 31, true);
                                assigned_actor_count++;
                                break;
                            }
                        }
                    }
                    if (!TEST_BIT(assigned_actors[actor_index >> 5], actor_index & 31)) {
                        if (ai_script_actor_has_scary_weapon(actor_indices[actor_index]))
                            scary_actor_count++;
                    }
                    total_actor_count++;
                }
                unassigned_actor_count = total_actor_count - assigned_actor_count;
            }

            if (unassigned_actor_count > 0) {

                for (short seat_index = 0; seat_index < num_seats; seat_index++) {
                    short unit_seat_index = seats[seat_index].seat_index;
                    long unit_index = seats[seat_index].unit_index;
                    if (ai_unit_seat_disallowed(NONE, unit_index, unit_seat_index, team)) {
                        SET_BIT(invalid_seats[seat_index >> 5], seat_index & 31, true);
                    } else if (!force) {
                        float min_distance_sq = k_real_max;
                        real_point3d seat_origin;
                        object_get_origin(unit_index, &seat_origin);
                        for (short actor_index = 0; actor_index < num_actors; actor_index++) {
                            actor_datum *actor = &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_indices[actor_index])];
                            if (!TEST_BIT(assigned_actors[actor_index >> 5], actor_index & 31)) {
                                float dist_sq = ai_script_points_distance_squared3d(&actor->position.body, &seat_origin);
                                min_distance_sq = dist_sq <= min_distance_sq ? dist_sq : min_distance_sq;
                            }
                        }
                        if (min_distance_sq > 225.0f)
                            SET_BIT(invalid_seats[seat_index >> 5], seat_index & 31, true);
                    }
                }

                {
                    long last_weapon_class = NONE;
                    long last_weapon_type = NONE;
                    long last_graph_index = NONE;
                    short last_new_index = NONE;
                    for (short actor_index = 0; actor_index < num_actors; actor_index++) {
                        if (!TEST_BIT(assigned_actors[actor_index >> 5], actor_index & 31)) {
                            actor_datum *actor = &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_indices[actor_index])];
                            unit_datum *unit = (unit_datum *)object_get_and_verify_type(actor->unit_index, 0x2003);
                            const c_animation_manager *animation_manager =
                                (const c_animation_manager *)((const char *)unit + unit->object.animation.offset);
                            long weapon_class = animation_manager->m_current_goal.weapon_class;
                            long weapon_type = animation_manager->m_current_goal.weapon_type;
                            long graph_index = animation_manager->m_graph_tag_index;
                            if (last_weapon_class != weapon_class || last_weapon_type != weapon_type || last_graph_index != graph_index) {
                                for (short seat_index = 0; seat_index < num_seats; seat_index++) {
                                    if (!TEST_BIT(invalid_seats[seat_index >> 5], seat_index & 31)) {
                                        short unit_seat_index = seats[seat_index].seat_index;
                                        long unit_index = seats[seat_index].unit_index;
                                        bool valid = false;
                                        if (force)
                                            valid = unit_has_seat_animation(actor->unit_index, unit_index, unit_seat_index);
                                        else
                                            valid = unit_has_seat_entrance_animation(actor->unit_index, unit_index, unit_seat_index);
                                        SET_BIT(valid_seats[actor_index][seat_index >> 5], seat_index & 31, valid);
                                    }
                                }
                                last_weapon_class = weapon_class;
                                last_weapon_type = weapon_type;
                                last_graph_index = graph_index;
                                last_new_index = actor_index;
                            } else {
                                memcpy(valid_seats[actor_index], valid_seats[last_new_index], sizeof(valid_seats[actor_index]));
                            }
                        }
                    }
                }

                while (unassigned_actor_count > 0) {
                    long root_unit_index = NONE;
                    long last_unit_index = NONE;
                    short best_seat_index = NONE;
                    bool driver_disallowed = false;
                    float best_preference = k_real_min;
                    for (short seat_index = 0; seat_index < num_seats; seat_index++) {
                        if (!TEST_BIT(invalid_seats[seat_index >> 5], seat_index & 31)) {
                            float preference = 0.0f;
                            if (seats[seat_index].unit_index != last_unit_index) {
                                last_unit_index = seats[seat_index].unit_index;
                                root_unit_index = ai_get_root_vehicle(last_unit_index);
                                short driver_seat = unit_get_driver_seat(root_unit_index);
                                driver_disallowed = ai_unit_seat_disallowed(NONE, root_unit_index, driver_seat, team);
                            }
                            preference = ai_unit_get_seat_preference(seats[seat_index].unit_index,
                                seats[seat_index].seat_index,
                                    driver_disallowed,
                                unassigned_actor_count > 1, scary_actor_count > 0);
                            if (preference > best_preference) {
                                best_preference = preference;
                                best_seat_index = seat_index;
                            }
                        }
                    }
                    if (best_seat_index == NONE)
                        break;

                    long best_actor_array_index = NONE;
                    long best_actor_index = NONE;
                    float best_score = 0.0f;
                    short best_unit_seat_index = seats[best_seat_index].seat_index;
                    long best_unit_index = seats[best_seat_index].unit_index;
                    long best_root_unit_index = ai_get_root_vehicle(best_unit_index);
                    real_point3d seat_origin;
                    object_get_origin(best_unit_index, &seat_origin);
                    for (short actor_index = 0; actor_index < num_actors; actor_index++) {
                        if (!TEST_BIT(assigned_actors[actor_index >> 5], actor_index & 31) &&
                            TEST_BIT(valid_seats[actor_index][best_seat_index >> 5], best_seat_index & 31)) {
                            if (ai_vehicle_desirable(actor_indices[actor_index], best_root_unit_index, false, false)) {
                                actor_datum *actor = &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(actor_indices[actor_index])];
                                float distance = ai_script_points_distance3d(&actor->position.body, &seat_origin);
                                float score = ai_unit_get_seat_score(actor_indices[actor_index],
                                    best_unit_index,
                                        best_unit_seat_index,
                                            distance,
                                                force);
                                short valid_seat_count = (short)bit_vector_count_bits(valid_seats[actor_index], k_max_squad_seats);
                                if (valid_seat_count == 1 && score > 0.0f)
                                    score += 100.0f;
                                if (score > best_score) {
                                    best_score = score;
                                    best_actor_index = actor_indices[actor_index];
                                    best_actor_array_index = actor_index;
                                }
                            }
                        }
                    }

                    if (best_actor_index != NONE && !ai_unit_seat_disallowed(best_actor_index,
                        best_unit_index,
                            best_unit_seat_index,
                                team)) {
                        actor_datum *actor = &((actor_datum *)actor_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(best_actor_index)];
                        if (actor->destination_unit_index != NONE &&
                            actor->destination_unit_index != best_unit_index &&
                            actor->vehicle_seat_index != best_unit_seat_index) {
                            action_submit_by_type(actor->unit_index, force ? 20 : 13);
                        }
                        actor_attach_to_vehicle(best_actor_index, best_root_unit_index, true);
                        actor_attach_to_seat(best_actor_index, best_unit_index, best_unit_seat_index, 0.0f, true);
                        if (force) {
                            action_request request;
                            ai_script_action_request_clear(&request);
                            request.action = 17;
                            request.enter_vehicle.vehicle = best_unit_index;
                            request.enter_vehicle.seat = best_unit_seat_index;
                            request.enter_vehicle.immediate = true;
                            request.enter_vehicle.force = false;
                            action_submit(actor->unit_index, &request);
                        }
                        if (ai_script_actor_has_scary_weapon(best_actor_index))
                            scary_actor_count--;
                        assigned_count++;
                        unassigned_actor_count--;
                        SET_BIT(assigned_actors[best_actor_array_index >> 5], best_actor_array_index & 31, true);
                        for (short actor_index = 0; actor_index < num_actors; actor_index++)
                            SET_BIT(valid_seats[actor_index][best_seat_index >> 5], best_seat_index & 31, false);
                    }
                    SET_BIT(invalid_seats[best_seat_index >> 5], best_seat_index & 31, true);
                }

            }
        }
    }

    return assigned_count;
}

// @address 0x5090e0 ?ai_vehicle_enter_internal@@YAXJJJ_N@Z
void ai_vehicle_enter_internal(long ai_index, long vehicle_index, long seat_set, bool force)
{
    if (ai_index != -1 && object_try_and_get_and_verify_type(vehicle_index, 2) != 0) {
        short actor_count = 0;
        long parent = vehicle_get_ultimate_parent(vehicle_index);
        ai_index_actor_iterator iterator;
        long actor_indices[k_max_squad_seats];
        unit_seat_source seats[64];
        short seat_count = (short)hs_get_unit_seats(parent, seat_set, _vehicle_seat_desire_any, _unit_seat_occupancy_empty, seats, 64);
        ai_index_actor_iterator_new(ai_index, &iterator);
        actor_datum *actor;
        while ((actor = (actor_datum *)ai_index_actor_iterator_next(&iterator)) != 0) {
            if (actor->unit_index != -1 && actor->active != 0) {
                actor_indices[actor_count] = iterator.actor_index;
                ++actor_count;
                if (actor_count >= k_max_squad_seats) break;
            }
        }
        ai_assign_actors_to_seats(actor_indices, actor_count, seats, seat_count, force);
    }
}

// @address 0x5091d0 ?ai_scripting_exit_vehicle@@YAXJJ@Z
void ai_scripting_exit_vehicle(long ai_index, long seat)
{
    ai_index_actor_iterator iterator;
    ai_index_actor_iterator_new(ai_index, &iterator);
    bool match;
    actor_datum *actor;
    while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
        match = seat == NONE;
        if (!match && actor->root_vehicle_index != NONE) {
            match = ai_script_unit_in_seat(actor->unit_index, seat);
        }
        if (match && actor->root_vehicle_index != NONE && actor->unit_index != NONE) {
            action_request request;
            ai_script_action_request_clear(&request);
            request.action = 13;
            request.exit_vehicle.exit = true;
            long unit = actor->unit_index;
            ::action_submit(unit, &request);
        }
    }
}

static __forceinline unit_datum *ai_script_object_try_get_unit_or_vehicle(long object_index)
{
    s_data_array *headers = (s_data_array *)object_header_data;
    if (object_index != NONE && (unsigned short)object_index < headers->first_unallocated) {
        s_object_header *header = (s_object_header *)(headers->data + headers->size * (unsigned short)object_index);
        if (header->identifier != 0 && header->identifier == (object_index >> 16) && ((1u << header->object_type) & 0x1003) != 0)
            return (unit_datum *)header->datum;
    }
    return 0;
}

// @address 0x509680 ai_scripting_prefer_target
extern "C" void ai_scripting_prefer_target(long object_list_index, bool enable)
{
    long iterator;
    long reference;
    long object_index = object_list_get_first_or_none(object_list_index, &iterator);
    if (object_index != NONE) {
        reference = iterator;
        do {
            unit_datum *object = ai_script_object_try_get_unit_or_vehicle(object_index);
            if (object) {
                if (enable) object->unit.unit_flags |= 0x100;
                else object->unit.unit_flags &= ~0x100UL;
            }
            object_index = object_list_get_next(object_list_index, &reference);
        } while (object_index != NONE);
    }
}

// @address 0x5097f0 ai_scripting_set_actor_value
extern "C" void ai_scripting_set_actor_value(long ai_index, short value, unsigned char flag)
{
    if (ai_index != NONE) {
        ai_index_actor_iterator iterator;
        ai_index_actor_iterator_new(ai_index, &iterator);
        actor_datum *actor;
        while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
            actor->target_ticks = value;
            actor->unowned_target_salient_raw = flag;
        }
    }
}

 

static __forceinline creature_datum *teleport_object_get_datum(long object_index)
{
    return (creature_datum *)datum_get<s_object_header>(object_header_data, object_index)->datum;
}
static __forceinline long teleport_object_get_ultimate_parent(long object_index)
{
    long parent;
    do {
        parent = object_index;
        object_index = teleport_object_get_datum(parent)->object.parent_object_index;
    } while (object_index != -1);
    return parent;
}
static __forceinline void teleport_swarm_creature_iterator_new(long swarm_index, teleport_swarm_creature_iterator *iterator)
{
    iterator->swarm_index = swarm_index;
    iterator->creature_index = -1;
    iterator->next_creature_index = datum_get<swarm_datum>(g_swarm_data, swarm_index)->first_object_index;
}
static __forceinline creature_datum *teleport_swarm_creature_iterator_next(teleport_swarm_creature_iterator *iterator)
{
    creature_datum *creature = 0;
    if (iterator->next_creature_index != -1) {
        iterator->creature_index = iterator->next_creature_index;
        creature = teleport_object_get_datum(iterator->creature_index);
        swarm_creature_state *data;
        if (creature->creature.link_type == 0 && (data = (swarm_creature_state *)((char *)creature + creature->creature.creature_reference.offset)) != 0)
            iterator->next_creature_index = data->next_swarm_creature_index;
        else
            iterator->next_creature_index = -1;
    }
    return creature;
}
static __forceinline real_vector3d *teleport_add_vectors3d(const real_vector3d *a, const real_point3d *b, real_vector3d *result)
{
    result->i = a->i + b->x;
    result->j = a->j + b->y;
    result->k = a->k + b->z;
    return result;
}
static __forceinline real_vector3d *teleport_scale_vector3d(const real_vector3d *a, float scale, real_vector3d *result)
{
    result->i = a->i * scale;
    result->j = a->j * scale;
    result->k = a->k * scale;
    return result;
}
static __forceinline real_point3d *teleport_point_from_line3d(const real_point3d *point,
            const real_vector3d *vector,
            float scale,
            real_point3d *result)
{
    result->x = point->x + vector->i * scale;
    result->y = point->y + vector->j * scale;
    result->z = point->z + vector->k * scale;
    return result;
}

// @address 0x509c90 ai_scripting_teleport
extern "C" void ai_scripting_teleport(long ai_index, long point_ref)
{
    real_point3d position;
    real_vector3d sum = *global_zero_vector3d;
    short count = 0;
    if (valid_point_ref(point_ref)) {
        cs_point_set *point_set = cs_get_point_set((unsigned short)(point_ref >> 16));
        cs_point *point = (cs_point *)tag_block_get_element_with_size(&point_set->points, (unsigned short)point_ref, sizeof(cs_point));
        ::ai_point_get_position(&point->position, &position);
        ai_index_actor_iterator iterator;
        actor_datum *actor;
        ai_index_actor_iterator_new(ai_index, &iterator);
        while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
            teleport_add_vectors3d(&sum, &actor->position.body, &sum);
            count++;
        }
        if (count > 0) {
            float scale = 1.0f / (float)count;
            teleport_scale_vector3d(&sum, scale, &sum);
            real_vector3d offset;
            offset.i = position.x - sum.i;
            offset.j = position.y - sum.j;
            offset.k = position.z - sum.k;
            ai_index_actor_iterator_new(ai_index, &iterator);
            while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
                if (actor->unit_index != -1) {
                    real_point3d destination;
                    destination.x = actor->position.body.x + offset.i;
                    destination.y = actor->position.body.y + offset.j;
                    destination.z = actor->position.body.z + offset.k;
                    long object_index = teleport_object_get_ultimate_parent(actor->unit_index);
                    object_set_position_direct(object_index, &destination, 0, 0, 0, false);
                    object_reset_after_teleport(object_index);
                    actor_datum *path_actor = datum_get<actor_datum>(actor_data, iterator.actor_index);
                    path_actor->path.valid = false;
                    path_actor->path.obstacle_object_index = -1;
                    path_actor->path.result_value = 0;
                    path_actor->path_result_time = 0;
                    path_actor->path_result_count = 0;
                    path_actor->path_state = 0;
                    path_actor->destination_orders.clear();
                } else if (actor->swarm_index != -1) {
                    real_vector3d swarm_offset = offset;
                    swarm_offset.k += 0.15f;
                    teleport_swarm_creature_iterator creature_iterator;
                    creature_datum *creature;
                    teleport_swarm_creature_iterator_new(actor->swarm_index, &creature_iterator);
                    while ((creature = teleport_swarm_creature_iterator_next(&creature_iterator)) != 0) {
                        if (creature->object.parent_object_index == -1) {
                            real_point3d origin;
                            object_get_origin(creature_iterator.creature_index, &origin);
                            teleport_point_from_line3d(&origin, &swarm_offset, 1.0f, &origin);
                            object_set_position_direct(creature_iterator.creature_index, &origin, 0, 0, 0, false);
                            object_reset_after_teleport(creature_iterator.creature_index);
                        }
                    }
                }
            }
        }
    }
}

// @address 0x509fe8 ?ai_scripting_renew@@YAXJ@Z
void ai_scripting_renew(long ai_index) {
    ai_index_actor_iterator iterator;
    ai_index_actor_iterator_new(ai_index, &iterator);
    actor_datum *actor;
    while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
        if (actor->unit_index != NONE) {
            character_grenade_properties *properties = script_grenade_properties(iterator.actor_index);
            unit_datum *unit = script_object_get(actor->unit_index);
            unit->object.body_vitality = unit->object.maximum_body_vitality > 0.0f ? 1.0f : 0.0f;
            unit->object.shield_vitality = unit->object.maximum_shield_vitality > 0.0f ? 1.0f : 0.0f;
            if (properties) {
                short desired_count = random_range(TLS_RANDOM_SEED, properties->minimum_count, (short)(properties->maximum_count + 1));
                short current_count = script_grenade_count(actor->unit_index, script_grenade_type(actor->unit_index));
                if (current_count < desired_count) script_add_grenades(actor->unit_index,
                    properties->identifier,
            (short)(desired_count - current_count));
            }
        }
    }
}

// @address 0x50a1e8 ai_scripting_force_active
extern "C" void ai_scripting_force_active(long ai_index, bool force_active)
{
    if (ai_globals->enabled && ai_index != NONE) {
        switch ((unsigned long)ai_index >> 29) {
        case 3:
        case 4: {
            long actor_index = ai_index_get_actor(ai_index);
            if (actor_index != NONE) {
                actor_datum *actor = (actor_datum *)datum_try_and_get(actor_data, actor_index);
                if (actor && actor->squad_index != NONE) {
                    squad_datum *squad = datum_get<squad_datum>(squad_data, actor->squad_index);
                    if (squad->flags & 4) {
                        if (force_active) {
                            squad->flags |= 0x200;
                            squad_force_activate(actor->squad_index);
                        } else {
                            squad->flags &= ~0x200;
                            squad_force_activate(actor->squad_index);
                        }
                    }
                }
            }
            break;
        }
        case 1:
        case 2: {
            ai_index_squad_iterator iterator;
            ::ai_index_squad_iterator_new(ai_index, &iterator);
            squad_datum *squad;
            while ((squad = ai_index_squad_iterator_next(&iterator)) != 0) {
                if (squad->flags & 4) {
                    if (force_active) squad->flags |= 0x200;
                    else squad->flags &= ~0x200;
                    squad_force_activate(iterator.squad_index);
                }
            }
            break;
        }
        }
    }
}

// @address 0x50a460 ?ai_vehicle_reserve_seat@@YA_NJJ_N@Z
bool ai_vehicle_reserve_seat(long index,long seats,bool reserve) {
 bool result=false;
 if(index!=-1) {
  unit_seat_source sources[64];
  short count=(short)hs_get_unit_seats(index,seats,_vehicle_seat_desire_any,_unit_seat_occupancy_any,sources,64);
  if(count>0) {
   for(short i=0;i<count;i++) {
    unit_seat_source *source=&sources[i];
    vehicle_datum *v=(vehicle_datum *)object_get_and_verify_type(source->unit_index,2);
    if(reserve) v->vehicle.locked_seats[source->seat_index>>5]|=1<<(source->seat_index&31);
    else v->vehicle.locked_seats[source->seat_index>>5]&=~(1<<(source->seat_index&31));
    result=true;
   }
  }
 }
 return result;
}

// @address 0x50a540 ai_vehicle_reserve
extern "C" bool ai_vehicle_reserve(long unit_index, bool reserve)
{
 bool result=false;
 if(unit_index!=-1) {
  unit_seat_source seats[64];
  short seat_count=unit_get_all_seats(unit_index,seats,64);
  long last_unit_index=-1;
  for(short seat_index=0;seat_index<seat_count;seat_index++) {
   unit_seat_source *seat=&seats[seat_index];
   if(last_unit_index!=seat->unit_index) {
    if(object_get_type(seat->unit_index)==1) {
     vehicle_datum *unit=(vehicle_datum *)object_get_and_verify_type(seat->unit_index,2);
     if(reserve) memset(unit->vehicle.locked_seats,-1,sizeof(unit->vehicle.locked_seats));
     else memset(unit->vehicle.locked_seats,0,sizeof(unit->vehicle.locked_seats));
    }
    last_unit_index=seat->unit_index;
   }
  }
  result=true;
 }
 return result;
}

static __forceinline void ai_play_line_direction_to_object(c_ai_direction *direction, long object_index)
{
    direction->type = 7;
    direction->object_index = object_index;
}
static __forceinline void ai_play_line_prop_iterator_new(actor_prop_ref_iterator *iterator, long actor_index)
{
    iterator->index = -1;
    iterator->next_index = datum_get<actor_datum>(actor_data, actor_index)->first_prop_ref_index;
}
static __forceinline prop_ref_datum *ai_play_line_prop_iterator_next(actor_prop_ref_iterator *iterator)
{
    prop_ref_datum *result = 0;
    if (iterator->next_index != -1) {
        iterator->index = iterator->next_index;
        result = datum_get<prop_ref_datum>(prop_ref_data, iterator->next_index);
        iterator->next_index = result->next_prop_ref_index;
    }
    return result;
}

// @address 0x50a858 ?ai_play_line_at_player@@YAFJJ@Z
short ai_play_line_at_player(long p0, long p1)
{
    float playing_time = 0.0f;
    float rounding = 0.5f;
    if (p1 != -1) {
        ai_index_actor_iterator iterator;
        ai_index_actor_iterator_new(p0, &iterator);
        if (ai_index_actor_iterator_next(&iterator)) {
            float out_time = playing_time;
            float best_value = playing_time;
            long best_index = -1;
            long actor_index = iterator.actor_index;
            mission_dialogue_play_line(actor_index, -1, false, p1, &out_time);
            if (out_time > best_value)
                playing_time = out_time;
            float one_tenth = 0.1f;
            float fifteen = 15.0f;
            actor_prop_ref_iterator iterator;
            ai_play_line_prop_iterator_new(&iterator, actor_index);
            prop_ref_datum *ref;
            while ((ref = ai_play_line_prop_iterator_next(&iterator)) != 0) {
                long prop_datum_index = ref->prop_index;
                prop_datum *prop = datum_get<prop_datum>(prop_data, prop_datum_index);
                if (prop->flag == 0)
                    continue;
                float value = one_tenth;
                signed char knowledge = ref->knowledge_state;
                if (knowledge >= 1 && knowledge <= 2)
                    value = 15.1f;
                float distance = ref->distance;
                if (!(distance >= fifteen))
                    value += fifteen - distance;
                if (value <= best_value)
                    continue;
                best_value = value;
                best_index = iterator.index;
            }
            if (best_index != -1) {
                prop_ref_datum *best_ref = datum_get<prop_ref_datum>(prop_ref_data, best_index);
                actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
                if (actor->attention_level <= 2) {
                    short ticks = game_time_globals->ticks_per_second;
                    long ticks_value = (long)((float)ticks * 2.0f + rounding);
                    actor->attention_ticks = (short)ticks_value;
                    ai_play_line_direction_to_object(&actor->attention_direction, best_ref->object_index);
                    actor->attention_level = 2;

                }
            }
        }
    }
    return (short)(int)(playing_time * 3e+01f + rounding);
}

// @address 0x50aa60 ai_play_line_on_object
extern "C" short ai_play_line_on_object(long object_index, long dialogue_id)
{
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x50aa60: the float loads through the constant-pool base register the function already holds
    float playing_time = k_real_constant_pool_half[-553];
#else
    float playing_time = 0.0f;
#endif
    if (dialogue_id != -1) {
        long playback_label = 0;
        float out_time = playing_time;
        s_ai_mission_dialogue_variant *variant = (s_ai_mission_dialogue_variant *)ai_mission_dialogue_try_and_get_variant(dialogue_id,
            0,
            &playback_label);
        if (variant != NULL) {
            long sound_index = variant->sound.index;
            if (sound_index != -1) {
                scripted_sound_new_internal(sound_index,
                    object_index,
#if defined(_XBOX) && !defined(__clang__)
                        // match: 0x50aa60: the float loads through the constant-pool base register the function already holds
                        k_real_constant_pool_half[-753],
#else
                        1.0f,
#endif
                            k_scripted_sound_default_gain,
                                k_scripted_sound_default_gain,
                                    playback_label,
                                        &out_time);
                playing_time = out_time;
            }
        }
    }
    return (short)(int)(playing_time * 3e+01f + k_real_constant_pool_half[0]);
}

// @address 0x50ac38 ai_script_duration_compare
extern "C" bool ai_script_duration_compare(s_duration_sort_entry p0, s_duration_sort_entry p1, const void *context)
{
    bool result = false;
    if (p0.priority >= p1.priority) result = true;
    return result;
}

// @address 0x50aef8 ?cs_command_script_running@@YA_NJJ@Z
bool cs_command_script_running(long ai_index, long script_index) {
    bool result = false;
    if (script_index != NONE) {
        ai_index_actor_iterator iterator;
        ai_index_actor_iterator_new(ai_index, &iterator);
        actor_datum *actor;
        while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
            long command_index = actor->command_index;
            while (command_index != NONE) {
                command_script_datum *command = datum_get<command_script_datum>(command_script_data, command_index);
                if (command->script_index == script_index) { result = true; break; }
                else command_index = command->next_cs;
            }
        }
    }
    return result;
}

// @address 0x50afa8 ?ai_scripting_place_internal@@YAXJ_N@Z
void ai_scripting_place_internal(long ai_index, bool force)
{
    if (ai_index != -1) {
        unsigned long type = (unsigned long)ai_index >> 29;
        if (type == 1 || type == 2) {
            ai_index_squad_iterator iterator;
            ::ai_index_squad_iterator_new(ai_index, &iterator);
            while (ai_index_squad_iterator_next(&iterator)) ::squad_create(iterator.squad_index, -1, -1, -1, false, true, force);
            ai_globals->current_squad = ai_index;
        } else if (type == 4) {
            short squad_index = (short)((ai_index >> 16) & 0x1fff);
            if (squad_index >= 0 && squad_index < global_scenario_get()->squads.count) {
                squad_definition *definition = (squad_definition *)tag_block_get_element_with_size(
                    &global_scenario_get()->squads, (unsigned short)squad_index, sizeof(squad_definition));
                short spawn_index = (short)((ai_index >> 8) & 255);
                if (spawn_index >= 0 && spawn_index < definition->fire_team_block.count) {
                    fire_team_definition *spawn_definition = (fire_team_definition *)tag_block_get_element_with_size(
                        &definition->fire_team_block, spawn_index, sizeof(fire_team_definition));
                    short location = (short)(ai_index & 255);
                    if (location >= 0 && location < spawn_definition->starting_locations.count) {
                        ::squad_create(squad_index, spawn_index, location, 1, false, true, force);
                        ai_globals->current_squad = (unsigned short)squad_index | 0x20000000;
                    }
                }
            }
        }
    }
}

// @address 0x50b100 ?ai_scripting_place2_internal@@YAXJF_N@Z
void ai_scripting_place2_internal(long ai_index, short count, bool force)
{
    if (ai_index != -1) {
        if ((ai_index & 0xe0000000) == 0x20000000 && count > 0)
            squad_create((short)ai_index, -1, -1, count, false, true, force);
        ai_globals->current_squad = ai_index;
    }
}

static __forceinline void squad_actor_iterator_try_new(squad_actor_iterator *iterator, long squad_index, bool active_only)
{
    if (ai_globals->enabled) {
        iterator->squad_index = squad_index;
        iterator->index = -1;
        iterator->active_only = active_only;
        if (squad_index == -1)
            iterator->next_index = ai_globals->first_actor_index;
        else
            iterator->next_index = datum_get<squad_datum>(squad_data, squad_index)->first_actor_index;
    }
}

// @address 0x50a360 ?ai_scripting_set_team@@YAXJF@Z
void ai_scripting_set_team(long ai_index, short team)
{
    if ((unsigned int)(ai_index & 0xE0000000) > 0x40000000) return;
    if (ai_index == -1) return;
    if (team == -1) return;
    ai_index_squad_iterator iterator;
    ::ai_index_squad_iterator_new(ai_index, &iterator);
    squad_datum *squad = ai_index_squad_iterator_next(&iterator);
    if (squad == 0) {
        ai_update_team_status();
        return;
    }
    char team_value = (char)team;
    do {
        squad->team = (unsigned char)team_value;
        squad_actor_iterator actor_iterator;
        squad_actor_iterator_try_new(&actor_iterator, iterator.squad_index, false);
        actor_datum *actor;
        while ((actor = ::squad_actor_iterator_next(&actor_iterator)) != 0)
            actor_set_team(actor_iterator.index, team);
        squad = ai_index_squad_iterator_next(&iterator);
    } while (squad != 0);
    ai_update_team_status();
}

static __forceinline void ai_script_set_actor_braindead(actor_datum *actor, long actor_index, bool braindead)
{
    if (braindead) {
        actor->behavior_mode = 0;
        actor_freeze(actor_index);
    } else if (actor->behavior_mode == 0) {
        actor->behavior_mode = 3;
    }
}
// @address 0x5092f0 ?ai_scripting_braindead_by_unit@@YAXJ_N@Z
void ai_scripting_braindead_by_unit(long object_list_index, bool braindead)
{
    long iterator;
    long object_index = object_list_get_first_or_none(object_list_index, &iterator);
    if (object_index != NONE) {
        long reference = iterator;
        do {
            unit_datum *unit = (unit_datum *)ai_script_object_try_get_unit_or_vehicle(object_index);
            if (unit) {
                if (unit->unit.actor_index != NONE)
                    ai_script_set_actor_braindead(datum_get<actor_datum>(actor_data,
                        unit->unit.actor_index),
                            unit->unit.actor_index,
                                braindead);
                long child_index = unit->object.first_child_object_index;
                while (child_index != NONE) {
                    s_object_header *header = datum_get<s_object_header>(object_header_data, child_index);
                    unit_datum *child = (unit_datum *)header->datum;
                    if (((1 << child->object.object_type) & 0x1003) != 0 && child->unit.actor_index != NONE)
                        ai_script_set_actor_braindead(
                            datum_get<actor_datum>(actor_data, child->unit.actor_index), child->unit.actor_index, braindead);
                    child_index = child->object.next_object_index;
                }
            }
            object_index = object_list_get_next(object_list_index, &reference);
        } while (object_index != NONE);
    }
}

// @address 0x50b180 ?ai_scripting_kill_internal@@YAXJ_N0@Z
void ai_scripting_kill_internal(long ai_index, bool silent, bool multiplayer_cinematic)
{
    if (ai_index != NONE) {
        ai_index_actor_iterator iterator;
        ai_index_actor_iterator_new(ai_index, &iterator);
        actor_datum *actor;
        while ((actor = ai_index_actor_iterator_next(&iterator)) != 0) {
            bool updating = iterator.actor_index == g_updating_actor_index;
            actor_kill(iterator.actor_index, silent, updating);
        }
    }
}

static __forceinline const s_game_cluster_bit_vectors *ai_script_game_get_cluster_pvs()
{
    return (const s_game_cluster_bit_vectors *)&tls_game_globals()->cluster_pvs;
}
static __forceinline void squad_cursor_new(clump_actor_cursor *iterator)
{
    if (ai_globals->enabled) {
        iterator->iterator.absolute_index = NONE;
        iterator->iterator.index = NONE;
        iterator->iterator.data = (s_data_array *)squad_data;
    }
}
static __forceinline squad_datum *squad_cursor_next(clump_actor_cursor *iterator)
{
    squad_datum *result = 0;
    if (ai_globals->enabled) {
        iterator->datum = data_iterator_next(&iterator->iterator);
        if (iterator->datum)
            result = (squad_datum *)iterator->datum;
        iterator->index = (unsigned short)iterator->iterator.index;
    }
    return result;
}

static __forceinline bool ai_script_bit_vector_and(long count, const unsigned long *a, const unsigned long *b)
{
    bool result = false;
    for (long index = ((count + 31) >> 5) - 1; index >= 0; index--) {
        if (a[index] & b[index])
            result = true;
    }
    return result;
}
static __forceinline bool script_actor_path_executing(long actor_index)
{
    actor_datum *actor = (actor_datum *)(actor_data->data + (unsigned short)actor_index * sizeof(actor_datum));
    bool result = false;
    if (actor->path.valid && actor->path_state == 1)
        result = true;
    return result;
}

static __forceinline bool ai_script_squad_test_actor_activation(long squad_index, const s_game_cluster_bit_vectors *pvs)
{
    squad_datum *squad = datum_get<squad_datum>(squad_data, squad_index);
    bool result = false;
    for (long actor_index = squad->first_actor_index; actor_index != NONE; ) {
        result = actor_test_activation_cluster_bit_vector(actor_index, pvs);
        if (result)
            break;
        actor_datum *actor = (actor_datum *)(actor_data->data + (unsigned short)actor_index * sizeof(actor_datum));
        actor_index = actor->next_squad_actor_index;
    }
    return result;
}

// @address 0x54bf90 ai_update_activation
extern "C" void ai_update_activation()
{
    long ticks = game_seconds_to_ticks_round(1.0f);
    const s_game_cluster_bit_vectors *pvs = ai_script_game_get_cluster_pvs();
    clump_actor_cursor iterator;
    squad_cursor_new(&iterator);
    squad_datum *squad;
    while ((squad = squad_cursor_next(&iterator)) != 0) {
        bool active = false;
        if (squad->count0 > 0) {
            active = (squad->flags & 0x200) != 0;
            if (!active || squad->last_activation_time == NONE)
                active |= ai_script_squad_test_actor_activation(iterator.index, pvs);
            if (active)
                squad->inactive_ticks = 0;
            else
                squad->inactive_ticks += (short)ticks;
            if (!active) {
                if (squad->inactive_ticks < 20 * game_time_globals->ticks_per_second && ::squad_valid_task(squad)) {
                    squad_area_iterator area_iterator;
                    squad_area_iterator_new(&area_iterator, (short)iterator.index, 7, 3, false, _pureform_none);
                    while (squad_area_iterator_next(&area_iterator) != NONE && !active) {
                        scenario *scenario = (struct scenario *)global_scenario;
                        zone_definition *zone = &((zone_definition *)scenario->zones.address)[(unsigned short)area_iterator.zone_index];
                        short structure_bsp_index = zone->structure_bsp_index;
                        if (structure_bsp_index != NONE && global_structure_bsp_is_active(structure_bsp_index)) {
                            area_definition *area = &((area_definition *)zone->areas.address)[area_iterator.area_index];
                            const unsigned long *cluster_bits = pvs->bit_vectors[structure_bsp_index].get_bits_direct();
                            structure_bsp *structure_bsp = global_structure_bsp_get(structure_bsp_index);
                            active |= ai_script_bit_vector_and(structure_bsp->clusters.count, cluster_bits, area->cluster_bits);
                        }
                    }
                }
            }
        }
        if (active) {
            squad->activation_ticks = (short)game_seconds_to_ticks_round(5.0f);
            if (!(squad->flags & 0x80))
                squad_activate(iterator.index);
        } else if ((squad->flags & 0x80) && squad->activation_ticks > ticks) {
            squad->activation_ticks -= (short)ticks;
        } else if (squad->flags & 0x80) {
            squad_deactivate(iterator.index);
        }
    }
    long actor_index = ai_globals->first_actor_index;
    long time = game_time_globals->time;
    while (actor_index != NONE) {
        actor_datum *actor = (actor_datum *)(actor_data->data + (unsigned short)actor_index * sizeof(actor_datum));
        bool active = false;
        long current_actor_index = actor_index;
        actor_index = actor->next_squad_actor_index;
        if (actor_runtime_within_structure_bsp(current_actor_index)) {
            active = actor_test_activation_cluster_bit_vector(current_actor_index, pvs);
            if (active) {
                actor->attach_time = time;
            }
            else if (actor->active && time - actor->attach_time < game_seconds_to_ticks_round(15.0f) &&
                script_actor_path_executing(current_actor_index))
            {
                real_point3d point;
                s_location location;
                ai_point_get_position(&actor->path.destination, &point);
                ai_find_location(&point, &location);
                if (location.cluster_reference.bsp_index != NONE)
                    active = game_cluster_bit_vector_test(pvs, location.cluster_reference.bsp_index, location.cluster_reference.cluster_index);
            }
        }
        actor_set_active(current_actor_index, active);
    }
}
