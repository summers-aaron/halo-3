// blamlibxbox/game_engine_juggernaut.obj: from the records 0x2edb68 0x2edc28 0x2edcf0 0x2edd78 0x2ede48 0x2ee200 0x2ee290 0x2ee2b8 0x2ee2e8 0x2ee3f8 0x2ee4a8 0x2ee508 0x2ee9c0 0x2eea78 0x2eeaa0 0x2eebd8 0x2eed20 0x2eee10 0x2eeeb8 0x2eef98 0x2ef068 0x2ef0b8 0x2ef180 0x2ef208 0x2ef358 0x2ef3d8 0x2ef510
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "game/game_engine_juggernaut_events.h"
#include "game/game_engine_kill_flags_types.h"
#include "math/real_math_types.h"
#include "memory/static_arrays.h"
#include "cseries/cseries_wrapped.h"
#include "memory/data_types.h"
#include "game/game_types.h"
#include "game/player_types.h"
#include "game/game_variant.h"
#include "game/game_options.h"
#include "game/game_globals_storage.h"
#include "game/player_traits.h"
#include "objects/object_datum.h"
#include "objects/object_iterator.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <new>
#include "game/game_engine.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_simulation_predicates.h"
#include "simulation/simulation.h"
#include "simulation/simulation_entity_types.h"
#include "game/game_engine_area_set.h"
#include "game/game_engine_util.h"
#include "objects/multiplayer_game_objects.h"
#include "objects/multiplayer_object_types.h"
#include "game/game_engine_vip.h"
#include "memory/data.h"
#include "game/game_engine_juggernaut.h"
#include "game/game_engine_event_data.h"
#include "game/area_cycle_options_types.h"
#include "game/game_engine_area_types.h"
#include "game/player_mapping_types.h"
#include "game/game_engine_area_set_types.h"
#include "game/game_engine_juggernaut_types.h"
#include "game/game_engine_interface_state_types.h"
#include "interface/chud/chud_user_data_types.h"
#include "game/game_engine_base_variant.h"
#include "game/game_engine_default.h"
#include "game/game_time_types.h"
#include "game/player_index_inlines.h"
#include "game/game_time_inlines.h"
#include "game/game_engine_juggernaut_variant.h"
#include "game/player_in_game_iterator.h"
#include "game/game_results_types.h"
#include "game/game_results.h"
#include "math/random_math_range.h"
#include "memory/data_typed_access.h"
#include "memory/data_handle_access.h"
#include "game/game_engine_end_condition.h"
// @address 0x2ee4a8 ?get_hud_interface_state@c_juggernaut_engine@@UBAXJPAUgame_engine_interface_state@@@Z
// @address 0x2eeaa0 ?build_simulation_update@c_juggernaut_engine@@UBAXPAKJPAX@Z
// @address 0x2ef358 ?find_selected_area@?$c_area_set@Vc_area@@$0M@@@QAAPAVc_area@@XZ

// @address 0x41f68 ??_7c_juggernaut_engine@@6B@
// @address 0x79d434 ?internal_juggernaut_engine@@3Vc_juggernaut_engine@@A
c_juggernaut_engine internal_juggernaut_engine;

class c_juggernaut_object_iterator : public c_object_iterator_base
{
public:
    void begin(unsigned long type_mask, unsigned long header_mask, unsigned long match_flags)
    {
        object_iterator_begin_internal(type_mask, header_mask, match_flags);
    }
    bool next() { return object_iterator_next_with_match_flags_internal(); }
};

namespace {
static __forceinline s_juggernaut_spawn_state *get_juggernaut_spawn_state()
{
    return &tls_game_engine_globals()->juggernaut();
}
static __forceinline bool juggernaut_exists()
{
    s_juggernaut_spawn_state *juggernaut = get_juggernaut_spawn_state();
    return juggernaut->player_index != -1 && juggernaut->state_status == 0;
}
static __forceinline void game_engine_player_about_to_spawn(long player_index)
{
}
}

static __forceinline const c_game_variant *juggernaut_current_game_variant()
{
    s_game_options *options = &tls_game_globals()->options;
    return options->game_mode == 2 ? &options->game_variant : 0;
}
namespace {
static __forceinline long juggernaut_player_index_from_user_index(long user_index)
{
    long player_index = -1;
    if (user_index != -1)
        player_index = tls_player_mapping_globals()->player_indices[user_index];
    return player_index;
}
static __forceinline void juggernaut_chud_submit_navpoint(long user_index, s_chud_navpoint *navpoint)
{
    if (user_index >= 0 && user_index < 4)
        chud_persistent_user_data()[user_index].m_navpoint_manager.submit_navpoint(navpoint);
}
static __forceinline bool juggernaut_game_is_predicted()
{
    game_globals_storage *globals = tls_game_globals();
    bool result = false;
    if (globals->options.simulation_type == 4)
        result = true;
    return result;
}
static __forceinline bool juggernaut_game_is_authoritative()
{
    return !juggernaut_game_is_predicted();
}
}

