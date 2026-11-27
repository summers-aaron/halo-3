// blamlibxbox/game_engine_ctf.obj: from the records 0x2e3498 0x2e34a8 0x2e3528 0x2e35f8 0x2e3748 0x2e3758 0x2e3830 0x2e3888 0x2e3900 0x2e39b0 0x2e3d08 0x2e3e78 0x2e3f48 0x2e3ff0 0x2e42a8 0x2e45a8 0x2e47b8 0x2e4828 0x2e4940 0x2e49a0 0x2e4a30 0x2e4b00 0x2e4b98 0x2e4d98 0x2e4f10 0x2e53c0 0x2e5438 0x2e5520 0x2e5600 0x2e5720 0x2e57c8 0x2e5a48 0x2e5ad0 0x2e5b78 0x2e5b90 0x2e5be8 0x2e5d00 0x2e5f20 0x2e5fb0 0x2e61a8 0x2e62a8 0x2e6390 0x2e6450 0x2e64e8 0x2e6580 0x2e6658 0x2e6748 0x2e6958 0x2e6ad8 0x2e6b68 0x2e6c68 0x2e6d50 0x2e6e48 0x2e6ee0 0x2e7020 0x2e70d0 0x2e7208 0x2e5db0 0x2e4f58
#include "items/item_types.h"
#include "cseries/bit_operations.h"
#include "math/random_math_range.h"
#include "memory/datum_index.h"
#include "memory/data_handle_access.h"
#include "game/game_results_storage_types.h"
#include "cseries/cseries_unicode.h"
#include "memory/tls_globals.h"
#include "interface/chud/chud_user_data_types.h"
#include "simulation/simulation_entity_types.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "game/game_results_event_carry_types.h"
#include "game/game_engine_kill_flags_types.h"
#include "cseries/cseries_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "cseries/cseries_wrapped.h"
#include "tag_files/tag_types.h"
#include "memory/data_types.h"
#include "game/game_results_team_statistics_types.h"
#include "game/game_results_team_data_types.h"
#include "objects/object_datum.h"
#include "game/game_time_types.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "items/multiplayer_weapon_tracker.h"
#include "objects/object_iterator.h"
#include "game/game.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_globals_storage.h"
#include "units/units.h"
#include "game/game_engine_area_set.h"
#include "game/game_engine_ctf.h"
#include "game/game_engine_notifications.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "game/game_results.h"
#include "motor/action_system.h"
#include "game/game_engine_vip.h"
#include "objects/object_placement.h"
#include "networking/network_utilities.h"
#include "physics/collision_types.h"
#include "game/game_engine_default.h"
#include "game/game_engine_event_data.h"
#include "game/game_results_damage_statistic_types.h"
#include "game/game_results_damage_type_description_types.h"
#include "game/game_results_damage_statistics_types.h"
#include "game/game_results_player_vs_player_statistic_types.h"
#include "game/game_results_damage_breakdown_types.h"
#include "game/game_results_statistic_shared_types.h"
#include "cache/cache_files.h"
#include "game/game_results_statistics_types.h"
#include "game/player_mapping_types.h"
#include "cseries/cseries.h"
#include "game/game_engine_area_set_types.h"
#include "scenario/scenario_tag.h"
#include <string.h>
#include <stdlib.h>
#include "interface/chud/chud_navpoint_types.h"
#include "game/game_engine_spawn_influencer.h"
#include "game/game_engine_ctf_types.h"
#include "game/game_engine_ctf_variant.h"
#include "game/game_engine_base_variant.h"
#include "game/game_results_statistic_types.h"
#include "game/game_results_types.h"
#include "game/game_results_player_data.h"
#include "game/game_engine_team_inline.h"
#include "game/game_engine_area_initialization.h"
#include "game/player_in_game_iterator.h"
#include "cseries/crt_xdk.h"
#include "game/game_engine_interface_state_types.h"
#include "game/game_engine_ctf_externs.h"
#include "scenario/scenario.h"
#include "game/player_with_unit_iterator.h"
#include "game/game_engine_list.h"
#include "game/game_engine_slayer_events.h"
#include "physics/collisions.h"
#include "game/game_time_inline.h"
#include "game/game_globals.h"
#include "game/multiplayer_constants.h"
#include "game/universal_types.h"
#include "game/game_engine_settings_definitions.h"
#include "game/multiplayer_runtime_globals.h"
#include "game/game_engine_end_condition.h"
#include "memory/static_arrays.h"
#include <stddef.h>

// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x2e53c0 ?get_defending_team_index@c_ctf_engine@@UBA?AW4e_game_team@@XZ
// @address 0x2e6ad8 ?team_flag_is_at_home@c_ctf_engine@@SA_NW4e_multiplayer_team_designator@@PAJ@Z

#ifndef GAME_ENGINE_CTF_UNIT_H
#define GAME_ENGINE_CTF_UNIT_H


#define UNIT_PAD(a, b) char pad_##a[(b) - (a)]
enum { k_maximum_teams = 8, k_ctf_flag_count = 9 };

/* ---- data arrays ---------------------------------------------------------- */

/* ---- c_area / c_area_set<c_area,3> (0x13c) -------------------------------- */

struct s_ctf_game_engine_globals {
    UNIT_PAD(0, 0xc);
    word   active_teams;              /* game_engine_next_active_team mask */
    UNIT_PAD(0xe, 0x10);
    short team_indices[9];
    UNIT_PAD(0x22, 0xe10a);
    short round_count_e10a;
    UNIT_PAD(0xe10c, 0xe18c);
    c_area_set<c_area, 3> goal_areas[9];         /* (player_in_enemy_area) */
    c_area_set<c_area, 3> flag_spawn_areas[9];   /* (get_flag_spawn_position) */
    bool   suppress_returned_event;
    bool   flag_returned;
    UNIT_PAD(0xf7c6, 0xf7c8);
    long   defending_team;            /* dev shared+4828 */
    long   sudden_death_timer;
    long ending_timer;
    word   flags_f7d4;
    word   variant_flags;             /* dev shared+4842: bit0 side switch, bit1 ... */
    long   flag_object_index[9];      /* dev shared+4844 */
    short  timers_f7fc[9];            /* dev shared+4880 */
    UNIT_PAD(0xf80e, 0xf810);
    long   flag_emblem_player[9];     /* dev shared+4900 */
    byte   bytes_f834[9];
    UNIT_PAD(0xf83d, 0xf83e);
    short  shorts_f83e[9];            /* dev shared+4946 */
    short  shorts_f850[9];
    UNIT_PAD(0xf862, 0x13d4c);
    long   game_engine_index;
    long   multiplayer_weapon_count;
    s_multiplayer_weapon_tracker multiplayer_weapons[8];
};

extern "C" {
}
struct s_game_engine_globals;
extern "C" {
}
// match: 0x2e4f58: direct TLS access retains the shared-state base materialization.
#define ctf_state ((s_ctf_game_engine_globals *)game_engine_globals)
extern "C" {
  /* ctf_state */
}
struct scenario;
#define global_scenario_get() global_scenario







struct s_ctf_weapon_definition { UNIT_PAD(0, 0x404); short multiplayer_weapon_type; };

class c_ctf_area_object_iterator : public c_object_iterator_base {
public:
    __forceinline void begin(unsigned long type_mask, unsigned long match_flags)
    {
        object_iterator_begin_internal(type_mask, 0, match_flags);
    }
    __forceinline bool next() { return object_iterator_next_with_match_flags_internal(); }
};

struct s_ctf_object_owner_data {
    UNIT_PAD(0, 0xa);
    unsigned short team_designator;
};

struct s_ctf_unit_index { long value; };




struct s_ctf_shared_globals
{
    UNIT_PAD(0, 0x1640);
    long sudden_death_timer;
    long ending_timer;
    word flags;
};

namespace {
struct s_ctf_object_datum {
    long definition_index;
    unsigned long object_flags;
    UNIT_PAD(0x8, 0x10);
    long parent_object_index;
    byte location;
    UNIT_PAD(0x15, 0x20);
    real_point3d position;
    UNIT_PAD(0x2c, 0x94);
    c_object_identifier identifier;
    UNIT_PAD(0x9c, 0x154);
    short multiplayer_data_offset;
    UNIT_PAD(0x156, 0x161);
    bool item_in_inventory;
    UNIT_PAD(0x162, 0x16c);
    long item_inventory_unit_index;
    UNIT_PAD(0x170, 0x17c);
    long unit_player_index;
    UNIT_PAD(0x180, 0x186);
    short multiplayer_weapon_identifier;
    UNIT_PAD(0x188, 0x246);
    word parent_seat_index;
    UNIT_PAD(0x248, 0x25a);
    signed char current_weapon;
    UNIT_PAD(0x25b, 0x260);
    long weapons[4];
};
}


static __forceinline long game_engine_next_active_team(long team)
{
    long result = -1;
    for (long i = team + 1; i < k_maximum_teams; i++) {
        if (TEST_BIT(ctf_state->active_teams, i)) {
            result = i;
            break;
        }
    }
    return result;
}
#endif

static __forceinline s_ctf_object_datum *ctf_object_get(long object_index)
{
    return (s_ctf_object_datum *)((s_object_header *)tls_object_header_data()->data)[datum_index_to_absolute_index(object_index)].datum;
}
static __forceinline player_datum *ctf_player_get(long player_index)
{
    return &((player_datum *)tls_player_data()->data)[datum_index_to_absolute_index(player_index)];
}
static __forceinline bool side_switch_ctf() { return (ctf_state->variant_flags >> 0) & 1; }
static __forceinline bool neutral_flag_ctf() { return (ctf_state->variant_flags >> 1) & 1; }
static __forceinline bool game_engine_ctf_is_predicted()
{
    bool result = false;
    if (tls_game_globals()->options.game_simulation == 4)
        result = true;
    return result;
}
/* the neutral flag (8) has no team */
static __forceinline long ctf_flag_identifier_to_team_index(short identifier)
{
    return identifier == 8 ? -1 : identifier;
}



