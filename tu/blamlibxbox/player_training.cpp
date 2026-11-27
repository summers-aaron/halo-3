// blamlibxbox/player_training.obj: from the records 0xa8508 0x118080 0x223c78 0x223d60 0x223dc8 0x223e18 0x223e88 0x223e90 0x223e98 0x2240a0 0x2242a0 0x224338 0x2243b8 0x225308 0x225d50 0x225eb8 0x225f78 0x2260d0 0x226170 0x226188
// @flags /TP /O2 /Oc
#include "interface/chud/chud_manager.h"
#include "game/player_training_tls.h"
#include "game/player_training_externs.h"
#include "units/unit_seat.h"
#include "game/game_time_inline.h"
#include "interface/chud/chud_messaging.h"
#include "game/game.h"
#include "interface/c_player_profile.h"
#include "interface/controller_types.h"
#include "memory/data_types.h"
#include "memory/bit_vectors.h"
#include "game/game_globals_storage.h"
#include "game/game_time_types.h"
#include "game/player_types.h"
#include "objects/object_datum.h"
#include "objects/damage_inline.h"
#include "memory/tls_globals.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "cache/restricted_memory_regions.h"
#include "game/game_allegiance.h"
#include "objects/objects.h"
#include "game/player_training.h"
#include "game/player_mapping_types.h"
#include "interface/chud/chud.h"
#include "interface/user_interface_text.h"
#include "items/weapons.h"
#include "units/vehicles.h"
#include "units/vehicle_definitions.h"
#include "interface/chud/chud_globals_definitions.h"
#include "units/unit_types.h"
#include "game/players.h"
#include "networking/online/voice_xenon.h"
#include "units/units.h"
#include "game/game_results_damage_type_description_types.h"
#include "items/weapon_interface_magazine_state_types.h"
#include "game/game_results_damage_breakdown_types.h"
#include "game/game_results.h"
#include "cseries/cseries.h"
// @address 0x223d60 ?player_training_profile_data_set_completion_count@@YAXPAUs_player_training_profile_data@@JJ@Z

struct player_action;
struct s_player_training_user;
struct s_player_training_entry_data;
struct s_player_training_state;
enum e_game_team;

#include <string.h>
#include <stdlib.h>
#include <ppcintrinsics.h>
#include <math.h>

#include "math/real_math.h"

#include "simulation/player_action.h"

#include "game/player_training_types.h"

#include "game/player_training_globals_types.h"

enum e_training_data_save
{
    _training_data_save_saved_game = 0,
    _training_data_save_old_map = 1
};

#include "cutscene/cinematic_globals_types.h"
#include "cutscene/cinematics.h"
#include "interface/chud/chud_motion_sensor.h"
#include "interface/chud/chud_user_data_types.h"
#include "units/unit_passenger_iterator_types.h"
#include "items/weapon_interface_state_types.h"

namespace {
struct s_player_training_globals_publisher
{
    static __forceinline void publish(void *allocation)
    {
        player_training_globals = (s_player_training_globals *)allocation;
    }
};
}

// @address 0x11df6d4 player_training_globals
__declspec(thread) s_player_training_globals *player_training_globals;

#include "game/player_access.h"

#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_region_members.h"
#include "cache/restricted_memory_regions_inline.h"
#include "multithreading/thread_tls.h"
#include "game/game_globals_inline.h"
#include "text/unicode.h"

#include "memory/datum_index.h"
#include "objects/object_type_access.h"
#include "game/player_training_constants.h"

static __forceinline long player_mapping_get_input_user(long player_index)
{
    long absolute_player_index = datum_index_to_absolute_index(player_index);
    return player_mapping_globals->player_input_user_mapping[absolute_player_index];
}

__forceinline bool c_recent_actions::did_player_recently_perform_action(long action, long ticks)
{
    long time = game_time_get();
    bool result = false;
    for (long i = 0; i < 4; i++)
    {
        if (m_actions[i].type == action && time - m_actions[i].time < ticks)
        {
            result = true;
            break;
        }
    }
    return result;
}

__forceinline void c_hold_watcher::check_action(long input_user_index, const player_action *action)
{
    if (action->action_context.interaction.type != m_action_type || action->action_context.interaction.object_index != m_object_index)
    {
        m_action_type = action->action_context.interaction.type;
        m_object_index = action->action_context.interaction.object_index;
        m_action_count = action->action_context.interaction.weapon_flags;
        m_hold_ticks = 0;
        m_secondary_hold_ticks = 0;
    }
    if (m_action_type == 1 || m_action_type == 3 || m_action_type == 4 || m_action_type == 5 || m_action_type == 8)
    {
        if (action->action_flags & 0x8)
            m_hold_ticks++;
    }
    if (m_action_type == 1)
    {
        if (action->action_flags & 0x80)
            m_secondary_hold_ticks++;
    }
}

// @address 0xa8508 ?player_training_examine_action@@YAXJPBUplayer_action@@@Z
void player_training_examine_action(long player_index, struct player_action const * action)
{
    player_datum *player = player_get(player_index);
    long input_user_index = player_mapping_get_input_user(player_index);
    if (input_user_index != NONE)
    {
        s_player_training_entry_data *entry_data = &((s_player_training_entry_data *)player_training_globals)[input_user_index];
        memcpy(&entry_data->last_action, &entry_data->current_action, sizeof(player_action));
        memcpy(&entry_data->current_action, action, sizeof(player_action));
        if (player->unit_index != NONE)
        {
            unit_datum *unit = ((unit_datum *)object_get_and_verify_type(player->unit_index, 0x2003));
            if (unit->unit.parent_seat_index == NONE && action->weapons.weapon_indices[0] != NONE &&
                action->weapons.weapon_indices[0] != unit->unit.desired_weapon_set.weapon_indices[0])
            {
                entry_data->weapon_switch_count++;
                entry_data->weapon_switch_ticks = (unsigned char)(2 * game_tick_rate());
            }
        }
        entry_data->hold_watcher.check_action(input_user_index, action);
    }
}

static __forceinline bool player_training_profile_union_bits(unsigned long *dst, const unsigned long *src)
{
    bool changed = 0;
    for (long i = 0; i < 2; ++i) {
        unsigned long old = dst[i];
        dst[i] |= src[i];
        changed |= dst[i] != old;
    }
    for (long i = 0; i < 2; ++i) {
        unsigned long old = dst[i + 2];
        dst[i + 2] |= src[i + 2];
        changed |= dst[i + 2] != old;
    }
    return changed;
}
// @address 0x223c78 player_training_profile_merge
extern "C" inline bool player_training_profile_merge(s_training_profile *dst, const s_training_profile *src)
{
    bool changed = player_training_profile_union_bits(dst->bits, src->bits);
    dst->bits[4] |= src->bits[4];
    dst->bits[5] |= src->bits[5];
    dst->bits[6] |= src->bits[6];
    dst->bits[7] |= src->bits[7];
    return changed;
}