static __forceinline s_object_header *juggernaut_object_get_header(long handle)
{
    return datum_get_typed<s_object_header>(tls_object_header_data(), handle);
}


static __forceinline object_datum *juggernaut_object_try_and_get_and_verify_type(long handle, unsigned long type_mask)
{
    s_object_header *header = (s_object_header *)datum_try_and_get_inline(tls_object_header_data(), handle);
    if (!header || !((1 << header->object_type) & type_mask))
        return 0;
    return header->datum;
}

static __forceinline s_multiplayer_object_properties *juggernaut_object_get_owner_data(long handle)
{
    object_datum *object = juggernaut_object_try_and_get_and_verify_type(handle, -1);
    if (object && object->multiplayer_data_offset != -1)
        return (s_multiplayer_object_properties *)((char *)object + object->multiplayer_data_offset);
    return 0;
}

// @address 0x8c43d0 g_juggernaut_variant
#if !defined(_XBOX) || defined(__clang__)
// The port links the variant pointer by its C name (game/game_engine_juggernaut_variant.h), so this unit
// owns the one external definition.
extern "C" {
c_game_engine_juggernaut_variant *g_juggernaut_variant;
}
#endif
// @address 0x22dc0 k_juggernaut_delay_rounding_bias
// @address 0x2ef3d8 ??0?$c_static_array@Vc_area@@$0M@@@QAA@XZ

namespace {
static __forceinline player_datum *juggernaut_player_get(long player_index)
{
    return datum_get_typed<player_datum>(tls_player_data(), player_index);
}

}

// @address 0x4313c8 ?get_type@c_juggernaut_engine@@UBAJXZ
long c_juggernaut_engine::get_type() const
{
    return _game_engine_type_jugg;
}

// @address 0x4311b0 ?get_simulation_entity_type@c_juggernaut_engine@@UBA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_juggernaut_engine::get_simulation_entity_type() const
{
    return _simulation_entity_type_juggernaut_engine_globals;
}

// @address 0x2edb68 ?initialize_for_new_map@c_juggernaut_engine@@UBA_NXZ
bool c_juggernaut_engine::initialize_for_new_map() const
{
    juggernaut_current_game_variant()->get_active_variant()->can_be_cast_to(_game_engine_juggernaut, (const void **)&g_juggernaut_variant);

    s_juggernaut_spawn_state *juggernaut = get_juggernaut_spawn_state();
    if (juggernaut)
    {
        juggernaut->state_word = 0;
        juggernaut->state_byte = 0;
        juggernaut->state_status = 0;
        juggernaut->player_index = -1;
        new (juggernaut) s_juggernaut_spawn_state;
        juggernaut->kills.set_all(0);
    }
    return true;
}


// @address 0x2edc28 ?initialize_for_new_round@c_juggernaut_engine@@UBA_NXZ
bool c_juggernaut_engine::initialize_for_new_round() const
{
    get_juggernaut_spawn_state()->areas.initialize(0x440, 0x10000, _multiplayer_team_designator_none);

    bool simulation_is_4 = false;
    if (tls_game_globals()->options.simulation_type == 4)
        simulation_is_4 = true;
    if (!simulation_is_4)
    {
        if (g_juggernaut_variant->get_initial_juggernaut() == 0)
        {
            ((void (*)(long))randomly_choose_juggernaut)(-1);
        }
        else
        {
            int *juggernaut_player = &tls_game_engine_globals()->juggernaut().player_index;
            if (*juggernaut_player != -1)
            {
                *juggernaut_player = -1;
                simulation_action_game_engine_globals_update(64);
            }
        }
        if (g_juggernaut_variant->get_destination_zones_enabled())
            cycle_zones(false);
    }
    return true;
}

