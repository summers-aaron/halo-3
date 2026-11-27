// ailibxbox/ai_vehicles.obj: from the records 0xae098 0xb5580 0xb56d0 0xb5870 0xb5978 0xb5a38 0x519360 0x519440 0x519570 0x519790 0x519920 0x519cf8 0x519e98 0x51a030 0x51a178 0x51a2d0 0x51a388 0x51a480 0x51a4f8 0x51a6d8 0x51a838 0x51a9d8 0x51acf0 0x51ae28 0x51aef8 0x51af98 0x51b100 0x51b1d8 0x51b3b8 0x51b420 0x51b4b8 0x51b568
#include "tag_files/tag_groups.h"
#include "memory/data.h"
#include "units/unit_seat_flags.h"
#include "units/unit_seat_helpers.h"
#include "game/game_time_inline.h"
#include "game/game_time_inlines.h"
#include "game/game_globals_inline.h"
#include "ai/ai_dialogue.h"
#include "math/real_math_constants.h"
#include "ai/actor_queries.h"
#include "ai/ai_information_data.h"
#include "ai/ai_player_types.h"
#include "units/unit_seat.h"
#include "units/vehicle_datum.h"
#include <string.h>
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "objects/object_datum.h"
#include "memory/data_types.h"
#include "tag_files/tag_types.h"
#include "game/game_time_types.h"
#include "game/game_types.h"
#include "game/game_globals_storage.h"
#include "units/vehicle_types.h"
#include "units/unit_passenger_iterator.h"
#include "units/motor_enums.h"
#include "ai/prop_ref_datum.h"
#include "ai/prop_state.h"
#include "ai/actor_globals.h"
#include "ai/squad_types.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include "ai/actors.h"
#include "objects/objects.h"
#include "units/units.h"
#include "ai/actor_dynamic_firing_position.h"
#include "ai/actor_perception.h"
#include "ai/ai_player.h"
#include "ai/ai_vehicles.h"
#include "ai/ai_vehicle_enterable.h"
#include "ai/prop_dialogue.h"
#include "ai/props.h"
#include "ai/squads.h"
#include "units/vehicles.h"
#include "ai/character_definitions.h"
#include "ai/character_properties_types.h"
#include "game/game_allegiance.h"
#include "game/player_types.h"
#include "math/real_math_types.h"
#include "ai/seat_entry_info_types.h"
#include "ai/squad_array_types.h"
#include "cache/cache_files.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "ai/actor_datum.h"
#include "units/unit_definitions.h"
#include "units/unit_seat_source.h"
#include "ai/ai_player_tls.h"
#include "units/vehicle_definitions.h"
#include "ai/dialogue_event.h"
#include "math/real_constant_pool_ten.h"
#include "ai/objective_definitions.h"
// @flags /TP /O2
// @rdata 0x82033bc0
static const float k_ai_vehicle_maximum_pickup_speed_squared = 0.25f;
// @rdata 0x820224f4
static const float k_ai_vehicle_seat_close_distance = 1.1f;
// @rdata 0x82022514
static const float k_ai_vehicle_seat_can_enter_dot = 0.6f;
// @rdata 0x8202251c
static const float k_ai_vehicle_seat_zero = 0.0f;
// @rdata 0x82022db8
static const float k_ai_vehicle_seat_aligned_distance = 0.4f;
// @address 0x51acf0 ?ai_vehicle_reserve_seat@@YA_NJJJM@Z
// @address 0x51a178 ?actor_attach_to_seat@@YAXJJFM_N@Z
// @address 0x51ae28 ?ai_vehicle_cancel_reservation@@YA_NJ@Z
// @address 0x519cf8 ?actor_handle_vehicle_detach@@YAXJJJF@Z
// @address 0x3548e8 ?vehicle_get_driver_seat@@YAFJPAJ@Z
// @address 0x7cff0c ?global_tag_instances@@3PBUcache_file_tag_instance@@B
// @address 0x9a080 ?object_get_origin@@YAPATreal_point3d@@JPAT1@@Z
// @address 0x9b308 ?normalize3d@@YAMPATreal_vector3d@@@Z
// @address 0x9ca90 ?game_team_is_ally@@YA_NW4e_game_team@@0@Z
// @address 0xa1ad0 ?vehicle_is_flipped@@YA_NJ@Z
// @address 0xa9548 ?vehicle_get_type@@YA?AW4e_vehicle_type@@J@Z
// @address 0xae150 ?character_definition_movement_properties_get@@YAPAUcharacter_movement_properties@@J@Z
// @address 0xae678 ?normalize2d@@YAMPATreal_vector2d@@@Z
// @address 0xaf9f0 ?prop_state_get@@YAPAUprop_state@@PBUprop_ref_datum@@@Z
// @address 0xbbee0 ?actor_unit_control_crouch@@YAXJ_N@Z
// @address 0x5006f0 ?actor_set_active@@YA_NJ_N@Z
// @address 0x55a1b8 ?actor_dynamic_firing_set_support_object@@YAXJJ@Z
// @address 0x9c9f8 ?game_team_is_enemy@@YA_NW4e_game_team@@0@Z
// @address 0xd9170 ?unit_get_best_seat_entrance_animation@@YAJJJFPATreal_point3d@@0@Z
// @address 0x51a838 ?ai_unit_get_seat_score@@YAMJJFM_N@Z

struct unit_seat_source;
struct s_task;
struct seat_entry_info;
enum e_unit_seat_flags;

extern "C" {
struct s_data_array;
}

namespace {
static __forceinline vehicle_datum *ai_vehicle_unit_get(long unit_index)
{
    return (vehicle_datum *)object_header_get(unit_index)->datum;
}
static __forceinline const unit_definition *ai_vehicle_definition_get(long definition_index)
{
    return (const unit_definition *)tag_get(0, definition_index);
}
static __forceinline bool ai_vehicle_flag_test(unsigned long flags, int bit)
{
    bool result = (flags & (1 << bit)) != 0;
    return result;
}
static __forceinline bool ai_vehicles_flag_test_rotl(unsigned long flags, int bit)
{
    bool result = (bool)(_rotl(flags, 32 - bit) & 1);
    return result;
}
}

extern "C" {
}
namespace {
const float ai_vehicles_attach_delay = 1.0f;
const float ai_vehicles_force_attach_delay = 0.5f;
const float ai_vehicles_pickup_wait_delay = 2.0f;
const float ai_vehicles_reservation_expire_delay = 5.0f;
}


