// blamlibxbox/game_engine_oddball.obj: from the records 0x2e1120 0x2e7998 0x2e7a28 0x2e7a38 0x2e7a48 0x2e7ad0 0x2e7b40 0x2e7b78 0x2e7cb8 0x2e7de0 0x2e7ee8 0x2e8008 0x2e8090 0x2e8178 0x2e81d0 0x2e8260 0x2e8428 0x2e8478 0x2e8518 0x2e8590 0x2e8818 0x2e8a28 0x2e8b40 0x2e8c10 0x2e8ef0 0x2e90a0 0x2e9190 0x2e9370
// @flags /TP /O2 /Oc
#include "memory/datum_index.h"
#include "math/random_math_range.h"
#include "memory/tls_globals.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "simulation/simulation_entity_types.h"
#include "tag_files/tag_groups.h"
#include "game/game_engine_kill_flags_types.h"
#include "math/real_math_globals.h"
#include "game/game_time_inline.h"
#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "memory/static_arrays.h"
#include "cseries/cseries_wrapped.h"
#include "game/game_time_types.h"
#include "game/game_types.h"
#include "game/player_traits.h"
#include "objects/object_iterator.h"
#include "objects/object_placement.h"
#include "objects/object_header.h"
#include "items/multiplayer_weapon_tracker.h"
#include "game/game_engine.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_notifications.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "units/units.h"
#include "units/unit_types.h"
#include "ai/weapon_datum_types.h"
#include "items/weapon_definitions.h"
#include "items/item_types.h"
#include "objects/object_definitions.h"
#include "game/player_absolute_index.h"
#include "game/game_engine_spawn_influencer.h"
#include "game/player_types.h"
#include "game/game_engine_event_data.h"
#include "cache/cache_files.h"
#include "game/game_engine_oddball.h"
#include "game/game_engine_interface_state_types.h"
#include "interface/chud/chud_user_data_types.h"
#include "game/game_results_event_carry_types.h"
#include "game/game_engine_scoring_globals.h"
#include "game/game_globals_storage.h"
#include "interface/chud/chud_navpoint_types.h"
#include "tag_files/tag_types.h"
#include "tag_files/string_ids.h"
#include "objects/object_types.h"
#include "game/game_engine_default.h"
#include <math.h>
#include <string.h>
#include "game/game_engine_oddball_variant.h"
#include "game/game_results_types.h"
#include "game/game_results.h"
#include "game/game_engine_slayer_events.h"
#include "game/player_in_game_iterator.h"

// @address 0x8c43c4 ?g_oddball_variant@@3PAVc_game_engine_oddball_variant@@A
#if !defined(_XBOX) || defined(__clang__)
// The shared .bss pool that holds the game-engine variant pointers cannot take a native definition on the
// Xbox link (wpl: shared BSS range), so this unit owns the one external definition for the port.
c_game_engine_oddball_variant *g_oddball_variant;
#endif
// @address 0x2e8478 ?assemble_spawn_influencers_for_player@c_oddball_engine@@UBAXJPAUs_netgame_goal_influencer@@PAJ@Z
// @address 0x2e7a48 ?emit_game_start_event@c_oddball_engine@@UBAXJ@Z
// @address 0x2e8260 ?player_killed_player@c_oddball_engine@@UBAXJJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z

class c_oddball_object_iterator : public c_object_iterator_base {
public:
    void begin(unsigned long type_mask, unsigned long header_mask, unsigned long match_flags)
    {
        object_iterator_begin_internal(type_mask, header_mask, match_flags);
    }
    bool next() { return object_iterator_next_with_match_flags_internal(); }
};

// @address 0x41ba8 ??_7c_oddball_engine@@6B@
// @address 0x79d428 ?internal_oddball_engine@@3Vc_oddball_engine@@A
c_oddball_engine internal_oddball_engine;

// @address 0x45fc28 ?get_type@c_oddball_engine@@UBAJXZ
long c_oddball_engine::get_type() const
{
    return _game_engine_type_oddball;
}

// @address 0x45fc28 ?get_simulation_entity_type@c_oddball_engine@@UBA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_oddball_engine::get_simulation_entity_type() const
{
    return _simulation_entity_type_oddball_engine_globals;
}

// @address 0x42d298 ?leader_event_lead_enabled@c_oddball_engine@@UBA_NXZ
bool c_oddball_engine::leader_event_lead_enabled() const
{
    return false;
}

namespace {
static __forceinline bool player_has_ball(long player_index)
{
    return game_engine_get_carried_multiplayer_weapon_index(player_index, 2) != -1;
}
}