// @address 0x2edcf0 ?emit_game_start_event@c_juggernaut_engine@@UBAXJ@Z
void c_juggernaut_engine::emit_game_start_event(long player_index) const
{
    s_game_engine_event_data event;
    if (!game_simulation::game_is_predicted()) {
        event.type = 8;
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
}

// @address 0x2edd78 ?apply_game_engine_traits_for_player@c_juggernaut_engine@@UBAXJPAVc_player_traits@@@Z
void c_juggernaut_engine::apply_game_engine_traits_for_player(long player_index, c_player_traits *traits) const
{
    bool match = player_index != -1 &&
        player_index == tls_game_engine_globals()->juggernaut().player_index &&
        tls_game_engine_globals()->juggernaut().state_status == 0;
    if (match)
    {
        if ((player_index != -1 && juggernaut_player_get(player_index)->unit_index != -1) ||
            game_engine_player_is_awaiting_respawn(player_index))
            traits->set(g_juggernaut_variant->get_juggernaut_traits(), false);
    }
}

// @address 0x2ee200 ?team_is_enemy@c_juggernaut_engine@@UBA_NW4e_game_team@@0@Z
bool c_juggernaut_engine::team_is_enemy(e_game_team team, e_game_team other_team) const
{
    bool result = team != other_team;
    if (g_juggernaut_variant->get_allied_against_juggernaut() &&
        get_juggernaut_spawn_state()->player_index != -1 &&
        (get_juggernaut_spawn_state()->state_status > 0 ||
         juggernaut_player_get(get_juggernaut_spawn_state()->player_index)->configuration.host.team_index != team &&
         juggernaut_player_get(get_juggernaut_spawn_state()->player_index)->configuration.host.team_index != other_team))
        result = false;
    return result;
}

// @address 0x2ee290 ?player_about_to_spawn@c_juggernaut_engine@@UBAXJ@Z
void c_juggernaut_engine::player_about_to_spawn(long player_index) const
{
    long absolute_player_index = player_index & 0xffff;
    get_juggernaut_spawn_state()->kills[absolute_player_index] = 0;
    game_engine_player_about_to_spawn(player_index);
}

namespace {
static __forceinline void juggernaut_initialize_event(s_game_engine_event_data *event, long type, long event_type)
{
    event->type = type;
    event->event = event_type;
    event->identifier = -1;
    event->flags = -1;
    event->cause_player_index = -1;
    event->cause_team_index = -1;
    event->effect_player_index = -1;
    event->effect_team_index = -1;
    event->data = 0;
    event->data_word = -1;
}
}

// @address 0x2ee2b8 ?player_changed_indices@c_juggernaut_engine@@UBAXJJ@Z
void c_juggernaut_engine::player_changed_indices(long old_player_index, long new_player_index) const
{
    if (old_player_index == get_juggernaut_spawn_state()->player_index)
    {
        get_juggernaut_spawn_state()->player_index = new_player_index;
        simulation_action_game_engine_globals_update(64);
    }
}


// @address 0x2ee2e8 ?submit_nav_points@c_juggernaut_engine@@UBAXJJ@Z
void c_juggernaut_engine::submit_nav_points(long user_index, long target_player_index) const
{
    c_game_engine::submit_nav_points(user_index, target_player_index);
    c_area *area = get_juggernaut_spawn_state()->areas.find_selected_area();
    if (area && game_engine_in_round()) {
        long player_index = juggernaut_player_index_from_user_index(user_index);
        s_chud_navpoint navpoint;
        navpoint.flags = 1;
        navpoint.name[0] = 0;
        navpoint.type = 2;
        navpoint.icon = 4;
        navpoint.state = !player_is_juggernaut(player_index);
        navpoint.object_index = 0;
        navpoint.scale = 1.0f;
        area->get_center(&navpoint.position);
        juggernaut_chud_submit_navpoint(user_index, &navpoint);
    }
}
// @address 0x2ee3f8 ?build_player_nav_point@c_juggernaut_engine@@UBA_NJJJ_NPAUs_chud_navpoint@@@Z
bool c_juggernaut_engine::build_player_nav_point(long user_index, long player_index, long target_player_index,
    bool target, s_chud_navpoint *navpoint) const
{
    bool result = c_game_engine::build_player_nav_point(user_index, player_index, target_player_index, target, navpoint);
    if (result && player_is_juggernaut(player_index) && juggernaut_player_get(player_index)->unit_index != -1)
    {
        navpoint->icon = 3;
        if (navpoint->type == -1)
        {
            navpoint->type = 1;
        }
        navpoint->flags |= 1;
    }
    return result;
}

void c_juggernaut_engine::get_hud_interface_state(long player_index, game_engine_interface_state *state) const
{
    unsigned char match = player_index != -1 &&
        player_index == tls_game_engine_globals()->juggernaut().player_index &&
        tls_game_engine_globals()->juggernaut().state_status == 0;
    if (match)
        state->variant_custom_flags |= 1;
}


static __forceinline void juggernaut_set_player(s_juggernaut_spawn_state *juggernaut, long player_index)
{
    if (juggernaut->player_index != player_index)
    {
        juggernaut->player_index = player_index;
        simulation_action_game_engine_globals_update(64);
    }
}

static __forceinline long juggernaut_zone_movement_time_in_seconds()
{
    long zero = 0;
    long times[11] = { zero, 10, 15, 30, 60, 120, 180, 240, 300, zero, zero };
    return times[g_juggernaut_variant->get_zone_movement()];
}

// @address 0x2ee508 ?update@c_juggernaut_engine@@UBAXXZ
void c_juggernaut_engine::update() const
{
    if (g_juggernaut_variant->get_destination_zones_enabled())
        get_juggernaut_spawn_state()->areas.update();

    long zero = 0;
    bool predicted = zero;
    if (tls_game_globals()->options.simulation_type == 4)
        predicted = true;
    if (predicted)
        return;

    if (tls_game_engine_globals()->juggernaut().player_index != -1)
    {
        if (!datum_try_and_get(tls_player_data(), tls_game_engine_globals()->juggernaut().player_index) ||
            (juggernaut_player_get(tls_game_engine_globals()->juggernaut().player_index)->flags & 2) != 0)
        {
            tls_game_engine_globals()->juggernaut().player_index = -1;
            simulation_action_game_engine_globals_update(64);
        }
    }

    if (-1 == tls_game_engine_globals()->juggernaut().player_index && g_juggernaut_variant->get_initial_juggernaut() == 0)
        ((void (*)(long))randomly_choose_juggernaut)(-1);

    if (g_juggernaut_variant->get_destination_zones_enabled())
    {
        int *player_index_ptr = &tls_game_engine_globals()->juggernaut().player_index;
        int zone_player_index = *player_index_ptr;
        bool zone_juggernaut_exists = zone_player_index != -1 && tls_game_engine_globals()->juggernaut().state_status == 0;
        if (zone_juggernaut_exists && tls_game_engine_globals()->juggernaut().state_byte == 0)
        {
            c_area *area = get_juggernaut_spawn_state()->areas.find_selected_area();
            if (area && player_in_multiplayer_object_boundary(zone_player_index, (const s_multiplayer_object_boundary_geometry_data *)area))
            {
                game_engine_adjust_player_score(*player_index_ptr, g_juggernaut_variant->get_destination_arrival_points());
                tls_game_engine_globals()->juggernaut().state_byte = 1;
                simulation_action_game_engine_globals_update(0x100);
                long zone_movement = g_juggernaut_variant->get_zone_movement();
                if ((unsigned long)zone_movement == 9 || (unsigned long)zone_movement == 10)
                    cycle_zones(true);
            }
        }

        if (juggernaut_zone_movement_time_in_seconds() != 0)
        {
            if (tls_game_time_globals()->time % game_seconds_integer_to_ticks(juggernaut_zone_movement_time_in_seconds()) == 0)
                cycle_zones(true);
        }
    }

    {
        int *juggernaut_player_index_ptr = &tls_game_engine_globals()->juggernaut().player_index;
        int lone_player_index = *juggernaut_player_index_ptr;
        bool lone_juggernaut_exists = lone_player_index != -1 && tls_game_engine_globals()->juggernaut().state_status == 0;
        if (lone_juggernaut_exists && simulation_starting_up())
        {
            bool lone_juggernaut = true;
            {
                c_player_in_game_iterator iterator;
                iterator.begin();
                while (iterator.next())
                {
                    if (iterator.get_index() != *juggernaut_player_index_ptr)
                    {
                        player_datum *player = (player_datum *)iterator.get_datum();
                        if (!g_juggernaut_variant->get_respawn_on_lone_juggernaut() || player->unit_index != -1)
                        {
                            lone_juggernaut = false;
                            break;
                        }
                    }
                }
            }
            if (get_ever_active_team_or_player_count() == 1)
                lone_juggernaut = false;
            if (lone_juggernaut)
            {
                if (g_juggernaut_variant->get_respawn_on_lone_juggernaut())
                {
                    c_player_in_game_iterator iterator;
                    iterator.begin();
                    while (iterator.next())
                    {
                        long player_index = iterator.get_index();
                        if (player_index == tls_game_engine_globals()->juggernaut().player_index)
                            continue;
                        player_datum *player = (player_datum *)iterator.get_datum();
                        if (game_engine_player_is_out_of_lives(player_index))
                        {
                            player->count = 1;
                            ::simulation_action_game_engine_player_update((short)player_index, 0x200);
                        }
                        player->respawn_timer = 0;
                        player->respawn_seconds = 0;
                        ::simulation_action_game_engine_player_update((short)player_index, 0x800);
                    }
                }
                else if (game_engine_in_round())
                {
                    game_engine_end_round_with_winner(game_engine_choose_winner_index(), false, game_engine_end_condition_lone_juggernaut);
                }
            }
        }
    }

    int stat_player_index = tls_game_engine_globals()->juggernaut().player_index;
    bool stat_juggernaut_exists = stat_player_index != -1 && tls_game_engine_globals()->juggernaut().state_status == 0;
    if (stat_juggernaut_exists && tls_game_time_globals()->time % tls_game_time_globals()->ticks_per_second == 0)
        game_results_statistic_increment((unsigned short)tls_game_engine_globals()->juggernaut().player_index,
            _game_team_none, _game_results_statistic_juggernaut_total_control_time, 1);

    update_delay_timers();
}

// @address 0x2ee9c0 ?promote_to_simulation_authority@c_juggernaut_engine@@UBAXXZ
void c_juggernaut_engine::promote_to_simulation_authority() const
{
    s_juggernaut_spawn_state *juggernaut = get_juggernaut_spawn_state();
    if (juggernaut->player_index != -1)
    {
        bool juggernaut_left_game = true;
        c_player_in_game_iterator iterator;
        iterator.begin();
        while (iterator.next())
        {
            if (iterator.get_index() == juggernaut->player_index)
            {
                juggernaut_left_game = false;
                break;
            }
        }
        if (juggernaut_left_game)
            juggernaut_set_player(juggernaut, -1);
    }
}

// @address 0x2eea78 ?build_simulation_baseline@c_juggernaut_engine@@UBAXJPAX@Z
void c_juggernaut_engine::build_simulation_baseline(long state_data_size, void *state_data) const
{
    s_juggernaut_simulation_state *snapshot = (s_juggernaut_simulation_state *)state_data;
    snapshot->state_word = 0;
    snapshot->state_byte = 0;
    snapshot->player_index = -1;
    build_global_baseline(&snapshot->base);
}


void c_juggernaut_engine::build_simulation_update(unsigned long *update_flags, long player_index, void *state) const
{
    s_juggernaut_simulation_state *snapshot = (s_juggernaut_simulation_state *)state;
    unsigned int changed;
    int current_player;
    unsigned short state_word;
    unsigned char state_byte;
    unsigned int base_flags;
    unsigned int base_changed;
    changed = 0;
    base_changed = changed;
    base_flags = *update_flags & 63;
    if (base_flags != 0) {
        build_global_update(base_flags, (unsigned long *)&base_changed,
            &snapshot->base);
        changed = base_changed;
    }
    if ((*update_flags & 64U) != 0) {
        current_player = tls_game_engine_globals()->juggernaut().player_index;
        if (snapshot->player_index != (current_player & 0xffff)) {
            changed = changed | 64;
            snapshot->player_index = current_player;
            base_changed = changed;
        }
    }
    if ((*update_flags & 128U) != 0) {
        state_word = tls_game_engine_globals()->juggernaut().state_word;
        if (snapshot->state_word != (short)state_word) {
            changed = changed | 128;
            snapshot->state_word = state_word;
            base_changed = changed;
        }
    }
    if ((*update_flags & 0x100U) != 0) {
        state_byte = tls_game_engine_globals()->juggernaut().state_byte;
        if (snapshot->state_byte != state_byte) {
            changed = changed | 0x100;
            snapshot->state_byte = state_byte;
        }
    }
    *update_flags = changed;
}

// @address 0x2eebd8 ?apply_simulation_update@c_juggernaut_engine@@UBA_NKJPBX@Z
bool c_juggernaut_engine::apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const
{
    const s_juggernaut_simulation_state *state = (const s_juggernaut_simulation_state *)state_data;
    bool result = true;
    unsigned long base_update_mask = update_mask & 63;
    if (base_update_mask != 0) {
        result = result && apply_global_update(base_update_mask,
            &state->base);
    }
    if (update_mask & 64)
        get_juggernaut_spawn_state()->player_index = player_index_from_absolute_player_index(state->player_index);
    if (update_mask & 128) {
        get_juggernaut_spawn_state()->state_word = state->state_word;
        get_juggernaut_spawn_state()->state_status = game_seconds_integer_to_ticks((short)get_juggernaut_spawn_state()->state_word);
    }
    if (update_mask & 256)
        get_juggernaut_spawn_state()->state_byte = state->state_byte;
    return result;
}

// @address 0x2eed20 ?randomly_choose_juggernaut@@YAXXZ
void randomly_choose_juggernaut()
{
    c_static_stack<long, 16> candidates;
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next())
    {
        if (iterator.get_index() != tls_game_engine_globals()->juggernaut().player_index && iterator.get_index() != -1)
        {
            long player = iterator.get_index();
            candidates.push_back(player);
        }
    }
    if (candidates.m_count > 0)
    {
        short index = random_range(tls_random_seed(), 0, (short)candidates.m_count);
        set_new_juggernaut(candidates.m_storage[index]);
    }
}

