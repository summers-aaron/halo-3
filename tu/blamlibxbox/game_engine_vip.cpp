// blamlibxbox/game_engine_vip.obj: from the records 0x2eb578 0x2eb598 0x2eb680 0x2eb970 0x2eba60 0x2ebaf0 0x2ebc38 0x2ebe10 0x2ec1a8 0x2ec260 0x2ec328 0x2ec438 0x2ec4b0 0x2ec5b0 0x2ec750 0x2ecad0 0x2ecad8 0x2ecb78 0x2ecbc8 0x2ecd48 0x2ecea0 0x2ecf20 0x2ecfc0 0x2ed118 0x2ed1f8 0x2ed2c0 0x2ed550 0x2ed5e8 0x2ed738 0x2ed7b8 0x2ed920 0x351ed0 0x351ff8
// @flags /TP /O2 /Oc
#include "memory/tls_globals.h"
#include "interface/chud/chud_user_data_types.h"
#include "game/game_engine_kill_flags_types.h"
#include "game/player_mapping_types.h"
#include "game/game_globals_storage.h"
#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "memory/static_arrays.h"
#include "cseries/cseries_wrapped.h"
#include "game/game_time_types.h"
#include "simulation/simulation_entity_types.h"
#include "game/game_types.h"
#include "objects/object_iterator.h"
#include "game/game_engine.h"
#include "game/game_engine_end_condition.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_area_set.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "objects/multiplayer_game_objects.h"
#include "game/game_engine_vip.h"
#include "memory/data.h"
#include "game/game_globals.h"
#include "rasterizer/rasterizer_implicit_geometry.h"
#include "game/game_engine_base_variant.h"
#include "game/game_variant.h"
#include "game/player_in_game_iterator.h"
#include "objects/object_datum.h"
#include "game/area_cycle_options_types.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "objects/multiplayer_object_types.h"
#include "interface/chud/chud_navpoint_types.h"
#include "game/game_engine_event_data.h"
#include "game/player_types.h"
#include "game/game_engine_interface_state_types.h"
#include "cseries/cseries.h"
#include "game/game_engine_vip_types.h"
#include "memory/data_absolute_index.h"
#include "game/player_traits.h"
#include "game/game_engine_kill_handler.h"
#include "game/game_engine_default.h"
#include "game/game_engine_vip_variant.h"
#include "game/game_results_types.h"
#include "game/game_results.h"
#include "rasterizer/rasterizer_implicit_geometry_types.h"
#include "game/player_access.h"
#include "objects/object_header_access.h"
#include "math/random_math_range.h"
#include <stdlib.h>
#include <new>
#include <math.h>
#include <string.h>
#if !defined(_XBOX) || defined(__clang__)
#include "game/game_engine_vip_globals.h"
#endif

class c_game_engine_vip_object_iterator : public c_object_iterator_base
{
public:
    void begin(unsigned long type_mask, unsigned long header_mask, unsigned long match_flags)
    {
        object_iterator_begin_internal(type_mask, header_mask, match_flags);
    }
    bool next() { return object_iterator_next_with_match_flags_internal(); }
};
// @address 0xa7970 ?next@c_player_in_game_iterator@@QAA_NXZ
// @address 0x351e38 ?player_in_multiplayer_object_boundary@@YA_NJPBUs_multiplayer_object_boundary_geometry_data@@@Z
// @address 0x2eb578 ?get_defending_team_index@c_vip_engine@@UBA?AW4e_game_team@@XZ
// @address 0x2ecad8 ?recover_state_before_promotion@c_vip_engine@@UBAXXZ
// @address 0x2ecb78 ?build_simulation_baseline@c_vip_engine@@UBAXJPAX@Z
// @address 0x2ecbc8 ?build_simulation_update@c_vip_engine@@UBAXPAKJPAX@Z
// @address 0x2ecd48 ?apply_simulation_update@c_vip_engine@@UBA_NKJPBX@Z
// @address 0x2ecea0 ?player_was_vip@@YA_NJPAW4e_multiplayer_team_designator@@@Z
// @address 0x2ed550 ?set_selected@c_destination_zone@@QAAX_N@Z
// @address 0x2ed738 ?find_selected_area@?$c_area_set@Vc_destination_zone@@$0M@@@QAAPAVc_destination_zone@@XZ
// @address 0x2ecf20 ?player_is_vip@@YA_NJPAW4e_multiplayer_team_designator@@@Z
// @address 0x278110 ?game_engine_team_index_to_team_designator@@YA?AW4e_multiplayer_team_designator@@W4e_game_team@@@Z

// @address 0x2ec328 ?submit_nav_points@c_vip_engine@@UBAXJJ@Z
// @address 0x2ed5e8 ?select_area@?$c_area_set@Vc_destination_zone@@$0M@@@QAAXW4e_area_cycle_options@@@Z
// @address 0x351ed0 multiplayer_object_build_boundary_primitive




template <typename t_area, long k_count>
__forceinline void c_area_set<t_area, k_count>::initialize(long type_mask, long match_flags, e_multiplayer_team_designator team_designator)
{
    m_team_designator = team_designator;
    m_area_type_flags = type_mask;
    m_area_flags = match_flags;
}
struct s_game_engine_state_data;
struct s_game_engine_globals;

// @address 0xac7c0 ?datum_try_and_get@@YAPAXPBUs_data_array@@J@Z
static __forceinline void *vip_datum_try_and_get(const s_data_array *data, long datum_index)
{
    void *result = 0;
    if (datum_index != -1) {
        long absolute_index = datum_index & 0xffff;
        if (absolute_index < data->first_unallocated) {
            short *datum = (short *)(data->data + data->size * absolute_index);
            long identifier = *datum;
            if (identifier != 0 && identifier == (datum_index >> 16))
                result = datum;
        }
    }
    return result;
}
static __forceinline long vip_player_index_from_absolute_index(long absolute_index)
{
    return datum_absolute_index_to_index(tls_player_data(), absolute_index);
}
struct s_chud_navpoint;
// @address 0x266a50 ?set@c_player_traits@@QAAXPBV1@_N@Z
// @address 0x208c38 ?game_engine_adjust_player_score@@YA_NJJ@Z
// @address 0x208758 ?game_engine_end_round_with_winner@@YAXJ_NW4e_game_engine_end_condition@@@Z