// @address 0xb56d0 ?ai_vehicle_occupied_by_enemy@@YA_NJW4e_game_team@@@Z
bool ai_vehicle_occupied_by_enemy(long vehicle_index, e_game_team team)
{
    bool result = false;
    c_multi_unit_passenger_iterator iterator;
    iterator.begin(vehicle_index);
    while (iterator.next()) {
        vehicle_datum *passenger = ai_vehicle_unit_get(iterator.m_passenger_index);
        if ((passenger->unit.actor_index != -1 || passenger->unit.player_index != -1)
            && (unsigned char)game_team_is_enemy((e_game_team)passenger->unit.team, team)) {
            short seat_index = passenger->unit.parent_seat_index;
            vehicle_datum *parent = ai_vehicle_unit_get(iterator.m_parent_index);
            const unit_definition *definition = ai_vehicle_definition_get(parent->object.definition_index);
            if (seat_index >= 0 && seat_index < definition->seats.s_tag_block::count) {
                const unit_seat *seat = &((unit_seat *)definition->seats.address)[iterator.m_seat_index];
                if (!ai_vehicle_flag_test(seat->flags, 11)) {
                    result = true;
                    break;
                }
            }
        }
    }
    return result;
}

long ai_get_root_vehicle(long vehicle_index)
{
    long current = vehicle_index;
    if ((int)vehicle_index != -1) {
        do {
            object_datum *object = object_header_get(current)->datum;
            bool linked = (object->flags >> 26) & 1;
            if (!linked) {
                break;
            }
            current = object->parent_object_index;
        } while (current != 0xffffffff);
    }
    if (object_header_get(current)->object_type != 1) {
        return vehicle_index;
    }
    return current;
}

short ai_vehicle_get_num_flip_actors(long object_index)
{
  object_datum *object = object_header_get(object_index)->datum;
  vehicle_definition *definition = (vehicle_definition *)tag_get(0, object->definition_index);
  return definition->havok_vehicle_physics.size > 0 ? 2 : 1;
}


struct seat_entry_info;



static __forceinline short ai_vehicles_player_index_get(long player_index)
{
    short result = -1;
    for (long index = 0; index < 4; ++index) {
        if (tls_ai_players()[index].id == player_index) {
            result = (short)index;
            break;
        }
    }
    return result;
}
// @address 0xd9308 ?unit_has_seat_entrance_animation@@YA_NJJF@Z

// @address 0xb5978 ?ai_unit_seat_allowed@@YA_NJJFW4e_game_team@@@Z
bool ai_unit_seat_allowed(long actor_index, long unit_index, short seat_index, e_game_team team)
{
    bool result = false;
    if (!ai_unit_seat_disallowed(actor_index, unit_index, seat_index, team)) {
        long current_unit_index = datum_get<actor_datum>(tls_actor_data(), actor_index)->unit_index;
        if (current_unit_index != -1) {
            vehicle_datum *current_unit = ai_vehicle_unit_get(current_unit_index);
            if (seat_index == current_unit->unit.parent_seat_index && unit_index == current_unit->object.parent_object_index) {
                result = true;
            } else if (unit_has_seat_entrance_animation(current_unit_index, unit_index, seat_index)) {
                result = true;
            }
        }
    }
    return result;
}

// @address 0xb5a38 ?ai_unit_seat_disallowed@@YA_NJJFW4e_game_team@@@Z
bool ai_unit_seat_disallowed(long actor_index, long unit_index, short seat_index, e_game_team team)
{
    bool result = false;
    long actor_unit_index = -1;
    long occupant_index = -1;
    actor_datum *actor = 0;
    vehicle_datum *unit = ai_vehicle_unit_get(unit_index);
    if (ai_vehicles_flag_test_rotl(unit->object.damage_flags, 2)) {
        result = true;
    } else {
        if (seat_index >= 0 && seat_index < 64
            && object_header_get(unit_index)->object_type == 1)
            occupant_index = ::unit_seat_get_occupant(unit_index, seat_index);
        if (!game_is_campaign() || !(unsigned char)game_team_is_enemy(team, _game_team_player)) {
            for (short player_index = 0; player_index < 4; ++player_index) {
                ai_player_state *state = &tls_ai_players()[player_index];
                if (state->id != -1 && state->unit_index == unit_index
                    && state->squad_index == seat_index && (short)state->flags > 0) {
                    player_datum *player = datum_get<player_datum>(tls_player_data(), state->id);
                    if (!(unsigned char)game_team_is_enemy(team, (e_game_team)player->configuration.host.team_index)) {
                        result = true;
                        goto done;
                    }
                }
            }
            if (occupant_index != -1 && ai_vehicle_unit_get(occupant_index)->unit.player_index != -1) {
                result = true;
                goto done;
            }
        }
        if (actor_index != -1) {
            actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
            actor_unit_index = actor->unit_index;
            if ((short &)actor->destination_seat_index == seat_index && actor->destination_unit_index == unit_index
                && actor->vehicle_desire >= 3 && (actor->vehicle_entry_flags & 4))
                goto done;
        }
        if (seat_index >= 0 && seat_index < 64
            && object_header_get(unit_index)->object_type == 1) {
            vehicle_datum *vehicle = (vehicle_datum *)unit;
            if (vehicle->vehicle.locked_seats[seat_index >> 5] & (1 << (seat_index & 31))) {
                result = true;
                if (actor && (short &)actor->destination_seat_index == seat_index && actor->destination_unit_index == unit_index)
                    result = false;
            }
            if (!result) {
                long reservation_index = ::ai_vehicle_get_reservation(unit_index, seat_index, team);
                if (occupant_index != -1 && occupant_index != actor_unit_index) {
                    result = true;
                    goto done;
                }
                else if (reservation_index != -1 && reservation_index != actor_index) {
                    actor_datum *reservation_actor = datum_get<actor_datum>(tls_actor_data(), reservation_index);
                    if (reservation_actor->vehicle_desire >= 3) {
                        result = true;
                        goto done;
                    }
                }
            }
        }
        if (!result && actor && actor->seat_reentry_timer > 0
            && actor->exited_vehicle_index == unit_index && actor->exited_seat_index == seat_index)
            result = true;
    }
done:
    return result;
}