// @address 0x2eee10 ?set_new_juggernaut@@YAXJ@Z
void set_new_juggernaut(long player_index)
{
    if (player_index != tls_game_engine_globals()->juggernaut().player_index)
    {
        tls_game_engine_globals()->juggernaut().player_index = player_index;
        if (player_index != -1)
        {
            if (g_juggernaut_variant->get_juggernaut_delay() != 0)
            {
                tls_game_engine_globals()->juggernaut().state_status = 
                    game_seconds_integer_to_ticks(g_juggernaut_variant->get_juggernaut_delay());
                tls_game_engine_globals()->juggernaut().state_word = g_juggernaut_variant->get_juggernaut_delay();
                simulation_action_game_engine_globals_update(128);
            }
            else
            {
                player_became_juggernaut(player_index);
            }
        }
        simulation_action_game_engine_globals_update(64);
    }
}

static __forceinline long juggernaut_round_delay_seconds(float seconds)
{
#if defined(_XBOX) && !defined(__clang__)
    return (long)(seconds + k_juggernaut_delay_rounding_bias);
#else
    return (long)(seconds + 0.5f);
#endif
}

// @address 0x2eeeb8 ?update_delay_timers@@YAXXZ
void update_delay_timers()
{
    if (tls_game_engine_globals()->juggernaut().state_status > 0)
    {
        long old_state_word = (short)tls_game_engine_globals()->juggernaut().state_word;
        long old_state_word01 = old_state_word;
        long old_state_word02 = old_state_word01;
        long old_state_word03 = old_state_word02;
        long old_state_word04 = old_state_word03;
        long old_state_word05 = old_state_word04;
        long old_state_word06 = old_state_word05;
        long old_state_word07 = old_state_word06;
        long old_state_word08 = old_state_word07;
        long old_state_word09 = old_state_word08;
        long old_state_word10 = old_state_word09;
        long old_state_word11 = old_state_word10;
        long old_state_word12 = old_state_word11;
        long old_state_word13 = old_state_word12;
        long old_state_word14 = old_state_word13;
        long old_state_word15 = old_state_word14;
        long old_state_word16 = old_state_word15;
        long old_state_word17 = old_state_word16;
        long old_state_word18 = old_state_word17;
        long old_state_word19 = old_state_word18;
        long old_state_word20 = old_state_word19;
        long old_state_word21 = old_state_word20;
        long old_state_word22 = old_state_word21;
        long old_state_word23 = old_state_word22;
        long old_state_word24 = old_state_word23;
        long old_state_word25 = old_state_word24;
        long old_state_word26 = old_state_word25;
        long old_state_word27 = old_state_word26;
        long old_state_word28 = old_state_word27;
        long old_state_word29 = old_state_word28;
        long old_state_word30 = old_state_word29;
        long old_state_word31 = old_state_word30;
        volatile long *status = (long *)&tls_game_engine_globals()->juggernaut().state_status;
        long timer = *status - 1;
        *status = timer;
        short state_word = (short)juggernaut_round_delay_seconds((float)ceil(tls_game_time_globals()->tick_length * (float)timer));
        tls_game_engine_globals()->juggernaut().state_word = state_word;
        if (state_word != old_state_word31)
            simulation_action_game_engine_globals_update(128);
        if (tls_game_engine_globals()->juggernaut().state_status == 0)
            player_became_juggernaut(tls_game_engine_globals()->juggernaut().player_index);
    }
}