// Private partial view of game_globals_storage (game/game_globals_storage.h), the runtime game state; it goes when this unit can include game/game_options.h.
namespace {
static __forceinline bool vip_game_is_predicted()
{
    game_globals_storage *globals = tls_game_globals();
    bool result = false;
    if (globals->options.simulation_type == 4)
        result = true;
    return result;
}
static __forceinline bool vip_game_is_authoritative()
{
    return !vip_game_is_predicted();
}
static __forceinline bool vip_game_in_progress()
{
    bool result = false;
    game_globals_storage *globals = tls_game_globals();
    if (globals && globals->game_in_progress && !globals->initializing && globals->map_active)
        result = true;
    return result;
}
}

extern "C" {
}

namespace {
static __forceinline long vip_player_index_from_user_index(long user_index)
{
    long player_index = NONE;
    if (user_index != NONE)
        player_index = tls_player_mapping_globals()->player_indices[user_index];
    return player_index;
}
static __forceinline void vip_chud_submit_navpoint(long user_index, s_chud_navpoint *navpoint)
{
    if (user_index >= 0 && user_index < 4)
        chud_persistent_user_data()[user_index].m_navpoint_manager.submit_navpoint(navpoint);
}
}
// @address 0x278198 ?game_engine_build_valid_team_mapping@@YAXXZ


__forceinline void c_destination_zone::team_visited(e_multiplayer_team_designator team_designator)
{
    team_designator_flags.set(team_designator, true);
    simulation_action_game_engine_globals_update(1 << (tls_game_engine_globals()->vip().destination_zones.get_absolute_index(this) + 15));
}
// @address 0x8c43cc g_game_engine_vip_variant
#if defined(_XBOX) && !defined(__clang__)
static c_game_engine_vip_variant *g_game_engine_vip_variant;
#else
// The port links the variant pointer by its C++ name (game/game_engine_vip_globals.h): multiplayer_game_objects.cpp
// reads it for the VIP influence boundary, so this unit owns the one external definition.
c_game_engine_vip_variant *g_game_engine_vip_variant;
#endif

// @address 0x208668 ?game_engine_get_round@@YAJXZ
// @address 0x24fc40 ?game_engine_send_event@@YAXPAUs_game_engine_event_data@@@Z
// @address 0x244bb8 ?game_results_statistic_increment@@YAXJW4e_game_team@@W4e_game_results_statistic@@J@Z
// Removed 0x2ed7b8: compiler-generated array initialization uses an unsigned loop unrolled six ways; natural class constructors emit a signed scalar loop.

// 0x2ecf20 (file-static in the dev build; external here so that it is emitted while its callers are not in the unit)
static __forceinline bool game_engine_vip_player_has_unit(long player_index)
{
    return player_index != NONE && player_get(player_index)->unit_index != NONE;
}

// @address 0x2eba60 ?player_changed_indices@c_vip_engine@@UBAXJJ@Z

// @address 0x2ec1a8 ?player_about_to_spawn@c_vip_engine@@UBAXJ@Z
static __forceinline bool game_engine_vip_team_has_vip(e_multiplayer_team_designator team_designator)
{
    bool result;
    if (team_designator != _multiplayer_team_designator_none && tls_game_engine_globals()->vip().vip_player_indices[team_designator] != NONE)
        result = true;
    else
        result = false;
    return result;
}

// @address 0x2ec438 ?build_player_nav_point@c_vip_engine@@UBA_NJJJ_NPAUs_chud_navpoint@@@Z

// @address 0x2ecad0 ?promote_to_simulation_authority@c_vip_engine@@UBAXXZ

// @address 0x2ed118 ?set_team_vip@@YAXW4e_multiplayer_team_designator@@J@Z

// @address 0x2e1300 ?build_player_nav_point@c_game_engine@@UBA_NJJJ_NPAUs_chud_navpoint@@@Z
// @address 0x2085c8 ?game_engine_in_round@@YA_NXZ
// @address 0x278490 ?game_engine_next_active_team@@YA?AW4e_game_team@@W41@@Z
// @address 0x2fe928 ?set_selected@c_area@@QAAX_N@Z
// @address 0x2fe9b8 ?is_selected@c_area@@QBA_NXZ
// @address 0x2ed2c0 ?sanity_check_vips@@YAXXZ
// @address 0x3a97c0 ?simulation_action_game_engine_globals_update@@YAXK@Z
// @address 0x2ed7b8 ??0?$c_static_array@Vc_destination_zone@@$0M@@@QAA@XZ

// @address 0x41e28 ??_7c_vip_engine@@6B@
// @address 0x79d430 ?internal_vip_engine@@3Vc_vip_engine@@A
c_vip_engine internal_vip_engine;

// @address 0x4311b0 ?get_type@c_vip_engine@@UBAJXZ
long c_vip_engine::get_type() const
{
    return _game_engine_type_vip;
}

// @address 0x42e810 ?get_simulation_entity_type@c_vip_engine@@UBA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_vip_engine::get_simulation_entity_type() const
{
    return _simulation_entity_type_vip_engine_globals;
}

// 0x2eb578
e_game_team c_vip_engine::get_defending_team_index() const
{
    return (e_game_team)tls_game_engine_globals()->vip().defending_team_index;
}

static __forceinline const c_game_variant *game_engine_vip_current_variant()
{
    s_game_options *options = &tls_game_globals()->options;
    return options->game_mode == 2 ? &options->game_variant : 0;
}
// @address 0x2eb598 ?initialize_for_new_map@c_vip_engine@@UBA_NXZ
bool c_vip_engine::initialize_for_new_map() const
{
    s_vip_globals *vip = &tls_game_engine_globals()->vip();
    if (vip) {
        new (vip) s_vip_globals;
        vip->vip_player_indices.set_all(-1);
        vip->defending_team_index = -1;
    }
    game_engine_vip_current_variant()->get_active_variant()->can_be_cast_to(_game_engine_vip, (const void **)&g_game_engine_vip_variant);
    // match: 0x2eb598: bind the array before the adjacent store to preserve its live range.
    c_static_array<long, 8> *vip_player_indices = &tls_game_engine_globals()->vip().vip_player_indices;
    tls_game_engine_globals()->vip().defending_team_index = -1;
    vip_player_indices->set_all(-1);
    return true;
}