// @address 0x519440 ?actor_handle_seat_exit@@YAXJJFPAW4e_motor_program@@@Z
void actor_handle_seat_exit(long actor_index, long vehicle_index, short seat_index, e_motor_program *motor_program)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if ((short &)actor->destination_seat_index == seat_index && actor->destination_unit_index == vehicle_index) {
        actor->vehicle_desire = 2;
        actor->reserved_ticks = 0;
        actor->vehicle_entry_flags &= ~4;
    }
    if (motor_program)
        *motor_program = _motor_program_none;
    if (object_header_get(vehicle_index)->object_type == 1) {
        actor_unit_control_crouch(actor_index, false);
        if (motor_program && vehicle_get_type(vehicle_index) == 2) {
            character_movement_properties *movement = character_definition_movement_properties_get(actor->character_index);
            if (movement && actor->unit_index != -1
                && object_header_get(actor->unit_index)->object_type == 0
                && (movement->flags & 0x200) != 0)
                *motor_program = _motor_program_flying;
        }
    }
}



// @address 0x519360 ?actor_handle_seat_exit@@YAXJJF@Z
void actor_handle_seat_exit(long actor_index, long vehicle_index, short seat_index)
{
    if (tls_actor_globals()->enabled
        && object_header_get(vehicle_index)->object_type == 1) {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        if (!::unit_seat_test_flag(vehicle_index, seat_index, _unit_seat_boarding_seat_bit)) {
            long root_vehicle = ai_get_root_vehicle(vehicle_index);
            if ((short)ai_vehicle_count_nonboarding_seats(root_vehicle, 0) > 1)
                actor_dialogue_event(actor_index, 58, root_vehicle, 0);
        }
        actor_unit_control_crouch(actor_index, false);
        memset(&actor->vehicle_turn, 0, sizeof(actor->vehicle_turn));
        actor->vehicle_turn.index = -1;
    }
}

// @address 0x22850 literal_10_0f
struct character_vehicle_properties;
// @address 0xb6668 ?character_vehicle_properties_get@@YAPAUcharacter_vehicle_properties@@JJ@Z
// @address 0x54e4f8 ?squad_adoption_distance_to_actor@@YAMJJF@Z

namespace {
}

// @address 0x519cf8 ?actor_handle_vehicle_detach@@YAXJJJF@Z
void actor_handle_vehicle_detach(long actor_index, long vehicle_index, long seat_reservation_index, short seat_index)
{
    if (!tls_actor_globals()->enabled)
        return;
    object_header_datum *entry = object_header_get(vehicle_index);
    if (entry->object_type != 1)
        return;
    vehicle_datum *vehicle = (vehicle_datum *)entry->datum;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (vehicle_get_type(vehicle_index) != 5 || vehicle->object.parent_object_index != -1) {
        character_vehicle_properties *properties =
            (character_vehicle_properties *)character_vehicle_properties_get(actor_index, vehicle->object.definition_index);
        if (properties != 0 && actor->original_squad_index != -1
            && (properties->flags & 1) != 0) {
            actor_change_squad(actor_index, (short)actor->original_squad_index);
        } else if (actor->original_squad_index != -1
                   && squad_adoption_distance_to_actor(actor->original_squad_index, actor_index, 0)
#if defined(_XBOX) && !defined(__clang__)
                          // match: 0x519cf8: 10.0f loads off the 0x82022850 GPR base (literal: fr31, savegpr 24->25)
                          < literal_10_0f[0])
#else
                          < 10.0f)
#endif
        {
            actor_change_squad(actor_index, (short)actor->original_squad_index);
        } else if (vehicle->vehicle.squad_index == -1
                   || squad_adoption_distance_to_actor(vehicle->vehicle.squad_index, actor_index, 0)
#if defined(_XBOX) && !defined(__clang__)
                          >= literal_10_0f[0])
#else
                          >= 10.0f)
#endif
        {
            actor->seeking_squad = true;
            actor->squad_seek_attempts = 0;
            actor->squad_seek_ticks = (short)game_seconds_integer_to_ticks(2);
            actor_dynamic_firing_set_support_object(actor_index, vehicle_index);
        }
    }
    if (!actor->seeking_squad)
        actor->original_squad_index = -1;
    actor->exited_seat_index = seat_index;
    actor->exited_vehicle_index = seat_reservation_index;
    actor->detach_timer = (short)game_seconds_integer_to_ticks(15);
}

// @address 0x519e98 ?ai_handle_driver_change@@YAXJ@Z
void ai_handle_driver_change(long unit_index)
{
    if (tls_actor_globals()->enabled) {
        vehicle_datum *vehicle = ai_vehicle_unit_get(unit_index);
        long child_index = vehicle->object.first_child_object_index;
        while (child_index != -1) {
            vehicle_datum *child = ai_vehicle_unit_get(child_index);
            if (((1 << child->object.object_type) & 0x1003) != 0 && child->unit.parent_seat_index == -1) {
                const unit_definition *definition = ai_vehicle_definition_get(child->object.definition_index);
                if (definition->spawned_actor.index != -1) {
                    if (vehicle->unit.driver_object_index != -1) {
                        vehicle_datum *controller = ai_vehicle_unit_get(vehicle->unit.driver_object_index);
                        if (child->unit.actor_index != -1
                            && datum_get<actor_datum>(tls_actor_data(), child->unit.actor_index)->team != controller->unit.team)
                            ::actor_delete(child->unit.actor_index, false);
                        if (child->unit.actor_index == -1) {
                            short squad_index = -1;
                            if (controller->unit.actor_index != -1)
                                squad_index = (short)datum_get<actor_datum>(tls_actor_data(), controller->unit.actor_index)->squad_index;
                            long actor_index = ::actor_create(child_index, definition->spawned_actor.index, (short)squad_index, false);
                            if (actor_index != -1) {
                                actor_set_active(actor_index, true);
                                actor_set_team(actor_index, (short)controller->unit.team);
                            }
                        }
                    } else if (child->unit.actor_index != -1) {
                        ::actor_delete(child->unit.actor_index, false);
                    }
                }
            }
            child_index = child->object.next_object_index;
        }
    }
}

void actor_attach_to_seat(long actor_index, long vehicle_index, short seat_index, float timeout, bool instant)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (vehicle_index == actor->destination_unit_index && seat_index == (short &)actor->destination_seat_index)
    {
        if (vehicle_index != -1 && seat_index != -1)
        {
            if (instant)
            {
                ::actor_handle_seat_attach(actor_index, vehicle_index, seat_index);
                actor->vehicle_desire = 3;
                actor->reserved_distance = FLT_MAX;
            }
            if (instant)
                actor->vehicle_entry_flags |= 4;
            else
                actor->vehicle_entry_flags &= ~4;
        }
    }
    else
    {
        ai_vehicle_cancel_reservation(actor_index);
        actor->destination_unit_index = -1;
        (short &)actor->destination_seat_index = -1;
        if (seat_index != -1)
        {
            ai_vehicle_reserve_seat(actor_index, vehicle_index, seat_index, timeout);
            if (instant)
            {
                ::actor_handle_seat_attach(actor_index, vehicle_index, seat_index);
                actor->vehicle_desire = 3;
                actor->reserved_distance = FLT_MAX;
                actor->reserved_timer = 0;
            }
            if (instant)
                actor->vehicle_entry_flags |= 4;
            else
                actor->vehicle_entry_flags &= ~4;
        }
    }
}