void player_training_profile_data_set_completion_count(
    s_player_training_profile_data *data, long training_index, long completion_count)
{
    bool low_bit = (completion_count & 1) != 0;
    bool high_bit = (completion_count & 2) != 0;
    if (low_bit)
        data->completion_count_low_bits[training_index >> 5] |= 1 << (training_index & 31);
    else
        data->completion_count_low_bits[training_index >> 5] &= ~(1 << (training_index & 31));
    if (high_bit)
        data->completion_count_high_bits[training_index >> 5] |= 1 << (training_index & 31);
    else
        data->completion_count_high_bits[training_index >> 5] &= ~(1 << (training_index & 31));
}

// @address 0x223dc8 ?player_training_initialize@@YAXXZ
void player_training_initialize()
{
    player_training_globals = (s_player_training_globals *)g_player_training_globals_allocator.reserve_memory(
        "player training globals", "globals", sizeof(s_player_training_globals), 0);
}

// @address 0x5bec98 ?player_training_dispose@@YAXXZ
void player_training_dispose(void)
{
}

// @address 0x223e18 ?player_training_initialize_for_new_map@@YAXXZ
void player_training_initialize_for_new_map()
{
    s_player_training_globals *globals = (s_player_training_globals *)player_training_globals;
    memset(globals, 0, sizeof(*globals));
    globals->scripted_training_enabled = true;
    for (long user_index = 0; user_index < 4; ++user_index)
        globals->entries[user_index].unit_index = -1;
    globals->scripted_training_text = 0;
    globals->show_scripted_training_text = false;
}

// @address 0x223e88 ?player_training_dispose_from_old_map@@YAXXZ
void player_training_dispose_from_old_map()
{
    player_training_save_changes_to_profiles(_training_data_save_old_map);
    return;
    player_training_save_changes_to_profiles(_training_data_save_saved_game);
    return;
}

// @address 0x223e90 ?player_training_handle_saved_game@@YAXJ@Z
void player_training_handle_saved_game(long saved_game_index)
{
    player_training_save_changes_to_profiles(_training_data_save_saved_game);
    return;
}

namespace {

static __forceinline long damage_reporting_info_get_modifier(unsigned char damage_reporting_info)
{
    return damage_reporting_info & 3;
}
static __forceinline long damage_reporting_type_get_breakdown_type(long type)
{
    long result = 0;
    if ((unsigned long)type <= 60)
        result = g_game_results_damage_type_descriptions[type].breakdown_type;
    return result;
}
}

struct cache_file_tag_instance;

static __forceinline long player_training_seconds_to_ticks(long seconds)
{
    return seconds * game_tick_rate();
}

static __forceinline long player_training_profile_data_get_completion_count(const s_player_training_profile_data *data, long training_index)
{
    long result = 0;
    bool low = (data->completion_count_low_bits[training_index >> 5] & (1 << (training_index & 31))) != 0;
    bool high = (data->completion_count_high_bits[training_index >> 5] & (1 << (training_index & 31))) != 0;
    result = low + (high ? 2 : 0);
    return result;
}

static __forceinline void player_training_profile_data_set_finished(s_player_training_profile_data *data, long training_index, long value)
{
    bool low = (value & 1) != 0;
    bool high = (value & 2) != 0;
    if (low)
        data->completion_count_low_bits[training_index >> 5] |= 1 << (training_index & 31);
    else
        data->completion_count_low_bits[training_index >> 5] &= ~(1 << (training_index & 31));
    if (high)
        data->completion_count_high_bits[training_index >> 5] |= 1 << (training_index & 31);
    else
        data->completion_count_high_bits[training_index >> 5] &= ~(1 << (training_index & 31));
}

struct s_chud_globals_definition;

static __forceinline unsigned long player_training_is_active(long input_user_index, long training_index)
{
    const unsigned long *bits = ((s_player_training_globals *)player_training_globals)->entries[input_user_index].active_bits;
    return bits[training_index >> 5] & (1 << (training_index & 31));
}
static __forceinline long player_training_get_completion_count(long input_user_index, long training_index)
{
    long result = 0;
    if (input_user_index != NONE)
    {
        result = player_training_profile_data_get_completion_count(
            (const s_player_training_profile_data *)&((s_player_training_entry_data *)player_training_globals)[input_user_index].profile, training_index);
    }
    return result;
}

static __forceinline long player_training_inventory_weapon(long unit_index, short weapon_slot)
{
    unit_datum *unit = (unit_datum *)object_get(unit_index);
    long result = NONE;
    if (weapon_slot != NONE)
        result = unit->unit.weapon_object_indices[weapon_slot];
    return result;
}
static __forceinline long player_training_primary_weapon(const long &unit_index)
{
    return player_training_inventory_weapon(unit_index, ((unit_datum *)object_get_and_verify_type(unit_index,
        0x2003))->unit.current_weapon_index);
}
static __forceinline long player_training_secondary_weapon(const long &unit_index)
{
    return player_training_inventory_weapon(unit_index, ((unit_datum *)object_get_and_verify_type(unit_index,
        0x2003))->unit.current_weapon_set.weapon_indices[1]);
}
static __forceinline long player_training_primary_weapon_value(long unit_index)
{
    return player_training_inventory_weapon(unit_index, ((unit_datum *)object_get_and_verify_type(unit_index,
        0x2003))->unit.current_weapon_index);
}
static __forceinline long player_training_secondary_weapon_value(long unit_index)
{
    return player_training_inventory_weapon(unit_index, ((unit_datum *)object_get_and_verify_type(unit_index,
        0x2003))->unit.current_weapon_set.weapon_indices[1]);
}
static __forceinline float player_training_target_distance_squared(const real_point3d *a, const real_point3d *b)
{
    real_vector3d delta;
    return magnitude_squared3d(vector_from_points3d(a, b, &delta));
}