static __forceinline e_game_team vip_next_active_team(e_game_team team)
{
    e_game_team result = _game_team_none;
    for (team = (e_game_team)(team + 1); team < 8; team = (e_game_team)(team + 1)) {
        if ((tls_game_engine_globals()->active_teams & (1 << team)) != 0) {
            result = team;
            break;
        }
    }
    return result;
}

// @address 0x2eb680 ?initialize_for_new_round@c_vip_engine@@UBA_NXZ
// 0x2eb680
bool c_vip_engine::initialize_for_new_round() const
{
    long teams[8];
    s_game_engine_event_data event;
    if (g_game_engine_vip_variant->get_destination_zones_enabled()) {
        c_area_set<c_destination_zone, 12> *zones = &tls_game_engine_globals()->vip().destination_zones;
        zones->initialize(1088, 0x8000, _multiplayer_team_designator_none);
        zones->update();
        if (vip_game_is_authoritative()) {
            c_destination_zone *selected_zone = zones->find_selected_area();
            if (selected_zone)
                selected_zone->set_selected(false);
        }
    }
    if (vip_game_is_authoritative()) {
        vip_select_next_destination_zone(false);
        if (g_game_engine_vip_variant->get_single_vip()) {
            for (long team_designator = 0; team_designator < 8; team_designator++)
                set_team_vip((e_multiplayer_team_designator)team_designator, NONE);
            if (game_engine_get_round() == 0) {
                long team_count = 0;
                for (e_game_team team = vip_next_active_team(_game_team_none); team != _game_team_none; team = vip_next_active_team(team))
                    teams[team_count++] = team;
                if (team_count > 0)
                    tls_game_engine_globals()->vip().defending_team_index = teams[random_range(tls_random_seed(), 0, (short)team_count)];
            } else {
                tls_game_engine_globals()->vip().defending_team_index
                    = game_engine_next_active_team((e_game_team)tls_game_engine_globals()->vip().defending_team_index);
                if (tls_game_engine_globals()->vip().defending_team_index == NONE)
                    tls_game_engine_globals()->vip().defending_team_index = vip_next_active_team(_game_team_none);
                event.cause_team_index = -1;
                event.identifier = -1;
                event.flags = -1;
                event.cause_player_index = -1;
                event.effect_player_index = -1;
                event.type = 7;
                event.effect_team_index = -1;
                event.data_word = -1;
                event.event = 4;
                event.data = 0;
                event.cause_team_index = tls_game_engine_globals()->vip().defending_team_index;
                game_engine_send_event(&event);
            }
            game_engine_build_valid_team_mapping();
            simulation_action_game_engine_globals_update(0x10000000);
            randomly_choose_vip(_multiplayer_team_designator_first);
        } else {
            game_engine_build_valid_team_mapping();
            for (long team_designator = 0; team_designator < 8; team_designator++)
                randomly_choose_vip((e_multiplayer_team_designator)team_designator);
        }
    }
    return true;
}

// @address 0x2eb970 ?emit_game_start_event@c_vip_engine@@UBAXJ@Z
// 0x2eb970
void c_vip_engine::emit_game_start_event(long player_index) const
{
    if (!vip_game_is_predicted()) {
        s_game_engine_event_data event;
        event.event = 0;
        event.data = 0;
        event.flags = player_index;
        event.identifier = -1;
        event.type = 7;
        event.cause_player_index = -1;
        event.cause_team_index = -1;
        event.effect_player_index = -1;
        event.effect_team_index = -1;
        event.data_word = -1;
        game_engine_send_event(&event);
        if (g_game_engine_vip_variant->get_single_vip() && game_engine_get_round() == 0) {
            event.cause_team_index = -1;
            event.type = 7;
            event.identifier = -1;
            event.cause_player_index = -1;
            event.effect_player_index = -1;
            event.effect_team_index = -1;
            event.event = 4;
            event.data = 0;
            event.data_word = -1;
            event.flags = player_index;
            event.cause_team_index = tls_game_engine_globals()->vip().defending_team_index;
            game_engine_send_event(&event);
        }
    }
}

// 0x2eba60
void c_vip_engine::player_changed_indices(long old_player_index, long new_player_index) const
{
    if (vip_game_is_authoritative()) {
        for (long team_designator = 0; team_designator < 8; team_designator++) {
            if (tls_game_engine_globals()->vip().vip_player_indices[team_designator] == old_player_index)
                set_team_vip((e_multiplayer_team_designator)team_designator, new_player_index);
        }
    }
}

struct s_player_datum;


// @address 0x2ebaf0 ?player_changed_teams@c_vip_engine@@UBAXJ@Z
// 0x2ebaf0
void c_vip_engine::player_changed_teams(long player_index) const
{
    if (vip_game_is_authoritative() && game_engine_in_round()) {
        for (long team_designator = 0; team_designator < 8; team_designator++) {
            if (tls_game_engine_globals()->vip().vip_player_indices[team_designator] == player_index) {
                set_team_vip((e_multiplayer_team_designator)team_designator, NONE);
                sanity_check_vips();
            }
        }
        long count = 0;
        e_game_team team_index = (e_game_team)player_get(player_index)->configuration.host.team_index;
        if (game_engine_team_index_to_team_designator(team_index) != _multiplayer_team_designator_none) {
            c_player_in_game_iterator iterator;
            iterator.begin();
            while (iterator.next()) {
                if ((e_game_team)((player_datum *)iterator.get_datum())->configuration.host.team_index == team_index)
                    count++;
            }
            if (count == 1 && !g_game_engine_vip_variant->get_single_vip())
                set_team_vip(game_engine_team_index_to_team_designator(team_index), player_index);
        }
    }
}