// @address 0x51a030 ?actor_attach_to_vehicle@@YAXJJ_N@Z
void actor_attach_to_vehicle(long actor_index, long vehicle_index, bool force)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (vehicle_index != actor->attached_vehicle_index) {
        float attach_delay = force ? ai_vehicles_force_attach_delay : ai_vehicles_attach_delay;
        long old_vehicle_index = actor->attached_vehicle_index;
        long old_seat_reservation_index = actor->destination_unit_index;
        short old_seat_index = (short &)actor->destination_seat_index;

        ai_vehicle_cancel_reservation(actor_index);
        if (actor->support_object_index == actor->attached_vehicle_index)
            actor_dynamic_firing_set_support_object(actor_index, -1);

        actor->reserved_ticks = 0;
        actor->attach_timer = (short)game_seconds_to_ticks_round(attach_delay);
        actor->attached_vehicle_index = vehicle_index;
        actor->destination_unit_index = -1;
        (short &)actor->destination_seat_index = -1;
        actor->vehicle_entry_flags = 0;
        if (force)
            actor->vehicle_entry_flags |= 4;

        if (vehicle_index == -1) {
            if (actor->vehicle_desire >= 2)
                actor_handle_vehicle_detach(actor_index, old_vehicle_index, old_seat_reservation_index, old_seat_index);
            actor->vehicle_desire = 0;
        } else {
            actor->vehicle_desire = 1;
            actor->exit_timer = 0;
        }
    }
}

// @address 0x51a2d0 ?ai_vehicle_choose_seat@@YA_NJ@Z
bool ai_vehicle_choose_seat(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    unit_seat_source seats[64];
    short seat_count = unit_get_all_seats(actor->attached_vehicle_index, seats, 64);
    seat_entry_info entry;
    bool found = ai_vehicle_find_seat(actor_index, actor->attached_vehicle_index, seats, seat_count, &entry);
    if (found) {
        actor_attach_to_seat(actor_index, entry.unit_index, entry.seat_index, entry.score, false);
    } else if ((unsigned short)(short &)actor->destination_seat_index != 0xffff) {
        actor_attach_to_seat(actor_index, -1, -1, 0.0f, false);
    }
    return found;
}

// @address 0x51a388 ?ai_vehicle_lock_actors@@YAXJW4e_game_team@@@Z
void ai_vehicle_lock_actors(long actor_index, e_game_team team)
{
    unit_seat_source seats[64];
    short seat_count = unit_get_all_seats(actor_index, seats, 64);
    for (short seat_index = 0; seat_index < seat_count; ++seat_index) {
        const unit_seat *seat = (const unit_seat *)seats[seat_index].definition;
        if (!ai_vehicles_flag_test_rotl(seat->flags, 11)) {
            short local_seat = seats[seat_index].seat_index;
            long seat_unit_index = seats[seat_index].unit_index;
            long reservation = ::ai_vehicle_get_reservation(seat_unit_index, local_seat, team);
            if (reservation != -1) {
                actor_datum *reserved_actor = datum_get<actor_datum>(tls_actor_data(), reservation);
                if (reserved_actor->vehicle_desire == 1) {
                    actor_handle_seat_attach(reservation, seat_unit_index, local_seat);
                    short reserved_flags = reserved_actor->vehicle_entry_flags;
                    reserved_actor->vehicle_desire = 3;
                    reserved_actor->reserved_timer = 0;
                    reserved_actor->vehicle_entry_flags = reserved_flags & ~2;
                }
            }
        }
    }
}

// @address 0x51a480 ?ai_vehicle_check_seat@@YA_NJPAUseat_entry_info@@@Z
bool ai_vehicle_check_seat(long actor_index, seat_entry_info *seat_entry)
{
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    memset(seat_entry, 0, sizeof(*seat_entry));
    if ((unsigned short)(short &)actor->destination_seat_index != 0xffff)
        result = ::ai_vehicle_evaluate_seat(actor_index, actor->destination_unit_index, (short &)actor->destination_seat_index, false, seat_entry);
    return result;
}

// @address 0x51a4f8 ai_vehicle_find_seat
// match: 0x51a4f8: pointer __restrict qualifier prevents alias reload
extern "C" bool ai_vehicle_find_seat(long actor_index, long unit_index, unit_seat_source *seats, short seat_count, seat_entry_info * __restrict info)
{
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if (actor->root_vehicle_index != -1) {
        if (actor->root_vehicle_index == unit_index && actor->unit_index != -1) {
            vehicle_datum *unit = ai_vehicle_unit_get(actor->unit_index);
            info->preference = 1.0f;
            info->can_enter = true;
            info->score = 1.0f;
            info->aligned = true;
            info->approach = actor->facing_vector;
            info->close = true;
            info->entrance = actor->position.body;
            info->seat_index = unit->unit.parent_seat_index;
            info->unit_index = unit->object.parent_object_index;
            result = true;
        }
    } else {
        seat_entry_info best;
        best.preference = 0.0f;
        best.score = 0.0f;
        for (short i = 0; i < seat_count; ++i) {
            seat_entry_info candidate;
            const unit_seat *seat = (const unit_seat *)seats[i].definition;
            if (ai_vehicles_flag_test_rotl(seat->flags, 11))
                continue;
            if (!::ai_vehicle_evaluate_seat(actor_index, seats[i].unit_index, seats[i].seat_index, false, &candidate))
                continue;
            if (candidate.preference <= 0.0f)
                continue;
            float candidate_score = candidate.score;
            if (candidate_score <= best.score)
                continue;
            if (candidate.preference < best.preference)
                continue;
            best = candidate;
            result = true;
        }
        if (result)
            *info = best;
    }
    return result;
}