// @address 0x2eef98 ?player_became_juggernaut@@YAXJ@Z
void player_became_juggernaut(long player_index)
{
    if (game_engine_in_round())
    {
        if (player_index != -1 && g_juggernaut_variant->m_zone_movement.m_storage == 10)
            cycle_zones(true);
    }
    if (player_index != -1)
    {
        s_game_engine_event_data event;
        juggernaut_initialize_event(&event, 8, 1);
        event.cause_player_index = player_index;
        game_engine_send_event((s_game_engine_event_data *)&event);
    }
    tls_game_engine_globals()->juggernaut().state_byte = 0;
    simulation_action_game_engine_globals_update(0x100);
}


// @address 0x2ede48 ?player_killed_player@c_juggernaut_engine@@UBAXJJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z
void c_juggernaut_engine::player_killed_player(
    long killer, long assistant, long victim, bool betrayal, c_flags<e_game_engine_kill_flags, long, 24> flags, long damage) const
{
    game_engine_handle_player_killed(this, killer, assistant, victim, betrayal, flags, damage);
    if (juggernaut_game_is_authoritative())
    {
        long killer_absolute_index = killer & 0xffff;
        if (!(g_juggernaut_variant->get_allied_against_juggernaut()) &&
            !player_is_juggernaut(killer) && !player_is_juggernaut(victim) &&
            killer != -1 && killer != victim && !betrayal)
            game_engine_adjust_player_score(killer, g_juggernaut_variant->m_kill_points);

        if (get_juggernaut_spawn_state()->player_index == -1)
        {
            if (g_juggernaut_variant->get_initial_juggernaut() == 1)
            {
                if (killer != victim)
                    set_new_juggernaut(killer);
            }
            else if (g_juggernaut_variant->get_initial_juggernaut() == 2)
            {
                if (!game_engine_player_is_out_of_lives(victim))
                    set_new_juggernaut(victim);
            }
        }
        else if (player_is_juggernaut(killer) && killer != victim)
        {
            game_engine_adjust_player_score(killer, g_juggernaut_variant->m_kill_as_juggernaut_points);
            game_results_statistic_increment(killer_absolute_index,
            _game_team_none, _game_results_statistic_juggernaut_kills_as_juggernaut, 1);
            if (killer != -1)
            {
                get_juggernaut_spawn_state()->kills[killer_absolute_index]++;
                long kills = get_juggernaut_spawn_state()->kills[killer_absolute_index];
                const long k_juggernaut_spree_kills = 5;
                const long k_juggernaut_frenzy_kills = 10;
                switch (kills)
                {
                case k_juggernaut_spree_kills:
                {
                    long event = 0;
                    ::game_engine_handle_time_messages(25, killer_absolute_index, killer, victim, (int *)(&event));
                    break;
                }
                case k_juggernaut_frenzy_kills:
                {
                    long event = 0;
                    ::game_engine_handle_time_messages(26, killer_absolute_index, killer, victim, (int *)(&event));
                    break;
                }
                }
            }
            if (g_juggernaut_variant->get_next_juggernaut() == 1 && !game_engine_player_is_out_of_lives(victim))
                set_new_juggernaut(victim);
        }
        else if (player_is_juggernaut(victim))
        {
            if (killer != -1 && killer != victim)
            {
                game_engine_adjust_player_score(killer, g_juggernaut_variant->m_juggernaut_kill_points);
                game_results_statistic_increment(killer_absolute_index,
            _game_team_none, _game_results_statistic_juggernaut_kills, 1);
                long event = 0;
                ::game_engine_handle_time_messages(49, killer_absolute_index, killer, victim, (int *)(&event));
                if (juggernaut_game_is_authoritative())
                {
                    s_game_engine_event_data event_data;
                    juggernaut_initialize_event(&event_data, 8, 3);
                    event_data.cause_player_index = killer;
                    event_data.cause_team_index = ((player_datum *)tls_player_data()->data)[killer_absolute_index].configuration.host.team;
                    event_data.effect_player_index = victim;
                    event_data.effect_team_index = datum_get_typed<player_datum>(tls_player_data(), victim)->configuration.host.team;
                    game_engine_send_event(&event_data);
                }
                if (g_juggernaut_variant->get_next_juggernaut() == 0)
                    set_new_juggernaut(killer);
            }
            else if (g_juggernaut_variant->get_next_juggernaut() == 0)
                ((void (*)(long))randomly_choose_juggernaut)(-1);

            unsigned char next_juggernaut = g_juggernaut_variant->get_next_juggernaut();
            if (next_juggernaut == 3)
                ((void (*)(long))randomly_choose_juggernaut)(-1);
            else if (game_engine_player_is_out_of_lives(victim))
            {
                if (next_juggernaut == 1 || next_juggernaut == 2)
                    game_engine_end_round_with_winner(game_engine_choose_winner_index(), false, game_engine_end_condition_juggernaut_out_of_lives);
            }
        }

        if (killer == -1 || killer == victim)
            game_engine_adjust_player_score(victim, g_juggernaut_variant->m_suicide_points);
        else if (betrayal)
            game_engine_adjust_player_score(killer, g_juggernaut_variant->m_betrayal_points);
    }
}