static __forceinline const s_multiplayer_object_boundary_geometry_data *vip_team_influence_boundary(long team_designator)
{
    return &tls_game_engine_globals()->vip().influence_boundaries[team_designator];
}

// @address 0x2ebc38 ?apply_game_engine_traits_for_player@c_vip_engine@@UBAXJPAVc_player_traits@@@Z
// 0x2ebc38
void c_vip_engine::apply_game_engine_traits_for_player(long player_index, c_player_traits *player_traits) const
{
    if (!vip_game_in_progress())
        return;
    player_datum *player = player_get(player_index);
    if (game_engine_team_index_to_team_designator((e_game_team)player->configuration.host.team_index) == _multiplayer_team_designator_none)
        return;
    e_multiplayer_team_designator team_designator;
    if (player_was_vip(player_index, &team_designator)) {
        bool player_has_unit = player_index != NONE && player->unit_index != NONE;
        if (player_has_unit || game_engine_player_is_awaiting_respawn(player_index)) {
            player_traits->set(g_game_engine_vip_variant->get_vip_traits(), false);
            return;
        }
    }
    long vip_player_index = tls_game_engine_globals()->vip().vip_player_indices[team_designator];
    bool vip_has_unit = vip_player_index != NONE && player_get(vip_player_index)->unit_index != NONE;
    if (vip_has_unit && player_in_multiplayer_object_boundary(player_index, vip_team_influence_boundary(team_designator))) {
        player_traits->set(g_game_engine_vip_variant->get_vip_influence_traits(), false);
        return;
    }
    if (g_game_engine_vip_variant->get_single_vip() && team_designator == _multiplayer_team_designator_first)
        player_traits->set(g_game_engine_vip_variant->get_vip_team_traits(), false);
}


// 0x2ec1a8
void c_vip_engine::player_about_to_spawn(long player_index) const
{
    if (g_game_engine_vip_variant->get_vip_selection() == 1 && vip_game_is_authoritative()) {
        e_multiplayer_team_designator team_designator
            = game_engine_team_index_to_team_designator((e_game_team)player_get(player_index)->configuration.host.team_index);
        if (!game_engine_vip_team_has_vip(team_designator))
            set_team_vip(team_designator, player_index);
    }
}

// 0x2ec260
static __forceinline void game_engine_vip_initialize_event(s_game_engine_event_data *event, long subtype)
{
    event->type = 7;
    event->event = subtype;
    event->identifier = NONE;
    event->flags = NONE;
    event->cause_player_index = NONE;
    event->cause_team_index = NONE;
    event->effect_player_index = NONE;
    event->effect_team_index = NONE;
    event->data = 0;
    event->data_word = NONE;
}
static __forceinline void game_engine_vip_increment_statistic(long player_index, long statistic)
{
    game_results_statistic_increment(player_index & 0xffff, (e_game_team)player_get(player_index)->configuration.host.team_index,
        (e_game_results_statistic)statistic, 1);
}

static __forceinline bool game_engine_vip_second_elapsed()
{
    return tls_game_time_globals()->time % tls_game_time_globals()->ticks_per_second == 0;
}

// @address 0x2ec750 ?player_update@c_vip_engine@@UBAXJ@Z
void c_vip_engine::player_update(long player_index) const
{
    c_game_engine::player_update(player_index);
    e_multiplayer_team_designator team_designator;
    if (player_is_vip(player_index, &team_designator)) {
        e_multiplayer_team_designator vip_team = team_designator;
        s_game_engine_globals *globals = tls_game_engine_globals();
        multiplayer_object_calculate_boundary_geometry(&globals->vip().influence_boundaries[vip_team], player_get(player_index)->unit_index);
        if (vip_game_is_authoritative() && game_engine_in_round()) {
            if (g_game_engine_vip_variant->get_destination_zones_enabled()) {
                c_destination_zone *zone = globals->vip().destination_zones.find_selected_area();
                if (zone) {
                    if (!zone->has_team_visited(vip_team) &&
                        player_in_multiplayer_object_boundary(player_index, (const s_multiplayer_object_boundary_geometry_data *)zone))
                    {
                        game_engine_adjust_player_score(player_index, g_game_engine_vip_variant->get_destination_arrival_points());
                        zone->team_visited(vip_team);
                        s_game_engine_event_data event;
                        game_engine_vip_initialize_event(&event, 5);
                        event.cause_player_index = player_index;
                        event.cause_team_index = (e_game_team)player_get(player_index)->configuration.host.team_index;
                        game_engine_send_event(&event);
                        if (g_game_engine_vip_variant->get_zone_movement() == 9) {
                            long rounds_won = game_engine_get_team_rounds_won((e_game_team)event.cause_team_index);
                            if (rounds_won < game_engine_get_score_to_win_round())
                                vip_select_next_destination_zone(true);
                        }
                    }
                }
            }
            if (game_engine_vip_second_elapsed())
                game_engine_vip_increment_statistic(player_index, _game_results_statistic_vip_time_as_vip);
        }
    } else if (vip_game_is_authoritative() && game_engine_in_round()) {
        if (game_engine_vip_team_has_vip(team_designator)) {
            long vip_player_index = tls_game_engine_globals()->vip().vip_player_indices[team_designator];
            if (game_engine_vip_player_has_unit(vip_player_index)
                && player_in_multiplayer_object_boundary(player_index, vip_team_influence_boundary(team_designator))
                && game_engine_vip_second_elapsed())
                game_results_statistic_increment(player_index & 0xffff,
                    (e_game_team)player_get(player_index)->configuration.host.team_index,
                    _game_results_statistic_vip_guard_time, 1);
        }
    }
}

// @address 0x2ec260 ?player_just_spawned@c_vip_engine@@UBAXJ@Z
void c_vip_engine::player_just_spawned(long player_index) const
{
    if (!player_is_vip(player_index, 0))
        return;
    if (!game_engine_in_round())
        return;
    if (player_index == NONE)
        return;
    s_game_engine_event_data event;
    event.type = 7;
    event.event = 1;
    event.identifier = -1;
    event.flags = -1;
    event.cause_player_index = player_index;
    event.cause_team_index = -1;
    event.effect_player_index = -1;
    event.effect_team_index = -1;
    event.data = 0;
    event.data_word = -1;
    unsigned short index = (unsigned short)player_index;
    game_engine_send_event(&event);
    game_results_statistic_increment(index, (e_game_team)player_get(index)->configuration.host.team_index, _game_results_statistic_vip_lives_as_vip, 1);
}