// @address 0x2e1120 ?get_score_string@c_oddball_engine@@UBAXJPAV?$c_static_wchar_string@$0BAA@@@@Z
void c_oddball_engine::get_score_string(long score, c_static_wchar_string<256> *score_string) const
{
    score_string->print(L"%d", score);
}

// @address 0x2e7998 ?initialize_for_new_map@c_oddball_engine@@UBA_NXZ
bool c_oddball_engine::initialize_for_new_map() const
{
    game_options *options = &((game_globals_storage *)tls_game_globals())->options;
    c_game_variant *variant = (options->game_mode == 2) ? &options->game_variant : 0;
    variant->get_active_variant()->can_be_cast_to(_game_engine_type_oddball, (const void **)&g_oddball_variant);
    memset(get_oddball_globals(), 0, sizeof(s_oddball_globals));
    oddball_reset_all();
    return 1;
}

// @address 0x2e8090 ?get_hud_interface_state@c_oddball_engine@@UBAXJPAUgame_engine_interface_state@@@Z
void c_oddball_engine::get_hud_interface_state(long player_index, game_engine_interface_state *state) const
{
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next())
    {
        long index = iterator.m_iterator.m_iterator.index;
        if (!player_has_ball(index))
            continue;
        long team_index = iterator.m_iterator.m_datum->configuration.host.team_index;
        if (team_index == ((player_datum *)tls_player_data()->data)[datum_index_to_absolute_index(player_index)].configuration.host.team_index)
        {
            state->team_in_hill = true;
            continue;
        }
        if (game_engine_has_teams() && team_index == state->rival_index)
        {
            state->enemy_in_hill = true;
            continue;
        }
        if (game_engine_has_teams())
            continue;
        if (index != state->rival_index)
            continue;
        state->enemy_in_hill = true;
    }
}

// @address 0x2e8178 ?apply_game_engine_traits_for_player@c_oddball_engine@@UBAXJPAVc_player_traits@@@Z
void c_oddball_engine::apply_game_engine_traits_for_player(long player_index, c_player_traits *player_traits) const
{
    if (game_engine_get_carried_multiplayer_weapon_index(player_index, 2) != -1)
        player_traits->set((const c_player_traits *)g_oddball_variant->get_carrier_traits(), false);
}

void c_oddball_engine::dispose_from_old_map() const
{
    g_oddball_variant = 0;
}

// @address 0x2e7a38 ?prepare_for_new_state@c_oddball_engine@@UBAXJ@Z
void c_oddball_engine::prepare_for_new_state(long incoming_state) const
{
    if ((incoming_state != 1)) return;
    oddball_reset_all();
    return;
}

bool c_oddball_engine::should_auto_pickup_weapon(long player_index, long weapon_index) const
{
    bool result = false;
    if (g_oddball_variant->get_auto_ball_pickup())
    {
        if (weapon_index != -1)
        {
            weapon_datum *weapon = OBJECT_GET(weapon_index, weapon_datum, _object_mask_weapon);
            result = TAG_GET('weap', weapon_definition, weapon->definition_index)->multiplayer_weapon_type == 2;
        }
    }
    return result;
}

// unnamed vtable slot (allow_weapon_pickup on c_oddball_engine): calls the base engine's method when the weapon has no multiplayer identifier
// @address 0x2e7b40 ?allow_weapon_pickup@c_oddball_engine@@UBA_NJJ@Z
bool c_oddball_engine::allow_weapon_pickup(long player_index, long object_index) const
{
    bool result = true;
    if (OBJECT_GET(object_index, weapon_datum, _object_mask_weapon)->multiplayer_weapon_identifier == -1)
        result = c_game_engine::allow_weapon_pickup(player_index, object_index);
    return result;
}

namespace {
static __forceinline unsigned char oddball_object_get_type(long object_index)
{
    return object_header_get(object_index)->object_type;
}
static __forceinline float oddball_game_ticks_to_seconds(float ticks)
{
    return ticks * game_tick_length();
}
}

float c_oddball_engine::compute_object_function(long object_index, long function_name) const
{
    float result = 0.0f;
    if (g_oddball_variant->get_ball_effect_enabled() && function_name == _string_id_oddball_effect
        && oddball_object_get_type(object_index) == _object_type_weapon)
    {
        weapon_datum *weapon = OBJECT_GET(object_index, weapon_datum, _object_mask_weapon);
        long ball_index = weapon->multiplayer_weapon_identifier;
        if (ball_index != -1)
        {
            const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)ball_index);
            long drop_ticks = get_oddball_globals()->ball_drop_ticks[ball_index];
            if (tracker->carrier_player_index != -1)
                result = 1.0f;
            else if (drop_ticks == -1)
                result = 0.0f;
            else
            {
                float elapsed = oddball_game_ticks_to_seconds((float)(game_time_get() - drop_ticks));
                result = 1.0f - elapsed / 2.0f;
                result = (result > 0.0f ? result : 0.0f) < 1.0f ? (result > 0.0f ? result : 0.0f) : 1.0f;
            }
        }
    }
    return result;
}