static __forceinline long ctf_team_statistic_get(e_game_team team, long statistic)
{
    long value = -1;
    if (team >= 0 && team < 16U)
        value = g_current_game_results.statistics.team[team].statistics[statistic].value;
    return value;
}
static __forceinline long game_results_statistic_get_current(long player, e_game_team team, long statistic)
{
    long result = -1;
    if (g_game_results_globals.active)
        result = ctf_team_statistic_get(team, statistic);
    return result;
}

static __forceinline s_ctf_game_engine_globals *ctf_globals_get() { return ctf_state; }


template<typename T, unsigned int N>
static __forceinline T &ctf_array_element(T (&array)[N], long index)
{
    return array[index];
}


// @address 0x8c43bc ctf_variant
static const c_game_engine_ctf_variant *ctf_variant;

// @address 0x41020 ??_7c_ctf_engine@@6B@
// @address 0x79d420 ?internal_ctf_engine@@3Vc_ctf_engine@@A
c_ctf_engine internal_ctf_engine;

// @address 0x42d380 ?get_type@c_ctf_engine@@UBAJXZ
long c_ctf_engine::get_type() const
{
    return _game_engine_type_ctf;
}

// @address 0x42d380 ?get_simulation_entity_type@c_ctf_engine@@UBA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_ctf_engine::get_simulation_entity_type() const
{
    return _simulation_entity_type_ctf_engine_globals;
}

// @address 0x42d298 ?leader_event_enabled@c_ctf_engine@@UBA_NXZ
bool c_ctf_engine::leader_event_enabled() const
{
    return false;
}

// @address 0x2e3498 ?get_sudden_death_time@c_ctf_engine@@UBAFXZ
short c_ctf_engine::get_sudden_death_time() const
{
    return ctf_variant->m_option_e;
}

// @address 0x2e34a8 ?initialize_object_data@c_ctf_engine@@UBAXJ@Z
void c_ctf_engine::initialize_object_data(long identifier) const
{
    ctf_array_element(ctf_globals_get()->timers_f7fc, identifier) = -1;
    ctf_array_element(ctf_globals_get()->shorts_f850, identifier) = 0;
    simulation_action_game_engine_globals_update(512);
    ctf_array_element(ctf_globals_get()->flag_object_index, identifier) = -1;
    simulation_action_game_engine_globals_update(256);
    ctf_array_element(ctf_globals_get()->bytes_f834, identifier) = 0;
    simulation_action_game_engine_globals_update(1024);
}

// @address 0x2e3528 ?initialize_for_new_round@c_ctf_engine@@UBA_NXZ
bool c_ctf_engine::initialize_for_new_round() const
{
    for (long team = 0; team < 9; team++) {
        ctf_array_element(ctf_globals_get()->goal_areas, team).initialize(0x440, 0x400, (e_multiplayer_team_designator)team);
        ctf_array_element(ctf_globals_get()->flag_spawn_areas, team).initialize(0x440, 0x100, (e_multiplayer_team_designator)team);
    }
    ctf_globals_get()->flags_f7d4 &= ~2;
    ctf_globals_get()->flags_f7d4 &= ~4;
    ctf_globals_get()->sudden_death_timer = 0;
    simulation_action_game_engine_globals_update(2048);
    ctf_globals_get()->flag_returned = 0;
    return true;
}

void c_ctf_engine::dispose_from_old_map() const
{
    ctf_variant = 0;
}


// @address 0x2e35f8 ?initialize_for_new_map@c_ctf_engine@@UBA_NXZ
bool c_ctf_engine::initialize_for_new_map() const
{
    game_options *options = &tls_game_globals()->options;
    const c_game_variant *variant = options->game_mode == 2 ? &options->game_variant : 0;
    variant->get_active_variant()->can_be_cast_to(_game_engine_ctf, (const void **)&ctf_variant);

    s_ctf_game_engine_globals *globals = ctf_globals_get();
    c_area_set<c_area, 3> *team_areas = globals->goal_areas;
    if (team_areas != 0)
        game_engine_initialize_team_area_sets(team_areas);
    // Zero the area sets and per-flag state through shorts_f850 (size is 8-aligned).
    memset(team_areas, 0, offsetof(s_ctf_game_engine_globals, shorts_f850) + sizeof(globals->shorts_f850) + 2
        - offsetof(s_ctf_game_engine_globals, goal_areas));
    for (long i = 0; i < 9; i++)
        globals->timers_f7fc[i] = -1;
    for (long i = 0; i < 9; i++)
        globals->flag_object_index[i] = -1;
    for (long i = 0; i < 9; i++)
        globals->shorts_f83e[i] = -1;
    for (long i = 0; i < 9; i++)
        globals->flag_emblem_player[i] = -1;

    signed char game_type = (signed char)ctf_variant->m_option_b.get();
    if (game_type != 1) {
        if (game_type == 2)
            globals->variant_flags |= 2;
    } else {
        globals->variant_flags |= 1;
    }
    return true;
}

// @address 0x2e3758 ?game_starting@c_ctf_engine@@UBAXXZ
void c_ctf_engine::game_starting() const
{
    if (game_engine_ctf_is_predicted())
        return;
    if (side_switch_ctf()) {
        ctf_globals_get()->defending_team = (e_game_team)game_engine_next_active_team(-1);
        simulation_action_game_engine_globals_update(64);
    }
    create_all_flags();
    ctf_globals_get()->sudden_death_timer = 0;
}

// @address 0x24fc40 ?game_engine_send_event@@YAXPAUs_game_engine_event_data@@@Z
// @address 0x2e6d50 ?team_near_object@@YA_NJM_N@Z
static __forceinline void game_engine_ctf_initialize_event(long event_type, long type, s_game_engine_event_data *event_data)
{
    event_data->type = event_type;
    event_data->event = type;
    event_data->identifier = -1;
    event_data->flags = -1;
    event_data->cause_player_index = -1;
    event_data->cause_team_index = -1;
    event_data->effect_player_index = -1;
    event_data->effect_team_index = -1;
    event_data->data = 0;
    event_data->data_word = -1;
}
static __forceinline void game_engine_ctf_set_event_cause_player_and_team(long player_index, s_game_engine_event_data *event_data)
{
    event_data->cause_player_index = player_index;
    event_data->cause_team_index = ctf_player_get(player_index)->configuration.host.team_index;
}
// @address 0x2e3830 ?emit_game_start_event@c_ctf_engine@@UBAXJ@Z
void c_ctf_engine::emit_game_start_event(long player_index) const
{
    s_game_engine_event_data event;
    game_engine_ctf_initialize_event(3, 0, &event);
    event.flags = player_index;
    game_engine_send_event(&event);
}

static __forceinline e_game_team game_engine_ctf_flag_identifier_to_team_index(short identifier)
{
    if (identifier == 8)
        return _game_team_none;
    return (e_game_team)identifier;
}

// @address 0x2e3888 ?emit_object_returned_event@c_ctf_engine@@UBAXJ_N@Z
void c_ctf_engine::emit_object_returned_event(long identifier, bool returned) const
{
    s_game_engine_event_data event;
    game_engine_ctf_initialize_event(3, (returned == false) + 4, &event);
    event.effect_team_index = game_engine_ctf_flag_identifier_to_team_index((short)identifier);
    game_engine_send_event(&event);
}

// @address 0x2e3900 ?player_activated@c_ctf_engine@@UBAXJ@Z
void c_ctf_engine::player_activated(long player_index) const
{
    player_datum *player = ctf_player_get(player_index);
    if (player->configuration.host.team_index >= 0 && player->configuration.host.team_index < 8 && game_engine_get_round() == 0) {
        emit_game_start_event(player_index);
        if (side_switch_ctf())
            side_switch_ctf_generate_side_event(player_index);
    }
}



static __forceinline long game_engine_ctf_current_weapon(const s_ctf_object_datum *unit)
{
    long weapon = NONE;
    if (unit->current_weapon != NONE)
        weapon = unit->weapons[unit->current_weapon];
    return weapon;
}
static __forceinline void game_engine_ctf_insert_score_event(long player_index, long identifier)
{
    if (g_game_results_globals.active && !g_game_results_globals.finished) {
        s_game_results_event event;
        memset(event.player_indices, 0, sizeof(event) - offsetof(s_game_results_event, player_indices));
        event.type = 3;
        event.player_indices[0] = (signed char)player_index;
        event.player_indices[1] = NONE;
        unsigned long seconds = (unsigned long)_time64(0);
        event.carry.carry_type = 1;
        event.carry.carry_value = identifier;
        event.carry.time = seconds - g_current_game_results.end_reason;
        if (game_results_get_player_position(player_index, &event.carry.score_position))
            game_results_insert_event(&event);
    }
}
static __forceinline long game_engine_ctf_inventory_weapon(long unit_index, short slot)
{
    long weapon_index = NONE;
    if (slot != NONE)
        weapon_index = ctf_object_get(unit_index)->weapons[slot];
    return weapon_index;
}