// 0x51acf0
bool ai_vehicle_reserve_seat(long actor_index, long vehicle_index, long seat_index, float distance)
{
    bool result = false;
    if (seat_index >= 0 && seat_index < 64) {
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        vehicle_datum *vehicle = (vehicle_datum *)object_header_get(vehicle_index)->datum;
        bool ally = actor->team == _game_team_player || game_team_is_ally((e_game_team)actor->team, _game_team_player);
        long reserved = ::ai_vehicle_get_reservation(vehicle_index, seat_index, (e_game_team)actor->team);
        if (reserved != -1) {
            actor_datum *previous = datum_get<actor_datum>(tls_actor_data(), reserved);
            ai_vehicle_cancel_reservation(reserved);
            (short &)previous->destination_seat_index = -1;
            previous->destination_unit_index = -1;
            if (previous->vehicle_desire >= 2) {
                previous->vehicle_desire = 2;
                previous->vehicle_entry_flags = previous->vehicle_entry_flags & ~4;
                previous->reserved_ticks = 0;
            }
        }
        result = true;
        vehicle->vehicle.reservations[(!ally) * 64 + seat_index] = actor_index;
        actor->reserved_distance = distance;
        (short &)actor->destination_seat_index = (short)seat_index;
        actor->destination_unit_index = vehicle_index;
    }
    return result;
}

// 0x51ae28
bool ai_vehicle_cancel_reservation(long actor_index)
{
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    short seat_index = (short &)actor->destination_seat_index;
    if (seat_index >= 0 && seat_index < 64) {
        vehicle_datum *vehicle = (vehicle_datum *)object_header_get(actor->destination_unit_index)->datum;
        bool ally = actor->team == _game_team_player || game_team_is_ally((e_game_team)actor->team, _game_team_player);
        vehicle->vehicle.reservations[(!ally) * 64 + seat_index] = -1;
        result = true;
    }
    return result;
}



// @address 0x51a6d8 ai_unit_get_seat_preference
float ai_unit_get_seat_preference(int object_index, short slot_index, unsigned char a, unsigned char b, unsigned char c)
{
    object_header_datum *header = object_header_get(object_index);
    object_datum *object = header->datum;
    unit_definition *definition = (unit_definition *)tag_get(0, object->definition_index);
    const unit_seat *seat = &((const unit_seat *)definition->seats.address)[slot_index];
#if defined(_XBOX) && !defined(__clang__)
    // match: 0x51a6d8: seat weights load off the k_real_constant_pool base held in r29 (literals: fr31 spill, savegpr 24->25)
    float base = k_real_constant_pool[200];
#else
    float base = 0.0f;
#endif
    if (vehicle_get_type(object_index) == _vehicle_type_turret && ((unsigned char)((seat->flags >> 3) & 1)) != 0)
#if defined(_XBOX) && !defined(__clang__)
        return k_real_constant_pool[2193];
#else
        return 9.0f;
#endif
    unsigned int flags = seat->flags;
    if (((unsigned char)((flags >> 2) & 1)) != 0) {
        if (object->object_type == 1 && ((unsigned char)((((const vehicle_definition *)definition)->flags >> 5) & 1)) == 0)
            return base;
        if (((unsigned char)((flags >> 3) & 1)) == 0 && b == 0)
#if defined(_XBOX) && !defined(__clang__)
            return k_real_constant_pool[2102];
#else
            return 3.0f;
#endif
#if defined(_XBOX) && !defined(__clang__)
        return k_real_constant_pool[2074];
#else
        return 5.0f;
#endif
    }
    if (((unsigned char)((flags >> 3) & 1)) != 0) {
        if (a != 0) {
            if (c != 0)
#if defined(_XBOX) && !defined(__clang__)
                return k_real_constant_pool[8170];
#else
                return 4.0f;
#endif
#if defined(_XBOX) && !defined(__clang__)
            return k_real_constant_pool[405];
#else
            return 10.0f;
#endif
        }
        if (b == 0)
            return base;
#if defined(_XBOX) && !defined(__clang__)
        return k_real_constant_pool[2102];
#else
        return 3.0f;
#endif
    }
    if (((unsigned char)((flags >> 11) & 1)) == 0) {
        if (a == 0)
            return base;
        if (c != 0)
#if defined(_XBOX) && !defined(__clang__)
            return k_real_constant_pool[624];
#else
            return 15.0f;
#endif
#if defined(_XBOX) && !defined(__clang__)
        return k_real_constant_pool[0];
#else
        return 1.0f;
#endif
    }
    return base;
}

// @address 0x51aef8 ?ai_vehicle_get_reservation@@YAJJJW4e_game_team@@@Z
long ai_vehicle_get_reservation(long vehicle_index, long seat_index, enum e_game_team team)
{
    int result = -1;
    bool ally;
    if (((unsigned int)seat_index) <= 63)
    {
        ally = ((int)team) == _game_team_player || game_team_is_ally((e_game_team)((int)team), _game_team_player);
        vehicle_datum *vehicle = (vehicle_datum *)object_header_get(vehicle_index)->datum;
        result = vehicle->vehicle.reservations[(!ally * 64) + ((unsigned int)seat_index)];
    }
    return result;
}

// @address 0x51b420 ?ai_vehicle_count_nonboarding_seats@@YAFJ_N@Z
short ai_vehicle_count_nonboarding_seats(long object_index, bool team)
{
  short count = 0;
  short entry_count = 0;
  unit_seat_source entries[64];

  ::unit_get_all_seats_internal(object_index, (unit_seat_source *)entries, (short *)(&entry_count), 64, false);
  for (short i = 0; i < entry_count; i++) {
    bool flag = (*(unsigned int *)entries[i].definition >> 11) & 1;
    if (!flag)
      count++;
  }
  return count;
}

// @address 0x51af98 ?ai_vehicle_handle_allegiance_change@@YAXJ@Z
void ai_vehicle_handle_allegiance_change(long actor_index)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    if ((unsigned short)(short &)actor->destination_seat_index != 0xffff) {
        bool ally = actor->team == _game_team_player || game_team_is_ally((e_game_team)actor->team, _game_team_player);
        short seat_index = (short &)actor->destination_seat_index;
        long unit_index = actor->destination_unit_index;
        vehicle_datum *vehicle = (vehicle_datum *)ai_vehicle_unit_get(unit_index);
        long ally_reservation = vehicle->vehicle.reservations[seat_index];
        long enemy_reservation = vehicle->vehicle.reservations[seat_index + 64];
        bool cleared = false;
        if (ally_reservation == actor_index) {
            if (!ally) {
                vehicle->vehicle.reservations[seat_index] = -1;
                cleared = true;
            }
        } else if (enemy_reservation == actor_index && ally) {
            vehicle->vehicle.reservations[seat_index + 64] = -1;
            cleared = true;
        }
        if (cleared) {
            actor->vehicle_desire = 1;
            actor->vehicle_entry_flags &= ~4;
            (short &)actor->destination_seat_index = -1;
            actor->destination_unit_index = -1;
            actor->reserved_ticks = 0;
            actor->exit_timer = 0;
            float attach_ticks = (float)tls_game_time_globals()->ticks_per_second * 0.5f;
            actor->attach_timer = (short)(long)(attach_ticks + 0.5f);
        }
    }
}
// @address 0x51b100 ?ai_vehicle_seats_available@@YA_NJJ@Z
bool ai_vehicle_seats_available(long actor_index, long vehicle_index)
{
    bool result = false;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    unit_seat_source seats[64];
    short seat_count = unit_get_all_seats(vehicle_index, seats, 64);
    for (short seat_index = 0; seat_index < seat_count; ++seat_index) {
        const unit_seat *seat = (const unit_seat *)seats[seat_index].definition;
        if (!ai_vehicles_flag_test_rotl(seat->flags, 11)) {
            short local_index = seats[seat_index].seat_index;
            long unit_index = seats[seat_index].unit_index;
            if (ai_unit_seat_allowed(actor_index, unit_index, local_index, (e_game_team)actor->team)) {
                result = true;
                break;
            }
        }
    }
    return result;
}