void c_oddball_engine::emit_game_start_event(long player_index) const
{
    bool predicted = false;
    if ((((game_globals_storage *)tls_game_globals())->options.simulation_type == 4))
        predicted = true;
    if (predicted == 0)
    {
        s_game_engine_event_data e;
        e.event = 0;
        e.data = 0;
        e.audience_player_index = player_index;
        e.type = 4;
        e.identifier = -1;
        e.cause_player_index = -1;
        e.cause_team_index = -1;
        e.effect_player_index = -1;
        e.effect_team_index = -1;
        e.data_word = -1;
        game_engine_send_event(&e);
    }
}
// @address 0x2e7cb8 ?multiplayer_weapon_picked_up@c_oddball_engine@@UBAXJJ@Z
void c_oddball_engine::multiplayer_weapon_picked_up(long picked_weapon_index, long picker_player_index) const
{
    int ball_slot;
    unit_datum *picker_unit;
    int cause_player;
    unsigned char is_predicted;
    s_game_engine_event_data e;
    is_predicted = 0;
    ball_slot = OBJECT_GET(picked_weapon_index, weapon_datum, _object_mask_weapon)->multiplayer_weapon_identifier;
    if (((game_globals_storage *)tls_game_globals())->options.simulation_type == 4)
        is_predicted = true;
    if (is_predicted) return;
    if (((unsigned int)ball_slot > 4)) return;
    get_oddball_globals()->score_ticks[ball_slot] = game_tick_rate();
    if ((picker_player_index == -1)) return;
    picker_unit = OBJECT_GET(picker_player_index, unit_datum, _object_mask_unit);
    if ((picker_unit->unit.player_index == -1)) return;
    e.type = 4; e.event = 2;
    e.identifier = -1;
    e.audience_player_index = -1;
    e.cause_player_index = -1;
    e.cause_team_index = -1;
    e.effect_player_index = -1;
    e.effect_team_index = -1;
    e.data = 0;
    e.data_word = -1;
    cause_player = picker_unit->unit.player_index;
    e.cause_player_index = cause_player;
    e.cause_team_index = ((player_datum
        *)tls_player_data()->data)[datum_index_to_absolute_index(cause_player)].configuration.host.team_index;
    game_engine_send_event(&e);
    return;
}
// @address 0x2e7de0 ?multiplayer_weapon_dropped@c_oddball_engine@@UBAXJJ@Z
void c_oddball_engine::multiplayer_weapon_dropped(long dropped_weapon_index, long unit_index) const
{
    int ball_slot;
    unit_datum *unit_object;
    int cause_player;
    int index;
    unsigned char is_predicted;
    s_game_engine_event_data drop_event;
    is_predicted = 0;
    ball_slot = OBJECT_GET(dropped_weapon_index, weapon_datum, _object_mask_weapon)->multiplayer_weapon_identifier;
    if (((game_globals_storage *)tls_game_globals())->options.simulation_type == 4)
        is_predicted = true;
    if (is_predicted) return;
    if (((unsigned int)ball_slot > 4)) return;
    if (unit_index != -1)
    {
        unit_object = OBJECT_GET(unit_index, unit_datum, _object_mask_unit);
        if (unit_object->unit.player_index != -1)
        {
    drop_event.cause_player_index = -1;
    drop_event.cause_team_index = -1;
    drop_event.identifier = -1;
    drop_event.audience_player_index = -1;
    drop_event.effect_player_index = -1;
    drop_event.type = 4;
    drop_event.effect_team_index = -1;
    drop_event.data_word = -1;
    drop_event.event = 3;
    drop_event.data = 0;
    cause_player = unit_object->unit.player_index;
    index = datum_index_to_absolute_index(cause_player);
    drop_event.cause_player_index = cause_player;
    drop_event.cause_team_index = ((player_datum *)tls_player_data()->data)[index].configuration.host.team_index;
    game_engine_send_event(&drop_event);
        }
    }
    get_oddball_globals()->score_ticks[ball_slot] = -1;
    return;
}
static __forceinline item_datum *oddball_nav_object_get(long index)
{
    return OBJECT_GET(index, item_datum, _object_mask_all);
}
// @address 0x2e7ee8 ?submit_nav_points@c_oddball_engine@@UBAXJJ@Z
void c_oddball_engine::submit_nav_points(long user_index, long player_index) const
{
    for (long ball_index = 0; ball_index < 5; ++ball_index) {
        const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)ball_index);
        if (tracker && tracker->weapon_index != -1 && tracker->carrier_player_index == -1) {
            s_chud_navpoint navpoint;
            long index = tracker->weapon_index;
            navpoint.object_index = index;
            navpoint.scale = 1.0f;
            navpoint.flags = 65;
            navpoint.type = 2;
            navpoint.icon = 7;
            navpoint.state = 1;
            navpoint.name[0] = 0;
            item_datum *object = oddball_nav_object_get(index);
            if (object->owner_relation) unit_get_head_position(object->unit_object_index, &navpoint.position);
            else
            {
                const long *src = (const long *)&object->object.bounding_center;
                long *dst = (long *)&navpoint.position;
                *dst++ = *src++;
                long y = *src++;
                long z = *src;
                *(dst + 1) = z;
                *dst = y;
            }
            if ((unsigned long)user_index <= 3)
                chud_persistent_user_data()[user_index].m_navpoint_manager.submit_navpoint(&navpoint);
        }
    }
    c_game_engine::submit_nav_points(user_index, player_index);
}

