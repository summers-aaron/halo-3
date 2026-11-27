// blamlibxbox/game_engine_infection.obj: from the records 0x2f2290 0x2f22d8 0x2f23b8 0x2f23c8 0x2f2580 0x2f2670 0x2f28e8 0x2f2a10 0x2f2ae8 0x2f2b70 0x2f2c10 0x2f2ce8 0x2f2dd0 0x2f2f28 0x2f2fb8 0x2f3340 0x2f3480 0x2f3500 0x2f38b8 0x2f3910 0x2f3928 0x2f3988 0x2f3b40 0x2f3ca0 0x2f3e48 0x2f3eb8 0x2f3f28 0x2f3fe8 0x2f4088 0x2f4170 0x2f42d0 0x2f43b8 0x2f4490 0x2f44f0 0x2f4658 0x2f47b0
// @flags /TP /O2 /J /Oc
#include <new>
#include "memory/tls_globals.h"
#include "simulation/simulation_entity_types.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "game/game_time_inline.h"
#include "memory/data_types.h"
#include "memory/static_arrays.h"
#include "game/game_types.h"
#include "game/player_traits.h"
#include "game/game_variant.h"
#include "game/game_globals_storage.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_engine_infection_variant.h"
#include "game/player_types.h"
#include <string.h>
#include "game/game_engine.h"
#include "game/game_engine_end_condition.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_scoring.h"
#include "game/game_engine_score_types.h"
#include "game/game_engine_spawning.h"
#include "objects/multiplayer_game_objects.h"
#include "game/game_results.h"
#include "game/game_engine_infection.h"
#include "game/game_engine_infection_types.h"
#include "game/game_engine_vip.h"
#include "memory/data.h"
#include "game/game_results_types.h"
#include "game/game_engine_base_variant.h"
#include "game/player_in_game_iterator.h"
#include "game/game_engine_event_data.h"
#include "game/area_cycle_options_types.h"
#include "game/statborg_entry_types.h"
#include "game/game_engine_events.h"
#include "game/game_engine_infection_globals.h"
#include "game/game_time_types.h"
#include "game/game_engine_spawn_influencer.h"
#include "game/game_engine_default.h"
#include "game/game_engine_area_set.h"
#include "game/game_engine_area_set_types.h"
#include "game/game_engine_current_inline.h"
#include "game/game_engine_interface_state_types.h"
#include "game/players_globals.h"
#include <stdlib.h>
#include "game/game_engine_juggernaut_events.h"
#include "memory/datum_index.h"
#include "math/random_math_range.h"

namespace {
const long k_infection_kill_milestone = 10;
enum e_infection_initial_zombie_count_settings {
    _initial_zombies_quarter,
    _initial_zombies_half,
    _initial_zombies_one
};
}
// @address 0x2f2b70 ?player_left@c_infection_engine@@UBAXJ@Z
// @address 0x2f2dd0 ?apply_game_engine_traits_for_player@c_infection_engine@@UBAXJPAVc_player_traits@@@Z
// @address 0x2ef358 ?find_selected_area@?$c_area_set@Vc_area@@$0M@@@QAAPAVc_area@@XZ
// @address 0x351e38 ?player_in_multiplayer_object_boundary@@YA_NJPBUs_multiplayer_object_boundary_geometry_data@@@Z
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z
// @address 0x29b5e8 ?adjust_player_stat@c_game_statborg@@QAAXJW4e_statborg_entry@@FJ_N@Z
// @address 0x84740 ?data_iterator_next@@YAPAXPAUs_data_iterator@@@Z
// @address 0xf6048 ?game_engine_get_statborg@@YAPAVc_game_statborg@@XZ
// @address 0x206e50 ?game_engine_get_player_kills@@YAJJ@Z
// @address 0x208758 ?game_engine_end_round_with_winner@@YAXJ_NW4e_game_engine_end_condition@@@Z
// @address 0x27b730 ?game_engine_limited_lives_active@@YA_NXZ
// @address 0x209ef0 ?game_engine_player_is_out_of_lives@@YA_NJ@Z
// @address 0x29e4a8 ?game_engine_scoring_build_score_list@@YAXPAUs_game_engine_score_list@@J_N@Z
// @address 0x24fc40 ?game_engine_send_event@@YAXPAUs_game_engine_event_data@@@Z
// @address 0x244bb8 ?game_results_statistic_increment@@YAXJW4e_game_team@@W4e_game_results_statistic@@J@Z
// @address 0xa7970 ?next@c_player_in_game_iterator@@QAA_NXZ
// @address 0x3a97c0 ?simulation_action_game_engine_globals_update@@YAXK@Z
// @address 0x3a9a60 ?simulation_action_game_statborg_update@@YAXK@Z
// @address 0x2f3b40 ?apply_simulation_update@c_infection_engine@@UBA_NKJPBX@Z
// @address 0x2f4088 ?compute_zombie_team_bitvector@@YAXXZ
// @address 0x2f42d0 ?human_count@@YAJXZ
// @address 0x2f43b8 ?get_last_human_player_index@@YAJXZ
// @address 0x2f4490 ?get_round_seconds_elapsed@@YAJXZ
// @address 0x2f4658 ?get_winner_player_index@@YAJXZ
// @address 0x8c43d8 g_infection_variant
#if !defined(_XBOX) || defined(__clang__)
// The port links the variant pointer by its C name (game/game_engine_infection_globals.h): game_results.cpp
// reads it for medal scoring, so this unit owns the one external definition.
extern "C" {
c_game_engine_infection_variant *g_infection_variant;
}
#endif

struct s_game_engine_globals;

// @address 0x42208 ??_7c_infection_engine@@6B@
// @address 0x79d43c ?internal_infection_engine@@3Vc_infection_engine@@A
c_infection_engine internal_infection_engine;