// @address 0x2243b8 ?training_is_triggerable@@YA_NJJ@Z
bool training_is_triggerable(long input_user_index, long training_index)
{
    bool result = false;
    s_player_training_entry_data *user = &((s_player_training_entry_data *)player_training_globals)[input_user_index];
    long player_index = NONE;
    if (input_user_index != NONE)
        player_index = player_mapping_globals->player_user_mapping[input_user_index];
    player_training_chud_entry *entry = 0;
    s_chud_globals_definition *definition = (s_chud_globals_definition *)chud_globals;
    if (definition && training_index >= 0 && training_index < definition->training_data.count)
        entry = &((player_training_chud_entry *)definition->training_data.address)[training_index];
    if (player_index != NONE && entry && entry->text && !((entry->flags & 1) && tls_game_globals()->options.game_mode == 2))
    {
        long absolute_player_index = datum_index_to_absolute_index(player_index);
        player_datum *player = player_get(player_index);
        long output_user_index = player_get_first_output_user(player_index);
        if (player->unit_index != NONE)
        {
            unit_datum *unit = ((unit_datum *)object_get_and_verify_type(player->unit_index, 0x2003));
            bool enemies_nearby = false;
            if (output_user_index != NONE)
            {
                bool nearby_result = false;
                if ((unsigned long)output_user_index <= 3)
                    nearby_result = c_chud_manager::x_persistent_user_data[output_user_index].m_motion_sensor.enemies_nearby();
                enemies_nearby = nearby_result;
            }
            switch (training_index)
            {
            case _player_training_vehicle_look:
                if (!inl_cinematic_in_progress() && unit->unit.parent_seat_index != NONE &&
                    unit->object.parent_object_index != NONE
                    && unit_in_driving_seat(player->unit_index, NONE, true))
                {
                    result = true;
                    if (!player_training_is_active(input_user_index, training_index))
                        user->look.initialized = false;
                }
                break;
            case _player_training_vehicle_move:
                if (!inl_cinematic_in_progress() && unit->unit.parent_seat_index != NONE &&
                    unit->object.parent_object_index != NONE
                    && unit_in_driving_seat(player->unit_index, NONE, true))
                {
                    result = true;
                    if (!player_training_is_active(input_user_index, training_index))
                        user->move.initialized = false;
                }
                break;
            case _player_training_look:
                if (!inl_cinematic_in_progress() && unit->object.parent_object_index == NONE)
                {
                    result = true;
                    if (user->unit_index != _player_training_look)
                        user->look.initialized = false;
                }
                break;
            case _player_training_move:
                if (!inl_cinematic_in_progress() && unit->object.parent_object_index == NONE)
                {
                    result = true;
                    if (user->unit_index != _player_training_move)
                        user->move.initialized = false;
                }
                break;
            case _player_training_unzoom:
                {
                    if (player_training_get_completion_count(input_user_index, _player_training_zoom) == 0
                        && ((unit_datum *)object_get_and_verify_type(player->unit_index, 0x2003))->unit.zoom_level != NONE)
                        result = true;
                }
                break;
            case _player_training_look_left_and_right:
                if (!inl_cinematic_in_progress() && training_is_completed(input_user_index, _player_training_look) &&
                    unit->object.parent_object_index == NONE)
                {
                    result = true;
                    if (!player_training_is_active(input_user_index, training_index))
                        user->look.initialized = false;
                }
                break;
            case 4:
                if (!inl_cinematic_in_progress())
                {
                    long parent_index = unit->object.parent_object_index;
                    if (parent_index != NONE && unit->unit.parent_seat_index != NONE)
                    {
                        unit_datum *parent = ((unit_datum *)object_get_and_verify_type(parent_index, 0x2003));
                        vehicle_definition *vehicle =
                            (vehicle_definition *)global_tag_instances[datum_index_to_absolute_index(parent->object.definition_index)]
                                .definition;
                        unit_seat *seat = &vehicle->seats[unit->unit.parent_seat_index];
                        if (!(bool)(_rotl(seat->flags, 30) & 1) && !(bool)(_rotl(seat->flags, 29) & 1) && !(bool)(_rotl(seat->flags,
                            21) & 1))
                        {
                            bool driver_found = false;
                            unit_passenger_iterator iterator;
                            unsigned short absolute_parent_index = (unsigned short)parent_index;
                            iterator.passenger_index = NONE;
                            iterator.seat_index = NONE;
                            iterator.unit_index = parent_index;
                            iterator.next_object_index = ((unit_datum *)object_get_and_verify_type(absolute_parent_index,
                                0x2003))->object.first_child_object_index;
                            while (unit_passenger_iterator_next(&iterator))
                            {
                                if (iterator.seat_index != NONE && iterator.passenger_index != player->unit_index
                                    && (bool)(_rotl(vehicle->seats[iterator.seat_index].flags, 30) & 1))
                                {
                                    driver_found = true;
                                    break;
                                }
                            }
                            if (!driver_found)
                                result = true;
                        }
                    }
                }
                break;
            case _player_training_weapon_kill:
                if (player_training_primary_weapon_value(player->unit_index) != NONE && !enemies_nearby)
                    result = true;
                break;
            case _player_training_headshot_kill:
                if ((player_training_primary_weapon_value(player->unit_index) != NONE && !enemies_nearby &&
                    training_is_completed(input_user_index, _player_training_weapon_kill))
                    || player_training_is_active(input_user_index, training_index))
                    result = true;
                break;
            case _player_training_flashlight:
                if (unit->unit.parent_seat_index == NONE)
                    result = (user->trigger_flags & 1) != 0;
                break;
            case _player_training_crouch:
                result = (user->trigger_flags & 2) != 0;
                break;
            case _player_training_stealth:
                result = (user->trigger_flags & 4) != 0;
                break;
            case _player_training_equipment:
                {
                    const long &unit_index = player->unit_index;
                    if (unit->unit.parent_seat_index == NONE && player_training_secondary_weapon(unit_index) == NONE
                        && (((unit_datum *)object_get_and_verify_type(player->unit_index,
                            0x2003))->unit.equipment_object_index != NONE || player_training_is_active(input_user_index, training_index)))
                        result = true;
                }
                break;
            case _player_training_jump:
                result = (user->trigger_flags & 16) != 0;
                break;
            case _player_training_grenade_kill:
                if (unit->unit.parent_seat_index == NONE && player_training_secondary_weapon_value(player->unit_index) == NONE
                    && (user->grenade_pickup_ticks || player_training_is_active(input_user_index, training_index)))
                    result = true;
                break;
            case _player_training_voice_chat:
                if (tls_game_globals()->options.game_mode == 2 || game_campaign_is_multiplayer())
                {
                    long state;
                    long talking;
                    voice_get_player_voice_state(absolute_player_index, &state, &talking);
                    if (state == 3)
                        result = true;
                }
                break;
            case _player_training_assassination:
                if (unit->object.parent_object_index == NONE && unit->unit.targeting.target_object_index != NONE)
                {
                    unit_datum *target = (unit_datum *)object_try_and_get_and_verify_type(unit->unit.targeting.target_object_index, ~0UL);
                    if (unit->unit.targeting.primary_autoaim_level >= 1.0f && target && ((1 << target->object.object_type) & 1)
                        && unit_can_be_assassinated(unit->unit.targeting.target_object_index, &unit->object.position,
                            &unit->unit.aiming_vector)
                        && player_training_target_distance_squared(&unit->object.bounding_center, &target->object.bounding_center) < 4.0f)
                    {
                        user->assassination_ticks++;
                        if ((unsigned char)user->assassination_ticks > 5)
                            result = true;
                    }
                    else if (player_training_is_active(input_user_index, training_index))
                        result = true;
                    else
                        user->assassination_ticks = 0;
                }
                break;
            case _player_training_zoom:
                if (!enemies_nearby && player_training_primary_weapon_value(player->unit_index) != NONE
                    && player_training_secondary_weapon(player->unit_index) == NONE &&
                        weapon_is_zoomable(player_training_primary_weapon(player->unit_index))
                    && weapon_rotate_zoom_level(player_training_primary_weapon(player->unit_index), 0) != NONE)
                {
                    result = true;
                    if (!player_training_is_active(input_user_index, training_index))
                        user->zoom.initialized = false;
                }
                break;
            case _player_training_vehicle_boarding:
                if (unit->unit.parent_seat_index == NONE)
                {
                    bool boardable = false;
                    if ((unsigned long)input_user_index <= 3)
                        boardable = c_chud_manager::x_persistent_user_data[input_user_index]
                            .m_motion_sensor.slow_moving_boardable_enemy_vehicles_nearby();
                    if (boardable || player_training_is_active(input_user_index, training_index))
                        result = true;
                }
                break;
            case _player_training_reload_primary_weapon:
                if (!enemies_nearby && player_training_primary_weapon(player->unit_index) != NONE)
                {
                    weapon_interface_state state;
                    weapon_build_weapon_interface_state(player_training_primary_weapon(player->unit_index), &state);
                    if (state.magazine_count > 0 && state.magazines[0].rounds_loaded_maximum != 0
                        && state.magazines[0].rounds_loaded * 2 <= state.magazines[0].rounds_loaded_maximum
                        && state.magazines[0].rounds_inventory_maximum != 0 && state.magazines[0].rounds_inventory > 0)
                        result = true;
                }
                break;
            case _player_training_reload_secondary_weapon:
                if (!enemies_nearby && player_training_secondary_weapon(player->unit_index) != NONE)
                {
                    weapon_interface_state state;
                    weapon_build_weapon_interface_state(player_training_secondary_weapon(player->unit_index), &state);
                    if (state.magazine_count > 0 && state.magazines[0].rounds_loaded_maximum != 0
                        && state.magazines[0].rounds_loaded * 2 <= state.magazines[0].rounds_loaded_maximum
                        && state.magazines[0].rounds_inventory_maximum != 0 && state.magazines[0].rounds_inventory > 0)
                        result = true;
                }
                break;
            case _player_training_fire_secondary_weapon:
                if (player_training_secondary_weapon(player->unit_index) != NONE &&
                    weapon_can_be_fired(player_training_secondary_weapon(player->unit_index)))
                    result = true;
                break;
            case _player_training_ghost_guns:
            case _player_training_ghost_boost:
                if (unit_in_driving_seat(player->unit_index, 3, false))
                    result = true;
                break;
            case _player_training_banshee_boost:
            case _player_training_banshee_bomb:
            case _player_training_vehicle_trick:
                if (unit_in_driving_seat(player->unit_index, 4, false))
                    result = true;
                break;
            case _player_training_warthog_slide:
                if (unit_in_driving_seat(player->unit_index, 1, false))
                    result = true;
                break;
            case _player_training_shield_depleted:
                if (player_training_is_active(input_user_index, training_index) ||
                    (unit->object.shield_vitality < 0.25f && !(bool)(_rotl(unit->object.damage_flags, 22) & 1)))
                    result = true;
                break;
            case _player_training_shield_recharge:
                if (player_training_is_active(input_user_index, training_index) ||
                    ((bool)(_rotl(unit->object.damage_flags, 22) & 1) && unit->object.shield_vitality < 0.25f))
                    result = true;
                break;
            case _player_training_activate_device_hold:
                if (user->hold_watcher.action_type() == 3 && user->hold_watcher.hold_ticks() >= 3)
                    result = true;
                break;
            case _player_training_enter_vehicle_hold:
                if (user->hold_watcher.action_type() == 5 && user->hold_watcher.hold_ticks() >= 3)
                {
                    unit_datum *vehicle = (unit_datum *)object_try_and_get_and_verify_type(user->hold_watcher.object_index(), 2);
                    if (vehicle &&
                        ((vehicle_definition *)global_tag_instances[datum_index_to_absolute_index(vehicle->object.definition_index)]
                                .definition)
                                ->havok_vehicle_physics.player_training_vehicle_type != 0)
                        result = true;
                }
                break;
            case _player_training_weapon_swap_hold:
                if (user->hold_watcher.action_type() == 1 && ((user->hold_watcher.action_count() & 1) ||
                    (user->hold_watcher.action_count() & 2))
                    && user->hold_watcher.hold_ticks() >= 3)
                    result = true;
                break;
            case _player_training_dual_wield_hold:
                if (user->hold_watcher.action_type() == 1 && ((user->hold_watcher.action_count() & 4) ||
                    (user->hold_watcher.action_count() & 8))
                    && user->hold_watcher.secondary_hold_ticks() >= 3)
                    result = true;
                break;
            }
        }
    }
    return result;
}