// @address 0x2e39b0 ?process_player_holding_object@c_ctf_engine@@UBAXJJ@Z
void c_ctf_engine::process_player_holding_object(long player_index, long identifier) const
{
    long player_team = 0;
    player_datum *player = ctf_player_get(player_index);
    long weapon_index = game_engine_ctf_inventory_weapon(player->unit_index, ctf_object_get(player->unit_index)->current_weapon);
    e_multiplayer_team_designator team = game_engine_team_index_to_team_designator((e_game_team)player->configuration.host.team_index);
    s_ctf_game_engine_globals *globals = ctf_globals_get();
    if (globals->goal_areas[team].player_in_set(player_index, 0)) {
        bool can_score = true;
        if (((byte)ctf_variant->m_flags.m_storage) & 1) {
            player_team = player->configuration.host.team_index;
            if (game_engine_multiplayer_weapon_get((short)player_team))
                can_score = (globals->bytes_f834[player_team] & 1) == 0;
            else
                can_score = false;
        }
        if (can_score) {
            e_game_team flag_team = game_engine_ctf_flag_identifier_to_team_index((short)identifier);
            game_engine_ctf_insert_score_event(player_index, identifier);
            player_score(player_index, flag_team);
            flag_reset(weapon_index, false);
            initialize_object_data(identifier);
            if (ctf_variant->m_option_c != 0) {
                c_player_in_game_iterator iterator;
                iterator.begin();
                while (iterator.next()) {
                    player_datum *other = iterator.get_datum();
                    if ((other->flags & 2) == 0 && (other->flags & 1) && other->unit_index == NONE) {
                        bool respawn = false;
                        switch (ctf_variant->m_option_c) {
                        case 1: if (other->configuration.host.team_index == player->configuration.host.team_index) respawn = true; break;
                        case 2: if (other->configuration.host.team_index != player->configuration.host.team_index) respawn = true; break;
                        case 3: respawn = true; break;
                        }
                        if (respawn)
                            player_force_spawn(iterator.get_index());
                    }
                }
            }
        } else if ((word)globals->shorts_f83e[player->configuration.host.team_index] == 0xffff) {
            s_game_engine_event_data event;
            game_engine_ctf_initialize_event(3, 14, &event);
            game_engine_ctf_set_event_cause_player_and_team(player_index, &event);
            event.effect_team_index = player->configuration.host.team_index;
            game_engine_send_event(&event);
            short ticks = tls_game_time_globals()->ticks_per_second;
            ctf_globals_get()->shorts_f83e[player->configuration.host.team_index] = ticks * 5;
        }
    }
}


// @address 0x2e3e78 ?update_flag_emblem@@YAXJ@Z
void update_flag_emblem(long weapon_index)
{
    int leading_player = -1;
    int leading_score = (int)0x80000000;
    c_player_in_game_iterator iterator;
    iterator.begin();
    int flag_team = ctf_object_get(weapon_index)->multiplayer_weapon_identifier;
    while ((unsigned char)iterator.next() != 0) {
        if (flag_team == 8 || iterator.get_datum()->configuration.host.team_index == flag_team) {
            int player_index = iterator.get_index();
            int player_score = game_engine_get_team_score(player_index);
            if (player_score > leading_score) {
                leading_player = player_index;
                leading_score = player_score;
            }
        }
    }
    if ((unsigned int)flag_team <= 8)
        ctf_state->flag_emblem_player[flag_team] = leading_player;
}

// @address 0x2e3f48 game_engine_ctf_update_sudden_death_state
extern "C" void game_engine_ctf_update_sudden_death_state(long player_index, game_engine_interface_state *state)
{
    bool sudden_death = (ctf_globals_get()->flags_f7d4 & 2) != 0;
    if (sudden_death && (ctf_globals_get()->flags_f7d4 & 4) == 0)
    {
        state->sudden_death = true;
        if (static_cast<const c_ctf_engine *>(current_game_engine())->get_sudden_death_time() < 0)
            state->sudden_death_active = false;
        else if ((ctf_globals_get()->flags_f7d4 & 4) != 0)
        {
            state->sudden_death = false;
            state->sudden_death_active = true;
        }
    }
}





static __forceinline const s_multiplayer_constants *game_engine_ctf_get_constants()
{
    const cache_file_tag_instance *tags = global_tag_instances;
    const s_multiplayer_globals_definition *multiplayer_globals = (const s_multiplayer_globals_definition *)
        tags[datum_index_to_absolute_index(global_game_globals->multiplayer_globals_index)].definition;
    const s_multiplayer_runtime_globals_definition *runtime =
        (const s_multiplayer_runtime_globals_definition *)multiplayer_globals->runtime.address;
    return (const s_multiplayer_constants *)runtime->multiplayer_constants.address;
}
static __forceinline bool game_engine_ctf_object_is_outside_map(const s_ctf_object_datum *weapon)
{
    return (weapon->object_flags >> 8) & 1;
}


static __forceinline const s_multiplayer_universal_globals_definition *game_engine_ctf_get_universal()
{
    const cache_file_tag_instance *tags = global_tag_instances;
    const s_multiplayer_globals_definition *multiplayer_globals = (const s_multiplayer_globals_definition *)
        tags[datum_index_to_absolute_index(global_game_globals->multiplayer_globals_index)].definition;
    return (const s_multiplayer_universal_globals_definition *)multiplayer_globals->universal.address;
}

static __forceinline unsigned long game_engine_ctf_color_to_pixel32(const real_rgb_color *color)
{
    unsigned long red = (long)(color->red * 255.0f);
    unsigned long green = (long)(color->green * 255.0f);
    unsigned long blue = (long)(color->blue * 255.0f);
    return ((((0xff00 | red) << 8) | green) << 8) | blue;
}
// @address 0x2e3ff0 ?get_hud_interface_state@c_ctf_engine@@UBAXJPAUgame_engine_interface_state@@@Z
void c_ctf_engine::get_hud_interface_state(long player_index, game_engine_interface_state *state) const
{
    long team = ctf_player_get(player_index)->configuration.host.team_index;
    const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)team);
    game_engine_ctf_update_sudden_death_state(player_index, state);
    if (tracker) {
        const s_multiplayer_universal_globals_definition *universal = game_engine_ctf_get_universal();
        const s_multiplayer_constants *constants = game_engine_ctf_get_constants();
        real_point3d position;
        object_get_origin(tracker->weapon_index, &position);
        if (ctf_array_element(ctf_globals_get()->flag_object_index, (short)team) > 0 &&
            player_within_distance_of_position(player_index, &position,
                constants->flag_touch_return_radius * constants->flag_touch_return_radius)) {
            c_static_wchar_string<256> text;
            string_list_get_normal_string(universal->strings_index, 0x40006, &text);
            usnzprintf(state->flag_return_text, 64, text.get_string());
            state->flag_return_visible = true;
            state->flag_return_progress = (float)ctf_array_element(ctf_globals_get()->flag_object_index, (short)team) /
                (float)(tls_game_time_globals()->ticks_per_second * ((short)ctf_variant->m_option_d));
            real_rgb_color color = network_get_team_color((e_game_team)team);
            state->flag_return_color = game_engine_ctf_color_to_pixel32(&color);
        }
    }
    tracker = game_engine_multiplayer_weapon_get((short)team);
    if (tracker) {
        if (tracker->carrier_player_index != NONE)
            state->ctf_flags |= 2;
        else if (ctf_globals_get()->bytes_f834[team] & 1)
            state->ctf_flags |= 4;
    }
    for (long identifier = 0; identifier < 9; identifier++) {
        const s_multiplayer_weapon_tracker *flag = game_engine_multiplayer_weapon_get((short)identifier);
        if (flag && flag->carrier_player_index != NONE) {
            player_datum *carrier = ctf_player_get(flag->carrier_player_index);
            if (carrier && carrier->configuration.host.team_index == team) {
                state->ctf_flags |= 1;
                break;
            }
        }
    }
}

// @address 0x2e42a8 ?update_object@c_ctf_engine@@UBA_NPBUs_multiplayer_weapon_tracker@@@Z
bool c_ctf_engine::update_object(const s_multiplayer_weapon_tracker *tracker) const
// match: 0x2e42a8: value range assumption guides branch lowering
{ __assume(this != 0);
    long touch_return_ticks = ((short)ctf_variant->m_option_d) * tls_game_time_globals()->ticks_per_second;
    long reset_timeout = ctf_variant->m_option_g;
    s_ctf_object_datum *weapon = ctf_object_get(tracker->weapon_index);
    long identifier = weapon->multiplayer_weapon_identifier;
    bool reset = false;
    if ((unsigned long)identifier <= 8) {
        bool carried = tracker->carrier_player_index != NONE;
        bool at_home = (ctf_globals_get()->bytes_f834[identifier] & 1) == 0;
        if (!game_engine_ctf_is_predicted()) {
            if (carried)
                game_results_insert_carry_event(tracker->carrier_player_index, _game_results_event_carry_type_ctf, tracker->id);
            if (carried || at_home) {
                if ((word)ctf_array_element(ctf_globals_get()->timers_f7fc, identifier) != 0xffff) {
                    ctf_array_element(ctf_globals_get()->shorts_f850, identifier) = -1;
                    ctf_array_element(ctf_globals_get()->timers_f7fc, identifier) = -1;
                    simulation_action_game_engine_globals_update(512);
                }
            }
            if (!at_home) {
                if (touch_return_ticks >= 0) {
                    if (team_near_object(tracker->weapon_index,
                        game_engine_ctf_get_constants()->flag_touch_return_radius, true) && !carried) {
                        ++ctf_array_element(ctf_globals_get()->flag_object_index, identifier);
                        if (ctf_array_element(ctf_globals_get()->flag_object_index, identifier) >= touch_return_ticks) {
                            ctf_globals_get()->flag_returned = true;
                            reset = true;
                        }
                        simulation_action_game_engine_globals_update(256);
                    } else if (ctf_array_element(ctf_globals_get()->flag_object_index, identifier) > 0) {
                        --ctf_array_element(ctf_globals_get()->flag_object_index, identifier);
                        simulation_action_game_engine_globals_update(256);
                    }
                }
                if (!carried && reset_timeout > 0) {
                    ++ctf_array_element(ctf_globals_get()->shorts_f850, identifier);
                    if (ctf_array_element(ctf_globals_get()->shorts_f850, identifier) >= tls_game_time_globals()->ticks_per_second) {
                        if (ctf_array_element(ctf_globals_get()->timers_f7fc, identifier) < 255)
                            ++ctf_array_element(ctf_globals_get()->timers_f7fc, identifier);
                        simulation_action_game_engine_globals_update(512);
                        ctf_array_element(ctf_globals_get()->shorts_f850, identifier) = -1;
                        if (ctf_array_element(ctf_globals_get()->timers_f7fc, identifier) >= reset_timeout)
                            reset = true;
                    }
                }
                if (reset && !game_engine_ctf_object_is_outside_map(weapon) && weapon->location == 0xff)
                    reset = false;
            }
            update_flag_emblem(tracker->weapon_index);
        }
    }
    if (ctf_array_element(ctf_globals_get()->shorts_f83e, identifier) > 0)
        --ctf_array_element(ctf_globals_get()->shorts_f83e, identifier);
    else
        ctf_array_element(ctf_globals_get()->shorts_f83e, identifier) = -1;
    return reset;
}