// 0x2ec328
void c_vip_engine::submit_nav_points(long user_index, long target_player_index) const
{
    c_destination_zone *area = tls_game_engine_globals()->vip().destination_zones.find_selected_area();
    if (area && game_engine_in_round()) {
        long player_index = vip_player_index_from_user_index(user_index);
        s_chud_navpoint navpoint;
        navpoint.flags = 65;
        navpoint.name[0] = 0;
        navpoint.type = 2;
        navpoint.icon = 4;
        navpoint.state = !player_is_vip(player_index, 0);
        navpoint.object_index = 0;
        navpoint.scale = 1.0f;
        area->get_center(&navpoint.position);
        vip_chud_submit_navpoint(user_index, &navpoint);
    }
    c_game_engine::submit_nav_points(user_index, target_player_index);
}

// 0x2ec438
bool c_vip_engine::build_player_nav_point(long user_index, long player_index, long target_player_index, bool target, s_chud_navpoint *navpoint) const
{
    bool result = c_game_engine::build_player_nav_point(user_index, player_index, target_player_index, target, navpoint);
    if (result && player_is_vip(player_index, 0)) {
        navpoint->icon = 9;
        if (navpoint->type == NONE)
            navpoint->type = 1;
        navpoint->flags |= 1;
        navpoint->flags |= 64;
    }
    return result;
}

// @address 0x2ec4b0 ?get_hud_interface_state@c_vip_engine@@UBAXJPAUgame_engine_interface_state@@@Z
// 0x2ec4b0
void c_vip_engine::get_hud_interface_state(long player_index, game_engine_interface_state *interface_state) const
{
    e_multiplayer_team_designator team_designator = _multiplayer_team_designator_none;
    game_engine_interface_state *state = interface_state;
    if (player_is_vip(player_index, &team_designator)) {
        state->variant_custom_flags |= 1;
    } else if (team_designator != _multiplayer_team_designator_none) {
        long vip_player_index = tls_game_engine_globals()->vip().vip_player_indices[team_designator];
        bool vip_has_unit = vip_player_index != NONE && player_get(vip_player_index)->unit_index != NONE;
        if (vip_has_unit && player_in_multiplayer_object_boundary(player_index, vip_team_influence_boundary(team_designator)))
            state->variant_custom_flags |= 2;
    }
}

static __forceinline long vip_update_round_time()
{
    if (tls_game_time_globals()->time - tls_game_engine_globals()->round_start_time < 0)
        return 0;
    else
        return tls_game_time_globals()->time - tls_game_engine_globals()->round_start_time;
}
// @address 0x2ec5b0 ?update@c_vip_engine@@UBAXXZ
void c_vip_engine::update() const
{
    if (g_game_engine_vip_variant->get_destination_zones_enabled()) {
        tls_game_engine_globals()->vip().destination_zones.update();
        c_area_set<c_destination_zone, 12> *area_set = &tls_game_engine_globals()->vip().destination_zones;
        for (c_destination_zone *area = area_set->begin(); area != area_set->end(); ++area) {
            if (area->is_selected())
                multiplayer_object_build_boundary_primitive(
                    (const vip_s_implicit_geometry_shape *)area->get_boundary_geometry(), area->get_color(), area->get_object_index());
        }
    }
    long zero = 0;
    bool predicted = zero;
    if (tls_game_globals()->options.simulation_type == 4)
        predicted = true;
    if (predicted)
        return;
    sanity_check_vips();
    if (g_game_engine_vip_variant->get_destination_zones_enabled()) {
        long times[11] = { zero, 10, 15, 30, 60, 120, 180, 240, 300, zero, zero };
        long movement_ticks = times[g_game_engine_vip_variant->get_zone_movement()] * tls_game_time_globals()->ticks_per_second;
        if (movement_ticks > 0) {
            long round_time = vip_update_round_time();
            if (round_time % movement_ticks == 0)
                vip_select_next_destination_zone(true);
        }
    }
}

// 0x2ecad0
void c_vip_engine::promote_to_simulation_authority() const
{
    sanity_check_vips();
}

// 0x2ecad8
void c_vip_engine::recover_state_before_promotion() const
{
    if (g_game_engine_vip_variant->get_single_vip()) {
        e_game_team defending_team = _game_team_none;
        for (long round_index = 0; round_index <= tls_game_engine_globals()->round_index; round_index++) {
            defending_team = game_engine_next_active_team(defending_team);
            if (defending_team == _game_team_none)
                defending_team = game_engine_next_active_team(_game_team_none);
        }
        tls_game_engine_globals()->vip().defending_team_index = defending_team;
        simulation_action_game_engine_globals_update(0x10000000);
    }
}

// 0x2ecb78
void c_vip_engine::build_simulation_baseline(long state_data_size, void *state_data) const
{
    s_vip_engine_state_data *state = (s_vip_engine_state_data *)state_data;
    for (long team = 0; team < 8; ++team)
        state->team_vip[team] = -1;
    for (long zone = 0; zone < 12; ++zone)
        state->zone_data[zone] = 0;
    state->defending_team = -1;
    build_global_baseline((s_game_engine_state_data *)state);
}