static __forceinline short unit_get_zoom_level(long unit_index)
{
    return ((unit_datum *)object_get_and_verify_type(unit_index, 0x2003))->unit.zoom_level;
}

// @address 0x225308 ?training_is_satisfied@@YA_NJJ@Z
bool training_is_satisfied(long input_user_index, long training_index)
{
    bool result = false;
    s_player_training_entry_data *user = &((s_player_training_entry_data *)player_training_globals)[input_user_index];
    long player_index = NONE;
    if (input_user_index != NONE)
        player_index = player_mapping_globals->player_user_mapping[input_user_index];
    if (player_index != NONE)
    {
        long absolute_player_index = datum_index_to_absolute_index(player_index);
        player_datum *player = player_get(player_index);
        if (player->unit_index != NONE)
        {
            unit_datum *unit = ((unit_datum *)object_get_and_verify_type(player->unit_index, 0x2003));
            switch (training_index)
            {
            case _player_training_vehicle_look:
                if (user->unit_index == _player_training_vehicle_look)
                {
                    if (!user->look.initialized)
                    {
                        user->look.yaw = user->current_action.facing.yaw;
                        user->look.pitch = user->current_action.facing.pitch;
                        user->look.initialized = true;
                    }
                    else
                    {
                        float yaw = user->current_action.facing.yaw;
                        float pitch = user->current_action.facing.pitch;
                        float yaw_delta = yaw - user->look.yaw;
                        float pitch_delta = pitch - user->look.pitch;
                        float threshold = 0.17453292f;
                        if (yaw_delta > 3.1415927f)
                            yaw_delta -= 6.2831855f;
                        if (yaw_delta < -3.1415927f)
                            yaw_delta += 6.2831855f;
                        if (fabs(yaw_delta) > threshold || fabs(pitch_delta) > threshold)
                            result = true;
                    }
                }
                break;
            case _player_training_vehicle_move:
                if (user->unit_index == _player_training_vehicle_move)
                {
                    if (!user->move.initialized)
                    {
                        object_get_origin(player->unit_index, &user->move.position);
                        user->move.initialized = true;
                    }
                    else
                    {
                        float threshold = 0.25f;
                        real_point3d origin;
                        object_get_origin(player->unit_index, &origin);
                        if (distance_squared3d(&origin, &user->move.position) > threshold)
                            result = true;
                    }
                }
                break;
            case _player_training_look:
                if (user->unit_index == _player_training_look)
                {
                    if (!user->look.initialized)
                    {
                        user->look.yaw = user->current_action.facing.yaw;
                        user->look.pitch = user->current_action.facing.pitch;
                        user->look.initialized = true;
                    }
                    else
                    {
                        float yaw = user->current_action.facing.yaw;
                        float pitch = user->current_action.facing.pitch;
                        float yaw_delta = yaw - user->look.yaw;
                        float pitch_delta = pitch - user->look.pitch;
                        float threshold = 0.17453292f;
                        if (yaw_delta > 3.1415927f)
                            yaw_delta -= 6.2831855f;
                        if (yaw_delta < -3.1415927f)
                            yaw_delta += 6.2831855f;
                        if (fabs(yaw_delta) > threshold || fabs(pitch_delta) > threshold)
                            result = true;
                    }
                }
                break;
            case _player_training_move:
                if (user->unit_index == _player_training_move)
                {
                    if (!user->move.initialized)
                    {
                        user->move.position = ((unit_datum *)object_get_and_verify_type(player->unit_index, 0x2003))->object.position;
                        user->move.initialized = true;
                    }
                    else
                    {
                        float threshold = 0.25f;
                        real_point3d origin = unit->object.position;
                        if (distance_squared3d(&origin, &user->move.position) > threshold)
                            result = true;
                    }
                }
                break;
            case _player_training_unzoom:
                if (unit_get_zoom_level(player->unit_index) == NONE)
                    result = true;
                break;
            case _player_training_look_left_and_right:
                if (user->unit_index == _player_training_look_left_and_right)
                {
                    if (!user->look.initialized)
                    {
                        user->look.left_seen = false;
                        user->look.right_seen = false;
                        user->look.yaw = user->current_action.facing.yaw;
                        user->look.initialized = true;
                    }
                    else
                    {
                        float yaw = user->current_action.facing.yaw;
                        float yaw_delta = yaw - user->look.yaw;
                        float threshold = 0.34906584f;
                        if (yaw_delta > 3.1415927f)
                            yaw_delta -= 6.2831855f;
                        if (yaw_delta < -3.1415927f)
                            yaw_delta += 6.2831855f;
                        if (yaw_delta < -threshold)
                            user->look.left_seen = true;
                        if (yaw_delta > threshold)
                            user->look.right_seen = true;
                        if (user->look.right_seen && user->look.left_seen)
                            result = true;
                    }
                }
                break;
            case 4:
                if (user->recent_actions.did_player_recently_perform_action(13, game_tick_rate()))
                    result = true;
                break;
            case _player_training_weapon_kill:
                if (user->weapon_kill_ticks > 0)
                {
                    result = true;
                    user->weapon_kill_ticks = 0;
                }
                break;
            case _player_training_headshot_kill:
                if (user->headshot_kill_ticks > 0)
                {
                    result = true;
                    user->headshot_kill_ticks = 0;
                }
                break;
            case _player_training_flashlight:
                if ((user->current_action.flags & 4) != 0 || (bool)(_rotl(unit->unit.unit_flags, 5) & 1))
                {
                    result = true;
                    user->trigger_flags &= ~1;
                }
                break;
            case _player_training_crouch:
                if ((user->current_action.flags & 1) != 0)
                {
                    result = true;
                    user->trigger_flags &= ~2;
                }
                break;
            case _player_training_stealth:
                if ((user->current_action.flags & 4) != 0)
                {
                    result = true;
                    user->trigger_flags &= ~4;
                }
                break;
            case _player_training_equipment:
                if ((user->current_action.flags & 16) != 0)
                {
                    result = true;
                    user->trigger_flags &= ~8;
                }
                break;
            case _player_training_jump:
                if ((user->current_action.flags & 2) != 0)
                {
                    result = true;
                    user->trigger_flags &= ~16;
                }
                break;
            case _player_training_grenade_kill:
                if (user->grenade_kill_ticks > 0)
                {
                    result = true;
                    user->grenade_kill_ticks = 0;
                }
                break;
            case _player_training_assassination:
                if (user->melee_kill_ticks > 0)
                {
                    result = true;
                    user->melee_kill_ticks = 0;
                }
                break;
            case 3:
                break;
            case _player_training_voice_chat:
                if (tls_game_globals()->options.game_mode == 2 || game_campaign_is_multiplayer())
                {
                    long state;
                    long talking;
                    voice_get_player_voice_state(absolute_player_index, &state, &talking);
                    if (talking == 1)
                        result = true;
                }
                break;
            case _player_training_zoom:
                if (user->unit_index == _player_training_zoom)
                {
                    if (!user->zoom.initialized)
                    {
                        user->zoom.initialized = true;
                        user->zoom.ticks = 0;
                    }
                    else
                    {
                        if (unit_get_zoom_level(player->unit_index) != NONE)
                            user->zoom.ticks++;
                        else
                            user->zoom.ticks = 0;
                        if (user->zoom.ticks > game_tick_rate() / 2)
                            result = true;
                    }
                }
                break;
            case _player_training_vehicle_boarding:
                if (user->recent_actions.did_player_recently_perform_action(16, game_tick_rate()))
                    result = true;
                break;
            case _player_training_reload_primary_weapon:
                if (player_training_primary_weapon(player->unit_index) != NONE)
                {
                    weapon_interface_state state;
                    weapon_build_weapon_interface_state(player_training_primary_weapon(player->unit_index), &state);
                    if (state.magazine_count > 0 && state.magazines[0].rounds_loaded_maximum != 0
                        && state.magazines[0].rounds_loaded < state.magazines[0].rounds_loaded_maximum
                        && (unit->unit.control_flags & 0x20000000) != 0)
                        result = true;
                }
                break;
            case _player_training_reload_secondary_weapon:
                if (player_training_secondary_weapon(player->unit_index) != NONE)
                {
                    weapon_interface_state state;
                    weapon_build_weapon_interface_state(player_training_secondary_weapon(player->unit_index), &state);
                    if (state.magazine_count > 0 && state.magazines[0].rounds_loaded_maximum != 0
                        && state.magazines[0].rounds_loaded < state.magazines[0].rounds_loaded_maximum
                        && (unit->unit.control_flags & 0x40000000) != 0)
                        result = true;
                }
                break;
            case _player_training_fire_secondary_weapon:
                if ((user->current_action.flags & 0x800000) != 0)
                    result = true;
                break;
            case _player_training_ghost_guns:
                if ((user->current_action.flags & 0x40000) != 0)
                    result = true;
                break;
            case _player_training_vehicle_trick:
                if (unit_in_driving_seat(player->unit_index, 4, false) && vehicle_is_running_trick(unit->object.parent_object_index))
                    result = true;
                break;
            case _player_training_banshee_bomb:
                if ((user->current_action.flags & 0x80000000) != 0)
                    result = true;
                break;
            case _player_training_warthog_slide:
                if ((user->current_action.flags & 0x4000) != 0)
                    result = true;
                break;
            case _player_training_banshee_boost:
            case _player_training_ghost_boost:
                if ((user->current_action.flags & 0x800) != 0)
                    result = true;
                break;
            case _player_training_shield_depleted:
                if ((bool)(_rotl(((unit_datum *)object_get_and_verify_type(player->unit_index, 0x2003))->object.damage_flags, 22) & 1))
                    result = true;
                break;
            case _player_training_shield_recharge:
                if (((unit_datum *)object_get_and_verify_type(player->unit_index, 0x2003))->object.shield_vitality >= 0.9999f)
                    result = true;
                break;
            case _player_training_activate_device_hold:
                if (user->device_touched_ticks > 0)
                    result = true;
                break;
            case _player_training_enter_vehicle_hold:
                if (user->recent_actions.did_player_recently_perform_action(17, game_tick_rate()))
                    result = true;
                break;
            case _player_training_weapon_swap_hold:
                if (user->hold_watcher.action_type() == 1
                    && ((user->hold_watcher.action_count() & 1) != 0 || (user->hold_watcher.action_count() & 2) != 0)
                    && user->hold_watcher.hold_ticks() >= 3
                    && (user->current_action.action_flags & 2) != 0)
                    result = true;
                break;
            case _player_training_dual_wield_hold:
                if (user->hold_watcher.action_type() == 1
                    && ((user->hold_watcher.action_count() & 4) != 0 || (user->hold_watcher.action_count() & 8) != 0)
                    && (user->current_action.action_flags & 32) != 0)
                    result = true;
                break;
            }
        }
    }
    return result;
}