// @address 0x2e47b8 ?object_get_emblem_player@c_ctf_engine@@UBAJJ@Z
long c_ctf_engine::object_get_emblem_player(long object_index) const
{
    long result = -1;
    s_ctf_object_datum *object = ctf_object_get(object_index);
    if ((1 << object->identifier.get_type() & 4) != 0) {
        s_ctf_object_datum *weapon = object;
        if (weapon->multiplayer_weapon_identifier >= 0 && weapon->multiplayer_weapon_identifier < 9)
            result = ctf_globals_get()->flag_emblem_player[weapon->multiplayer_weapon_identifier];
    }
    return result;
}
// @address 0x2e4940 ?player_killed_player_get_kill_message@c_ctf_engine@@UBAJJJ_N@Z
long c_ctf_engine::player_killed_player_get_kill_message(long killer_player_index, long killed_player_index, bool is_betrayal) const
{
    int result = -1;
    if (killer_player_index != -1 && (int)killed_player_index != -1 && is_betrayal == 0 &&
        game_engine_get_carried_multiplayer_weapon_index((long)killed_player_index, 1) != -1) {
        result = 0x28;
    }
    return result;
}

// @address 0x2e49a0 ?assemble_spawn_influencers_for_player@c_ctf_engine@@UBAXJPAUs_netgame_goal_influencer@@PAJ@Z
void c_ctf_engine::assemble_spawn_influencers_for_player(long player_index, s_netgame_goal_influencer *influencers,
    long *influencer_count) const
{
    *influencer_count = 0;
    for (long team = 0; team < 9; team++) {
        long weapon_index = -1;
        bool flag_at_home = team_flag_is_at_home((e_multiplayer_team_designator)team, &weapon_index);
        if (flag_at_home && weapon_index != -1) {
            influencers[*influencer_count].type = 12;
            influencers[*influencer_count].object_index = weapon_index;
            *influencer_count = *influencer_count + 1;
            if (*influencer_count == 16)
                break;
        }
    }
}
// @address 0x2e4a30 ?compare_teams@c_ctf_engine@@UBAJW4e_game_team@@0@Z
long c_ctf_engine::compare_teams(e_game_team team_a, e_game_team team_b) const
{
    long result = 0;
    if (::game_engine_is_team_active(team_a) && ::game_engine_is_team_active(team_b))
        result = game_results_statistic_get_current(-1, team_b, 16) - game_results_statistic_get_current(-1, team_a, 16);
    return result;
}
// @address 0x2e4b00 ?multiplayer_weapon_register@c_ctf_engine@@UBAXJ@Z
static __forceinline void game_engine_ctf_flag_update_color(long weapon_index)
{
    s_ctf_object_datum *weapon = ctf_object_get(weapon_index);
    if (weapon->multiplayer_weapon_identifier != -1) {
        long team = (weapon->multiplayer_weapon_identifier >= 0 && weapon->multiplayer_weapon_identifier < k_maximum_teams) ?
            weapon->multiplayer_weapon_identifier : -1;
        real_rgb_color color = network_get_team_color((e_game_team)team);
        object_set_base_change_color_by_index(weapon_index, 0, &color);
    }
}

void c_ctf_engine::multiplayer_weapon_register(long weapon_index) const
{
    game_engine_ctf_flag_update_color(weapon_index);
}

static __forceinline short ctf_weapon_get_multiplayer_type(long weapon_index)
{
    const s_ctf_weapon_definition *definition =
        (const s_ctf_weapon_definition
            *)global_tag_instances[datum_index_to_absolute_index(ctf_object_get(weapon_index)->definition_index)].definition;
    return definition->multiplayer_weapon_type;
}
// @address 0x2e4b98 ?multiplayer_weapon_picked_up@c_ctf_engine@@UBAXJJ@Z
void c_ctf_engine::multiplayer_weapon_picked_up(long weapon_index, long unit_index) const
{
    if (!game_engine_ctf_is_predicted() && ctf_weapon_get_multiplayer_type(weapon_index) == 1) {
        s_ctf_object_datum *weapon = ctf_object_get(weapon_index);
        s_ctf_object_datum *unit = ctf_object_get(unit_index);
        long flag_index = weapon->multiplayer_weapon_identifier;
        if (flag_index != -1) {
            bool stolen = false;
            if (unit->unit_player_index != -1) {
                player_datum *player = ctf_player_get(unit->unit_player_index);
                if (!TEST_BIT(ctf_globals_get()->bytes_f834[flag_index], 0)) {
                    game_results_statistic_increment(datum_index_to_absolute_index(unit->unit_player_index),
                        (e_game_team)player->configuration.host.team_index,
                        _game_results_statistic_ctf_flag_grabs, 1);
                    stolen = true;
                }
            }
            ctf_globals_get()->bytes_f834[flag_index] |= 1;
            simulation_action_game_engine_globals_update(0x400);
            ctf_globals_get()->flag_object_index[flag_index] = -1;
            simulation_action_game_engine_globals_update(0x100);
            if (flag_index >= 0 && flag_index < 9) {
                ctf_globals_get()->timers_f7fc[flag_index] = -1;
                ctf_globals_get()->shorts_f850[flag_index] = 0;
                simulation_action_game_engine_globals_update(0x200);
            }
            if (unit->unit_player_index != -1 && flag_index >= 0 && flag_index < 9) {
                s_game_engine_event_data event_data;
                game_engine_ctf_initialize_event(3, stolen ? 2 : 1, &event_data);
                game_engine_ctf_set_event_cause_player_and_team(unit->unit_player_index, &event_data);
                event_data.effect_team_index = ctf_flag_identifier_to_team_index((short)flag_index);
                game_engine_send_event(&event_data);
            }
        }
    }
}
// @address 0x2e4d98 ?multiplayer_weapon_dropped@c_ctf_engine@@UBAXJJ@Z
void c_ctf_engine::multiplayer_weapon_dropped(long weapon_index, long unit_index) const
{
    if (!game_engine_ctf_is_predicted() && ctf_weapon_get_multiplayer_type(weapon_index) == 1) {
        s_ctf_object_datum *weapon = ctf_object_get(weapon_index);
        long flag_index = weapon->multiplayer_weapon_identifier;
        if (flag_index >= 0 && flag_index < 9) {
            s_ctf_object_datum *unit = ctf_object_get(unit_index);
            if (TEST_BIT(ctf_globals_get()->bytes_f834[flag_index], 0)) {
                ctf_globals_get()->timers_f7fc[flag_index] = -1;
                ctf_globals_get()->shorts_f850[flag_index] = 0;
            } else {
                ctf_globals_get()->timers_f7fc[flag_index] = -1;
                ctf_globals_get()->shorts_f850[flag_index] = 0;
            }
            simulation_action_game_engine_globals_update(0x200);
            if (unit->unit_player_index != -1 && !TEST_BIT(ctf_globals_get()->flags_f7d4, 0)) {
                s_game_engine_event_data event_data;
                game_engine_ctf_initialize_event(3, 3, &event_data);
                game_engine_ctf_set_event_cause_player_and_team(unit->unit_player_index, &event_data);
                event_data.effect_team_index = ctf_flag_identifier_to_team_index((short)flag_index);
                game_engine_send_event(&event_data);
            }
        }
    }
}
// @address 0x2e4f10 ?is_team_near_object@c_ctf_engine@@UBA_NJM@Z
bool c_ctf_engine::is_team_near_object(long object_index, float distance) const
{
    bool result = false;
    bool (*near_object)(long, float, bool) = team_near_object;
    if (near_object(object_index, distance, false))
        result = true;
    return result;
}
e_game_team c_ctf_engine::get_defending_team_index() const
{
    e_game_team result = _game_team_none;
    if (global_scenario_get()->flags & 8)
        result = (e_game_team)game_engine_next_active_team(-1);
    else
        result = (e_game_team)ctf_state->defending_team;
    return result;
}
// @address 0x2e5438 ?static_spawn_zone_is_valid_for_player@c_ctf_engine@@UBA_NJPBUs_static_spawn_zone@@@Z
bool c_ctf_engine::static_spawn_zone_is_valid_for_player(long player_index, const s_static_spawn_zone *zone) const
{
    bool result = true;
    if ((zone->flags & 1) != 0 || (zone->flags & 2) != 0) {
        player_datum *player = ctf_player_get(player_index);
        long team_index = side_switch_ctf() ? ctf_globals_get()->defending_team : player->configuration.host.team_index;
        const s_multiplayer_weapon_tracker *weapon = game_engine_multiplayer_weapon_get((short)team_index);
        if (weapon != 0) {
            bool away = (ctf_globals_get()->bytes_f834[team_index] & 1) == 0;
            if (((zone->flags & 1) != 0 && away) || ((zone->flags & 2) != 0 && !away))
                result = false;
        }
    }
    return result;
}
static __forceinline long ctf_object_get_ultimate_parent(long object_index)
{
    long ultimate_parent_index = NONE;
    while (object_index != NONE)
    {
        ultimate_parent_index = object_index;
        object_index = ctf_object_get(object_index)->parent_object_index;
    }
    return ultimate_parent_index;
}
// @address 0x2e5520 ?allow_weapon_pickup@c_ctf_engine@@UBA_NJJ@Z
bool c_ctf_engine::allow_weapon_pickup(long player_index, long object_index) const
{
    bool result = true;
    if (ctf_weapon_get_multiplayer_type(object_index) != 1)
        result = c_game_engine::allow_weapon_pickup(player_index, object_index);
    if (result && player_index != NONE && object_index != NONE) {
        long identifier = ctf_object_get(object_index)->multiplayer_weapon_identifier;
        if (ctf_weapon_get_multiplayer_type(object_index) == 1) {
            if (identifier != NONE && identifier != 8 && identifier == ctf_player_get(player_index)->configuration.host.team_index)
                result = false;
        }
    }
    return result;
}