bool c_oddball_engine::build_player_nav_point(long user_index, long player_index, long target_player_index, bool target,
    s_chud_navpoint *navpoint) const
{
    bool result = c_game_engine::build_player_nav_point(user_index, player_index, target_player_index, target, navpoint);
    if (result && game_engine_get_carried_multiplayer_weapon_index(player_index, 2) != -1)
    {
        navpoint->icon = 7;
        if (navpoint->type == -1)
            navpoint->type = 1;
        navpoint->flags |= 1;
    }
    return result;
}

long c_oddball_engine::get_player_state_index(long player_index, bool *state_is_valid) const
{
    long result = -1;
    *state_is_valid = true;
    if (player_has_ball(player_index))
        result = 15;
    return result;
}

void c_oddball_engine::assemble_spawn_influencers_for_player(long player_index,
    s_netgame_goal_influencer *influencers, long *influencer_count) const
{
    int weapon_id = 0;
    *influencer_count = 0;
    while (true) {
        const s_multiplayer_weapon_tracker *weapon = game_engine_multiplayer_weapon_get((short)weapon_id);
        long *count = influencer_count;
        if (weapon != 0 && weapon->weapon_index != -1) {
            influencers[*count].type = 0xb;
            influencers[*count].object_index = weapon->weapon_index;
            *count = *count + 1;
        }
        if (*count == 0x10)
            break;
        weapon_id = weapon_id + 1;
        if (5 <= weapon_id)
            return;
    }
}

void c_oddball_engine::build_simulation_baseline(long state_data_size, void *state_data) const
{
    s_oddball_engine_state_data *state = (s_oddball_engine_state_data *)state_data;
    memset(state, 0, sizeof(*state));
    build_global_baseline((s_game_engine_state_data *)state);
    short no_player = -1;
    state->owner_player_indices.set_all(no_player);
}

void c_oddball_engine::build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const
{
    s_oddball_engine_state_data *state = (s_oddball_engine_state_data *)state_data;
    unsigned long changed = 0;
    unsigned long base_mask = *update_mask & 63;
    if (base_mask)
        build_global_update(base_mask, &changed, (s_game_engine_state_data *)state);
    if (*update_mask & 64)
    {
        for (long ball_index = 0; ball_index < 5; ++ball_index)
        {
            if (state->spawn_timers[ball_index] != get_oddball_globals()->ball_object_indices[ball_index])
            {
                state->spawn_timers[ball_index] = get_oddball_globals()->ball_object_indices[ball_index];
                changed |= 64;
            }
        }
    }
    if (*update_mask & 128)
    {
        for (long ball_index = 0; ball_index < 5; ++ball_index)
        {
            if (!(fabs(state->spawn_positions[ball_index].x - get_oddball_globals()->ball_positions[ball_index].x) < 0.0001f &&
                  fabs(state->spawn_positions[ball_index].y - get_oddball_globals()->ball_positions[ball_index].y) < 0.0001f &&
                  fabs(state->spawn_positions[ball_index].z - get_oddball_globals()->ball_positions[ball_index].z) < 0.0001f))
            {
                state->spawn_positions[ball_index] = get_oddball_globals()->ball_positions[ball_index];
                changed |= 128;
            }
        }
    }
    if (*update_mask & 256)
    {
        for (long ball_index = 0; ball_index < 5; ++ball_index)
        {
            if (state->reset_timers[ball_index] != get_oddball_globals()->ball_timers[ball_index])
            {
                state->reset_timers[ball_index] = get_oddball_globals()->ball_timers[ball_index];
                changed |= 256;
            }
        }
    }
    if (*update_mask & 512)
    {
        for (long ball_index = 0; ball_index < 5; ++ball_index)
        {
            long player_index = get_oddball_globals()->carrier_player_indices[ball_index] == -1 ? -1 :
                (unsigned short)get_oddball_globals()->carrier_player_indices[ball_index];
            if (state->owner_player_indices[ball_index] != player_index)
            {
                state->owner_player_indices[ball_index] = (short)player_index;
                changed |= 512;
            }
            if (state->ball_seconds[ball_index] != get_oddball_globals()->ball_states[ball_index])
            {
                state->ball_seconds[ball_index] = get_oddball_globals()->ball_states[ball_index];
                changed |= 512;
            }
        }
    }
    *update_mask = changed;
}

