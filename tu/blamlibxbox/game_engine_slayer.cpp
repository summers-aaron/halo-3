// blamlibxbox/game_engine_slayer.obj: from the records 0x2e7450 0x2e74b8 0x2e74c8 0x2e7520 0x2e75e8 0x2e7818 0x2e7870 0x2e78d0 0x2e7930
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "game/game_types.h"
#include "memory/tls_globals.h"
#include "game/player_types.h"
#include "game/game_variant.h"
#include "game/game_options.h"
#include "game/game_globals_storage.h"
#include "game/player_traits.h"
#include <string.h>
#include "game/game_engine.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "simulation/simulation_entity_types.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "game/game_engine_kill_flags_types.h"
#include "game/game_engine_slayer_events.h"
#include "game/game_engine_slayer.h"
#include "game/game_engine_event_data.h"
#include "memory/bitstream.h"
#include "game/game_variant_inline.h"
#include "game/game_simulation_predicates.h"
#include "game/player_access.h"

// @address 0x8c43c0 ?slayer_variant@@3PAVc_game_engine_slayer_variant@@A
#if !defined(_XBOX) || defined(__clang__)
// The shared .bss pool that holds the game-engine variant pointers cannot take a native definition on the
// Xbox link (wpl: shared BSS range), so this unit owns the one external definition for the port.
c_game_engine_slayer_variant *slayer_variant;
#endif

struct s_slayer_engine_state_data
{
    s_game_engine_state_data global_state;
};

// @address 0x41a68 ??_7c_slayer_engine@@6B@
// @address 0x79d424 ?internal_slayer_engine@@3Vc_slayer_engine@@A
c_slayer_engine internal_slayer_engine;

// @address 0x3fc598 ?get_type@c_slayer_engine@@UBAJXZ
long c_slayer_engine::get_type() const
{
    return _game_engine_type_slayer;
}

// @address 0x2e7450 ?initialize_for_new_map@c_slayer_engine@@UBA_NXZ
bool c_slayer_engine::initialize_for_new_map() const
{
    current_game_variant()->get_active_variant()->can_be_cast_to(_game_engine_slayer, (const void **)&slayer_variant);
    return true;
}

// @address 0x2e74b8 ?dispose_from_old_map@c_slayer_engine@@UBAXXZ
void c_slayer_engine::dispose_from_old_map() const
{
    slayer_variant = 0;
}


// @address 0x2e74c8 ?emit_game_start_event@c_slayer_engine@@UBAXJ@Z
void c_slayer_engine::emit_game_start_event(long player_index) const
{
    s_game_engine_event_data event;
    event.type = 2;
    event.event = 0;
    event.identifier = -1;
    event.data = 0;
    event.audience_player_index = player_index;
    event.cause_player_index = -1;
    event.cause_team_index = -1;
    event.effect_player_index = -1;
    event.effect_team_index = -1;
    event.data_word = -1;
    game_engine_send_event(&event);
}

// @address 0x2e7818 ?player_assisted_with_kill@c_slayer_engine@@UBAXJ@Z
void c_slayer_engine::player_assisted_with_kill(long player_index) const
{
    if (game_simulation::game_is_predicted())
        return;
    if (player_index == -1)
        return;
    signed char assist_points = slayer_variant->get_assist_points();
    if (assist_points == 0)
        return;
    game_engine_adjust_player_score(player_index, assist_points);
}

void c_slayer_engine::build_simulation_baseline(long state_data_size, void *state_data) const
{
    s_slayer_engine_state_data *state = (s_slayer_engine_state_data *)state_data;
    memset(state, 0, sizeof(*state));
    build_global_baseline(&state->global_state);
}

void c_slayer_engine::build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const
{
    s_slayer_engine_state_data *state = (s_slayer_engine_state_data *)state_data;
    unsigned long actual_update_mask = 0;
    unsigned long global_update_mask = *update_mask & 63;
    if (global_update_mask != 0)
        build_global_update(global_update_mask, &actual_update_mask, &state->global_state);
    *update_mask = actual_update_mask;
}