// @address 0x2e5600 ?player_is_helping_flag_carrier@@YA_NJ@Z
bool player_is_helping_flag_carrier(long player_index)
{
    player_datum *player = ctf_player_get(player_index);
    bool helping = false;
    if (player->unit_index != NONE)
    {
        long ultimate_parent_index = ctf_object_get_ultimate_parent(player->unit_index);
        if (ultimate_parent_index != player->unit_index)
        {
            c_player_with_unit_iterator iterator;
            iterator.begin();
            while (iterator.next())
            {
                if (iterator.get_index() != player_index &&
                    iterator.get_datum()->configuration.host.team_index == player->configuration.host.team_index &&
                    game_engine_get_carried_multiplayer_weapon_index(iterator.get_index(), 1) != NONE &&
                    iterator.get_datum()->unit_index != NONE &&
                    ctf_object_get_ultimate_parent(iterator.get_datum()->unit_index) == ultimate_parent_index)
                {
                    helping = true;
                }
            }
        }
    }
    return helping;
}
// @address 0x2e5720 game_engine_ctf_team_has_flag_carrier
extern "C" bool game_engine_ctf_team_has_flag_carrier(long team_index)
{
    bool result = false;
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next())
    {
        if (iterator.get_datum()->configuration.host.team_index == team_index &&
            game_engine_get_carried_multiplayer_weapon_index(iterator.get_index(), 1) != NONE)
        {
            result = true;
            break;
        }
    }
    return result;
}
static __forceinline bool ctf_player_has_flag(long player_index)
{
    return game_engine_get_carried_multiplayer_weapon_index(player_index, 1) != NONE;
}

static __forceinline long ctf_player_index_from_user_index(long user_index)
{
    long player_index = NONE;
    if (user_index != NONE)
        player_index = tls_player_mapping_globals()->player_indices[user_index];
    return player_index;
}



static __forceinline void ctf_chud_submit_navpoint(long user_index, s_chud_navpoint *navpoint)
{
    if ((unsigned long)user_index <= 3)
        chud_persistent_user_data()[user_index].m_navpoint_manager.submit_navpoint(navpoint);
}
static __forceinline void ctf_engine_build_goal_nav_point(s_chud_navpoint *navpoint, long object_index, bool has_flag,
    const real_point3d *position)
{
    navpoint->object_index = object_index;
    navpoint->flags = 193;
    navpoint->type = 2;
    navpoint->icon = -1;
    navpoint->scale = 1.0f;
    navpoint->state = !has_flag;
    navpoint->name[0] = 0;
    navpoint->position = *position;
}

// @address 0x2e57c8 ?submit_nav_points@c_ctf_engine@@UBAXJJ@Z
void c_ctf_engine::submit_nav_points(long user_index, long target_player_index) const
{
    c_game_engine::submit_nav_points(user_index, target_player_index);

    s_chud_navpoint navpoint;
    long player_index = ctf_player_index_from_user_index(user_index);
    player_datum *player = ctf_player_get(player_index);
    bool has_flag = player_is_helping_flag_carrier(player_index) || ctf_player_has_flag(player_index);

    if (game_engine_ctf_team_has_flag_carrier(player->configuration.host.team_index) && game_engine_in_round())
    {
        e_multiplayer_team_designator team_designator =
            game_engine_team_index_to_team_designator((e_game_team)player->configuration.host.team_index);
        if (team_designator != _multiplayer_team_designator_none)
        {
            c_area_set<c_area, 3> *goal_areas = &ctf_globals_get()->goal_areas[team_designator];
            for (c_area *area = goal_areas->begin(); area != goal_areas->end(); ++area)
            {
                const real_point3d *center = &area->m_boundary_geometry.matrix.position;
                ctf_engine_build_goal_nav_point(&navpoint, area->m_object_index, has_flag, center);
                ctf_chud_submit_navpoint(user_index, &navpoint);
            }
        }
    }

    if (game_engine_in_round())
    {
        for (long team_index = 0; team_index < k_maximum_teams; ++team_index)
        {
            const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)team_index);
            if (tracker != 0 && tracker->carrier_player_index == NONE)
            {
                bool away = (ctf_globals_get()->bytes_f834[team_index] & 1) == 0;
                bool enemy_team = team_index != player->configuration.host.team_index;
                if (enemy_team || !away || side_switch_ctf())
                {
                    if (ctf_engine_build_flag_nav_point(&navpoint, tracker->weapon_index, true, enemy_team, away))
                    {
                        ctf_chud_submit_navpoint(user_index, &navpoint);
                    }
                }
            }
        }
    }
}

// @address 0x2e5a48 ?build_player_nav_point@c_ctf_engine@@UBA_NJJJ_NPAUs_chud_navpoint@@@Z
bool c_ctf_engine::build_player_nav_point(long user_index, long player_index, long target_player_index, bool target,
    s_chud_navpoint *navpoint) const
{
    bool result = c_game_engine::build_player_nav_point(user_index, player_index, target_player_index, target, navpoint);
    if (result && ctf_player_has_flag(player_index))
    {
        navpoint->icon = 6;
        if (navpoint->type == -1)
        {
            navpoint->type = 1;
        }
        navpoint->flags |= 1;
    }
    return result;
}

// @address 0x2e5ad0 ?prepare_for_new_state@c_ctf_engine@@UBAXJ@Z
void c_ctf_engine::prepare_for_new_state(long state) const
{
    if (state == 1) {
        long team = game_engine_next_active_team((e_game_team)ctf_globals_get()->defending_team);
        if (team == -1)
            team = game_engine_next_active_team(-1);
        if (team == -1 && ctf_globals_get()->defending_team != -1)
            team = ctf_globals_get()->defending_team;
        if (game_engine_get_round() != 0)
            side_switch_ctf_switch_sides((e_game_team)team);
    }
}

// @address 0x2e5b78 ?test_flag@c_ctf_engine@@UBA_NJ@Z
bool c_ctf_engine::test_flag(long flag) const
{
    bool result = false;
    long tested_flag = flag;
    if (tested_flag == 0)
        result = true;
    return result;
}

// @address 0x2e5b90 ?apply_game_engine_traits_for_player@c_ctf_engine@@UBAXJPAVc_player_traits@@@Z
void c_ctf_engine::apply_game_engine_traits_for_player(long player_index, c_player_traits *traits) const
{
    if (ctf_player_has_flag(player_index))
    {
        traits->set(ctf_variant->get_carrier_traits(), false);
    }
}

// @address 0x2e5be8 ?validate_team_designator_for_new_map@c_ctf_engine@@UBA_NW4e_multiplayer_team_designator@@@Z
bool c_ctf_engine::validate_team_designator_for_new_map(e_multiplayer_team_designator team_designator) const
{
    if (!ctf_globals_get()->goal_areas[team_designator].exist())
        ctf_globals_get()->goal_areas[team_designator].initialize(0x440, 0x100, team_designator);
    if (!ctf_globals_get()->flag_spawn_areas[team_designator].exist())
        ctf_globals_get()->flag_spawn_areas[team_designator].initialize(0x440, 0x400, team_designator);
    return ctf_globals_get()->flag_spawn_areas[team_designator].exist()
        || ctf_globals_get()->goal_areas[team_designator].exist();
}

extern "C" {
}
// @address 0x2e5d00 get_flag_spawn_position
// match: 0x2e5d00: __restrict loads the area point's y and z before storing y to out_position (lwz r8/r10, then stw)
extern "C" unsigned char get_flag_spawn_position(int team, real_point3d *__restrict out_position, c_area **out_point)
{
    c_area_set<c_area, 3> *areas = &ctf_state->flag_spawn_areas[team];
    bool result = false;
    long count = areas->get_area_count();
    bool valid = count > 0;
    if (valid) {
        short index = (short)(((random_seed_next(tls_random_seed()) >> 16) * (short)count) >> 16);
        c_area *point = &(*areas)[index];
        *out_position = point->m_boundary_geometry.matrix.position;
        if (out_point)
            *out_point = point;
        result = 1;
    }
    return result;
}
static __forceinline const c_ctf_engine *ctf_current_game_engine()
{
    if (ctf_state->game_engine_index >= 1 && ctf_state->game_engine_index <= 10)
        return static_cast<const c_ctf_engine *>(game_engines[ctf_state->game_engine_index]);
    return 0;
}
// @address 0x2e5f20 ?create_all_flags@@YAXXZ
void create_all_flags(void)
{
    for (long flag_index = 0; flag_index < 9; flag_index++) {
        if (ctf_current_game_engine()->object_should_exist(flag_index))
            create_flag(flag_index);
    }
}

static __forceinline bool ctf_flag_should_exist(long flag_index)
{
    return ctf_current_game_engine()->object_should_exist(flag_index);
}