// @address 0x51b4b8 ?ai_vehicle_get_num_reservations@@YAFJW4e_game_team@@_N@Z
short ai_vehicle_get_num_reservations(long unit_index, e_game_team team, bool include_self)
{
    short count = 0;
    unit_seat_source seats[64];
    short seat_count = unit_get_all_seats(unit_index, seats, 64);
    for (short seat_index = 0; seat_index < seat_count; ++seat_index) {
        const unit_seat *seat = (const unit_seat *)seats[seat_index].definition;
        if (!ai_vehicles_flag_test_rotl(seat->flags, 11)) {
            short seat_index_value = seats[seat_index].seat_index;
            long seat_unit_index = seats[seat_index].unit_index;
            long reservation = ::ai_vehicle_get_reservation(seat_unit_index, seat_index_value, team);
            if (reservation != -1)
                ++count;
        }
    }
    // match: 0x51b4b8: value range assumption guides branch lowering
    __assume(team >= 0);
    return count;
}
// @address 0x51b1d8 ?ai_vehicle_turret_valid_angle@@YA_NJJJ@Z
bool ai_vehicle_turret_valid_angle(long actor_index, long vehicle_index, long prop_index)
{
    vehicle_datum *vehicle = ai_vehicle_unit_get(vehicle_index);
    const unit_definition *definition = ai_vehicle_definition_get(vehicle->object.definition_index);
    bool result = false;
    if (prop_index != -1 && vehicle_get_type(vehicle_index) == 5) {
        if (ai_vehicle_flag_test(definition->flags, 0))
            result = true;
        else if (vehicle->object.parent_object_index != -1) {
            vehicle_datum *parent = ai_vehicle_unit_get(vehicle->object.parent_object_index);
            if (((1 << parent->object.object_type) & 0x1003) != 0) {
                vehicle_datum *unit = parent;
                if (unit->unit.actor_index != -1 || unit->unit.driver_object_index != -1)
                    result = true;
            }
        }
        if (!result) {
            const unit_seat *gunner_seat = 0;
            result = true;
            for (short seat_index = 0; seat_index < definition->seats.s_tag_block::count; ++seat_index) {
                const unit_seat *seat = &((unit_seat *)definition->seats.address)[seat_index];
                if (ai_vehicle_flag_test(seat->flags, 3))
                    gunner_seat = seat;
            }
            if (gunner_seat) {
                prop_ref_datum *prop_ref = datum_get<prop_ref_datum>(tls_prop_ref_data(), prop_index);
                prop_state *state = prop_state_get(prop_ref);
                real_point3d origin;
                object_get_origin(vehicle_index, &origin);
                real_vector3d forward;
                ::object_get_orientation(vehicle_index, &forward, 0);
                forward.k = 0.0f;
                if (normalize3d(&forward) > 0.0f) {
                    real_vector3d direction;
                    const real_point3d *target = &state->position;
                    const real_point3d * source = &origin;
                    vector_from_points3d(source, target, &direction);
                    direction.k = 0.0f;
                    if (normalize3d(&direction) > 0.0f) {
                        float angle = signed_angle_between_vectors2d(&forward.xy, &direction.xy);
                        if (angle <= gunner_seat->left_aiming_angle || angle > gunner_seat->right_aiming_angle)
                            result = false;
                    }
                }
            }
        }
    }
    return result;
}

// the driver's seat reservation; the image places it under its record's symbol
// @address 0x51b3b8 ?ai_vehicle_get_driver@@YAJJW4e_game_team@@@Z
long ai_vehicle_get_driver(long unit_index, enum e_game_team _sym_p1)
{
    int result = -1;
    long seat_object = -1;
    int vehicle = ai_get_root_vehicle(unit_index);
    short seat = vehicle_get_driver_seat(vehicle, &seat_object);
    if (seat_object != -1)
        result = ::ai_vehicle_get_reservation(seat_object, seat, (e_game_team)((int)_sym_p1));
    return result;
}



// This delta intentionally swaps x and y; only its magnitude is used.
static __forceinline real_vector3d *ai_vehicles_vector_from_points3d_swapped(const real_point3d *p0, const real_point3d *p1, real_vector3d *v)
{
    v->i = p1->y - p0->y;
    v->j = p1->x - p0->x;
    v->k = p1->z - p0->z;
    return v;
}
// @address 0xb5870 ai_vehicle_enterable
unsigned char ai_vehicle_enterable(unsigned short actor_index, int vehicle_index, float radius, unsigned char check_distance)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    bool result = true;
    if (vehicle_is_flipped(vehicle_index)) {
        result = 0;
    } else if (check_distance) {
        real_point3d origin;
        real_vector3d delta;
        object_get_origin(vehicle_index, &origin);
        if (magnitude_squared3d(ai_vehicles_vector_from_points3d_swapped(&actor->position.body, &origin, &delta)) > radius * radius) {
            result = 0;
        } else {
            object_datum *object = object_header_get(vehicle_index)->datum;
            if (magnitude_squared3d(&object->linear_velocity) > k_ai_vehicle_maximum_pickup_speed_squared)
                result = 0;
        }
    }
    return result;
}