// counts one completion of a training; returns true when it is finished
// @address 0x225d50 ?increment_training_completion@@YA_NJPAUs_player_training_user@@PBUs_player_training_entry_data@@PAUs_player_training_state@@@Z
bool increment_training_completion(long training_index, struct s_player_training_user *_sym_p1,
    struct s_player_training_entry_data const *_sym_p2, struct s_player_training_state *_sym_p3)
{
    long completion_count = player_training_profile_data_get_completion_count(
        (s_player_training_profile_data *)&((s_player_training_entry_data *)_sym_p1)->profile, training_index);
    bool result = false;
    if (!((const player_training_chud_entry *)_sym_p2)) {
        ((s_training_entry *)_sym_p3)->state = 4;
        ((s_training_entry *)_sym_p3)->ticks = (short)player_training_seconds_to_ticks(5);
    }
    else if (((const player_training_chud_entry *)_sym_p2)->completion_count == 0 ||
        ((const player_training_chud_entry *)_sym_p2)->completion_count > 3)
    {
        ((s_training_entry *)_sym_p3)->state = 4;
        ((s_training_entry *)_sym_p3)->ticks = (short)player_training_seconds_to_ticks(((const player_training_chud_entry *)_sym_p2)->delay_seconds);
    } else if (completion_count + 1 >= ((const player_training_chud_entry *)_sym_p2)->completion_count) {
        result = true;
        ((s_training_entry *)_sym_p3)->state = 5;
        ((s_training_entry *)_sym_p3)->ticks = 0;
        player_training_profile_data_set_finished((s_player_training_profile_data *)&((s_player_training_entry_data *)_sym_p1)->profile, training_index, 3);
    } else {
        ((s_training_entry *)_sym_p3)->state = 4;
        ((s_training_entry *)_sym_p3)->ticks = (short)player_training_seconds_to_ticks(((const player_training_chud_entry *)_sym_p2)->delay_seconds);
        player_training_profile_data_set_completion_count(
            (s_player_training_profile_data *)&((s_player_training_entry_data *)_sym_p1)->profile, training_index, completion_count + 1);
    }
    return result;
}