bool c_oddball_engine::apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const
{
    const s_oddball_engine_state_data *state = (const s_oddball_engine_state_data *)state_data;
    bool success = true;
    unsigned long base_mask = update_mask & 63;
    if (base_mask)
    {
        bool base_success = apply_global_update(base_mask, (const s_game_engine_state_data *)state);
        success = success && base_success;
    }
    if (update_mask & 64)
    {
        for (long ball_index = 0; ball_index < 5; ++ball_index)
            get_oddball_globals()->ball_object_indices[ball_index] = state->spawn_timers[ball_index];
    }
    if (update_mask & 128)
    {
        for (long ball_index = 0; ball_index < 5; ++ball_index)
            get_oddball_globals()->ball_positions[ball_index] = state->spawn_positions[ball_index];
    }
    if (update_mask & 256)
    {
        for (long ball_index = 0; ball_index < 5; ++ball_index)
            get_oddball_globals()->ball_timers[ball_index] = state->reset_timers[ball_index];
    }
    if (update_mask & 512)
    {
        for (long ball_index = 0; ball_index < 5; ++ball_index)
        {
            void *player = datum_try_and_get_absolute(tls_player_data(), state->owner_player_indices[ball_index]);
            long player_index = -1;
            if (player)
                player_index = player_index_from_absolute_player_index(state->owner_player_indices[ball_index]);
            get_oddball_globals()->carrier_player_indices[ball_index] = player_index;
            get_oddball_globals()->ball_states[ball_index] = state->ball_seconds[ball_index];
        }
    }
    return success;
}

// @address 0x2e8b40 ?oddball_ball_is_away_from_spawn_position@@YA_NJ@Z
bool oddball_ball_is_away_from_spawn_position(long ball_index)
{
    const s_multiplayer_weapon_tracker *entry = game_engine_multiplayer_weapon_get((short)ball_index);
    bool result = false;
    if (entry != 0 && entry->weapon_index != -1 && entry->carrier_unit_index == -1) {
        real_point3d pos;
        object_get_origin(entry->weapon_index, &pos);
        real_point3d *ref = &get_oddball_globals()->ball_positions[ball_index];
        float dx = ref->x - pos.x;
        float dz = ref->z - pos.z;
        float dy = ref->y - pos.y;
        float d = dx * dx;
        d = d + dz * dz;
        d = d + dy * dy;
        if (d > 0.25f)
            result = true;
    }
    return result;
}

// @address 0x2e90a0 ?oddball_reset_ball@@YA_NJ@Z
bool oddball_reset_ball(long ball_index)
{
    const s_multiplayer_weapon_tracker *weapon = game_engine_multiplayer_weapon_get((short)ball_index);
    bool result = false;
    if (weapon && weapon->weapon_index != -1) {
        real_point3d position;
        long spawn_index;
        long parent_index;
        if (oddball_find_ball_spawn_position(ball_index, &position, &spawn_index, &parent_index)) {
            game_engine_multiplayer_weapon_reset(weapon->weapon_index, &position);
            object_attach_to_marker_with_type(parent_index, 0x40004, weapon->weapon_index, 0x40004);
            ::simulation_action_object_update(weapon->weapon_index, 2048);
            s_game_engine_globals *globals = tls_game_engine_globals();
            globals->oddball.ball_timers[ball_index] = 0;
            globals->oddball.ball_positions[ball_index] = position;
            simulation_action_game_engine_globals_update(384);
            result = true;
        }
    }
    return result;
}