// @address 0xb5580 ?ai_vehicle_desirable@@YA_NJJ_N0@Z
bool ai_vehicle_desirable(long actor_index, long vehicle_index, bool p2, bool p3)
{
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    vehicle_datum *vehicle = ai_vehicle_unit_get(vehicle_index);
    bool result = true;
    bool check_seats = p3;
    if (actor->root_vehicle_index == vehicle_index)
        check_seats = false;
    if (ai_vehicle_flag_test(vehicle->object.damage_flags, 2)) {
        result = false;
    } else if (ai_vehicle_occupied_by_enemy(vehicle_index, (e_game_team)actor->team)) {
        result = false;
    } else if (check_seats && !ai_vehicle_seats_available(actor_index, vehicle_index)) {
        result = false;
    } else if (p2 && vehicle_get_type(vehicle_index) == 5
               && !ai_vehicle_turret_valid_angle(actor_index, vehicle_index, actor->target_prop_index)) {
        result = false;
    } else if (actor->squad_index != -1) {
        squad_datum *squad = datum_get<squad_datum>(tls_squad_data(), actor->squad_index);
        s_task *task = ::squad_get_task(squad);
        if (task != 0)
            result = (task->vehicle_inhibit_flags & 4) == 0;
    }
    return result;
}

// @address 0x519920 ?actor_handle_seat_attach@@YA_NJJF@Z
bool actor_handle_seat_attach(long actor_index, long vehicle_index, short seat_index)
{
    bool result = false;
    if (tls_actor_globals()->enabled
        && object_header_get(vehicle_index)->object_type == 1) {
        bool is_driver = ::unit_seat_test_flag(vehicle_index, seat_index, _unit_seat_driver_bit);
        long root_vehicle_index = ai_get_root_vehicle(vehicle_index);
        actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
        vehicle_datum *vehicle = (vehicle_datum *)object_header_get(root_vehicle_index)->datum;
        result = true;
        if (actor->seeking_squad) {
            actor->seeking_squad = false;
        } else if (actor->squad_index != -1 && actor->original_squad_index == -1) {
            actor->original_squad_index = actor->squad_index;
        }
        if (actor->squad_index != vehicle->vehicle.squad_index) {
            if (vehicle_get_type(root_vehicle_index) != 5 || vehicle->object.parent_object_index != -1) {
                bool compatible = is_driver;
                bool other_flag = false;
                if (!compatible) {
                    short driver_seat = ::unit_get_driver_seat(root_vehicle_index);
                    long reservation = ::ai_vehicle_get_reservation(root_vehicle_index, driver_seat, (e_game_team)actor->team);
                    if (reservation != -1
                        && datum_get<actor_datum>(tls_actor_data(), reservation)->vehicle_desire >= 3)
                        compatible = true;
                }
                if (!compatible) {
                    long driver_index = vehicle->unit.driver_object_index;
                    if (driver_index != -1) {
                        vehicle_datum *driver = ai_vehicle_unit_get(driver_index);
                        if (driver->unit.player_index != -1
                            && !(unsigned char)game_team_is_enemy((e_game_team)actor->team, (e_game_team)driver->unit.team)) {
                            compatible = true;
                            other_flag = true;
                        }
                    }
                }
                if (compatible) {
                    squad_datum *squad = 0;
                    if (vehicle->vehicle.squad_index != -1)
                        squad = datum_get<squad_datum>(tls_squad_data(), vehicle->vehicle.squad_index);
                    if (squad != 0
                        && !(unsigned char)game_team_is_enemy((e_game_team)squad->team, (e_game_team)actor->team)) {
                        actor_change_squad(actor_index, (short)vehicle->vehicle.squad_index);
                    } else if (is_driver) {
                        if (actor->squad_index == -1 && actor->original_squad_index != -1)
                            actor_change_squad(actor_index, (short)actor->original_squad_index);
                        if (actor->squad_index != -1)
                            squad_vehicle_change_squad(root_vehicle_index, actor->squad_index);
                    } else if (!other_flag) {
                        if (squad == 0)
                            actor_change_squad(actor_index, -1);
                        else
                            result = false;
                    }
                }
            }
        }
        if (result) {
            actor->exited_seat_index = -1;
            actor->exited_vehicle_index = -1;
            actor->detach_timer = -1;
            actor->seat_attach_timer = (short)(long)((float)tls_game_time_globals()->ticks_per_second * 10.0f + 0.5f);
            if (is_driver) {
                unit_seat_source seats[64];
                short seat_count = unit_get_all_seats(root_vehicle_index, seats, 64);
                for (short i = 0; i < seat_count; ++i) {
                    const unit_seat *seat = (const unit_seat *)seats[i].definition;
                    if (!ai_vehicles_flag_test_rotl(seat->flags, 11)) {
                        short local_seat = seats[i].seat_index;
                        long seat_unit_index = seats[i].unit_index;
                        long reservation = ::ai_vehicle_get_reservation(seat_unit_index, local_seat, (e_game_team)actor->team);
                        if (reservation != -1) {
                            actor_datum *reserved = datum_get<actor_datum>(tls_actor_data(), reservation);
                            if (reserved->vehicle_desire >= 2 && reserved->squad_index != actor->squad_index)
                                actor_change_squad(reservation, (short)actor->squad_index);
                        }
                    }
                }
            }
        }
    }
    return result;
}




static __forceinline actor_datum *ai_vehicles_evaluate_actor_get(long index)
{
    return datum_get<actor_datum>(tls_actor_data(), index);
}