bool c_slayer_engine::apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const
{
    const s_slayer_engine_state_data *state = (const s_slayer_engine_state_data *)state_data;
    bool success = true;
    unsigned long global_update_mask = update_mask & 63;
    if (global_update_mask != 0)
    {
        bool global_success = apply_global_update(global_update_mask, &state->global_state);
        success = success && global_success;
    }
    return success;
}

// @address 0x2f3910 ?test_flag@c_slayer_engine@@UBA_NJ@Z
bool c_slayer_engine::test_flag(long flag) const
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

// @address 0x42d298 ?get_simulation_entity_type@c_slayer_engine@@UBA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_slayer_engine::get_simulation_entity_type() const
{
    return _simulation_entity_type_slayer_engine_globals;
}

// @address 0x2e75e8 ?player_killed_player@c_slayer_engine@@UBAXJJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z
void c_slayer_engine::player_killed_player(long killer, long dead_unit, long victim, bool betrayal,
    c_flags<e_game_engine_kill_flags, long, 24> flags, long spree) const
{
    game_engine_handle_player_killed(this, killer, dead_unit, victim, betrayal, flags, spree);
    if (game_simulation::game_is_authoritative()) {
        player_datum *player = player_get(victim);
        bool suicide = victim != -1 && killer == victim;
        long killer_score = 0;
        long victim_score = 0;
        long kill_points = slayer_variant->m_kill_points;
        long death_points = slayer_variant->m_death_points;
        long suicide_points = slayer_variant->m_suicide_points;
        long betrayal_points = slayer_variant->m_betrayal_points;
        long leader_points = slayer_variant->m_leader_killed_points;
        long elimination_points = slayer_variant->m_elimination_points;
        long assassination_points = slayer_variant->m_assassination_points;
        long headshot_points = slayer_variant->m_headshot_points;
        long melee_points = slayer_variant->m_melee_points;
        long sticky_points = slayer_variant->m_sticky_points;
        long splatter_points = slayer_variant->m_splatter_points;
        long spree_points = slayer_variant->m_killing_spree_points;
        if (killer != -1 && !(player->flags & 2)) {
            if (suicide) killer_score += suicide_points;
            else if (betrayal) {
                killer_score += betrayal_points;
                victim_score += death_points;
            } else {
                killer_score += kill_points;
                victim_score += death_points;
            }
        }
        if (!betrayal && !suicide) {
            if (flags.test(_game_engine_kill_flag_melee) && !flags.test(_game_engine_kill_flag_assassination)) killer_score += melee_points;
            if (flags.test(_game_engine_kill_flag_assassination)) killer_score += assassination_points;
            if (flags.test(_game_engine_kill_flag_splatter)) killer_score += splatter_points;
            if (flags.test(_game_engine_kill_flag_sticky)) killer_score += sticky_points;
            if (flags.test(_game_engine_kill_flag_headshot)) killer_score += headshot_points;
            if (flags.test(_game_engine_kill_flag_leader)) killer_score += leader_points;
            if (flags.test(_game_engine_kill_flag_team_elimination) || flags.test(_game_engine_kill_flag_solo_elimination)) killer_score += elimination_points;
            if (spree >= 5 && spree % 5 == 0) killer_score += spree_points;
        }
        if (killer_score != 0) game_engine_adjust_player_score(killer, killer_score);
        if (victim_score != 0) game_engine_adjust_player_score(victim, victim_score);
    }
}


// @address 0x2e7520 ?apply_game_engine_traits_for_player@c_slayer_engine@@UBAXJPAVc_player_traits@@@Z
void c_slayer_engine::apply_game_engine_traits_for_player(long player_index, c_player_traits *player_traits) const
{
    if (current_game_variant())
    {
        bool team_leader = false;
        bool player_leader = game_engine_get_player_place(player_index) == 0;
        if (game_engine_has_teams())
        {
            game_engine_are_teams_tied();
            player_datum *player = player_get(player_index);
            team_leader = game_engine_find_team_place(player->configuration.host.team_index) == 0;
        }
        if (team_leader || player_leader)
            player_traits->set(&slayer_variant->m_leader_traits, false);
    }
}