void c_oddball_engine::player_killed_player(
    long killer, long assistant, long victim, bool teamkill, c_flags<e_game_engine_kill_flags, long, 24> flags, long damage) const
{
    long absolute_index = datum_index_to_absolute_index(killer);
    long kill_points = g_oddball_variant->m_kill_points;
    long ball_kill_points = g_oddball_variant->m_ball_kill_points;
    long carrier_kill_points = g_oddball_variant->m_carrier_kill_points;
    game_engine_handle_player_killed((void *)this, killer, assistant, victim, teamkill, flags, damage);
    if (kill_points && !teamkill && killer != -1 && killer != victim)
        game_engine_adjust_player_score(killer, kill_points);
    if (game_engine_get_carried_multiplayer_weapon_index(victim, 2) != -1 && !teamkill && killer != -1 && killer != victim)
    {
        long team = game_engine_has_teams() ? ((player_datum *)tls_player_data()->data)[absolute_index].configuration.host.team_index : -1;
        game_results_statistic_increment(absolute_index, (e_game_team)team, _game_results_statistic_oddball_ball_carrier_kills, 1);
        if (carrier_kill_points)
            game_engine_adjust_player_score(killer, carrier_kill_points);
    }
    if (killer != -1 && !teamkill && killer != victim && game_engine_get_carried_multiplayer_weapon_index(killer, 2) != -1 &&
        ((((unsigned long)flags & 1) != 0) || (unsigned char)(((unsigned long)flags >> 1) & 1) != 0))
    {
        long team = game_engine_has_teams() ? ((player_datum *)tls_player_data()->data)[absolute_index].configuration.host.team_index : -1;
        game_results_statistic_increment(absolute_index, (e_game_team)team, _game_results_statistic_oddball_kills_as_carrier, 1);
        int event = 0;
        ::game_engine_handle_time_messages(50, absolute_index, killer, victim, (int *)(&event));
        if (ball_kill_points)
            game_engine_adjust_player_score(killer, ball_kill_points);
    }
}

// @address 0x2e8a28 oddball_award_carry_points
extern "C" void oddball_award_carry_points(long player_index)
{
    short points = g_oddball_variant->m_carrying_points;
    if (points == 0 || player_index == -1)
        return;
    long old_score = player_get_composite_score(player_index);
    game_engine_adjust_player_score(player_index, points);
    long new_score = player_get_composite_score(player_index);
    if (old_score == new_score)
        return;
    long score_to_win = game_engine_get_score_to_win_round();
    long event_index = -1;
    if (score_to_win > 10 && score_to_win - new_score == 10)
        event_index = 6;
    if (score_to_win > 25 && score_to_win - new_score == 25)
        event_index = 7;
    if (score_to_win > 50 && score_to_win - new_score == 50)
        event_index = 8;
    else if (event_index == -1)
        return;
    s_game_engine_event_data event;
    event.type = 4;
    event.event = event_index;
    event.identifier = -1;
    event.audience_player_index = -1;
    event.cause_team_index = -1;
    event.effect_player_index = -1;
    event.effect_team_index = -1;
    event.data = 0;
    event.cause_player_index = player_index;
    event.data_word = -1;
    event.cause_team_index = ((player_datum
        *)tls_player_data()->data)[datum_index_to_absolute_index(player_index)].configuration.host.team_index;
    game_engine_send_event(&event);
}