// @address 0x2e5fb0 ?ensure_all_flags_exist@@YA_NXZ
bool ensure_all_flags_exist(void)
{
    bool result = true;
    long flag_object_indices[9];
    memset(flag_object_indices, -1, sizeof(flag_object_indices));
    long weapons_to_delete[8];
    long weapon_to_delete_count = 0;
    for (long weapon_tracker_index = 0; weapon_tracker_index < ctf_state->multiplayer_weapon_count; weapon_tracker_index++) {
        long weapon_index = ctf_state->multiplayer_weapons[weapon_tracker_index].weapon_index;
        s_ctf_object_datum *weapon = ctf_object_get(weapon_index);
        bool delete_weapon = true;
        if (ctf_flag_should_exist(weapon->multiplayer_weapon_identifier)) {
            if (flag_object_indices[weapon->multiplayer_weapon_identifier] == NONE) {
                delete_weapon = false;
                flag_object_indices[weapon->multiplayer_weapon_identifier] = weapon_index;
            }
            else {
                result = false;
            }
        }
        else {
            result = false;
        }
        if (delete_weapon)
            weapons_to_delete[weapon_to_delete_count++] = weapon_index;
    }
    for (long delete_index = 0; delete_index < weapon_to_delete_count; delete_index++) {
        game_engine_multiplayer_weapon_deregister(weapons_to_delete[delete_index]);
        object_delete(weapons_to_delete[delete_index]);
    }
    for (long flag_index = 0; flag_index < 9; flag_index++) {
        if (ctf_flag_should_exist(flag_index) && flag_object_indices[flag_index] == NONE) {
            create_flag(flag_index);
            result = false;
        }
    }
    return result;
}

// @address 0x2e61a8 ?flag_reset@@YAXJ_N@Z
void flag_reset(long flag_object_index, bool returned)
{
    s_ctf_object_datum *flag = ctf_object_get(flag_object_index);
    long team_index = flag->multiplayer_weapon_identifier;
    if (team_index == NONE)
        return;
    e_multiplayer_team_designator team_designator = game_engine_team_index_to_team_designator((e_game_team)team_index);
    if (neutral_flag_ctf())
        team_designator = _multiplayer_team_designator_none;
    c_area *spawn_point;
    real_point3d spawn_position;
    if (get_flag_spawn_position(team_designator, &spawn_position, &spawn_point)) {
        if (flag->parent_object_index != NONE)
            object_detach(flag_object_index);
        ctf_globals_get()->flags_f7d4 |= 1;
        game_engine_multiplayer_weapon_reset(flag_object_index, &spawn_position);
        ctf_globals_get()->flags_f7d4 &= ~1;
        object_attach_to_marker_with_type(spawn_point->get_object_index(), 0x40004, flag_object_index, 0x40004);
        ::simulation_action_object_update(flag_object_index, 0x800);
    }
}

// @address 0x2e62a8 ?player_score@@YAXJW4e_game_team@@@Z
void player_score(long player_index, e_game_team team)
{
    if (::game_engine_adjust_player_score(player_index, 1)) {
        game_results_statistic_increment(datum_index_to_absolute_index(player_index),
            (e_game_team)ctf_player_get(player_index)->configuration.host.team_index,
            _game_results_statistic_ctf_flag_scores, 1);
        int medal_data = 0;
        ::game_engine_handle_time_messages(51, datum_index_to_absolute_index(player_index), player_index, NONE, (int *)(&medal_data));
    }
    s_game_engine_event_data event;
    game_engine_ctf_initialize_event(3, 6, &event);
    event.cause_player_index = player_index;
    event.cause_team_index = ctf_player_get(player_index)->configuration.host.team_index;
    event.effect_team_index = team;
    game_engine_send_event(&event);
}

// @address 0x2e6390 ?side_switch_ctf_generate_side_event@@YAXJ@Z
void side_switch_ctf_generate_side_event(long player_index)
{
    if (!game_engine_ctf_is_predicted() && side_switch_ctf()) {
        s_game_engine_event_data event_data;
        game_engine_ctf_initialize_event(3, 7, &event_data);
        event_data.cause_team_index = ctf_globals_get()->defending_team;
        event_data.flags = player_index;
        game_engine_send_event(&event_data);
    }
}

// @address 0x2e6450 ?side_switch_ctf_switch_sides@@YAXW4e_game_team@@@Z
void side_switch_ctf_switch_sides(e_game_team team)
{
    if (!game_engine_ctf_is_predicted())
        game_engine_multiplayer_weapons_delete_all();
    ctf_globals_get()->defending_team = team;
    simulation_action_game_engine_globals_update(64);
    if (!game_engine_ctf_is_predicted()) {
        game_engine_build_valid_team_mapping();
        side_switch_ctf_generate_side_event(-1);
        create_all_flags();
    }
}

// @address 0x2e64e8 ?promote_to_simulation_authority@c_ctf_engine@@UBAXXZ
void c_ctf_engine::promote_to_simulation_authority() const
{
    for (long team = 0; team < 9; team++) {
        ctf_globals_get()->goal_areas[team].initialize(0x440, 0x400, (e_multiplayer_team_designator)team);
        ctf_globals_get()->flag_spawn_areas[team].initialize(0x440, 0x100, (e_multiplayer_team_designator)team);
    }
    verify_state();
}

// @address 0x2e6580?recover_state_before_promotion@c_ctf_engine@@UBAXXZ
void c_ctf_engine::recover_state_before_promotion() const
{
    if (side_switch_ctf() && ctf_globals_get()->round_count_e10a > 0) {
        e_game_team defending_team = (e_game_team)game_engine_next_active_team(-1);
        for (long round = 0; round < ctf_globals_get()->round_count_e10a; round++) {
            defending_team = game_engine_next_active_team(defending_team);
            if (defending_team == _game_team_none)
                defending_team = game_engine_next_active_team(_game_team_none);
        }
        ctf_globals_get()->defending_team = defending_team;
        simulation_action_game_engine_globals_update(64);
    }
}

// @address 0x2e6658 ?build_simulation_baseline@c_ctf_engine@@UBAXJPAX@Z
void c_ctf_engine::build_simulation_baseline(long state_data_size, void *state_data) const
{
    s_ctf_engine_state_data *state = (s_ctf_engine_state_data *)state_data;
    memset(state, 0, sizeof(s_ctf_engine_state_data));
    build_global_baseline((s_game_engine_state_data *)state);
    state->defensive_team = (e_game_team)game_engine_next_active_team(-1);
    memset(state->flag_reset_timers, -1, sizeof(state->flag_reset_timers));
    memset(state->touch_return_timers, -1, sizeof(state->touch_return_timers));
    memset(state->flag_emblem_player, -1, sizeof(state->flag_emblem_player));
}
// @address 0x2e6748 ?build_simulation_update@c_ctf_engine@@UBAXPAKJPAX@Z
void c_ctf_engine::build_simulation_update(unsigned long *flags, long state_data_size, void *state_data) const
{
    s_ctf_engine_state_data *state = (s_ctf_engine_state_data *)state_data;
    unsigned long changed = 0;
    unsigned long base_flags = *flags & 63;
    if (base_flags != 0)
        build_global_update(base_flags, &changed, (s_game_engine_state_data *)state);
    if ((*flags & 64) != 0) {
        if (state->defensive_team != ctf_globals_get()->defending_team) {
            state->defensive_team = ctf_globals_get()->defending_team;
            changed |= 64;
        }
    }
    if ((*flags & 512) != 0) {
        for (long team = 0; team < 9; team++) {
            if (state->flag_reset_timers[team] != ctf_globals_get()->timers_f7fc[team]) {
                state->flag_reset_timers[team] = ctf_globals_get()->timers_f7fc[team];
                changed |= 512;
            }
        }
    }
    if ((*flags & 256) != 0) {
        for (long team = 0; team < 9; team++) {
            if (state->touch_return_timers[team] != ctf_globals_get()->flag_object_index[team]) {
                state->touch_return_timers[team] = ctf_globals_get()->flag_object_index[team];
                changed |= 256;
            }
        }
    }
    if ((*flags & 1024) != 0) {
        for (long team = 0; team < 9; team++) {
            if (state->flag_weapon_flags[team] != ctf_globals_get()->bytes_f834[team]) {
                state->flag_weapon_flags[team] = ctf_globals_get()->bytes_f834[team];
                changed |= 1024;
            }
        }
    }
    if ((*flags & 2048) != 0) {
        if (state->helper_flags != ctf_globals_get()->flags_f7d4) {
            state->helper_flags = ctf_globals_get()->flags_f7d4;
            changed |= 2048;
        }
    }
    *flags = changed;
}
// @address 0x2e6958 ?apply_simulation_update@c_ctf_engine@@UBA_NKJPBX@Z
bool c_ctf_engine::apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const
{
    const s_ctf_engine_state_data *state = (const s_ctf_engine_state_data *)state_data;
    bool success = true;
    unsigned long base_mask = update_mask & 0x3f;
    if (base_mask)
        success = success && apply_global_update(base_mask, (const s_game_engine_state_data *)state);
    if (update_mask & 0x40) {
        if (ctf_globals_get()->defending_team != state->defensive_team)
            side_switch_ctf_switch_sides((e_game_team)state->defensive_team);
    }
    if (update_mask & 0x200) {
        for (long team = 0; team < 9; team++)
            ctf_globals_get()->timers_f7fc[team] = state->flag_reset_timers[team];
    }
    if (update_mask & 0x100) {
        for (long team = 0; team < 9; team++)
            ctf_globals_get()->flag_object_index[team] = state->touch_return_timers[team];
    }
    if (update_mask & 0x400) {
        for (long team = 0; team < 9; team++)
            ctf_globals_get()->bytes_f834[team] = state->flag_weapon_flags[team];
    }
    if (update_mask & 0x800)
        ctf_globals_get()->flags_f7d4 = state->helper_flags;
    return success;
}
// match: 0x2e6ad8: __restrict issues the `*flag_weapon_index = -1` store ahead of the tracker table lwzx
bool c_ctf_engine::team_flag_is_at_home(e_multiplayer_team_designator team_designator, long *__restrict flag_weapon_index)
{
    bool result = false;
    long team_index = game_engine_team_designator_to_team_index(team_designator);
    *flag_weapon_index = -1;
    if (team_designator >= 0 && team_designator < 9) {
        const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)team_index);
        if (tracker) {
            result = (ctf_globals_get()->bytes_f834[team_index] & 1) != 0;
            *flag_weapon_index = tracker->weapon_index;
        }
    }
    return result;
}
// @address 0x2e6b68 ?object_should_exist@c_ctf_engine@@UBA_NJ@Z
bool c_ctf_engine::object_should_exist(long identifier) const
{
    bool result = false;
    if (identifier >= 0 && identifier < 8) {
        if (!neutral_flag_ctf() && ::game_engine_is_team_active((e_game_team)identifier)) {
            if (side_switch_ctf())
                result = identifier == ctf_globals_get()->defending_team;
            else
                result = true;
        }
    } else if (identifier == 8) {
        result = neutral_flag_ctf();
    }
    return result;
}