// 0x2ecbc8
void c_vip_engine::build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const
{
    s_vip_engine_state_data *state = (s_vip_engine_state_data *)state_data;
    unsigned long changed_mask = 0;
    unsigned long global_update_mask = *update_mask & 63;
    if (global_update_mask)
        build_global_update(global_update_mask, &changed_mask, (s_game_engine_state_data *)state);
    for (long team = 0; team < 8; ++team) {
        if (*update_mask & (1 << (team + 6))) {
            long absolute_player_index = (unsigned short)tls_game_engine_globals()->vip().vip_player_indices[team];
            if (state->team_vip[team] != absolute_player_index) {
                state->team_vip[team] = (short)absolute_player_index;
                changed_mask |= 1 << (team + 6);
            }
        }
    }
    for (long zone = 0; zone < tls_game_engine_globals()->vip().destination_zones.get_area_count(); ++zone) {
        if (*update_mask & (1 << (zone + 15))) {
            if (state->zone_data[zone] != tls_game_engine_globals()->vip().destination_zones[zone].team_designator_flags) {
                state->zone_data[zone] = tls_game_engine_globals()->vip().destination_zones[zone].team_designator_flags;
                changed_mask |= 1 << (zone + 15);
            }
        }
    }
    if (*update_mask & 0x10000000) {
        if (state->defending_team != tls_game_engine_globals()->vip().defending_team_index) {
            state->defending_team = tls_game_engine_globals()->vip().defending_team_index;
            changed_mask |= 0x10000000;
        }
    }
    *update_mask = changed_mask;
}

// 0x2ecd48
bool c_vip_engine::apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const
{
    const s_vip_engine_state_data *state = (const s_vip_engine_state_data *)state_data;
    bool result = true;
    unsigned long global_update_mask = update_mask & 63;
    if (global_update_mask)
        result = result && apply_global_update(global_update_mask, (const s_game_engine_state_data *)state);
    for (long team = 0; team < 8; ++team) {
        if (update_mask & (1 << (team + 6)))
            tls_game_engine_globals()->vip().vip_player_indices[team] = vip_player_index_from_absolute_index(state->team_vip[team]);
    }
    for (long zone = 0; zone < 12; ++zone) {
        if (update_mask & (1 << (zone + 15)))
            tls_game_engine_globals()->vip().destination_zones[zone].team_designator_flags.set_unsafe(state->zone_data[zone]);
    }
    if (update_mask & 0x10000000)
        tls_game_engine_globals()->vip().defending_team_index = state->defending_team;
    return result;
}

// 0x2ecea0
bool player_was_vip(long player_index, e_multiplayer_team_designator *team_designator)
{
    e_multiplayer_team_designator designator
        = game_engine_team_index_to_team_designator((e_game_team)player_get(player_index)->configuration.host.team_index);
    if (team_designator)
        *team_designator = designator;
    return designator != _multiplayer_team_designator_none && tls_game_engine_globals()->vip().vip_player_indices[designator] == player_index;
}

// 0x2ecf20
bool player_is_vip(long player_index, e_multiplayer_team_designator *team_designator)
{
    if (team_designator)
        *team_designator = _multiplayer_team_designator_none;
    return player_index != NONE && player_was_vip(player_index, team_designator) && game_engine_vip_player_has_unit(player_index);
}


static __forceinline bool vip_player_was_vip_inline(long player_index)
{
    player_datum *player = (player_datum *)(tls_player_data()->data + (player_index & 0xffff) * sizeof(player_datum));
    e_multiplayer_team_designator team = game_engine_team_index_to_team_designator((e_game_team)player->configuration.host.team_index);
    return team != _multiplayer_team_designator_none && tls_game_engine_globals()->vip().vip_player_indices[team] == player_index;
}

// @address 0x2ecfc0 ?randomly_choose_vip@@YAXW4e_multiplayer_team_designator@@@Z
// 0x2ecfc0
void randomly_choose_vip(e_multiplayer_team_designator team_designator)
{
    long candidates[16];
    long candidate_count = 0;
    long previous_vip = NONE;
    c_player_in_game_iterator iterator;
    iterator.begin();
    while (iterator.next() && candidate_count < sizeof(candidates) / sizeof(candidates[0])) {
        if (game_engine_team_index_to_team_designator((e_game_team)((player_datum *)iterator.get_datum())->configuration.host.team_index) == team_designator) {
            long player_index = iterator.get_index();
            if (vip_player_was_vip_inline(player_index))
                previous_vip = player_index;
            else
                candidates[candidate_count++] = player_index;
        }
    }
    if (candidate_count == 0) {
        set_team_vip(team_designator, previous_vip);
    } else {
        short choice = random_range(tls_random_seed(), 0, (short)candidate_count);
        set_team_vip(team_designator, candidates[choice]);
    }
}

// 0x2ed118
void set_team_vip(e_multiplayer_team_designator team_designator, long player_index)
{
    bool invalid_team = false;
    if (g_game_engine_vip_variant->get_single_vip()) {
        if (tls_game_engine_globals()->vip().defending_team_index != tls_game_engine_globals()->team_mapping[team_designator])
            invalid_team = true;
    }
    if (player_index != tls_game_engine_globals()->vip().vip_player_indices[team_designator] && !invalid_team) {
        if (game_engine_in_round() && player_index != NONE) {
            if (g_game_engine_vip_variant->get_zone_movement() == 10)
                vip_select_next_destination_zone(true);
        }
        tls_game_engine_globals()->vip().vip_player_indices[team_designator] = player_index;
        simulation_action_game_engine_globals_update(1 << (team_designator + 6));
    }
}

// @address 0x2ed1f8 vip_select_next_destination_zone
// 0x2ed1f8
extern "C" void vip_select_next_destination_zone(unsigned char send_event)
{
    if (!g_game_engine_vip_variant->get_destination_zones_enabled())
        return;
    tls_game_engine_globals()->vip().destination_zones.select_area((e_area_cycle_options)(g_game_engine_vip_variant->get_zone_order() != 0));
    if (send_event == 0)
        return;
    if (!game_engine_in_round())
        return;
    s_game_engine_event_data event;
    event.type = 7;
    event.event = 6;
    event.identifier = -1;
    event.flags = -1;
    event.cause_player_index = -1;
    event.cause_team_index = -1;
    event.effect_player_index = -1;
    event.effect_team_index = -1;
    event.data = 0;
    event.data_word = -1;
    game_engine_send_event(&event);
}