// @address 0x51a9d8 ?ai_vehicle_evaluate_seat@@YA_NJJF_NPAUseat_entry_info@@@Z
bool ai_vehicle_evaluate_seat(long actor_index, long unit_index, short seat_index, bool use_3d, seat_entry_info *info)
{
    actor_datum *actor = ai_vehicles_evaluate_actor_get(actor_index);
    // match: 0x51a9d8: pointer __restrict qualifier prevents alias reload
    seat_entry_info *__restrict evaluation = (seat_entry_info *)info;
    bool result = false;
    if (ai_unit_seat_allowed(actor_index, unit_index, seat_index, (e_game_team)actor->team)) {
        real_point3d start, entrance;
        if (unit_get_best_seat_entrance_animation(actor->unit_index, unit_index, seat_index, &start, &entrance) != -1) {
            float score = 0.0f;
            float distance = 0.0f;
            long root = ai_get_root_vehicle(unit_index);
            short driver_seat = ::unit_get_driver_seat(root);
            bool disallowed = ai_unit_seat_disallowed(-1, root, driver_seat, (e_game_team)actor->team);
            bool reserved = ai_vehicle_get_num_reservations(root, (e_game_team)actor->team, false) > ((short &)actor->destination_seat_index != -1);
            float preference = ai_unit_get_seat_preference(unit_index, seat_index, reserved || disallowed, reserved, false);
            if (use_3d) {
                float y = entrance.y - actor->position.body.y;
                float z = entrance.z - actor->position.body.z;
                float x = entrance.x - actor->position.body.x;
                distance = sqrtf(x * x + (z * z + y * y));
            } else {
                float x = entrance.x - actor->position.body.x;
                float y = entrance.y - actor->position.body.y;
                distance = sqrtf(y * y + x * x);
            }
            score = ai_unit_get_seat_score(actor_index, unit_index, seat_index, distance, false);
            long reservation = ::ai_vehicle_get_reservation(unit_index, seat_index, (e_game_team)actor->team);
            actor_datum *reservation_actor = 0;
            if (reservation != -1 && reservation != actor_index)
                reservation_actor = ai_vehicles_evaluate_actor_get(reservation);
            if (reservation == -1 || reservation == actor_index
                || score >= reservation_actor->reserved_distance - preference) {
                if (evaluation) {
                    real_vector3d approach;
                    approach.i = start.x - entrance.x;
                    approach.j = start.y - entrance.y;
                    approach.k = k_ai_vehicle_seat_zero;
                    if (normalize2d(&approach.xy) == k_ai_vehicle_seat_zero)
                        approach = actor->facing_vector;
                    float dot = dot_product2d(&approach.xy, &actor->facing_vector.xy);
                    evaluation->can_enter = dot > k_ai_vehicle_seat_can_enter_dot;
                    evaluation->aligned = distance < k_ai_vehicle_seat_aligned_distance;
                    evaluation->close = distance < k_ai_vehicle_seat_close_distance && dot > k_ai_vehicle_seat_zero;
                    evaluation->preference = preference;
                    evaluation->score = score + preference;
                    evaluation->approach = approach;
                    evaluation->entrance = entrance;
                    evaluation->seat_index = seat_index;
                    evaluation->unit_index = unit_index;
                }
                result = true;
            }
        }
    }
    return result;
}

// @address 0x519570 ai_player_handle_seat_enter
extern "C" void ai_player_handle_seat_enter(long player_index, long vehicle_index, short seat_index)
{
    if (tls_actor_globals()->enabled
        && object_header_get(vehicle_index)->object_type == 1) {
        ai_player_state *state = ai_player_state_get(player_index);
        if (state) {
            state->flags = 0;
            state->unit_index = -1;
            state->squad_index = -1;
        }
        if (!::unit_seat_test_flag(vehicle_index, seat_index, _unit_seat_boarding_seat_bit)
            && !::unit_seat_test_flag(vehicle_index, seat_index, _unit_seat_driver_bit)
            && !::unit_seat_test_flag(vehicle_index, seat_index, _unit_seat_gunner_bit)) {
            c_dialogue_event event(58, datum_get<player_datum>(tls_player_data(), player_index)->unit_index,
                vehicle_index, -1, -1, 0);
            ai_dialogue_event(&event);
        }
        unit_seat_source seats[64];
        long team = datum_get<player_datum>(tls_player_data(), player_index)->configuration.host.team_index;
        short count = unit_get_all_seats(ai_get_root_vehicle(vehicle_index), seats, 64);
        for (short index = 0; index < count; ++index) {
            short local_index = seats[index].seat_index;
            long unit_index = seats[index].unit_index;
            if (!ai_vehicles_flag_test_rotl(((unit_seat *)seats[index].definition)->flags, 11)) {
                long reservation = ::ai_vehicle_get_reservation(unit_index, local_index, (e_game_team)team);
                if (reservation != -1) {
                    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), reservation);
                    if (actor->vehicle_desire == 2)
                        actor->reserved_ticks = (short)game_seconds_to_ticks_round(ai_vehicles_reservation_expire_delay);
                }
            }
        }
    }
}

// @address 0x519790 ?ai_player_handle_seat_exit@@YAXJJF@Z
void ai_player_handle_seat_exit(long player_index, long vehicle_index, short seat_index)
{
    if (tls_actor_globals()->enabled
        && object_header_get(vehicle_index)->object_type == 1) {
        ai_player_state *state = ai_player_state_get(player_index);
        short player_slot = ai_vehicles_player_index_get(player_index);
        if (state) {
            state->unit_index = vehicle_index;
            state->squad_index = seat_index;
            state->flags = (short)game_seconds_to_ticks_round(3.0f);
            c_multi_unit_passenger_iterator iterator;
            iterator.begin(vehicle_index);
            while (iterator.next()) {
                long actor_index = ai_vehicle_unit_get(iterator.m_passenger_index)->unit.actor_index;
                if (actor_index != -1) {
                    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
                    actor->pickup_player = player_slot;
                    actor->pickup_wait_ticks = (short)game_seconds_to_ticks_round(ai_vehicles_pickup_wait_delay);
                    actor->pickup_state = 0;
                }
            }
        }
    }
}
// @address 0x51a838 ?ai_unit_get_seat_score@@YAMJJFM_N@Z
float ai_unit_get_seat_score(long actor_index, long vehicle_index, short seat_index, float distance, bool immediate)
{
    float score = 0.0f;
    bool immediate_value = immediate;
    actor_datum *actor = datum_get<actor_datum>(tls_actor_data(), actor_index);
    object_datum *vehicle = object_header_get(vehicle_index)->datum;
    unit_definition *definition = (unit_definition *)tag_get(0, vehicle->definition_index);
    unit_seat *seat = &((unit_seat *)definition->seats.address)[seat_index];
    if (immediate_value || distance <= 30.0f) {
        if (actor->destination_unit_index == vehicle_index && (short &)actor->destination_seat_index == seat_index)
            score += 100.0f;
        else if (actor->attached_vehicle_index == ai_get_root_vehicle(vehicle_index) && actor->vehicle_desire >= 3)
            return 0.0f;
        if (::actor_get_standing(actor_index) == 3 && unit_seat_test_flag(vehicle_index, seat_index, _unit_seat_leader_bit))
            score += 100.0f;
        if (immediate_value)
            score += 1.0f;
        else
            score += 10.0f / (distance + 1.0f);
        long unit_index = datum_get<actor_datum>(tls_actor_data(), actor_index)->unit_index;
        bool scary_weapon = false;
        if (unit_index != -1)
            scary_weapon = unit_has_scary_weapon(unit_index);
        if (scary_weapon) {
            bool gunner_seat = (seat->flags >> _unit_seat_gunner_bit) & 1;
            bool driver_seat = (seat->flags >> _unit_seat_driver_bit) & 1;
            if (!gunner_seat && !driver_seat)
                score *= 5.0f;
            else
                score *= 0.1f;
        }
    }
    return score;
}