static __forceinline void game_engine_ctf_item_get_position_even_if_in_inventory(long item_index, real_point3d *position)
{
    item_datum *item = (item_datum *)ctf_object_get(item_index);
    if (item->owner_relation)
        unit_get_head_position(item->unit_object_index, position);
    else
        *position = item->object.bounding_center;
}

// @address 0x2e6c68 ?ctf_engine_build_flag_nav_point@@YA_NPAUs_chud_navpoint@@J_N11@Z
bool ctf_engine_build_flag_nav_point(s_chud_navpoint *navpoint, long object_index, bool carried, bool at_home, bool away)
{
    navpoint->flags = 193;
    navpoint->object_index = object_index;
    navpoint->type = 2;
    navpoint->icon = carried ? 6 : 5;
    navpoint->state = at_home ? 2 : (away ? 1 : 0);
    navpoint->scale = 1.0f;
    navpoint->name[0] = 0;
    game_engine_ctf_item_get_position_even_if_in_inventory(object_index, &navpoint->position);
    return true;
}


bool team_near_object(long object_index, float distance, bool include_all)
{
    bool result = false;
    long identifier = ctf_object_get(object_index)->multiplayer_weapon_identifier;
    if (identifier != -1) {
        float distance_squared = distance * distance;
        long team_index = ctf_flag_identifier_to_team_index((short)identifier);
        real_point3d position;
        object_get_origin(object_index, &position);
        c_player_with_unit_iterator iterator;
            iterator.begin();
        while (iterator.next()) {
            long player_team = iterator.get_datum()->configuration.host.team_index;
            if ((include_all && player_team == team_index) || (!include_all && player_team != team_index)) {
                if (player_within_distance_of_position(iterator.get_index(), &position, distance_squared)) {
                    result = true;
                    break;
                }
            }
        }
    }
    return result;
}

// @address 0x2e6e48 player_within_distance_of_position
bool player_within_distance_of_position(long player_index, const real_point3d *point, float distance_squared)
{
    player_datum *player = ctf_player_get(player_index);
    bool result = false;
    long unit_index = player->unit_index;
    if (unit_index != -1) {
        real_point3d origin;
        object_get_origin(unit_index, &origin);
        if (distance_squared3d(&origin, point) < distance_squared)
            result = true;
    }
    return result;
}

// @address 0x2e6ee0 ?verify_state@c_ctf_engine@@UBA_NXZ
bool c_ctf_engine::verify_state() const
{
    bool result = ensure_all_flags_exist();
    for (long flag_index = 0; flag_index < 9; flag_index++) {
        const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)flag_index);
        if (tracker && tracker->weapon_index != NONE) {
            if (tracker->carrier_player_index != NONE && tracker->carrier_unit_index != NONE &&
                ctf_player_get(tracker->carrier_player_index)->configuration.host.team_index == flag_index)
                action_submit_event(tracker->carrier_unit_index, 46);
            if (!TEST_BIT(ctf_globals_get()->bytes_f834[flag_index], 0)) {
                bool needs_update = false;
                if (tracker->carrier_unit_index != NONE) {
                    needs_update = true;
                } else {
                    long parent_index = ctf_object_get(tracker->weapon_index)->parent_object_index;
                    if (parent_index == NONE || ctf_object_get(parent_index)->identifier.get_type() == 0)
                        needs_update = true;
                }
                if (needs_update) {
                    SET_BIT(ctf_globals_get()->bytes_f834[flag_index], 0, true);
                    simulation_action_game_engine_globals_update(0x400);
                    result = false;
                }
            }
        }
    }
    return result;
}



static __forceinline s_ctf_object_datum *ctf_object_try_and_get_and_verify_type(long object_index, unsigned long type_mask)
{
    s_object_header *header = (s_object_header *)datum_try_and_get_inline(tls_object_header_data(), object_index);
    if (!header || !((1 << header->object_type) & type_mask))
        return 0;
    return (s_ctf_object_datum *)header->datum;
}
static __forceinline s_ctf_object_owner_data *game_engine_ctf_object_get_owner_data(long object_index)
{
    s_ctf_object_datum *object = ctf_object_try_and_get_and_verify_type(object_index, -1);
    if (object && object->multiplayer_data_offset != NONE)
        return (s_ctf_object_owner_data *)((char *)object + object->multiplayer_data_offset);
    return 0;
}
static __forceinline s_object_header *game_engine_ctf_object_get_header(long object_index)
{
    return &((s_object_header *)tls_object_header_data()->data)[datum_index_to_absolute_index(object_index)];
}

// @address 0x2e7020 player_in_enemy_area
extern "C" bool player_in_enemy_area(long player_index, c_area **area)
{
    bool result = false;
    long team_index = ctf_player_get(player_index)->configuration.host.team_index;
    e_multiplayer_team_designator team_designator = game_engine_team_index_to_team_designator((e_game_team)team_index);
    for (long team = 0; team < 9; team++) {
        if (team != team_designator && team != _multiplayer_team_designator_none) {
            if (ctf_state->goal_areas[team].player_in_set(player_index, area)) {
                result = true;
                break;
            }
        }
    }
    return result;
}

// @address 0x2e70d0 ?initialize@?$c_area_set@Vc_area@@$02@@QAAXJJW4e_multiplayer_team_designator@@@Z
template <typename t_area_type, long k_area_count>
void c_area_set<t_area_type, k_area_count>::initialize(long type_flags, long area_flags, e_multiplayer_team_designator team_designator)
{
    m_team_designator = team_designator;
    m_area_type_flags = 0x440;
    m_area_flags = area_flags;
    update_areas();
    if (!game_engine_ctf_is_predicted()) {
        t_area_type *selected_area = find_selected_area();
        if (selected_area)
            selected_area->set_selected(false);
    }
}

static __forceinline long game_engine_ctf_area_get_object_index(const c_area &area) { return area.m_object_index; }
static __forceinline void game_engine_ctf_area_set_object_index(c_area &area, long index) { area.m_object_index = index; }
static __forceinline void game_engine_ctf_area_calculate_boundary_geometry(c_area &area)
{
    unsigned __int64 wide = (unsigned int)area.m_object_index;
    if ((int)wide != -1)
        calculate_influence_boundary_geometry((int)wide, &area, 0);
}

// @address 0x2e7208 ?update_areas@?$c_area_set@Vc_area@@$02@@IAAXXZ
template <typename t_area_type, long k_area_count>
void c_area_set<t_area_type, k_area_count>::update_areas()
{
    long handles[20];
    t_area_type temporary;
    c_ctf_area_object_iterator iterator;
    long count = 0;

    iterator.begin(m_area_type_flags, m_area_flags);
    while (iterator.next() && count < 20) {
        long object_index = iterator.get_index();
        s_ctf_object_owner_data *owner_data = game_engine_ctf_object_get_owner_data(object_index);
        if (!TEST_BIT(game_engine_ctf_object_get_header(object_index)->flags, 4)) {
            if (!owner_data || owner_data->team_designator == m_team_designator)
                handles[count++] = object_index;
        }
    }
    qsort(handles, count, sizeof(long), c_area_set_compare_by_order_index);
    if (count > k_area_count)
        count = k_area_count;
    for (long i = 0; i < count; i++) {
        long object_index = handles[i];
        if (game_engine_ctf_area_get_object_index(m_areas[i]) != object_index) {
            memcpy(&temporary, &m_areas[i], sizeof(t_area_type));
            for (long j = i; j < m_area_count; j++) {
                if (game_engine_ctf_area_get_object_index(m_areas[j]) == object_index) {
                    memcpy(&m_areas[i], &m_areas[j], sizeof(t_area_type));
                    memcpy(&m_areas[j], &temporary, sizeof(t_area_type));
                    break;
                }
            }
        }
        game_engine_ctf_area_set_object_index(m_areas[i], object_index);
        game_engine_ctf_area_calculate_boundary_geometry(m_areas[i]);
    }
    m_area_count = count;
}

template void c_area_set<c_area, 3>::initialize(long, long, e_multiplayer_team_designator);
template void c_area_set<c_area, 3>::update_areas();

// @address 0x2e45a8 ?update@c_ctf_engine@@UBAXXZ
void c_ctf_engine::update() const
{
    if (game_engine_in_round()) {
        for (long i = 0; i < 9; i++) {
            ctf_globals_get()->goal_areas[i].update_areas();
            ctf_globals_get()->flag_spawn_areas[i].update_areas();
        }
        if (!game_engine_ctf_is_predicted())
            verify_state();
        for (long flag_index = 0; flag_index < 9; flag_index++) {
            const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)flag_index);
            if (tracker) {
                bool needs_reset = update_object(tracker);
                if (!game_is_predicted() && (game_engine_game_object_has_fallen_too_far(tracker->weapon_index) || needs_reset)) {
                    if (!ctf_globals_get()->suppress_returned_event) {
                        emit_object_returned_event(flag_index, ctf_globals_get()->flag_returned);
                        ctf_globals_get()->flag_returned = 0;
                    } else {
                        ctf_globals_get()->suppress_returned_event = 0;
                    }
                    flag_reset(tracker->weapon_index, false);
                    initialize_object_data(flag_index);
                }
            }
        }
        if (!game_engine_ctf_is_predicted() && side_switch_ctf() &&
            ctf_globals_get()->defending_team >= 0 && ctf_globals_get()->defending_team < 8 &&
            !::game_engine_is_team_active((e_game_team)ctf_globals_get()->defending_team))
            game_engine_end_round_with_winner(-1, false, game_engine_ctf_end_condition_20);
    }
}