// 0x2ed2c0
void sanity_check_vips()
{
    c_game_engine_vip_variant *variant = g_game_engine_vip_variant;
    for (long team_designator = 0; team_designator < 8; team_designator++) {
        if ((game_engine_vip_team_has_vip((e_multiplayer_team_designator)team_designator)
                && (!vip_datum_try_and_get(tls_player_data(), tls_game_engine_globals()->vip().vip_player_indices[team_designator])
                    || (player_get(tls_game_engine_globals()->vip().vip_player_indices[team_designator])->flags & 2) != 0
                    || game_engine_team_index_to_team_designator((e_game_team)((player_datum *)datum_try_and_get(tls_player_data(),
                        tls_game_engine_globals()->vip().vip_player_indices[team_designator]))->configuration.host.team_index) != team_designator))
            || (variant->get_single_vip() && team_designator != _multiplayer_team_designator_first)) {
            set_team_vip((e_multiplayer_team_designator)team_designator, NONE);
            variant = g_game_engine_vip_variant;
        }
        if ((!variant->get_single_vip() || team_designator == _multiplayer_team_designator_first)
            && (variant->get_vip_selection() == 0 || variant->get_vip_selection() == 3)
            && !game_engine_vip_team_has_vip((e_multiplayer_team_designator)team_designator)) {
            randomly_choose_vip((e_multiplayer_team_designator)team_designator);
            variant = g_game_engine_vip_variant;
        }
    }
}

// 0x2ed550
void c_destination_zone::set_selected(bool selected)
{
    if (selected) {
        clear_team_designators();
        simulation_action_game_engine_globals_update(1 << (tls_game_engine_globals()->vip().destination_zones.get_absolute_index(this) + 15));
    }
    c_area::set_selected(selected);
}

// 0x2ed5e8
template <typename t_area, long k_count>
void c_area_set<t_area, k_count>::select_area(e_area_cycle_options area_cycle_options)
{
    if (vip_game_is_authoritative()) {
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
            long current_index = !current_area ? NONE : get_absolute_index(current_area);
            selected_area = &m_areas[(current_index + 1) % m_area_count];
            break;
        }
        case _area_cycle_random: {
            long previous_object_index = current_area ? current_area->get_object_index() : NONE;
            do {
                selected_area = &m_areas[random_range(tls_random_seed(), 0, (short)m_area_count)];
            } while (m_area_count > 1 && selected_area->get_object_index() == previous_object_index);
            break;
        }
        default:
            // match: 0x2ed5e8: without it the random case gains a bne to the exit for out-of-range options, so the switch tests only _area_cycle_in_order
            __assume(0);
        }
        if (selected_area)
            selected_area->set_selected(true);
    }
}

// 0x2ed738
template <typename t_area, long k_count>
t_area *c_area_set<t_area, k_count>::find_selected_area()
{
    t_area *selected_area = 0;
    for (t_area *area = begin(); area != end(); area++)
    {
        if (area->is_selected())
        {
            selected_area = area;
            break;
        }
    }
    return selected_area;
}


template void c_area_set<c_destination_zone, 12>::select_area(e_area_cycle_options);
template c_destination_zone *c_area_set<c_destination_zone, 12>::find_selected_area();


// 0x351ed0
static __forceinline s_multiplayer_object_properties *game_engine_vip_object_get_owner_data(long object_index)
{
    object_datum *object = (object_datum *)object_try_and_get_and_verify_type_inline(object_index, -1);
    if (object && object->multiplayer_data_offset != -1)
        return (s_multiplayer_object_properties *)((char *)object + object->multiplayer_data_offset);
    return 0;
}