// @address 0x45fc50 ?get_type@c_infection_engine@@UBAJXZ
long c_infection_engine::get_type() const
{
    return _game_engine_infection;
}

// @address 0x2f22d8 ?initialize_for_new_map@c_infection_engine@@UBA_NXZ
bool c_infection_engine::initialize_for_new_map() const
{
    game_options *options = &((game_globals_storage *)tls_game_globals())->options;
    const c_game_variant *variant = options->game_mode == 2 ? &options->game_variant : 0;
    variant->get_active_variant()->can_be_cast_to(_game_engine_infection, (const void **)&g_infection_variant);

    game_engine_infection_state *state = &((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection;
    if (state) {
        new (state) game_engine_infection_state;
        state->zombies = 0;
        state->starting_zombies = 0;
        state->haven_timer = 0;
        state->zombie_teams = 0;
        state->haven_players = 0;
        long *kills = state->infection_kills;
        for (unsigned int index = 0; index < 16; ++index)
            kills[index] = 0;
        long *scores = state->zombie_kills;
        for (unsigned int index = 0; index < 16; ++index)
            scores[index] = 0;
        state->last_human_bonus_awarded = false;
    }
    return true;
}

namespace {
static __forceinline game_engine_infection_state *infection_globals()
{
    return &((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection;
}
}

// @address 0x2f2290 ?get_player_team_designator@c_infection_engine@@UBA?AW4e_multiplayer_team_designator@@J@Z
e_multiplayer_team_designator c_infection_engine::get_player_team_designator(long player_index) const
{
    e_multiplayer_team_designator result = _multiplayer_team_designator_defender;
    if (player_is_zombie(player_index))
        result = _multiplayer_team_designator_attacker;
    return result;
}

// @address 0x2f23b8 ?dispose_from_old_map@c_infection_engine@@UBAXXZ
void c_infection_engine::dispose_from_old_map() const
{
    g_infection_variant = 0;
}

static __forceinline long infection_game_engine_round_time_get()
{
    if (game_time_get() - ((s_game_engine_globals *)tls_game_engine_globals())->round_start_time < 0)
        return 0;
    return game_time_get() - ((s_game_engine_globals *)tls_game_engine_globals())->round_start_time;
}

// @address 0x2f38b8 ?team_is_enemy@c_infection_engine@@UBA_NW4e_game_team@@0@Z
bool c_infection_engine::team_is_enemy(e_game_team team, e_game_team other_team) const
{
    return ((infection_globals()->zombie_teams & (1 << other_team)) != 0) -
           ((infection_globals()->zombie_teams & (1 << team)) != 0) != 0;
}

// @address 0x2f3910 ?test_flag@c_infection_engine@@UBA_NJ@Z
bool c_infection_engine::test_flag(long flag) const
{
    bool result = false;
    switch (flag)
    {
    case 1:
        result = true;
        break;
    }
    return result;
}

// @address 0x2f3928 ?build_simulation_baseline@c_infection_engine@@UBAXJPAX@Z
void c_infection_engine::build_simulation_baseline(long state_data_size, void *state_data) const
{
    s_infection_engine_state_data *state = (s_infection_engine_state_data *)state_data;
    memset(state, 0, sizeof(*state));
    build_global_baseline((s_game_engine_state_data *)state);
}

// @address 0x2f3988 ?build_simulation_update@c_infection_engine@@UBAXPAKJPAX@Z
void c_infection_engine::build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const
{
    s_infection_engine_state_data *state = (s_infection_engine_state_data *)state_data;
    unsigned long changed = 0;
    unsigned long base_mask = *update_mask & 63;
    if (base_mask)
        build_global_update(base_mask, &changed, (s_game_engine_state_data *)state);
    if (*update_mask & 64)
    {
        if (state->zombie_bitvector != infection_globals()->zombies)
        {
            state->zombie_bitvector = infection_globals()->zombies;
            changed |= 64;
        }
    }
    if (*update_mask & 128)
    {
        if (state->first_zombie_bitvector != infection_globals()->starting_zombies)
        {
            state->first_zombie_bitvector = infection_globals()->starting_zombies;
            changed |= 128;
        }
    }
    if (*update_mask & 256)
    {
        if (state->players_in_safe_haven_bitvector != infection_globals()->haven_players)
        {
            state->players_in_safe_haven_bitvector = infection_globals()->haven_players;
            changed |= 256;
        }
    }
    if (*update_mask & 512)
    {
        if (state->safe_haven_timer != infection_globals()->haven_timer)
        {
            state->safe_haven_timer = infection_globals()->haven_timer;
            changed |= 512;
        }
    }
    if (*update_mask & 1024)
    {
        if (state->zombie_team_bitvector != infection_globals()->zombie_teams)
        {
            state->zombie_team_bitvector = infection_globals()->zombie_teams;
            changed |= 1024;
        }
    }
    *update_mask = changed;
}

bool c_infection_engine::apply_simulation_update(unsigned long update_mask, long update_sequence, const void *state_data) const
{
    const s_infection_engine_state_data *state = (const s_infection_engine_state_data *)state_data;
    bool success = true;
    if (update_mask & 63)
        success = apply_global_update(update_mask & 63, (const s_game_engine_state_data *)state) && success;
    if (update_mask & 64)
    {
        if (infection_globals()->zombies != state->zombie_bitvector)
            infection_globals()->zombies = state->zombie_bitvector;
    }
    if (update_mask & 128)
    {
        if (infection_globals()->starting_zombies != state->first_zombie_bitvector)
            infection_globals()->starting_zombies = state->first_zombie_bitvector;
    }
    if (update_mask & 256)
        infection_globals()->haven_players = state->players_in_safe_haven_bitvector;
    if (update_mask & 512)
        infection_globals()->haven_timer = state->safe_haven_timer;
    if (update_mask & 1024)
        infection_globals()->zombie_teams = state->zombie_team_bitvector;
    return success;
}

long get_round_seconds_elapsed()
{
    return (long)game_ticks_to_seconds((float)infection_game_engine_round_time_get());
}

// @address 0x42e650 ?get_simulation_entity_type@c_infection_engine@@UBA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_infection_engine::get_simulation_entity_type() const
{
    return _simulation_entity_type_infection_engine_globals;
}

class c_game_engine;
static __forceinline c_game_statborg *infection_get_statborg() {
    return current_game_engine() ? &((s_game_engine_globals *)tls_game_engine_globals())->statborg : 0;
}
// @address 0x2f2580 ?stats_reset_for_round_switch@c_infection_engine@@UBAXXZ
void c_infection_engine::stats_reset_for_round_switch() const {
    c_game_statborg *statborg=infection_get_statborg();
    if(statborg) {
        c_data_iterator<player_datum> iterator; iterator.begin(tls_player_data());
        while(iterator.next()) {
            statborg->reset_player_stat(iterator.get_index(), _statborg_entry_infection_time_as_human);
            statborg->reset_player_stat(iterator.get_index(), _statborg_entry_infection_infections);
            statborg->reset_player_stat(iterator.get_index(), _statborg_entry_infection_zombie_kills);
        }
    }
}
// @address 0x2f28e8 ?is_multiplayer_object_type_allowed@c_infection_engine@@UBA_NJ@Z
bool c_infection_engine::is_multiplayer_object_type_allowed(long type) const
{
    bool result = true;
    if (type == 11 || type == 12)
        result = false;
    return result;
}
static __forceinline bool infection_is_predicted() { bool result=false; if(tls_game_globals()->options.game_simulation==4) result=true; return result; }
static __forceinline bool infection_is_authoritative() { return !infection_is_predicted(); }
static __forceinline game_engine_infection_engine_globals *infection_engine_globals() { return (game_engine_infection_engine_globals
    *)tls_game_engine_globals(); }
// @address 0x2f2a10 ?player_added@c_infection_engine@@UBAXJ@Z
void c_infection_engine::player_added(long player_index) const
{
    if (infection_is_authoritative()) {
        player_set_zombie_status(player_index, false, false);
        infection_engine_globals()->infection.human_flags.bits &= ~(1 << (datum_index_to_absolute_index(player_index)));
        simulation_action_game_engine_globals_update(256);
        c_game_statborg *statborg = game_engine_get_statborg();
        if (statborg) {
            statborg->reset_player_stat(player_index, _statborg_entry_infection_time_as_human);
            statborg->reset_player_stat(player_index, _statborg_entry_infection_infections);
            statborg->reset_player_stat(player_index, _statborg_entry_infection_zombie_kills);
        }
    }
}
struct s_game_engine_event_data;
// @address 0x2f2ae8 ?emit_game_start_event@c_infection_engine@@UBAXJ@Z
void c_infection_engine::emit_game_start_event(long player_index) const
{
    if (!infection_is_predicted()) {
        s_game_engine_event_data l50;
        l50.type = 11;
        l50.event = 0;
        l50.data = 0;
        l50.identifier = -1;
        l50.flags = player_index;
        l50.cause_player_index = -1;
        l50.cause_team_index = -1;
        l50.effect_player_index = -1;
        l50.effect_team_index = -1;
        l50.data_word = -1;
        game_engine_send_event((s_game_engine_event_data *)&l50);
    }
}
void c_infection_engine::player_left(long player_index) const
{
    if (infection_is_authoritative() && player_is_human(player_index))
    {
        game_results_statistic_increment(datum_index_to_absolute_index(player_index), _infection_team_none,
            _game_results_statistic_infection_time_as_human, get_round_seconds_elapsed());
        game_engine_get_statborg()->adjust_player_stat(player_index, _statborg_entry_infection_time_as_human, (short)get_round_seconds_elapsed(), -1, false);
    }
}
// @address 0x2f2c10 ?player_changed_indices@c_infection_engine@@UBAXJJ@Z
void c_infection_engine::player_changed_indices(long old_index, long new_index) const {
    if(infection_is_authoritative()) {
        long old_absolute=datum_index_to_absolute_index(old_index);
        long new_absolute=datum_index_to_absolute_index(new_index);
        infection_engine_globals()->infection.original_zombie_flags.set(new_absolute,
            infection_engine_globals()->infection.original_zombie_flags.test(old_absolute));
        infection_engine_globals()->infection.human_flags.set(new_absolute,infection_engine_globals()->infection.human_flags.test(old_absolute));
        infection_engine_globals()->infection.zombie_flags.set(new_absolute,infection_engine_globals()->infection.zombie_flags.test(old_absolute));
        simulation_action_game_engine_globals_update(448);
    }
}
namespace {
static __forceinline player_datum *infection_player_get(long player_index) {
    return &((player_datum *)tls_player_data()->data)[datum_index_to_absolute_index(player_index)];
}
}
// @address 0x2f2ce8 ?player_about_to_spawn@c_infection_engine@@UBAXJ@Z
void c_infection_engine::player_about_to_spawn(long player_index) const {
    long absolute_index=datum_index_to_absolute_index(player_index);
    infection_engine_globals()->infection.infection_kills[absolute_index]=0;
    infection_engine_globals()->infection.zombie_kills[absolute_index]=0;
    if(infection_is_authoritative()) {
        bool zombie=player_is_zombie(player_index);
        if(zombie != infection_engine_globals()->infection.zombie_team_flags.test(infection_player_get(player_index)->configuration.host.team_index)) {
            if(zombie)
                infection_engine_globals()->infection.zombie_team_flags.bits |= 1 << infection_player_get(player_index)->configuration.host.team_index;
            else
                infection_engine_globals()->infection.zombie_team_flags.bits &= ~(1 << infection_player_get(player_index)->configuration.host.team_index);
            simulation_action_game_engine_globals_update(1024);
        }
    }
}

static __forceinline bool infection_player_is_zombie(long player_index) {
    long absolute_index=datum_index_to_absolute_index(player_index);
    return player_index!=-1 && infection_engine_globals()->infection.zombie_flags.test(absolute_index) && !game_engine_player_is_out_of_lives(player_index);
}
static __forceinline bool infection_player_is_human(long player_index) {
    long absolute_index=datum_index_to_absolute_index(player_index);
    return player_index!=-1 && !infection_engine_globals()->infection.zombie_flags.test(absolute_index) && !game_engine_player_is_out_of_lives(player_index);
}
// @address 0x2f3e48 player_is_zombie
extern "C" bool player_is_zombie(long player_index) { return infection_player_is_zombie(player_index); }

static __forceinline bool infection_safe_havens_enabled()
{
    return ((const c_game_engine_infection_variant *)g_infection_variant)->m_safe_havens != 0;
}

struct s_multiplayer_object_boundary_geometry_data;
static __forceinline c_game_variant *infection_game_engine_get()
{
    game_options *options = &((game_globals_storage *)tls_game_globals())->options;
    return (options->game_mode == 2) ? &options->game_variant : 0;
}

void c_infection_engine::apply_game_engine_traits_for_player(long player_index, c_player_traits *player_traits) const
{
    if (infection_game_engine_get()) {
        game_engine_infection_engine_globals *globals = (game_engine_infection_engine_globals *)tls_game_engine_globals();
        c_area *selected_area = globals->infection.safe_havens.find_selected_area();
        bool previously_zombie = (globals->infection.starting_zombies & (1 << (datum_index_to_absolute_index(player_index)))) != 0;
        bool is_last_human = get_last_human_player_index() - player_index == 0;
        bool in_boundary = selected_area != 0 &&
            player_in_multiplayer_object_boundary(player_index, (const s_multiplayer_object_boundary_geometry_data *)selected_area);
        if (player_is_zombie(player_index)) {
            player_traits->set(&g_infection_variant->m_traits, false);
            if (previously_zombie)
                player_traits->set(&g_infection_variant->m_first_zombie_traits, false);
        } else {
            if (is_last_human)
                player_traits->set(&g_infection_variant->m_last_human_traits, false);
            if (in_boundary)
                player_traits->set(&g_infection_variant->m_safe_haven_defender_traits, false);
        }
    }
}

// @address 0x2f2f28 ?assemble_spawn_influencers_for_player@c_infection_engine@@UBAXJPAUs_netgame_goal_influencer@@PAJ@Z
void c_infection_engine::assemble_spawn_influencers_for_player(long player_index, s_netgame_goal_influencer *influencers, long *influencer_count) const
{
    *influencer_count = 0;
    if (infection_safe_havens_enabled()) {
        c_area *area = (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->safe_havens.find_selected_area();
        if (area) {
            if (player_is_zombie(player_index)) {
                influencers->type = 16;
                influencers->object_index = area->get_object_index();
            } else {
                influencers->type = 15;
                influencers->object_index = area->get_object_index();
            }
            *influencer_count = 1;
        }
    }
}

// @address 0x2f3340 ?player_update@c_infection_engine@@UBAXJ@Z
void c_infection_engine::player_update(long player_index) const
{
    this->c_game_engine::player_update(player_index);
    if (infection_is_authoritative()) {
        game_engine_infection_state *globals = &((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection;
        c_area *selected_area = (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->safe_havens.find_selected_area();
        long absolute_index = datum_index_to_absolute_index(player_index);
        bool was_inside = (globals->haven_players & (1 << absolute_index)) != 0;
        bool inside;
        if (selected_area != 0)
            inside = player_in_multiplayer_object_boundary(player_index, (const s_multiplayer_object_boundary_geometry_data *)selected_area);
        else
            inside = false;
        const c_game_engine_infection_variant *variant = (const c_game_engine_infection_variant *)g_infection_variant;
        if (variant->m_safe_havens != 0 && inside && !was_inside && !player_is_zombie(player_index))
            game_engine_adjust_player_score(player_index, variant->m_limit_signed_bytes[0]);
        if (was_inside != inside) {
            if (inside)
                (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->haven_players |= 1 << absolute_index;
            else
                (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->haven_players &= ~(1 << absolute_index);
            simulation_action_game_engine_globals_update(256);
        }
    }
}

// @address 0x2f3480 ?get_hud_interface_state@c_infection_engine@@UBAXJPAUgame_engine_interface_state@@@Z
void c_infection_engine::get_hud_interface_state(long player_index, game_engine_interface_state *view) const
{
    if (player_is_zombie(player_index))
        view->variant_custom_flags |= 1;
    else
        view->variant_custom_flags &= ~1;
    if (player_index == get_last_human_player_index())
        view->variant_custom_flags |= 2;
    else
        view->variant_custom_flags &= ~2;
}
bool player_is_human(long player_index) { return infection_player_is_human(player_index); }
static __forceinline c_game_statborg *infection_comparison_statborg() {
    c_game_statborg *result=0;
    if(current_game_engine()) result=&((s_game_engine_globals *)tls_game_engine_globals())->statborg;
    return result;
}
void compute_zombie_team_bitvector(void)
{
    infection_engine_globals()->infection.zombie_team_flags.bits = 0;
    c_player_in_game_iterator players;
    players.m_iterator.m_iterator.absolute_index = -1;
    players.m_iterator.m_iterator.index = -1;
    players.m_iterator.m_iterator.data = tls_player_data();
    while (players.next()) {
        if (infection_player_is_zombie(players.m_iterator.m_iterator.index))
            infection_engine_globals()->infection.zombie_team_flags.bits |= 1 << ((player_datum *)players.m_iterator.m_datum)->configuration.host.team_index;
    }
    simulation_action_game_engine_globals_update(1024);
}
long human_count(void)
{
    long count = 0;
    c_player_in_game_iterator players;
    players.m_iterator.m_iterator.absolute_index = -1;
    players.m_iterator.m_iterator.index = -1;
    players.m_iterator.m_iterator.data = tls_player_data();
    while (players.next()) {
        if (!infection_player_is_zombie(players.m_iterator.m_iterator.index) && (!game_engine_limited_lives_active() || ((player_datum
            *)players.m_iterator.m_datum)->lives > 0))
            count++;
    }
    return count;
}
long get_last_human_player_index(void)
{
    long result = -1;
    c_player_in_game_iterator players;
    players.m_iterator.m_iterator.absolute_index = -1;
    players.m_iterator.m_iterator.index = -1;
    players.m_iterator.m_iterator.data = tls_player_data();
    while (players.next()) {
        long player_index = players.m_iterator.m_iterator.index;
        if (infection_player_is_human(player_index)) {
            if (result != -1) {
                result = -1;
                break;
            }
            result = player_index;
        }
    }
    return result;
}
namespace {
static __forceinline e_infection_initial_zombie_count_settings infection_initial_zombie_setting()
{
    return (e_infection_initial_zombie_count_settings)((const c_game_engine_infection_variant *)g_infection_variant)->m_initial_zombie_count;
}
static __forceinline long infection_total_players_in_game()
{
    return tls_players_globals()->count;
}
}
// @address 0x2f44f0 ?get_initial_zombie_count@@YAJXZ
long get_initial_zombie_count()
{
    long zombie_count = 0;
    e_infection_initial_zombie_count_settings setting = infection_initial_zombie_setting();
    switch (setting) {
    case _initial_zombies_quarter:
        zombie_count = (long)(0.25f * (float)infection_total_players_in_game() + 0.5f);
        break;
    case _initial_zombies_half:
        zombie_count = (long)(0.5f * (float)infection_total_players_in_game() + 0.5f);
        break;
    default:
        zombie_count = infection_initial_zombie_setting() - _initial_zombies_one + 1;
        break;
    }
    return (zombie_count > 1 ? zombie_count : 1) < infection_total_players_in_game() - 1
        ? (zombie_count > 1 ? zombie_count : 1)
        : infection_total_players_in_game() - 1;
}

long get_winner_player_index(void)
{
    long player_count = 0;
    long winner = -1;
    bool tied = false;
    long candidates[16];
    c_player_in_game_iterator iter;
    iter.m_iterator.m_iterator.absolute_index = -1;
    iter.m_iterator.m_iterator.index = -1;
    iter.m_iterator.m_iterator.data = tls_player_data();
    while (iter.next() && (unsigned long)player_count < 16)
        candidates[player_count++] = iter.m_iterator.m_iterator.index;
    if (player_count > 0) {
        winner = candidates[0];
        for (long i = 1; i < player_count; i++) {
            if (game_engine_get_player_kills(candidates[i]) > game_engine_get_player_kills(winner)) {
                winner = candidates[i];
                tied = false;
            } else if (game_engine_get_player_kills(candidates[i]) == game_engine_get_player_kills(winner)) {
                c_game_statborg *statborg = game_engine_get_statborg();
                if (statborg) {
                    long winner_infection_time = statborg->get_player_stat(winner, 8);
                    long candidate_infection_time = statborg->get_player_stat(candidates[i], 8);
                    if (candidate_infection_time > winner_infection_time) {
                        winner = candidates[i];
                        tied = false;
                    } else if (candidate_infection_time == winner_infection_time) {
                        tied = true;
                    }
                } else {
                    tied = true;
                }
            }
        }
    }
    return tied ? -1 : winner;
}
// match: 0x2f47b0: canonical const accessor changes narrowing; keep the original non-const read locally.
static __forceinline short infection_get_player_stat(c_game_statborg *statborg, long player, long stat)
{
    return statborg->m_player_stats[(unsigned short)player][stat];
}
int compare_by_infection_time(const void *a, const void *b) {
    c_game_statborg *statborg=infection_comparison_statborg();
    long player_a=*(const long *)a;
    long player_b=*(const long *)b;
    long a_infection_time=infection_get_player_stat(statborg,player_a,8);
    long b_infection_time=infection_get_player_stat(statborg,player_b,8);
    if(a_infection_time == b_infection_time) {
        long a_zombie_time=infection_get_player_stat(statborg,player_a,9);
        long b_zombie_time=infection_get_player_stat(statborg,player_b,9);
        long a_human_time=infection_get_player_stat(statborg,player_a,10);
        long b_human_time=infection_get_player_stat(statborg,player_b,10);
        if(infection_engine_globals()->infection.original_zombie_flags.test((unsigned short)player_a)) {
            return b_zombie_time-a_zombie_time;
        }
        bool a_is_zombie=player_is_zombie(player_a);
        if(a_is_zombie && player_is_zombie(player_b)) {
            return a_human_time+a_zombie_time-b_human_time-b_zombie_time;
        } else if(a_is_zombie) {
            return -1;
        } else if(player_is_zombie(player_b)) {
            return 1;
        } else {
            return a_human_time-b_human_time;
        }
    } else {
        return a_infection_time-b_infection_time;
    }
}
// @address 0x8d430 qsort
struct s_game_engine_event_data;

// @address 0x2f2670 ?prepare_for_new_state@c_infection_engine@@UBAXJ@Z
void c_infection_engine::prepare_for_new_state(long new_state) const
{
    if (infection_is_authoritative() && new_state == 3) {
        s_game_engine_score_list score_list;
        game_engine_scoring_build_score_list(&score_list, 0, false);
        switch ((unsigned long)(signed char)((c_game_engine_infection_variant *)g_infection_variant)->m_next_zombie) {
        case 1: {
            long initial_count = get_initial_zombie_count();
            long first_zombie = score_list.player_count;
            long first_human = score_list.player_count;
            qsort(&score_list, score_list.player_count, 4, compare_by_infection_time);
            for (long i = 0; i < score_list.player_count; ++i) {
                if (!((&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->starting_zombies & (1 << (unsigned
                    short)score_list.player_indices[i]))) {
                    first_zombie = i;
                    break;
                }
            }
            for (long i = 0; i < score_list.player_count; ++i) {
                if (player_is_human(score_list.player_indices[i])) {
                    first_human = i;
                    break;
                }
            }
            long start = first_zombie;
            if (start + initial_count > first_human) start = first_human - initial_count;
            if (start < 0) start = 0;
            (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->starting_zombies = 0;
            for (long i = start; i < start + initial_count; ++i)
                (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->starting_zombies |= 1 << (unsigned
                    short)score_list.player_indices[i];
            (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->zombies =
                (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->starting_zombies;
            simulation_action_game_engine_globals_update(192);
            break;
        }
        case 0: {
            (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->starting_zombies = 0;
            (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->zombies = 0;
            long initial_count = get_initial_zombie_count();
            for (long i = 0; i < initial_count; ++i)
                (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->starting_zombies |= 1 << (unsigned
                    short)score_list.player_indices[i];
            (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->zombies =
                (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->starting_zombies;
            simulation_action_game_engine_globals_update(192);
            break;
        }
        case 2:
            (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->zombies =
                (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->starting_zombies;
            simulation_action_game_engine_globals_update(64);
            break;
        }
    }
}

// @address 0x2f3ca0 ?choose_new_zombies@@YAXJ@Z
void choose_new_zombies(long desired)
{
    long total = 0, non_zombies = 0;
    long players[16];
    c_player_in_game_iterator iterator;
    iterator.m_iterator.m_iterator.absolute_index = -1;
    iterator.m_iterator.m_iterator.index = -1;
    iterator.m_iterator.m_iterator.data = tls_player_data();
    while (iterator.next() && (unsigned long)total < 16) {
        long player_index = iterator.m_iterator.m_iterator.index;
        if ((1 << (datum_index_to_absolute_index(player_index))) & (&((game_engine_infection_engine_globals
            *)tls_game_engine_globals())->infection)->starting_zombies) {
            players[total++] = player_index;
        } else {
            players[total++] = players[non_zombies];
            players[non_zombies++] = player_index;
        }
    }
    if (desired >= total) desired = total - 1;
    if (total == 1) desired = 1;
    for (long i = 0; i < desired; ++i) {
        long index = random_range(tls_random_seed(), 0, (short)(non_zombies > 0 ? non_zombies : total));
        long player = players[index];
        player_set_zombie_status(player, true, true);
        if (index < non_zombies) {
            players[index] = players[--non_zombies];
            players[non_zombies] = players[--total];
        } else {
            players[index] = players[--total];
        }
    }
}

// @address 0x2f3f28 ?player_set_zombie_status@@YAXJ_N0@Z
void player_set_zombie_status(long player_index, bool zombie, bool starting)
{
    long absolute_index = datum_index_to_absolute_index(player_index);
    if (zombie) infection_handle_player_infection(player_index, starting);
    if (zombie) (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->zombies |= 1 << absolute_index;
    else (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->zombies &= ~(1 << absolute_index);
    simulation_action_game_engine_globals_update(64);
    if (!zombie) {
        if (zombie) (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->starting_zombies |= 1 << absolute_index;
        else (&((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection)->starting_zombies &= ~(1 << absolute_index);
        simulation_action_game_engine_globals_update(128);
    }
}

// @address 0x2f3fe8 infection_handle_player_infection
void infection_handle_player_infection(long p0, bool p1)
{
    int tr11;
    tr11 = 0;
    if (((game_globals_storage *)tls_game_globals())->options.game_simulation == 4) {
        tr11 = 1;
    }
    if ((unsigned int)(tr11 & 255) == 0) {
        s_game_engine_event_data l50;
        l50.type = 11;
        l50.event = (p1 != 0) + 2;
        l50.identifier = -1;
        l50.flags = -1;
        l50.cause_player_index = p0;
        l50.cause_team_index = -1;
        l50.effect_player_index = -1;
        l50.effect_team_index = -1;
        l50.data = 0;
        l50.data_word = -1;
        game_engine_send_event((s_game_engine_event_data *)&l50);
    }
    return;
}

// @address 0x2f4170 ?end_round@@YAXXZ
void end_round()
{
    c_game_statborg *stats = infection_get_statborg();
    if (stats) {
        c_player_in_game_iterator iter;
        iter.m_iterator.m_iterator.absolute_index = -1;
        iter.m_iterator.m_iterator.index = -1;
        iter.m_iterator.m_iterator.data = tls_player_data();
        while (iter.next()) {
            if (infection_player_is_human(iter.m_iterator.m_iterator.index)) {
                stats->adjust_player_stat(iter.m_iterator.m_iterator.index, _statborg_entry_infection_time_as_human,
                    (short)get_round_seconds_elapsed(), -1, false);
                game_results_statistic_increment(datum_index_to_absolute_index(iter.m_iterator.m_iterator.index), _infection_team_none,
                    _game_results_statistic_infection_time_as_human, get_round_seconds_elapsed());
            }
        }
    }
    bool only_player = infection_total_players_in_game() == 1;
    game_engine_end_round_with_winner(get_winner_player_index(), only_player, game_engine_end_condition_infection);
}

namespace {

static __forceinline c_game_engine_infection_variant *infection_kill_variant() {
    return (c_game_engine_infection_variant *)g_infection_variant;
}
static __forceinline game_engine_infection_state *infection_kill_tracking() {
    return &((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection;
}
static __forceinline void infection_initialize_event(long type, long index, s_game_engine_event_data *event) {
    event->type = type; event->event = index;
    event->identifier = -1; event->flags = -1; event->cause_player_index = -1; event->cause_team_index = -1;
    event->effect_player_index = -1; event->effect_team_index = -1; event->data = 0; event->data_word = -1;
}
}

// @address 0x2f2fb8 ?player_killed_player@c_infection_engine@@UBAXJJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z
void c_infection_engine::player_killed_player(long killer, long assistant, long victim, bool betrayal, c_flags<e_game_engine_kill_flags,
    long, 24> flags, long damage) const
{
    game_engine_handle_player_killed(this, killer, assistant, victim, betrayal, (unsigned long)flags.get(), damage);
    if (infection_is_authoritative())
    {
        long victim_absolute_index = datum_index_to_absolute_index(victim);
        long killer_absolute_index = datum_index_to_absolute_index(killer);
        game_engine_infection_engine_globals *globals = infection_engine_globals();
        bool victim_not_zombie = !globals->infection.zombie_flags.test(victim_absolute_index);
        c_game_statborg *statborg = game_engine_get_statborg();
        if (killer == victim)
        {
            game_engine_adjust_player_score(killer, infection_kill_variant()->m_limit_signed_bytes[1]);
        }
        else if (player_is_zombie(killer) && killer != -1
                 && (globals->infection.zombie_team_flags.bits & (1 << infection_player_get(killer)->configuration.host.team_index)) != 0)
        {
            if (player_is_zombie(victim))
            {
                game_engine_adjust_player_score(killer, infection_kill_variant()->m_time_signed_bytes[0]);
            }
            else
            {
                s_game_engine_event_data event;
                infection_initialize_event(11, 1, &event);
                event.cause_player_index = killer;
                event.effect_player_index = victim;
                game_engine_send_event((s_game_engine_event_data *)&event);
                player_set_zombie_status(victim, true, false);
                game_engine_adjust_player_score(killer, infection_kill_variant()->m_points_signed_bytes[1]);
                statborg->adjust_player_stat(killer, _statborg_entry_infection_infections, 1, -1, false);
                game_results_statistic_increment(killer_absolute_index, _infection_team_none, _game_results_statistic_infection_infections, 1);
                infection_kill_tracking()->infection_kills[killer_absolute_index]++;
                long kills = infection_kill_tracking()->infection_kills[killer_absolute_index];
                switch (kills)
                {
                case 5:
                {
                    int event_value = 0;
                    ::game_engine_handle_time_messages(27, killer_absolute_index, killer, victim, (int *)(&event_value));
                    break;
                }
                case k_infection_kill_milestone:
                {
                    int event_value = 0;
                    ::game_engine_handle_time_messages(28, killer_absolute_index, killer, victim, (int *)(&event_value));
                    break;
                }
                }
            }
        }
        else if (killer != -1)
        {
            if (player_is_zombie(victim))
            {
                game_engine_adjust_player_score(killer, infection_kill_variant()->m_points_signed_bytes[0]);
                game_results_statistic_increment(killer_absolute_index, _infection_team_none, _game_results_statistic_infection_zombie_kills, 1);
                game_engine_get_statborg()->adjust_player_stat(killer, _statborg_entry_infection_zombie_kills, 1, -1, false);
                infection_kill_tracking()->zombie_kills[killer_absolute_index]++;
                long kills = infection_kill_tracking()->zombie_kills[killer_absolute_index];
                switch (kills)
                {
                case 5:
                {
                    int event_value = 0;
                    ::game_engine_handle_time_messages(29, killer_absolute_index, killer, victim, (int *)(&event_value));
                    break;
                }
                case k_infection_kill_milestone:
                {
                    int event_value = 0;
                    ::game_engine_handle_time_messages(30, killer_absolute_index, killer, victim, (int *)(&event_value));
                    break;
                }
                }
            }
            else
            {
                game_engine_adjust_player_score(killer, infection_kill_variant()->m_time_signed_bytes[0]);
            }
        }
        if (victim_not_zombie)
        {
            if (human_count() == 0)
                game_results_medal_increment(victim);
            if ((game_engine_limited_lives_active() && infection_player_get(victim)->lives == 0) || player_is_zombie(killer))
            {
                statborg->adjust_player_stat(victim, _statborg_entry_infection_time_as_human, (short)get_round_seconds_elapsed(), -1, false);
                game_results_statistic_increment(victim_absolute_index, _infection_team_none,
                    _game_results_statistic_infection_time_as_human, get_round_seconds_elapsed());
            }
        }
    }
}

namespace {
static __forceinline game_engine_infection_state *game_engine_infection_round_state()
{
    return &((game_engine_infection_engine_globals *)tls_game_engine_globals())->infection;
}
static __forceinline long game_engine_infection_seconds_to_ticks(long seconds)
{
    return game_tick_rate() * seconds;
}
}
struct vip_s_implicit_geometry_shape;

namespace {
static __forceinline void game_engine_infection_award_last_human_bonus(long player_index)
{
    if (player_index != -1)
        game_results_medal_increment(player_index);
}
static __forceinline void game_engine_infection_announce_last_human(long player_index)
{
    s_game_engine_event_data event;
    infection_initialize_event(11, 4, &event);
    event.cause_player_index = player_index;
    event.cause_team_index = infection_player_get(player_index)->configuration.host.team_index;
    event.effect_player_index = player_index;
    event.effect_team_index = infection_player_get(player_index)->configuration.host.team_index;
    game_engine_send_event(&event);
}
}
// @address 0x2f3500 ?update@c_infection_engine@@UBAXXZ
void c_infection_engine::update() const
{
    if (infection_is_authoritative()) {
        if (infection_safe_havens_enabled() && (short)g_infection_variant->m_safe_haven_movement_time != 0) {
            if (--game_engine_infection_round_state()->haven_timer <= 0) {
                game_engine_infection_round_state()->haven_timer = game_engine_infection_seconds_to_ticks(
                    (short)g_infection_variant->m_safe_haven_movement_time);
                game_engine_infection_round_state()->safe_havens.select_area((e_area_cycle_options)(g_infection_variant->m_safe_havens != 2));
                game_engine_infection_round_state()->haven_players = 0;
                simulation_action_game_engine_globals_update(768);
                if (g_infection_variant->m_flags_byte & 1) {
                    c_player_in_game_iterator players;
                    players.m_iterator.m_iterator.absolute_index = -1;
                    players.m_iterator.m_iterator.index = -1;
                    players.m_iterator.m_iterator.data = tls_player_data();
                    while (players.next()) {
                        player_datum *player = (player_datum *)players.m_iterator.m_datum;
                        if (player->respawn_timer > 0) {
                            player->respawn_timer = 0;
                            player->respawn_seconds = 0;
                            ::simulation_action_game_engine_player_update((short)players.m_iterator.m_iterator.index, 11);
                        }
                    }
                }
            } else {
                simulation_action_game_engine_globals_update(512);
            }
        }
        if (game_engine_in_round()) {
            long last_human = get_last_human_player_index();
            if (last_human != -1) {
                if (!game_engine_infection_round_state()->last_human_bonus_awarded) {
                    game_engine_infection_announce_last_human(last_human);
                    game_engine_infection_round_state()->last_human_bonus_awarded = true;
                }
            } else {
                game_engine_infection_round_state()->last_human_bonus_awarded = false;
            }
            long time_limit = game_engine_infection_seconds_to_ticks(g_infection_variant->get_miscellaneous_options()->get_round_time_limit_minutes()) * 60;
            if (time_limit > 0 && time_limit - infection_game_engine_round_time_get() <= 0) {
                game_engine_infection_award_last_human_bonus(last_human);
                end_round();
            } else {
                long zombies = 0;
                c_player_in_game_iterator players;
                players.m_iterator.m_iterator.absolute_index = -1;
                players.m_iterator.m_iterator.index = -1;
                players.m_iterator.m_iterator.data = tls_player_data();
                while (players.next()) {
                    if (player_is_zombie(players.m_iterator.m_iterator.index))
                        ++zombies;
                }
                if (zombies == 0) {
                    game_engine_infection_award_last_human_bonus(last_human);
                    end_round();
                } else if (human_count() == 0 && tls_player_data()->actual_count > 1) {
                    end_round();
                }
            }
        }
    }
    if (infection_safe_havens_enabled()) {
        game_engine_infection_round_state()->safe_havens.update();
        c_area_set<c_area, 12> *areas = &game_engine_infection_round_state()->safe_havens;
        for (c_area *area = areas->begin(); area != areas->end(); ++area) {
            if (area->is_selected())
                multiplayer_object_build_boundary_primitive((const vip_s_implicit_geometry_shape *)area, area->get_color(), area->get_object_index());
        }
    }
}

// @address 0x2f23c8 ?initialize_for_new_round@c_infection_engine@@UBA_NXZ
bool c_infection_engine::initialize_for_new_round() const
{
    if (infection_safe_havens_enabled())
        game_engine_infection_round_state()->safe_havens.initialize(0x440, 0x1000, _multiplayer_team_designator_none);

    game_engine_infection_round_state()->last_human_bonus_awarded = false;
    if (infection_is_authoritative()) {
        if (game_engine_get_round() == 0 || (unsigned char)g_infection_variant->m_next_zombie == 3) {
            game_engine_infection_round_state()->zombies = 0;
            choose_new_zombies(get_initial_zombie_count());
            game_engine_infection_round_state()->starting_zombies = game_engine_infection_round_state()->zombies;
            simulation_action_game_engine_globals_update(0xc0);
        } else {
            c_player_in_game_iterator players;
            players.m_iterator.m_iterator.absolute_index = -1;
            players.m_iterator.m_iterator.index = -1;
            players.m_iterator.m_iterator.data = tls_player_data();
            while (players.next()) {
                long player_index = players.m_iterator.m_iterator.index;
                if (player_is_zombie(player_index))
                    infection_handle_player_infection(player_index, true);
            }
        }
        compute_zombie_team_bitvector();
        if (g_infection_variant->m_safe_havens != 0) {
            game_engine_infection_round_state()->safe_havens.select_area((e_area_cycle_options)(g_infection_variant->m_safe_havens != 2));
            game_engine_infection_round_state()->haven_timer = game_engine_infection_seconds_to_ticks(
                (short)g_infection_variant->m_safe_haven_movement_time);
            simulation_action_game_engine_globals_update(0x200);
        }
    }
    return true;
}