// @address 0x2ef068 ?player_is_juggernaut@@YA_NJ@Z
bool player_is_juggernaut(long player_index)
{
    return player_index != -1 &&
        player_index == get_juggernaut_spawn_state()->player_index &&
        get_juggernaut_spawn_state()->state_status == 0;
}

// @address 0x2ef0b8 ?cycle_zones@@YAX_N@Z
void cycle_zones(bool send_event)
{
    get_juggernaut_spawn_state()->areas.select_area(
        g_juggernaut_variant->m_zone_order.m_storage != 0 ? _area_cycle_in_order : _area_cycle_random);
    if (get_juggernaut_spawn_state()->areas.m_area_count > 1)
    {
        if (send_event)
        {
            s_game_engine_event_data event;
            juggernaut_initialize_event(&event, 8, 2);
            game_engine_send_event((s_game_engine_event_data *)&event);
        }
        tls_game_engine_globals()->juggernaut().state_byte = 0;
        simulation_action_game_engine_globals_update(0x100);
    }
}


// @address 0x2ef180 ?initialize@?$c_area_set@Vc_area@@$0M@@@QAAXJJW4e_multiplayer_team_designator@@@Z
template<class t_area, long k_area_count>
void c_area_set<t_area, k_area_count>::initialize(long type_mask, long match_flags, e_multiplayer_team_designator team_designator)
{
    m_team_designator = _multiplayer_team_designator_none;
    m_area_type_flags = 0x440;
    m_area_flags = match_flags;
    update_areas();
    if (juggernaut_game_is_authoritative())
    {
        t_area *area = find_selected_area();
        if (area)
            area->set_selected(false);
    }
}