// @address 0x2e4828 ?player_killed_player@c_ctf_engine@@UBAXJJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z
void c_ctf_engine::player_killed_player(long killer_player_index, long object_index, long victim_player_index, bool betrayal,
    c_flags<e_game_engine_kill_flags, long, 24> kill_flags, long damage) const
{
    bool is_betrayal = betrayal;
    long victim_index = victim_player_index;
    game_engine_handle_player_killed(this, killer_player_index, object_index, victim_index, is_betrayal, kill_flags, damage);
    if (killer_player_index != -1 && victim_index != -1 && !is_betrayal) {
        if (game_engine_get_carried_multiplayer_weapon_index(victim_index, 1) != -1) {
            long killer_absolute_index = datum_index_to_absolute_index(killer_player_index);
            if (killer_player_index != victim_index) {
                game_results_statistic_increment(killer_absolute_index,
                    (e_game_team)((player_datum *)tls_player_data()->data)[killer_absolute_index].configuration.host.team_index,
                    _game_results_statistic_ctf_flag_carrier_kills, 1);
                if (get_type() == _game_engine_ctf) {
                    int time_message_data = 0;
                    ::game_engine_handle_time_messages(52, killer_absolute_index, killer_player_index, victim_index,
                        (int *)(&time_message_data));
                }
            }
        }
        if (game_engine_get_carried_multiplayer_weapon_index(killer_player_index, 1) != -1) {
            if (get_type() == _game_engine_ctf) {
                int time_message_data = 0;
                ::game_engine_handle_time_messages(53, (unsigned short)killer_player_index, killer_player_index,
                    victim_index, (int *)(&time_message_data));
            }
        }
    }
}

static __forceinline bool ctf_game_is_finished() { return tls_game_globals()->game_finished; }


static __forceinline s_ctf_object_datum *ctf_unit_get(long unit_index)
{
    return (s_ctf_object_datum *)((s_object_header *)tls_object_header_data()->data)[(word)unit_index].datum;
}
static __forceinline s_ctf_unit_index ctf_player_unit_get(const player_datum *player)
{
    return *(const s_ctf_unit_index *)&player->unit_index;
}
static __forceinline long ctf_unit_inventory_get_weapon(long unit_index, short inventory_index)
{
    long weapon_index = NONE;
    if (inventory_index != NONE)
        weapon_index = ctf_unit_get(unit_index)->weapons[inventory_index];
    return weapon_index;
}
// @address 0x2e3d08 ?player_update@c_ctf_engine@@UBAXJ@Z
void c_ctf_engine::player_update(long player_index) const
{
    c_game_engine::player_update(player_index);
    if (!game_engine_ctf_is_predicted() && game_engine_in_round() && !ctf_game_is_finished()) {
        player_datum *player = ctf_player_get(player_index);
        if (player->unit_index != NONE) {
            s_ctf_object_datum *unit = ctf_object_get(player->unit_index);
            s_ctf_unit_index unit_index = ctf_player_unit_get(player);
            long weapon_index = ctf_unit_inventory_get_weapon(unit_index.value, ctf_unit_get(unit_index.value)->current_weapon);
            if (unit->parent_seat_index == 0xffff && weapon_index != NONE && (short)weapon_index != NONE &&
                ctf_weapon_get_multiplayer_type(weapon_index) == 1) {
                long identifier = ctf_object_get(weapon_index)->multiplayer_weapon_identifier;
                if (identifier != NONE)
                    process_player_holding_object(player_index, identifier);
            }
        }
    }
}

struct s_damage_owner;

// @address 0x2e5db0 ?create_flag@@YAXJ@Z
void create_flag(long flag_index)
{
    ctf_current_game_engine()->initialize_object_data(flag_index);
    e_multiplayer_team_designator team_designator = game_engine_team_index_to_team_designator((e_game_team)flag_index);
    if (neutral_flag_ctf())
        team_designator = _multiplayer_team_designator_none;
    c_area *spawn_point;
    real_point3d spawn_position;
    if (get_flag_spawn_position(team_designator, &spawn_position, &spawn_point)) {
        object_placement_data placement;
        long definition_index = ctf_current_game_engine()->get_object_definition_index();
        object_placement_data_new(&placement, definition_index, NONE, 0);
        placement.position = spawn_position;
        long flag_object_index = object_new(&placement);
        if (flag_object_index != NONE)
            simulation_action_object_create(flag_object_index);
        object_attach_to_marker_with_type(spawn_point->get_object_index(), 0x40004, flag_object_index, 0x40004);
        simulation_action_object_update(flag_object_index, 0x800);
        if (flag_object_index != NONE)
            game_engine_multiplayer_weapon_register(flag_object_index, (short)flag_index);
    }
}
// match: 0x2e4f58: retain the shared timer view's base address calculation.

static __forceinline s_ctf_shared_globals *get_shared_globals() { return (s_ctf_shared_globals *)ctf_state->goal_areas; }
// @address 0x2e4f58 ?get_time_left_in_ticks@c_ctf_engine@@UBAJJ_N0@Z
long c_ctf_engine::get_time_left_in_ticks(long time, bool advance, bool sudden_death) const
{
    long time_left = time;
    if (sudden_death && (get_shared_globals()->sudden_death_timer != 0 || time == 1) && get_sudden_death_time() != 0)
    {
        bool contested = false;
        c_player_in_game_iterator iterator;
        iterator.begin();
        while (iterator.next())
        {
            long player_index = iterator.get_index();
            if (game_engine_get_carried_multiplayer_weapon_index(player_index, 1) != -1)
            {
                contested = true;
                break;
            }
            player_datum *player = ctf_player_get(player_index);
            long team_index = player->configuration.host.team_index;
            const s_game_globals *game_globals = global_game_globals;
            const cache_file_tag_instance *tags = global_tag_instances;
            const s_multiplayer_globals_definition *multiplayer_globals = (const s_multiplayer_globals_definition *)
                tags[datum_index_to_absolute_index(game_globals->multiplayer_globals_index)].definition;
            const s_multiplayer_runtime_globals_definition *runtime =
                (const s_multiplayer_runtime_globals_definition *)multiplayer_globals->runtime.address;
            const s_multiplayer_constants *constants = (const s_multiplayer_constants *)runtime->multiplayer_constants.address;
            if (team_index >= 0 && team_index < 8)
            {
                short team;
                if (neutral_flag_ctf())
                    team = 8;
                else
                    team = (short)team_index;
                const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get(team);
                if (tracker)
                {
                    if (is_team_near_object(tracker->weapon_index, constants->flag_enemy_contest_radius))
                    {
                        contested = true;
                        break;
                    }
                    real_point3d flag_position;
                    object_get_origin(tracker->weapon_index, &flag_position);
                    if (player->unit_index != -1)
                    {
                        real_point3d player_position;
                        object_get_origin(player->unit_index, &player_position);
                        collision_result collision;
                        if (is_team_near_object(tracker->weapon_index, constants->flag_enemy_contest_fallback_radius))
                        {
                            s_collision_test_flags flags = { 9, 0x181 };
                            if (collision_test_line(flags, &player_position, &flag_position, player->unit_index,
                                tracker->weapon_index, &collision))
                            {
                                contested = true;
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (contested)
        {
            if (get_sudden_death_time() < 0)
            {
                time_left = game_tick_rate() * 5;
                get_shared_globals()->sudden_death_timer = time_left;
                SET_BIT(get_shared_globals()->flags, 1, true);
                SET_BIT(get_shared_globals()->flags, 2, false);
                simulation_action_game_engine_globals_update(0x800);
            }
            else
            {
                bool active = TEST_BIT(get_shared_globals()->flags, 1);
                SET_BIT(get_shared_globals()->flags, 2, false);
                simulation_action_game_engine_globals_update(0x800);
                if (active)
                {
                    if (get_shared_globals()->sudden_death_timer > 0)
                    {
                        if (advance)
                            get_shared_globals()->sudden_death_timer--;
                    }
                    else
                    {
                        SET_BIT(get_shared_globals()->flags, 1, false);
                        simulation_action_game_engine_globals_update(0x800);
                    }
                    time_left = get_shared_globals()->sudden_death_timer;
                }
                else if (time != 0)
                {
                    SET_BIT(get_shared_globals()->flags, 1, true);
                    simulation_action_game_engine_globals_update(0x800);
                    get_shared_globals()->sudden_death_timer =
                        static_cast<const c_ctf_engine *>(current_game_engine())->get_sudden_death_time() * game_tick_rate();
                    time_left = get_shared_globals()->sudden_death_timer;
                }
            }
        }
        else if (TEST_BIT(get_shared_globals()->flags, 1))
        {
            if (!TEST_BIT(get_shared_globals()->flags, 2))
            {
                get_shared_globals()->ending_timer = (6 * game_tick_rate()) - 1;
                SET_BIT(get_shared_globals()->flags, 2, true);
                simulation_action_game_engine_globals_update(0x800);
            }
            else if (get_shared_globals()->ending_timer > 0)
            {
                if (advance)
                    get_shared_globals()->ending_timer--;
            }
            else
            {
                SET_BIT(get_shared_globals()->flags, 1, false);
                SET_BIT(get_shared_globals()->flags, 2, false);
                simulation_action_game_engine_globals_update(0x800);
            }
            time_left = get_shared_globals()->ending_timer;
        }
    }
    return time_left;
}