// @address 0x225eb8 ?unit_in_driving_seat@@YA_NJJ_N@Z
bool unit_in_driving_seat(long unit_index, long seat_index, bool include_any_driver)
{
    unit_datum *unit = ((unit_datum *)object_get_and_verify_type(unit_index, 0x2003));
    bool result = false;
    if (unit->object.parent_object_index != NONE && unit->unit.parent_seat_index != NONE
        && ((1 << object_get_type(unit->object.parent_object_index)) & 2) != 0)
    {
        unit_datum *parent = ((unit_datum *)object_get_and_verify_type(unit->object.parent_object_index, 2));
        vehicle_definition *definition =
            (vehicle_definition *)global_tag_instances[datum_index_to_absolute_index(parent->object.definition_index)].definition;
        unit_seat *seat = &definition->seats[unit->unit.parent_seat_index];
        if ((bool)(_rotl(seat->flags, 30) & 1)
            && (definition->havok_vehicle_physics.player_training_vehicle_type == seat_index || (include_any_driver &&
                definition->havok_vehicle_physics.player_training_vehicle_type != 0)))
            result = true;
    }
    return result;
}

// @address 0x225f78 ?player_training_notify_kill@@YAXJJE@Z
void player_training_notify_kill(long player_index, long object_index, unsigned char damage_reporting_info)
{
    long input_user_index = player_mapping_get_input_user(player_index);
    unit_datum *object = ((unit_datum *)object_get_and_verify_type(object_index, -1));
    if (input_user_index != -1 && ((1 << object->object.object_type) & 0x1003) != 0) {
        player_datum *player = player_get(player_index);
        unit_datum *unit = object;
        if ((unsigned char)::game_team_is_enemy((e_game_team)player->configuration.host.team_index, (e_game_team)unit->unit.team) != 0) {
            s_player_training_entry_data *entry_data = &((s_player_training_entry_data *)player_training_globals)[input_user_index];
            long damage_type = damage_reporting_info_get_type(damage_reporting_info);
            long modifier = damage_reporting_info_get_modifier(damage_reporting_info);
            long breakdown_type = damage_reporting_type_get_breakdown_type(damage_type);
            if (damage_type == 3 && modifier == 2)
                entry_data->melee_kill_ticks = (unsigned char)game_tick_rate();
            else if (breakdown_type == 4)
                entry_data->grenade_kill_ticks = (unsigned char)game_tick_rate();
            else if (breakdown_type == 3)
                entry_data->headshot_kill_ticks = (unsigned char)game_tick_rate();
            else if (breakdown_type == 2)
                entry_data->weapon_kill_ticks = (unsigned char)game_tick_rate();
        }
    }
}

typedef unsigned int uint;
class c_restricted_memory;
template<>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),&__tls_set_g_player_training_globals_allocator>::
reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
    // match: 0x2260d0: folding alignment_bits to 0 keeps li r7,0 in the add_member argument setup
    __assume(alignment_bits == 0);
    m_member_index = restricted_region_add_member<s_player_training_globals_publisher>(3, name, type, size, alignment_bits,
        __tls_set_g_player_training_globals_allocator, 0, 0);
    m_thread_id = get_current_thread_index();
    return restricted_region_get_member_address(3, m_member_index);
}

// @address 0x226170 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_training_globals_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
template<>
void *t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),&__tls_set_g_player_training_globals_allocator>::
allocate(unsigned int size, const char *name)
{
    return reserve_memory(name, "", size, 0);
}

// @address 0x226188 ?deallocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_player_training_globals_allocator@@YAXPAX@Z@@UAAXPAX@Z
template<>
void t_restricted_allocation_manager<3,static_cast<void (*)(void *)>(0),static_cast<void (*)(void *)>(0),&__tls_set_g_player_training_globals_allocator>::
deallocate(void *allocation)
{
    int value = m_member_index;
    player_training_globals = 0;
    restricted_region_free_member(3, value);
}

static __forceinline long player_training_first_input_user(void)
{
    long result = NONE;
    for (long input_user_index = 0; input_user_index < 4; input_user_index++) {
        if (player_mapping_globals->player_user_mapping[input_user_index] != NONE) {
            result = input_user_index;
            break;
        }
    }
    return result;
}

// @address 0x2242a0 player_training_set_user_bit
void player_training_set_user_bit(int bit)
{
    long input_user_index = player_training_first_input_user();
    if (input_user_index != NONE) {
        s_player_training_globals *globals = (s_player_training_globals *)player_training_globals;
        int mask = 1 << bit;
        do {
            globals->entries[input_user_index].trigger_flags |= mask;
            input_user_index = players_get_next_user_with_player(input_user_index);
        } while (input_user_index != NONE);
    }
}