// @address 0x2ef208 ?select_area@?$c_area_set@Vc_area@@$0M@@@QAAXW4e_area_cycle_options@@@Z
template<class t_area, long k_area_count>
void c_area_set<t_area, k_area_count>::select_area(e_area_cycle_options area_cycle_options)
{
    if (juggernaut_game_is_authoritative()) {
        t_area *current_area = find_selected_area();
        if (current_area)
            current_area->set_selected(false);
        else
            area_cycle_options = _area_cycle_in_order;
        t_area *selected_area = 0;
        switch (area_cycle_options) {
        case _area_cycle_in_order: {
            if (m_area_count <= 0)
                return;
            long current_index = !current_area ? -1 : get_absolute_index(current_area);
            selected_area = &m_areas[(current_index + 1) % m_area_count];
            break;
        }
        case _area_cycle_random: {
            long previous_object_index = current_area ? current_area->get_object_index() : -1;
            do {
                selected_area = &m_areas[random_range(tls_random_seed(), 0, (short)m_area_count)];
            } while (m_area_count > 1 && selected_area->get_object_index() == previous_object_index);
            break;
        }
        default:
            // match: 0x2ef208: unreachable hint omits dead branch check
            __assume(0);
        }
        if (selected_area)
            selected_area->set_selected(true);
    }
}