void oddball_update_ball(long ball_index)
{
    const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)ball_index);
    long carrier_player_index = -1;
    if (tracker && tracker->weapon_index != -1)
    {
        carrier_player_index = tracker->carrier_player_index;
        weapon_datum *ball = OBJECT_GET(tracker->weapon_index, weapon_datum, _object_mask_all);
        get_oddball_globals()->ball_object_indices[ball_index] = 0;
        simulation_action_game_engine_globals_update(64);
        if (carrier_player_index == -1 && oddball_ball_is_away_from_spawn_position(ball_index))
        {
            long return_ticks = game_tick_rate() * g_oddball_variant->m_ball_inactive_respawn_delay;
            short *timer = &get_oddball_globals()->ball_timers[ball_index];
            long ticks = *timer + 1;
            if (ticks > return_ticks)
                ticks = return_ticks;
            *timer = (short)ticks;
            simulation_action_game_engine_globals_update(256);
            if (get_oddball_globals()->ball_timers[ball_index] >= return_ticks &&
                ball->item.object.parent_object_index == -1 &&
                (unsigned char)((ball->item.object.flags >> 8) & 1) &&
                oddball_reset_ball(ball_index))
            {
                s_game_engine_event_data event;
                event.identifier = -1;
                event.audience_player_index = -1;
                event.cause_player_index = -1;
                event.cause_team_index = -1;
                event.effect_player_index = -1;
                event.type = 4;
                event.event = 4;
                event.effect_team_index = -1;
                event.data = 0;
                event.data_word = -1;
                game_engine_send_event(&event);
            }
        }
        else
        {
            get_oddball_globals()->ball_timers[ball_index] = 0;
            simulation_action_game_engine_globals_update(256);
        }

        if (carrier_player_index == -1)
        {
            if (game_engine_game_object_has_fallen_too_far(tracker->weapon_index))
                object_delete(tracker->weapon_index);
        }
        else
        {
            get_oddball_globals()->ball_drop_ticks[ball_index] = game_time_get();
        }
    }

    if (get_oddball_globals()->carrier_player_indices[ball_index] != carrier_player_index)
    {
        get_oddball_globals()->carrier_player_indices[ball_index] = carrier_player_index;
        get_oddball_globals()->ball_states[ball_index] = 0;
        get_oddball_globals()->score_ticks[ball_index] = (signed char)game_tick_rate();
        simulation_action_game_engine_globals_update(512);
    }

    get_oddball_globals()->score_ticks[ball_index]--;
    if (get_oddball_globals()->score_ticks[ball_index] <= 0)
    {
        get_oddball_globals()->score_ticks[ball_index] = (signed char)game_tick_rate();
        get_oddball_globals()->ball_states[ball_index] = (get_oddball_globals()->ball_states[ball_index] + 1) % 120;
        simulation_action_game_engine_globals_update(512);
        if (carrier_player_index != -1)
        {
            game_results_insert_carry_event(carrier_player_index, _game_results_event_carry_type_oddball, tracker->id);
            long absolute_player_index = datum_index_to_absolute_index(carrier_player_index);
            long team_index = game_engine_has_teams() ? ((player_datum
                *)tls_player_data()->data)[absolute_player_index].configuration.host.team_index : -1;
            game_results_statistic_increment(absolute_player_index, (e_game_team)team_index,
                _game_results_statistic_oddball_time_with_ball, 1);
        }
        if (game_engine_in_round())
            oddball_award_carry_points(carrier_player_index);
    }
}
static __forceinline float oddball_spawn_distance_squared(const real_point3d &a, const real_point3d &b)
{
    float x = a.x - b.x, y = a.y - b.y, z = a.z - b.z;
    return x * x + (z * z + y * y);
}
// @address 0x2e8c10 ?oddball_find_ball_spawn_position@@YA_NJPATreal_point3d@@PAJ1@Z
bool oddball_find_ball_spawn_position(long ball_index, real_point3d *point, long *weapon_definition, long *object_index)
{
    bool result = false;
    long candidates[8];
    long count = 0, priority = 0x7fffffff;
    c_oddball_object_iterator iterator;
    iterator.begin(0x440, 0, 128);
    while (iterator.next()) {
        long index = iterator.get_index();
        object_datum *object = (object_datum *)object_try_and_get_and_verify_type(index, 0xffffffff);
        if (!object || object->multiplayer_properties.offset == -1) continue;
        s_multiplayer_object_properties *properties = (s_multiplayer_object_properties *)((char *)object +
            object->multiplayer_properties.offset);
        if (!properties || properties->object_type != 14) continue;
        long object_priority = properties->spawn_sequence;
        if (object_priority > priority) continue;
        bool available = true;
        real_point3d position;
        object_get_origin(index, &position);
        for (long ball = 0; ball < 5; ball++) {
            const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)ball);
            if (tracker && tracker->weapon_index != -1 && tracker->carrier_unit_index == -1) {
                const real_point3d *spawn = &get_oddball_globals()->ball_positions[ball];
                real_point3d ball_origin;
                object_get_origin(tracker->weapon_index, &ball_origin);
                if (oddball_spawn_distance_squared(*spawn, ball_origin) < 0.25f && oddball_spawn_distance_squared(*spawn,
                    position) < 0.010000001f) {
                    available = false;
                    break;
                }
            }
        }
        if (available) {
            if (object_priority < priority) { count = 0; priority = object_priority; }
            else if (count >= sizeof(candidates) / sizeof(candidates[0])) break;
            candidates[count++] = index;
        }
    }
    if (count > 0) {
        short selected = (short)(((random_seed_next(tls_random_seed()) >> 16) * (short)count) >> 16);
        long index = candidates[selected];
        object_datum *object = OBJECT_GET(candidates[selected], object_datum, _object_mask_all);
        s_multiplayer_object_properties_definition *attachment = (s_multiplayer_object_properties_definition *)TAG_GET('obje',
            s_object_definition, object->definition_index)->multiplayer_object.address;
        if (attachment->boundary_effect.index != -1 && attachment->boundary_effect.group_tag == 0x77656170) {
            object_get_origin(index, point);
            *weapon_definition = attachment->boundary_effect.index;
            if (object_index) *object_index = index;
            result = true;
        }
    }
    return result;
}