// @address 0x224338 ?training_is_completed@@YA_NJJ@Z
bool training_is_completed(long user_index, long training_index)
{
    bool result = false;
    if (user_index != -1) {
        s_player_training_profile_data *bits = &((s_player_training_entry_data *)player_training_globals)[user_index].profile.completion;
        long state = (bit_vector_test(bits->completion_count_high_bits, training_index) ? 2 : 0) + (bit_vector_test(bits->completion_count_low_bits,
            training_index) ? 1 : 0);
        result = state == 3;
    }
    return result;
}

struct c_controller_interface;

// @address 0x223e98 ?player_training_load_profiles@@YAXXZ
void player_training_load_profiles(void)
{
    s_user_globals *users = player_mapping_globals;
    for (long user = player_training_first_input_user(); user != -1; user = players_get_next_user_with_player(user)) {
        long player = -1;
        if (user != -1)
            player = users->player_user_mapping[user];
        unsigned long controller_index = users->player_input_controller_mapping[(unsigned short)player];
        if (controller_index <= 3) {
            s_controller *controller = &controller_globals().controllers[controller_index];
            c_player_profile_interface *profile = controller->get_player_profile_interface();
            unsigned __int64 identifier;
            unsigned __int64 empty_identifier;
            memset(&empty_identifier, 0, sizeof(empty_identifier));
            s_training_profile data;
            data.completion = profile->training_data;
            ((const c_controller_interface *)controller)
                ->get_player_identifier(((s_player_identifier *)&identifier));
            if (memcmp(&empty_identifier, &identifier, sizeof(identifier)) == 0) {
                memset(&((s_player_training_entry_data *)player_training_globals)[user].profile, 0, sizeof(s_training_profile));
                ((s_player_training_entry_data *)player_training_globals)[user].loaded = false;
            } else {
                s_player_training_entry_data *training = &((s_player_training_entry_data *)player_training_globals)[user];
                if (memcmp(&identifier, &training->identifier, sizeof(identifier)) == 0) {
                    player_training_profile_merge(&training->profile, &data);
                } else {
                    training->identifier = identifier;
                    memset(&training->profile, 0, sizeof(s_training_profile));
                    player_training_profile_merge(&training->profile, &data);
                    training->loaded = false;
                    for (long i = 0; i < k_player_training_count; ++i) {
                        s_training_entry *entry = &training->entries[i];
                        entry->ticks = 0;
                        entry->state = training_is_completed(user, i) ? 5 : 0;
                    }
                }
            }
        }
    }
}

// @address 0x2240a0 ?player_training_save_changes_to_profiles@@YAXW4e_training_data_save@@@Z
void player_training_save_changes_to_profiles(e_training_data_save training_data_save)
{
    ::player_training_load_profiles();
    for (long user = player_training_first_input_user(); user != NONE; user = players_get_next_user_with_player(user)) {
        long player = -1;
        if (user != -1)
            player = player_mapping_globals->player_user_mapping[user];
        long controller_index = player_mapping_globals->player_input_controller_mapping[(unsigned short)player];
        if (controller_index < 0 || controller_index >= 4)
            continue;
        s_controller *controller = &controller_globals().controllers[controller_index];
        c_player_profile_interface *profile = controller->get_player_profile_interface();
        bool dirty = false;
        unsigned __int64 identifier;
        unsigned __int64 empty_identifier;
        memset(&empty_identifier, 0, sizeof(empty_identifier));
        s_training_profile data;
        memcpy(&data, &profile->training_data, sizeof(data));
        ((const c_controller_interface *)controller)
            ->get_player_identifier(((s_player_identifier *)&identifier));
        if (memcmp(&empty_identifier, &identifier, sizeof(identifier)) != 0) {
            s_player_training_entry_data *training = &((s_player_training_entry_data *)player_training_globals)[user];
            if (memcmp(&identifier, &training->identifier, sizeof(identifier)) == 0 && player_training_profile_merge(&data,
                &training->profile)) {
                dirty = true;
                training->loaded = true;
            }
        }
        if (training_data_save == _training_data_save_saved_game) {
            if (dirty)
                ((c_player_profile_interface *)profile)->set_training_data((const s_player_training_profile_data *)&data, true);
        } else if (((s_player_training_entry_data *)player_training_globals)[user].loaded) {
            ((c_player_profile_interface *)profile)->set_training_data((const s_player_training_profile_data *)&data, true);
            ((s_player_training_entry_data *)player_training_globals)[user].loaded = false;
        }
    }
}

// --- player_training_update_player support -----------------------------------
template<long k_maximum_count> class c_static_wchar_string;

static __forceinline short game_difficulty_level_get()
{
    return tls_game_globals()->options.game_mode == 1 ? tls_game_globals()->options.campaign_difficulty : 1;
}

static __forceinline player_training_chud_entry *get_player_training_entry_data(long training_index)
{
    player_training_chud_entry *result = 0;
    if (chud_globals && training_index >= 0 && training_index < ((s_chud_globals_definition *)chud_globals)->training_data.count)
        result = (player_training_chud_entry *)tag_block_get_element_with_size(
            &((s_chud_globals_definition *)chud_globals)->training_data, training_index, sizeof(player_training_chud_entry));
    return result;
}

static __forceinline void chud_clear_player_training_text(long input_user_index)
{
    if ((unsigned long)input_user_index <= 3)
        c_chud_manager::x_persistent_user_data[input_user_index].m_display_state.display_active = 0;
}