template<class t_area, long k_area_count>
t_area *c_area_set<t_area, k_area_count>::find_selected_area()
{
    t_area *selected_area = 0;
    t_area *end_area = end();
    for (t_area *area = begin(); area != end_area; area++)
    {
        if (area->is_selected())
        {
            selected_area = area;
            break;
        }
    }
    return selected_area;
}

// @address 0x2ef510 ?update_areas@?$c_area_set@Vc_area@@$0M@@@IAAXXZ
template<class t_area, long k_area_count>
void c_area_set<t_area, k_area_count>::update_areas()
{
    long handles[20];
    t_area temporary;
    c_juggernaut_object_iterator iterator;
    long count = 0;

    iterator.begin(m_area_type_flags, 0, m_area_flags);
    while (iterator.next() && count < 20)
    {
        long handle = iterator.get_index();
        s_multiplayer_object_properties *owner_data = juggernaut_object_get_owner_data(handle);
        if (!(juggernaut_object_get_header(handle)->flags & 0x10))
        {
            if (!owner_data || owner_data->team_designator == m_team_designator)
                handles[count++] = handle;
        }
    }
    qsort(handles, count, sizeof(handles[0]), c_area_set_compare_by_order_index);
    if (count > k_area_count)
        count = k_area_count;
    for (long i = 0; i < count; i++)
    {
        long handle = handles[i];
        if (m_areas[i].get_object_index() != handle)
        {
            temporary = m_areas[i];
            for (long j = i; j < m_area_count; j++)
            {
                if (m_areas[j].get_object_index() == handle)
                {
                    m_areas[i] = m_areas[j];
                    m_areas[j] = temporary;
                    break;
                }
            }
        }
        m_areas[i].set_object_index(handle);
        m_areas[i].calculate_boundary_geometry();
    }
    m_area_count = count;
}

template void c_area_set<c_area, 12>::initialize(long, long, e_multiplayer_team_designator);
template void c_area_set<c_area, 12>::select_area(e_area_cycle_options);
template c_area *c_area_set<c_area, 12>::find_selected_area();
template void c_area_set<c_area, 12>::update_areas();