// @address 0x2e8ef0 ?oddball_reset_all@@YAXXZ
void oddball_reset_all(void)
{
    bool predicted = false;
    if (((game_globals_storage *)tls_game_globals())->options.simulation_type == 4)
        predicted = true;
    if (predicted == 0) {
        game_engine_multiplayer_weapons_delete_all();
        get_oddball_globals()->ball_object_indices.set_all(0);
        get_oddball_globals()->ball_positions.set_all(*global_origin3d);
        get_oddball_globals()->ball_timers.set_all(0);
        get_oddball_globals()->carrier_player_indices.set_all(-1);
        get_oddball_globals()->score_ticks.set_all(0);
        get_oddball_globals()->ball_states.set_all(0);
    }
    get_oddball_globals()->ball_drop_ticks.set_all(-1);
}

static __forceinline bool oddball_game_is_predicted()
{
    bool result = false;
    if (((game_globals_storage *)tls_game_globals())->options.simulation_type == 4)
        result = true;
    return result;
}

// @address 0x2e81d0 ?update@c_oddball_engine@@UBAXXZ
void c_oddball_engine::update() const
{
    if (!oddball_game_is_predicted()) {
        long ball_count = g_oddball_variant->get_ball_count();
        for (long ball_index = 0; ball_index < ball_count; ball_index++) {
            oddball_create_ball(ball_index);
            oddball_update_ball(ball_index);
        }
    }
}

static __forceinline void game_engine_oddball_initialize_event(long type, long index, s_game_engine_event_data *event)
{
    event->type = type; event->event = index;
    event->identifier = -1; event->flags = -1; event->cause_player_index = -1; event->cause_team_index = -1;
    event->effect_player_index = -1; event->effect_team_index = -1; event->data = 0; event->data_word = -1;
}
static __forceinline long oddball_game_seconds_integer_to_ticks(long seconds)
{
    return seconds * game_tick_rate();
}
// @address 0x2e9190 ?oddball_create_ball@@YAXJ@Z
void oddball_create_ball(long ball_index)
{
    const s_multiplayer_weapon_tracker *weapon = game_engine_multiplayer_weapon_get((short)ball_index);
    if (weapon != 0 && weapon->weapon_index != -1) {
        get_oddball_globals()->ball_object_indices[ball_index] = 0;
        simulation_action_game_engine_globals_update(64);
        return;
    }
    long respawn_ticks = oddball_game_seconds_integer_to_ticks(g_oddball_variant->get_ball_spawn_delay());
    long timer = get_oddball_globals()->ball_object_indices[ball_index] + 1;
    if (timer > respawn_ticks)
        timer = respawn_ticks;
    get_oddball_globals()->ball_object_indices[ball_index] = timer;
    simulation_action_game_engine_globals_update(64);
    if (get_oddball_globals()->ball_object_indices[ball_index] >= respawn_ticks) {
        long weapon_definition;
        real_point3d position;
        long parent_index;
        if (oddball_find_ball_spawn_position(ball_index, &position, &weapon_definition, &parent_index)) {
            object_placement_data placement;
            object_placement_data_new(&placement, weapon_definition, -1, 0);
            placement.position = position;
            long object_index = object_new(&placement);
            if (object_index != -1) {
                get_oddball_globals()->ball_timers[ball_index] = 0;
                get_oddball_globals()->ball_positions[ball_index] = position;
                simulation_action_game_engine_globals_update(384);
                game_engine_multiplayer_weapon_register(object_index, (short)ball_index);
                object_attach_to_marker_with_type(parent_index, 0x40004, object_index, 0x40004);
                s_game_engine_event_data event;
                event.type = 4; event.event = 1;
                event.identifier = -1; event.flags = -1; event.cause_player_index = -1; event.cause_team_index = -1;
                event.effect_player_index = -1; event.effect_team_index = -1; event.data = 0; event.data_word = -1;
                game_engine_send_event(&event);
                simulation_action_object_create(object_index);
            }
        }
    }
}