// @address 0x2ed920 ?update_areas@?$c_area_set@Vc_destination_zone@@$0M@@@IAAXXZ
template <typename t_area, long k_count>
void c_area_set<t_area, k_count>::update_areas()
{
    long handles[20];
    t_area temporary;
    c_game_engine_vip_object_iterator iterator;
    long count = 0;

    iterator.begin(m_area_type_flags, 0, m_area_flags);
    while (iterator.next() && count < 20) {
        long handle = iterator.get_index();
        s_multiplayer_object_properties *owner_data = game_engine_vip_object_get_owner_data(handle);
        if (!(object_header_get(handle)->flags & 0x10)) {
            if (!owner_data || (unsigned short)owner_data->team_designator == m_team_designator)
                handles[count++] = handle;
        }
    }
    qsort(handles, count, 4, c_area_set_compare_by_order_index);
    if (count > k_count)
        count = k_count;
    for (long i = 0; i < count; i++) {
        long handle = handles[i];
        if (m_areas[i].get_object_index() != handle) {
            temporary = m_areas[i];
            for (long j = i; j < m_area_count; j++) {
                if (m_areas[j].get_object_index() == handle) {
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

template void c_area_set<c_destination_zone, 12>::update_areas();

extern "C" void multiplayer_object_build_boundary_primitive(const vip_s_implicit_geometry_shape *shape_view, const void *color, long object_index)
{
    const s_multiplayer_object_boundary_geometry_data *shape = (const s_multiplayer_object_boundary_geometry_data *)shape_view;
    if (object_index != NONE)
    {
        object_datum *object = object_get(object_index);
        s_multiplayer_object_properties *properties = (s_multiplayer_object_properties *)((char *)object + object->multiplayer_data_offset);
        if (properties)
        {
            long primitive_object_index = properties->standard_shader;
            if (primitive_object_index != NONE)
            {
                s_implicit_geometry_primitive *primitive;
                long primitive_type;
                if (shape->type != 1)
                {
                    if (shape->type != 2)
                    {
                        if (shape->type != 3)
                            return;
                        float b3 = shape->neg_height;
                        float b2 = shape->height;
                        float b1 = shape->depth;
                        float b0 = shape->width;
                        primitive = (s_implicit_geometry_primitive *)implicit_geometry_new_primitive(primitive_object_index, color, &shape->matrix);
                        if (!primitive)
                            return;
                        primitive->parameters[2] = (float)fabs(b2);
                        primitive->parameters[0] = b0;
                        primitive->parameters[1] = b1;
                        primitive_type = 2;
                        primitive->parameters[3] = (float)fabs(b3);
                    }
                    else
                    {
                        float b3 = shape->neg_height;
                        float b2 = shape->height;
                        float b0 = shape->width;
                        primitive = (s_implicit_geometry_primitive *)implicit_geometry_new_primitive(primitive_object_index, color, &shape->matrix);
                        if (!primitive)
                            return;
                        primitive->parameters[1] = (float)fabs(b2);
                        primitive->parameters[0] = b0;
                        primitive->parameters[2] = (float)fabs(b3);
                        primitive_type = 0;
                    }
                }
                else
                {
                    float b0 = shape->width;
                    primitive = (s_implicit_geometry_primitive *)implicit_geometry_new_primitive(primitive_object_index, color, &shape->matrix);
                    if (!primitive)
                        return;
                    primitive->parameters[0] = b0;
                    primitive_type = 1;
                }
                primitive->type = primitive_type;
            }
        }
    }
}

// 0x351ff8
// @address 0x351ff8 ?calculate_influence_boundary_geometry@@YAXIPAXH@Z

void calculate_influence_boundary_geometry(unsigned int object_index, void *area, int unused)
{
    s_multiplayer_object_boundary_geometry_data *geometry = (s_multiplayer_object_boundary_geometry_data *)area;
    memset(geometry, 0, sizeof(s_multiplayer_object_boundary_geometry_data));
    s_object_header *header = object_header_get(object_index);
    object_datum *object = (object_datum *)header->datum;
    short offset = object->multiplayer_data_offset;
    if (offset != -1)
        multiplayer_object_calculate_boundary_geometry_from_properties(
            object_index, (const s_multiplayer_object_properties *)((char *)object + offset), geometry, false);
}

static __forceinline void game_engine_vip_award_takedown_medal(long player_index, long victim_index)
{
    int medal_flags = 0;
    game_engine_handle_time_messages(48, player_index & 0xffff, player_index, victim_index, &medal_flags);
}

static __forceinline void game_engine_vip_adjust_player_results_statistic(
    long player_index, bool adjust_team, e_game_results_statistic statistic, long adjustment)
{
    long absolute_player_index = player_index & 0xffff;
    e_game_team team_index = _game_team_none;
    if (adjust_team)
    {
        player_datum *player = &((player_datum *)tls_player_data()->data)[absolute_player_index];
        team_index = (e_game_team)player->configuration.host.team_index;
    }
    game_results_statistic_increment(absolute_player_index, team_index, statistic, adjustment);
}

// @address 0x2ebe10 ?player_killed_player@c_vip_engine@@UBAXJJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z
// 0x2ebe10
void c_vip_engine::player_killed_player(long killer_player_index, long object_index, long victim_player_index, bool betrayal,
    c_flags<e_game_engine_kill_flags, long, 24> kill_flags, long damage) const
{
    game_engine_handle_player_killed((void *)this, killer_player_index, object_index, victim_player_index, betrayal, kill_flags, damage);
    if (vip_game_is_authoritative())
    {
        bool end_round = false;
        e_multiplayer_team_designator team_designator;
        c_game_engine_vip_variant *variant;
        if (player_was_vip(victim_player_index, &team_designator))
        {
            e_multiplayer_team_designator vip_team = team_designator;
            s_game_engine_event_data event;
            game_engine_vip_initialize_event(&event, 3);
            event.effect_player_index = victim_player_index;
            event.effect_team_index = tls_game_engine_globals()->team_mapping[vip_team];
            end_round = g_game_engine_vip_variant->get_should_end_round_on_vip_death();
            if (killer_player_index != victim_player_index && killer_player_index != NONE)
            {
                if (!betrayal)
                {
                    event.event = 2;
                    event.cause_player_index = killer_player_index;
                    game_engine_adjust_player_score(killer_player_index, g_game_engine_vip_variant->get_takedown_points());
                    game_engine_vip_increment_statistic(killer_player_index, _game_results_statistic_vip_takedowns);
                    game_engine_vip_award_takedown_medal(killer_player_index, victim_player_index);
                    if (player_is_vip(killer_player_index, 0))
                    {
                        game_engine_adjust_player_score(killer_player_index, g_game_engine_vip_variant->get_kill_as_vip_points());
                        game_engine_vip_increment_statistic(killer_player_index, _game_results_statistic_vip_kills_as_vip);
                    }
                }
                else
                {
                    game_engine_adjust_player_score(killer_player_index, g_game_engine_vip_variant->get_vip_suicide_points());
                }
            }
            game_engine_send_event(&event);
            game_engine_adjust_player_score(victim_player_index, g_game_engine_vip_variant->get_vip_death_points());
            variant = g_game_engine_vip_variant;
            if (!variant->get_should_end_round_on_vip_death())
            {
                unsigned long selection = variant->get_vip_selection();
                if (selection == 0)
                    randomly_choose_vip(vip_team);
                else if (selection == 1 || (selection == 2 && victim_player_index != killer_player_index && killer_player_index != NONE && !betrayal))
                    set_team_vip(vip_team, NONE);
            }
        }
        else if (!betrayal)
        {
            if (player_is_vip(killer_player_index, 0))
            {
                game_engine_adjust_player_score(killer_player_index, g_game_engine_vip_variant->get_kill_as_vip_points());
                game_engine_vip_adjust_player_results_statistic(killer_player_index, true, _game_results_statistic_vip_kills_as_vip, 1);
            }
            else
            {
                game_engine_adjust_player_score(killer_player_index, g_game_engine_vip_variant->get_kill_points());
            }
            if (!game_engine_vip_team_has_vip(team_designator))
            {
                variant = g_game_engine_vip_variant;
                if (variant->get_vip_selection() == 2 && game_engine_in_round())
                    set_team_vip(team_designator, victim_player_index);
            }
        }
        variant = g_game_engine_vip_variant;
        if (killer_player_index == victim_player_index || killer_player_index == NONE)
        {
            if (player_was_vip(victim_player_index, &team_designator))
                game_engine_adjust_player_score(victim_player_index, variant->get_vip_suicide_points());
            else
                game_engine_adjust_player_score(victim_player_index, variant->get_suicide_points());
        }
        else if (betrayal && !player_was_vip(victim_player_index, &team_designator))
        {
            game_engine_adjust_player_score(killer_player_index, variant->get_betrayal_points());
        }
        if (end_round)
            game_engine_end_round_with_winner(NONE, false, game_engine_end_condition_vip_killed);
    }
}