// @address 0x118080 ?player_training_update_player@@YAXJ@Z
void player_training_update_player(long player_index)
{
    long input_user_index = player_mapping_get_input_user(player_index);
    bool campaign = game_is_campaign()
        && (game_difficulty_level_get() == 3 || game_difficulty_level_get() == 2);
    if (input_user_index != NONE && ((s_player_training_globals *)player_training_globals)->scripted_training_enabled)
    {
        s_player_training_entry_data *user = &((s_player_training_globals *)player_training_globals)->entries[input_user_index];
        long selected_index = user->unit_index;
        bool has_active_training = false;
        if (selected_index != NONE)
        {
            s_training_entry *state = &user->entries[selected_index];
            switch (state->state)
            {
            case 2:
            case 3:
                has_active_training = true;
                break;
            }
        }

        if (user->device_touched_ticks != 0)
            user->device_touched_ticks--;
        if (user->weapon_switch_ticks != 0)
        {
            user->weapon_switch_ticks--;
            if (user->weapon_switch_ticks == 0)
                user->weapon_switch_count = 0;
        }
        if (user->grenade_kill_ticks != 0)
            user->grenade_kill_ticks--;
        if (user->headshot_kill_ticks != 0)
            user->headshot_kill_ticks--;
        if (user->melee_kill_ticks != 0)
            user->melee_kill_ticks--;
        if (user->weapon_kill_ticks != 0)
            user->weapon_kill_ticks--;
        if (user->grenade_pickup_ticks != 0)
            user->grenade_pickup_ticks--;
        if (user->equipment_pickup_ticks != 0)
            user->equipment_pickup_ticks--;

        for (long i = 0; i < k_player_training_count; i++)
        {
            s_training_entry *state = &user->entries[i];
            bool active = false;
            switch (state->state)
            {
            case 0:
                if (training_is_triggerable(input_user_index, i))
                    active = true;
                break;
            case 1:
            case 2:
                active = true;
                break;
            case 4:
                state->ticks--;
                if (state->ticks <= 0)
                {
                    state->ticks = 0;
                    state->state = 0;
                }
                break;
            }
            long bit_index = i & 31;
            if (active)
                user->active_bits[i >> 5] |= 1 << bit_index;
            else
                user->active_bits[i >> 5] &= ~(1 << bit_index);
        }

        if (!has_active_training)
        {
            bool found = false;
            for (long i = 0; i < k_player_training_count; i++)
            {
                if ((user->active_bits[i >> 5] & (1 << (i & 31))) != 0)
                {
                    if (!found)
                    {
                        found = true;
                        selected_index = i;
                        continue;
                    }
                    bool eligible = false;
                    switch (i)
                    {
                    case _player_training_activate_device_hold: case _player_training_enter_vehicle_hold:
                    case _player_training_weapon_swap_hold: case _player_training_dual_wield_hold:
                    case _player_training_fire_secondary_weapon:
                    case _player_training_unzoom: case _player_training_assassination: case _player_training_vehicle_boarding:
                    case _player_training_shield_recharge: case _player_training_shield_depleted:
                        eligible = true;
                        break;
                    }
                    if (!eligible)
                        continue;
                    selected_index = i;
                }
            }
        }

        if (selected_index != NONE)
        {
            player_training_chud_entry *entry = get_player_training_entry_data(selected_index);
            s_training_entry *state = &user->entries[selected_index];
            if (selected_index != user->unit_index)
            {
                if (user->unit_index != NONE)
                {
                    s_training_entry *previous_state = &user->entries[user->unit_index];
                    previous_state->ticks = 0;
                    previous_state->state = 0;
                }
                user->unit_index = selected_index;
                user->state_ticks = 0;
            }

            switch (state->state)
            {
            case 0:
                state->state = 1;
                state->ticks = (short)(entry ? game_seconds_to_ticks_round(entry->seconds) : 0);
                break;
            case 1:
                if (training_is_satisfied(input_user_index, selected_index))
                {
                    increment_training_completion(selected_index, (s_player_training_user *)user,
                        (const s_player_training_entry_data *)entry, (s_player_training_state *)state);
                    user->unit_index = NONE;
                }
                else if (training_is_triggerable(input_user_index, selected_index))
                {
                    state->ticks--;
                    if (state->ticks <= 0)
                    {
                        state->state = 2;
                        state->ticks = 0;
                    }
                }
                else
                {
                    state->state = 0;
                    state->ticks = 0;
                    user->unit_index = NONE;
                }
                break;
            case 2:
                user->state_ticks++;
                if (training_is_satisfied(input_user_index, selected_index))
                {
                    increment_training_completion(selected_index, (s_player_training_user *)user,
                        (const s_player_training_entry_data *)entry, (s_player_training_state *)state);
                    user->unit_index = NONE;
                    user->state_ticks = 0;
                }
                else if (!training_is_triggerable(input_user_index, selected_index))
                {
                    state->state = 3;
                    state->ticks = (short)(entry ? player_training_seconds_to_ticks(entry->ticks) : 0);
                }
                break;
            case 3:
                user->state_ticks++;
                if (training_is_satisfied(input_user_index, selected_index))
                {
                    increment_training_completion(selected_index, (s_player_training_user *)user,
                        (const s_player_training_entry_data *)entry, (s_player_training_state *)state);
                    user->unit_index = NONE;
                    user->state_ticks = 0;
                }
                else if (training_is_triggerable(input_user_index, selected_index))
                {
                    state->state = 2;
                    state->ticks = 0;
                }
                else
                {
                    state->ticks--;
                    if (state->ticks <= 0)
                    {
                        state->ticks = 0;
                        state->state = 0;
                        user->unit_index = NONE;
                        user->state_ticks = 0;
                    }
                }
                break;
            case 4:
                state->ticks--;
                if (state->ticks <= 0)
                {
                    state->ticks = 0;
                    state->state = 0;
                }
                break;
            }
        }

        for (long training_index = 0; training_index < k_player_training_count; training_index++)
        {
            if (training_index != user->unit_index
                && training_is_completed(input_user_index, training_index) == 0)
            {
                bool eligible = false;
                switch (training_index)
                {
                case _player_training_fire_secondary_weapon: case _player_training_weapon_kill: case _player_training_grenade_kill:
                case _player_training_headshot_kill: case _player_training_assassination: case _player_training_reload_primary_weapon:
                case _player_training_voice_chat: case _player_training_equipment:
                    eligible = true;
                    break;
                }
                if (eligible && training_is_satisfied(input_user_index, training_index))
                {
                    player_training_chud_entry *entry = get_player_training_entry_data(training_index);
                    s_training_entry *state = &user->entries[training_index];
                    increment_training_completion(training_index, (s_player_training_user *)user,
                        (const s_player_training_entry_data *)entry, (s_player_training_state *)state);
                }
            }
        }
    }

    if (input_user_index != NONE && !campaign)
    {
        s_player_training_globals *globals = (s_player_training_globals *)player_training_globals;
        if (globals->scripted_training_enabled)
        {
            bool has_text = false;
            wchar_t buffer[256];
            buffer[0] = 0;
            long string_index = 0;
            if (globals->show_scripted_training_text && globals->scripted_training_text != 0 && globals->scripted_training_text != NONE)
            {
                has_text = true;
                string_index = globals->scripted_training_text;
            }
            else if (globals->entries[input_user_index].unit_index != NONE)
            {
                s_training_entry *state = &globals->entries[input_user_index].entries[globals->entries[input_user_index].unit_index];
                if (state->state == 2 || state->state == 3)
                {
                    player_training_chud_entry *entry = get_player_training_entry_data(globals->entries[input_user_index].unit_index);
                    if (entry)
                    {
                        if (entry->delay == 0 || entry->delay * game_tick_rate() > globals->entries[input_user_index].state_ticks)
                        {
                            has_text = true;
                            long text_result = 0;
                            player_training_chud_entry *text_entry =
                                get_player_training_entry_data(globals->entries[input_user_index].unit_index);
                            if (text_entry)
                                text_result = text_entry->text;
                            string_index = text_result;
                        }
                    }
                }
            }

            if (has_text)
            {
                wchar_t message[256];
                message[0] = 0;
                chud_get_string(input_user_index, string_index, (c_static_wchar_string<256> *)message);
                ustrnzcpy_inline(buffer, message, 256);
                user_interface_parse_string(input_user_index, buffer, 256);
            }

            if (has_text && buffer[0] != 0)
                chud_set_player_training_text(input_user_index, buffer);
            else
                chud_clear_player_training_text(input_user_index);
        }
        else
        {
            chud_clear_player_training_text(input_user_index);
        }
    }
    else
    {
        chud_clear_player_training_text(input_user_index);
    }
}

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_player_training_globals_allocator> t_player_training_globals_allocator;
t_player_training_globals_allocator g_player_training_globals_allocator;
