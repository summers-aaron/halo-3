// blamlibxbox/game_engine_king.obj: from the records 0x2e9678 0x2e96b8 0x2e9770 0x2e9780 0x2e9808 0x2e9890 0x2e9a10 0x2e9e60 0x2ea000 0x2ea090 0x2ea098 0x2ea0a8 0x2ea2b8 0x2ea300 0x2ea328 0x2ea3c0 0x2ea448 0x2ea538 0x2ea620 0x2eab60 0x2eac00 0x2eae88 0x2eaf10 0x2eb060 0x2eb0e0 0x2eb160 0x2eb330
// @compiler xdk6130
// @flags /TP /O2 /J /Oc
#include "game/player_with_unit_iterator.h"
#include "interface/chud/chud_user_data_types.h"
#include "game/player_color_types.h"
#include "game/game_results_event_carry_types.h"
#include "game/game_engine_kill_flags_types.h"
#include "memory/tls_globals.h"
#include "math/real_math_types.h"
#include "memory/data_types.h"
#include "game/game_time_types.h"
#include "game/player_traits.h"
#include "game/game_variant.h"
#include "objects/object_iterator.h"
#include "cseries/crt_xdk.h"
#include <string.h>
#include <stdlib.h>
#include "game/game_engine.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "game/game_engine_area_set.h"
#include "game/game_engine_team.h"
#include "game/game_engine_util.h"
#include "objects/multiplayer_game_objects.h"
#include "game/game_engine_vip.h"
#include "game/game_engine_king.h"
#include "game/player_types.h"
#include "networking/network_utilities.h"
#include "objects/object_datum.h"
#include "game/area_cycle_options_types.h"
#include "game/team_scoring_method_types.h"
#include "game/game_engine_interface_state_types.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "game/game_results.h"
#include "game/game_results_types.h"
#include "memory/static_arrays.h"
#include "game/game_engine_king_globals.h"
#include "game/game_engine_default.h"
#include "game/game_engine_base_variant.h"
#include "game/game_globals_storage.h"
#include "game/game_engine_area_set_types.h"
#include "game/game_engine_event_data.h"
#include <new>
#include "game/player_mapping_types.h"
#include "interface/chud/chud_navpoint_types.h"
#include "game/game_engine_kill_handler.h"
#include "memory/data.h"
#include "memory/data_absolute_index.h"
#include "memory/datum_index.h"
#include "math/random_math_range.h"
#include "tag_files/string_ids.h"
#include "simulation/simulation_entity_types.h"
typedef struct __regfix_ret_king_engine_player_get { player_datum * v; } __regfix_ret_king_engine_player_get;
struct s_king_area_storage
{
    char data[0x60];
    long object_index;
};
class c_king_object_iterator : public c_object_iterator_base
{
public:
    void begin(unsigned long type_mask, unsigned long header_mask, unsigned long match_flags)
    {
        object_iterator_begin_internal(type_mask, header_mask, match_flags);
    }
    bool next() { return object_iterator_next_with_match_flags_internal(); }
};

// @address 0x41ce8 ??_7c_king_engine@@6B@
// @address 0x79d42c ?internal_king_engine@@3Vc_king_engine@@A
c_king_engine internal_king_engine;

// @address 0x3fc550 ?get_type@c_king_engine@@UBAJXZ
long c_king_engine::get_type() const
{
    return _game_engine_type_koth;
}

// @address 0x3fc550 ?get_simulation_entity_type@c_king_engine@@UBA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_king_engine::get_simulation_entity_type() const
{
    return _simulation_entity_type_king_engine_globals;
}

// @address 0x42d298 ?leader_event_lead_enabled@c_king_engine@@UBA_NXZ
bool c_king_engine::leader_event_lead_enabled() const
{
    return false;
}

// @address 0x2e9678 ?compute_object_function@c_king_engine@@UBAMJJ@Z
// @address 0x2e9770 ?dispose_from_old_map@c_king_engine@@UBAXXZ
// @address 0x2ea090 ?score_header_string@c_king_engine@@UBAXPAV?$c_static_wchar_string@$0BAA@@@@Z
// @address 0x2ea2b8 ?apply_game_engine_traits_for_player@c_king_engine@@UBAXJPAVc_player_traits@@@Z
// @address 0x2ea300 ?player_about_to_spawn@c_king_engine@@UBAXJ@Z
// @address 0x2ea3c0 ?build_simulation_baseline@c_king_engine@@UBAXJPAX@Z
// @address 0x2ea448 ?build_simulation_update@c_king_engine@@UBAXPAKJPAX@Z
// @address 0x2ea538 ?apply_simulation_update@c_king_engine@@UBA_NKJPBX@Z
// @address 0x2eab60 ?teams_in_hill_from_players_in_hill@@YAGG@Z
// @address 0x2eb0e0 ??$count_bits@G@@YAKG@Z
// @address 0x8c43c8 g_king_variant
#if !defined(_XBOX) || defined(__clang__)
// The port links the variant pointer by its C name (game/game_engine_king_settings_types.h):
// game_engine_koth_traits.cpp reads it for the moving-hill switch, so this unit owns the one external definition.
extern "C" {
c_game_engine_king_variant *g_king_variant;
}
#endif

typedef struct s_game_engine_globals s_game_engine_globals;

static __forceinline s_king_globals *king_globals_get() {
    return &tls_game_engine_globals()->king();
}
float c_king_engine::compute_object_function(long object_index, long function_name) const
{
    float result = 0.0f;
    if (function_name == _string_id_mp_goal_active && king_globals_get()->players_in_hill != 0)
        result = 1.0f;
    return result;
}

void c_king_engine::dispose_from_old_map() const
{
    g_king_variant = 0;
}

// @address 0x2ea000 ?get_hud_interface_state@c_king_engine@@UBAXJPAUgame_engine_interface_state@@@Z
void c_king_engine::get_hud_interface_state(long player_index, game_engine_interface_state *state) const
{
    unsigned long teams_in_hill = teams_in_hill_from_players_in_hill(king_globals_get()->players_in_hill);
    player_datum *player = &((player_datum *)player_data->data)[(unsigned short)player_index];
    long team_mask = 1 << player->configuration.host.team_index;
    state->team_in_hill = (team_mask & teams_in_hill) != 0;
    state->enemy_in_hill = (teams_in_hill & ~team_mask) != 0;
}

void c_king_engine::score_header_string(c_static_wchar_string<256> *string) const
{
    game_engine_get_universal_string(0x115, string);
}

// @address 0x2ea098 ?recompute_team_score@c_king_engine@@UBAXW4e_game_team@@JW4e_team_scoring_method@@@Z
void c_king_engine::recompute_team_score(e_game_team team, long score_delta, e_team_scoring_method scoring_method) const
{
    if (scoring_method == _team_scoring_method_none)
        scoring_method = _team_scoring_method_sum;
    c_game_engine::recompute_team_score(team, score_delta, scoring_method);
}

void c_king_engine::apply_game_engine_traits_for_player(long player_index, c_player_traits *player_traits) const
{
    long absolute_index = datum_index_to_absolute_index(player_index);
    player_datum *players = (player_datum *)player_data->data;
    if (players[absolute_index].hill_traits_flag == 0)
        return;
    player_traits->set(&g_king_variant->m_inside_hill_traits, false);
}

void c_king_engine::player_about_to_spawn(long player_index) const
{
    long absolute_index = datum_index_to_absolute_index(player_index);
    king_globals_get()->player_hill_ticks[absolute_index] = 0;
    c_game_engine::player_about_to_spawn(player_index);
}

void c_king_engine::build_simulation_baseline(long state_data_size, void *state_data) const
{
    s_king_engine_state_data *king_state = (s_king_engine_state_data *)state_data;
    memset(king_state, 0, sizeof(s_king_engine_state_data));
    king_state->first = (short)(king_get_hill_move_in_seconds(g_king_variant) * tls_game_time_globals()->ticks_per_second);
    build_global_baseline((s_game_engine_state_data *)king_state);
}

void c_king_engine::build_simulation_update(unsigned long *update_mask, long state_data_size, void *state_data) const
{
    s_king_engine_state_data *king_state = (s_king_engine_state_data *)state_data;
    unsigned long updated_mask = 0;
    unsigned long global_mask = *update_mask & 63;
    if (global_mask)
        build_global_update(global_mask, &updated_mask, (s_game_engine_state_data *)king_state);
    if (*update_mask & 64) {
        if (king_state->second != king_globals_get()->players_in_hill) {
            king_state->second = king_globals_get()->players_in_hill;
            updated_mask |= 64;
        }
    }
    if (*update_mask & 128) {
        if (king_state->first != king_globals_get()->hill_move_ticks) {
            king_state->first = (short)king_globals_get()->hill_move_ticks;
            updated_mask |= 128;
        }
    }
    *update_mask = updated_mask;
}

bool c_king_engine::apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const
{
    const s_king_engine_state_data *king_state = (const s_king_engine_state_data *)state_data;
    bool success = true;
    unsigned long global_mask = update_mask & 63;
    if (global_mask) {
        bool handled = apply_global_update(global_mask, (const s_game_engine_state_data *)king_state);
        success = success && handled;
    }
    if ((update_mask & 64) && king_globals_get()->players_in_hill != king_state->second)
        king_globals_get()->players_in_hill = king_state->second;
    if ((update_mask & 128) && king_globals_get()->hill_move_ticks != king_state->first)
        king_globals_get()->hill_move_ticks = king_state->first;
    return success;
}

unsigned short teams_in_hill_from_players_in_hill(unsigned short players_in_hill)
{
    unsigned short teams_in_hill = 0;
    for (long player_index = 0; player_index < 16; ++player_index) {
        if (players_in_hill & (1 << player_index)) {
            player_datum *player = (player_datum *)datum_try_and_get_absolute(player_data, player_index);
            if (player && player->configuration.host.team_index >= 0 && player->configuration.host.team_index < 16)
                teams_in_hill |= 1 << player->configuration.host.team_index;
        }
    }
    return teams_in_hill;
}
template <typename t_type> unsigned long count_bits(t_type value)
{
    unsigned long bits = value;
    bits = ((bits >> 1) & 0x55555555) + (bits & 0x55555555);
    bits = ((bits >> 2) & 0x33333333) + (bits & 0x33333333);
    bits = ((bits >> 4) & 0x0f0f0f0f) + (bits & 0x0f0f0f0f);
    bits = ((bits >> 8) & 0x00ff00ff) + (bits & 0x00ff00ff);
    return bits;
}
template unsigned long count_bits<unsigned short>(unsigned short value);

namespace {
static __forceinline bool king_game_is_predicted()
{
    game_globals_storage *globals = tls_game_globals();
    bool result = false;
    if (globals->options.game_simulation == 4)
        result = true;
    return result;
}
static __forceinline bool king_game_is_authoritative()
{
    return !king_game_is_predicted();
}
}

// @address 0x2eb160 ??0?$c_static_array@Vc_area@@$09@@QAA@XZ

template<class t_area, long k_area_count>
inline t_area *c_area_set<t_area, k_area_count>::find_selected_area()
{
    t_area *result = 0;
    t_area *end = &m_areas[0] + m_area_count;
    for (t_area *area = &m_areas[0]; area != end; ++area) {
        if (area->is_selected()) {
            result = area;
            break;
        }
    }
    return result;
}
// @address 0x2eaf10 ?select_area@?$c_area_set@Vc_area@@$09@@QAAXW4e_area_cycle_options@@@Z
template<class t_area, long k_area_count>
void c_area_set<t_area, k_area_count>::select_area(e_area_cycle_options area_cycle_options)
{
    if (king_game_is_authoritative()) {
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
            // match: 0x2eaf10: unreachable hint omits dead branch check
            __assume(0);
        }
        if (selected_area)
            selected_area->set_selected(true);
    }
}
template void c_area_set<c_area, 10>::select_area(e_area_cycle_options);
// @address 0x2eb060 ?find_selected_area@?$c_area_set@Vc_area@@$09@@QAAPAVc_area@@XZ
template c_area *c_area_set<c_area, 10>::find_selected_area();

static __forceinline s_object_header *king_object_get_header(long handle)
{
    return datum_get<s_object_header>(tls_object_header_data(), handle);
}
static __forceinline void *king_datum_try_and_get(const s_data_array *data, long handle)
{
    if (handle == -1)
        return 0;
    long absolute_index = datum_index_to_absolute_index(handle);
    if (absolute_index >= data->first_unallocated)
        return 0;
    short *datum = (short *)(data->data + data->size * absolute_index);
    long identifier = *datum;
    if (identifier == 0 || identifier != (handle >> 16))
        return 0;
    return datum;
}
static __forceinline object_datum *king_object_try_and_get_and_verify_type(long handle, unsigned long type_mask)
{
    s_object_header *header = (s_object_header *)king_datum_try_and_get(tls_object_header_data(), handle);
    if (!header || !((1 << header->object_type) & type_mask))
        return 0;
    return (object_datum *)header->object;
}
static __forceinline s_multiplayer_object_properties *king_object_get_owner_data(long handle)
{
    object_datum *object = king_object_try_and_get_and_verify_type(handle, -1);
    if (object && object->multiplayer_data_offset != -1)
        return (s_multiplayer_object_properties *)((char *)object + object->multiplayer_data_offset);
    return 0;
}

// @address 0x2eae88 king_initialize_areas
extern "C" void king_initialize_areas(c_area_set<c_area, 10> *areas, long type_mask, long match_flags, long team_designator)
{
    areas->m_team_designator = 8;
    areas->m_area_type_flags = 0x440;
    areas->m_area_flags = 0x800;
    areas->update();
    bool predicted = false;
    if (tls_game_globals()->options.game_simulation == 4)
        predicted = true;
    if (!predicted) {
        c_area *selected_area = areas->find_selected_area();
        if (selected_area)
            selected_area->set_selected(false);
    }
}

// @address 0x2eb330 ?update_areas@?$c_area_set@Vc_area@@$09@@IAAXXZ
template<class t_area, long k_area_count>
void c_area_set<t_area, k_area_count>::update_areas()
{
    long handles[20];
    s_king_area_storage temporary;
    c_king_object_iterator iterator;
    long count = 0;

    iterator.begin(m_area_type_flags, 0, m_area_flags);
    while (iterator.next() && count < 20) {
        long handle = iterator.get_index();
        s_multiplayer_object_properties *owner_data = king_object_get_owner_data(handle);
        if (!(king_object_get_header(handle)->flags & 0x10)) {
            if (!owner_data || owner_data->team_designator == m_team_designator)
                handles[count++] = handle;
        }
    }
    qsort(handles, count, 4, c_area_set_compare_by_order_index);
    if (count > 10)
        count = 10;
    for (long i = 0; i < count; i++) {
        long handle = handles[i];
        if (m_areas[i].get_object_index() != handle) {
            memcpy(&temporary, &m_areas[i], sizeof(c_area));
            for (long j = i; j < m_area_count; j++) {
                if (m_areas[j].get_object_index() == handle) {
                    memcpy(&m_areas[i], &m_areas[j], sizeof(c_area));
                    memcpy(&m_areas[j], &temporary, sizeof(c_area));
                    break;
                }
            }
        }
        m_areas[i].set_object_index(handle);
        m_areas[i].calculate_boundary_geometry();
    }
    m_area_count = count;
}

template void c_area_set<c_area, 10>::update_areas();

// @address 0x2e9808 ?emit_game_start_event@c_king_engine@@UBAXJ@Z
void c_king_engine::emit_game_start_event(long player_index) const
{
    bool predicted = false;
    if (tls_game_globals()->options.game_simulation == 4)
        predicted = true;
    if (!predicted) {
        s_game_engine_event_data event;
        event.event = 0;
        event.data = 0;
        event.flags = player_index;
        event.type = 6;
        event.identifier = -1;
        event.cause_player_index = -1;
        event.cause_team_index = -1;
        event.effect_player_index = -1;
        event.effect_team_index = -1;
        event.data_word = -1;
        game_engine_send_event(&event);
    }
}

static __forceinline const c_game_variant *king_current_game_variant()
{
    game_options *options = &tls_game_globals()->options;
    return options->game_mode == 2 ? &options->game_variant : 0;
}

// @address 0x2e96b8 ?initialize_for_new_map@c_king_engine@@UBA_NXZ
bool c_king_engine::initialize_for_new_map() const
{
    king_current_game_variant()->get_active_variant()->can_be_cast_to(_game_engine_king, (const void **)&g_king_variant);

    s_king_globals *king = &tls_game_engine_globals()->king();
    if (king) {
        new (king) s_king_globals;
    }
    memset(king, 0, sizeof(s_king_globals));
    return true;
}

// @address 0x2e9780 ?initialize_for_new_round@c_king_engine@@UBA_NXZ
bool c_king_engine::initialize_for_new_round() const
{
    short value;
    s_game_engine_globals *engine;
    engine = tls_game_engine_globals();
    value = king_get_hill_move_in_seconds(g_king_variant);
    short factor = tls_game_time_globals()->ticks_per_second;
    engine->king().hill_move_ticks = (int)value * (int)factor;
    king_initialize_areas(&engine->king().areas, 0x440, 0x800, 8);
    ((c_area_set<c_area, 10> *)&king_globals_get()->areas)->select_area((e_area_cycle_options)g_king_variant->m_moving_hill_order);
    return true;
}

// @address 0x2ea328 ?promote_to_simulation_authority@c_king_engine@@UBAXXZ
void c_king_engine::promote_to_simulation_authority() const
{
    s_game_engine_globals *game = tls_game_engine_globals();
    c_area_set<c_area, 10> *areas = &game->king().areas;
    if (!areas->find_selected_area()) {
        bool round_started = game->king().areas.m_area_count > 0;
        if (!round_started)
            king_initialize_areas(areas, 0x440, 0x800, 8);
        ((c_area_set<c_area, 10> *)&king_globals_get()->areas)
            ->select_area((e_area_cycle_options)g_king_variant->m_moving_hill_order);
    }
}

// @address 0x2e9890 ?player_update@c_king_engine@@UBAXJ@Z
void c_king_engine::player_update(long player_index) const
{
    c_game_engine::player_update(player_index);
    player_datum *player = &((player_datum *)player_data->data)[(unsigned short)player_index];
    if (player->unit_index != -1 && !tls_game_globals()->finished && !king_game_is_predicted()) {
        c_area_set<c_area, 10> *set = &tls_game_engine_globals()->king().areas;
        unsigned char in_hill = false;
        if (game_engine_in_round()) {
            c_area *selected = set->find_selected_area();
            if (selected) {
                in_hill = player_in_multiplayer_object_boundary(player_index, (const s_multiplayer_object_boundary_geometry_data *)selected);
            } else {
                for (long i = 0; i < set->m_area_count && !in_hill; ++i) {
                    if (player_in_multiplayer_object_boundary(player_index, (const s_multiplayer_object_boundary_geometry_data *)&set->m_areas[i]))
                        in_hill = true;
                }
            }
        }
        if (in_hill) {
            if (player->ticks_in_hill < 32767)
                ++player->ticks_in_hill;
            player->ticks_outside_hill = 0;
        } else {
            player->ticks_in_hill = 0;
            if (player->ticks_outside_hill < 32767)
                ++player->ticks_outside_hill;
        }
    } else {
        player->ticks_in_hill = 0;
        player->ticks_outside_hill = 0;
        player->hill_counter = 0;
    }
}

static __forceinline long king_nav_player_index(long user_index)
{
    long result = -1;
    if (user_index != -1)
        result = tls_player_mapping_globals()->player_indices[user_index];
    return result;
}
static __forceinline float king_nav_height(const c_area *area)
{
    float result = 0.0f;
    if (area->m_boundary_geometry.type == 2 || area->m_boundary_geometry.type == 3)
        result = area->m_boundary_geometry.height;
    return result;
}
static __forceinline void king_nav_center(const c_area *area, real_point3d *point)
{
    *point = area->m_boundary_geometry.matrix.position;
    point->z += king_nav_height(area) * 0.5f;
}

static __forceinline long king_player_index_from_absolute_player_index(long absolute_index)
{
    return datum_absolute_index_to_index(player_data, absolute_index);
}
static __forceinline void king_engine_player_iterator_begin(c_player_with_unit_iterator *iterator)
{
    iterator->iterator.absolute_index = -1;
    iterator->iterator.index = -1;
    iterator->iterator.data = player_data;
}
static __forceinline void king_engine_initialize_event(long event_type, long event_index, s_game_engine_event_data *event)
{
    event->type = event_type;
    event->event = event_index;
    event->identifier = -1;
    event->flags = -1;
    event->cause_player_index = -1;
    event->cause_team_index = -1;
    event->effect_player_index = -1;
    event->effect_team_index = -1;
    event->data = 0;
    event->data_word = -1;
}
static __forceinline void king_engine_set_event_cause_player_and_team(long player_index, s_game_engine_event_data *event)
{
    event->cause_player_index = player_index;
    event->cause_team_index = datum_get<player_datum>(player_data, player_index)->configuration.host.team_index;
}

// @address 0x2e9a10 ?update@c_king_engine@@UBAXXZ
void c_king_engine::update() const
{
    if (!tls_game_globals()->finished && !king_game_is_predicted()) {
        unsigned short players_in_hill = 0;
        c_player_with_unit_iterator iterator;
        king_engine_player_iterator_begin(&iterator);
        if (iterator.next()) {
            do {
                if (((player_datum *)iterator.datum)->hill_traits_flag != 0)
                    players_in_hill |= (unsigned short)(1 << (unsigned short)iterator.iterator.index);
            } while (iterator.next());
        }
        if (players_in_hill != king_globals_get()->players_in_hill) {
            king_engine_handle_hill_messages(king_globals_get()->players_in_hill, players_in_hill);
            king_globals_get()->players_in_hill = players_in_hill;
            simulation_action_game_engine_globals_update(64);
        }
        king_engine_update_scoring();
        if (king_get_hill_move_in_seconds(g_king_variant) != 0) {
            --king_globals_get()->hill_move_ticks;
            if (king_globals_get()->hill_move_ticks <= 0) {
                king_globals_get()->hill_move_ticks = king_get_hill_move_in_seconds(g_king_variant) * tls_game_time_globals()->ticks_per_second;
                ((c_area_set<c_area, 10> *)&tls_game_engine_globals()->king().areas)
                    ->select_area((e_area_cycle_options)g_king_variant->m_moving_hill_order);
                s_game_engine_event_data event;
                king_engine_initialize_event(6, 4, &event);
                game_engine_send_event(&event);
                long *values = king_globals_get()->player_hill_ticks.m_storage;
                for (long i = 0; i < 16; ++i)
                    values[i] = 0;
            }
            simulation_action_game_engine_globals_update(128);
        }
    }
    tls_game_engine_globals()->king().areas.update();
    unsigned short players = king_globals_get()->players_in_hill;
    real_argb_color color = game_engine_area_color(_real_argb_grey);
    unsigned short teams = teams_in_hill_from_players_in_hill(players);
    unsigned long ones = players;
    ones = ((ones >> 1) & 0x55555555) + (ones & 0x55555555);
    ones = ((ones >> 2) & 0x33333333) + (ones & 0x33333333);
    ones = ((ones >> 4) & 0x0f0f0f0f) + (ones & 0x0f0f0f0f);
    ones = ((ones >> 8) & 0x00ff00ff) + (ones & 0x00ff00ff);
    if (ones == 1 || count_bits(teams) == 1) {
        if (game_engine_has_teams()) {
            for (long team_index = 0; team_index < 8; ++team_index) {
                if ((teams & (1 << team_index)) != 0) {
                    color.alpha = 1.0f;
                    color.rgb = network_get_team_color((e_game_team)team_index);
                    break;
                }
            }
        }
        else {
            unsigned short bits = (unsigned short)(players >> 1);
            long absolute_index = 0;
            if (bits != 0) {
                do {
                    bits = (unsigned short)(bits >> 1);
                    ++absolute_index;
                } while (bits != 0);
            }
            long index = king_player_index_from_absolute_player_index(absolute_index);
            if (index != -1) {
                real_rgb_color primary[4];
                bool secondary[4];
                game_engine_get_change_colors(index, &king_engine_player_get(index)->configuration.client.appearance, _game_team_none, primary, secondary);
                color.alpha = 1.0f;
                color.rgb = primary[0];
            }
        }
    }
    c_area_set<c_area, 10> *areas = &tls_game_engine_globals()->king().areas;
    c_area *selected_area = areas->find_selected_area();
    if (selected_area) {
        selected_area->set_color(color);
        for (c_area *area = &areas->m_areas[0]; area != &areas->m_areas[0] + areas->m_area_count; ++area) {
            if (area->is_selected())
                multiplayer_object_build_boundary_primitive((const vip_s_implicit_geometry_shape *)area, area->get_color(), area->get_object_index());
        }
    }
}

// @address 0x2eac00 ?king_engine_handle_hill_messages@@YAXJJ@Z
void king_engine_handle_hill_messages(long old_players_in_hill, long new_players_in_hill)
{
    unsigned short old_teams = teams_in_hill_from_players_in_hill((unsigned short)old_players_in_hill);
    unsigned short new_teams = teams_in_hill_from_players_in_hill((unsigned short)new_players_in_hill);
    if (old_teams == new_teams)
        return;
    c_player_with_unit_iterator iterator;
    if (new_teams != 0 && (new_teams & (new_teams - 1)) == 0) {
        long player_index = -1;
        king_engine_player_iterator_begin(&iterator);
        while (iterator.next()) {
            player_datum *player = (player_datum *)iterator.datum;
            if (player->hill_traits_flag != 0 && player->configuration.host.team_index >= 0 && player->configuration.host.team_index < 16
                && (new_teams & (1 << player->configuration.host.team_index)) != 0) {
                player_index = iterator.iterator.index;
                break;
            }
        }
        s_game_engine_event_data event;
        king_engine_initialize_event(6, ((game_engine_has_teams() ? -1 : 0) & 4) + 1, &event);
        if (player_index != -1)
            king_engine_set_event_cause_player_and_team(player_index, &event);
        game_engine_send_event(&event);
    }
    bool capture = game_engine_has_teams()
        ? (count_bits(new_teams) >= 2 && count_bits(old_teams) <= 1)
        : (count_bits<unsigned long>(new_players_in_hill) >= 2 && count_bits<unsigned long>(old_players_in_hill) <= 1);
    if (capture) {
        king_engine_player_iterator_begin(&iterator);
        while (iterator.next()) {
            player_datum *player = (player_datum *)iterator.datum;
            if (player->hill_traits_flag != 0 && player->configuration.host.team_index >= 0 && player->configuration.host.team_index < 16) {
                s_game_engine_event_data event;
                king_engine_initialize_event(6, ((game_engine_has_teams() ? -1 : 0) & 4) + 2, &event);
                king_engine_set_event_cause_player_and_team(iterator.iterator.index, &event);
                game_engine_send_event(&event);
            }
        }
    }
}

// @address 0x2e9e60 ?submit_nav_points@c_king_engine@@UBAXJJ@Z
void c_king_engine::submit_nav_points(long user_index, long player_index) const
{
    c_game_engine::submit_nav_points(user_index, player_index);
    s_game_engine_globals *globals = tls_game_engine_globals();
    c_area *selected = globals->king().areas.find_selected_area();
    if (selected) {
        c_area *area = (c_area *)selected;
        s_chud_navpoint navpoint;
        long mapped_index = king_nav_player_index(user_index);
        navpoint.flags = mapped_index != -1 &&
                player_in_multiplayer_object_boundary(mapped_index, (const s_multiplayer_object_boundary_geometry_data *)selected)
            ? 64
            : 65;
        navpoint.object_index = area->m_object_index;
        navpoint.type = 2;
        navpoint.icon = 8;
        long color = 1;
        if (mapped_index != -1) {
            player_datum *player = &((player_datum *)player_data->data)[(unsigned short)mapped_index];
            unsigned short teams = ::teams_in_hill_from_players_in_hill(globals->king().players_in_hill);
            if (count_bits(teams) == 1) {
                long team_mask = 1 << player->configuration.host.team_index;
                if (teams & team_mask)
                    color = 0;
                else if (teams & ~(1 << player->configuration.host.team_index))
                    color = 2;
            }
        }
        navpoint.state = color;
        navpoint.scale = 1.0f;
        navpoint.name[0] = 0;
        king_nav_center(area, &navpoint.position);
        if ((unsigned long)user_index <= 3)
            chud_persistent_user_data()[user_index].m_navpoint_manager.submit_navpoint(&navpoint);
    }
}

static __forceinline player_datum *king_engine_player_get(long index)
{
    return &((player_datum *)player_data->data)[datum_index_to_absolute_index(index)];
}
static __forceinline __regfix_ret_king_engine_player_get king_engine_player_get_rfret(long index)
{
    { __regfix_ret_king_engine_player_get rf_ret; rf_ret.v = (&((player_datum *)player_data->data)[datum_index_to_absolute_index(index)]); return rf_ret; }
}
static __forceinline void king_engine_adjust_statistic(long index, long statistic)
{
    long absolute_index = datum_index_to_absolute_index(index);
    long team = game_engine_has_teams() ? king_engine_player_get(index)->configuration.host.team_index : -1;
    ::game_results_statistic_increment(absolute_index, (e_game_team)team, (e_game_results_statistic)statistic, 1);
}
// @address 0x2ea0a8 ?player_killed_player@c_king_engine@@UBAXJJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z
void c_king_engine::player_killed_player(
    long killer, long assistant, long victim, bool betrayal, c_flags<e_game_engine_kill_flags, long, 24> flags, long damage) const
{
    short kill_points = g_king_variant->m_kill_points;
    game_engine_handle_player_killed((void *)this, killer, assistant, victim, betrayal, flags, damage);
    if (killer != -1) {
        if (king_engine_player_get_rfret(victim).v->ticks_in_hill > 0 && !betrayal && killer != victim)
            king_engine_adjust_statistic(killer, 28);
        if (!betrayal && killer != victim && king_engine_player_get_rfret(killer).v->ticks_in_hill > 0)
            king_engine_adjust_statistic(killer, 27);
        if (killer != victim && kill_points != 0) {
            game_results_insert_carry_event(killer, _game_results_event_carry_type_king, -1);
            game_engine_adjust_player_score(killer, kill_points);
        }
        long absolute_index = datum_index_to_absolute_index(killer);
        if (!betrayal && killer != victim && king_engine_player_get_rfret(killer).v->ticks_in_hill > 0) {
            king_engine_adjust_statistic(killer, 27);
            ++king_globals_get()->player_hill_ticks[absolute_index];
            if (king_globals_get()->player_hill_ticks[absolute_index] == 5) {
                long event = 0;
                ::game_engine_handle_time_messages(32, absolute_index, killer, victim, (int *)(&event));
            }
        } else {
            king_globals_get()->player_hill_ticks[absolute_index] = 0;
        }
    }
}

static __forceinline e_game_team king_engine_player_team(long absolute_index)
{
    return game_engine_has_teams() ? (e_game_team)((player_datum *)player_data->data)[absolute_index].configuration.host.team_index : _game_team_none;
}

// @address 0x2ea620 ?king_engine_update_scoring@@YAXXZ
void king_engine_update_scoring(void)
{
    bool has_teams = game_engine_has_teams();
    long players_in_hill = 0;
    long teams_in_hill = 0;
    {
        c_player_with_unit_iterator iterator;
        king_engine_player_iterator_begin(&iterator);
        while (iterator.next()) {
            player_datum *player = (player_datum *)iterator.datum;
            if (player->ticks_in_hill != 0) {
                players_in_hill |= 1 << (unsigned char)iterator.iterator.index;
                if (has_teams && player->configuration.host.team_index >= 0 && player->configuration.host.team_index < 8)
                    teams_in_hill |= 1 << player->configuration.host.team_index;
            }
        }
    }
    bool uncontested = has_teams
        ? (teams_in_hill != 0 && (teams_in_hill & (teams_in_hill - 1)) == 0)
        : (players_in_hill != 0 && (players_in_hill & (players_in_hill - 1)) == 0);
    if (!game_engine_in_round())
        return;
    if (has_teams) {
        for (long team = 0; team < 8; team++) {
            bool team_in_hill = true;
            long count;
            long points;
            if ((1 << team) & teams_in_hill) {
                king_globals_get()->team_outside_ticks[team] = 0;
                king_globals_get()->team_inside_ticks[team]++;
                count = king_globals_get()->team_inside_ticks[team];
                points = g_king_variant->m_inside_hill_points;
                if (uncontested)
                    points += g_king_variant->m_uncontested_hill_bonus;
            } else {
                team_in_hill = false;
                king_globals_get()->team_inside_ticks[team] = 0;
                king_globals_get()->team_outside_ticks[team]++;
                count = king_globals_get()->team_outside_ticks[team];
                points = g_king_variant->m_outside_hill_points;
            }
            if (count % tls_game_time_globals()->ticks_per_second == 0 && points != 0) {
                c_player_with_unit_iterator player_iterator;
                king_engine_player_iterator_begin(&player_iterator);
                while (player_iterator.next()) {
                    player_datum *player = (player_datum *)player_iterator.datum;
                    short *ticks = team_in_hill ? &player->ticks_in_hill : &player->ticks_outside_hill;
                    if (player->configuration.host.team_index == team && *ticks != 0) {
                        bool best = true;
                        long player_index = player_iterator.iterator.index;
                        if (g_king_variant->m_team_scoring_method == 3) {
                            c_player_with_unit_iterator other;
                            king_engine_player_iterator_begin(&other);
                            while (other.next()) {
                                player_datum *other_player = (player_datum *)other.datum;
                                short other_ticks = team_in_hill ? other_player->ticks_in_hill : other_player->ticks_outside_hill;
                                if (other.datum != player_iterator.datum && other_player->configuration.host.team_index == player->configuration.host.team_index
                                    && other_ticks != 0
                                    && (other_ticks > *ticks || (other_ticks == *ticks && (short)player_index < (short)other.iterator.index))) {
                                    best = false;
                                    break;
                                }
                            }
                        }
                        if (best) {
                            game_engine_adjust_player_score(player_index, points);
                            if (g_king_variant->m_team_scoring_method == 3) {
                                *ticks = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
    } else {
        c_player_with_unit_iterator player_iterator;
        king_engine_player_iterator_begin(&player_iterator);
        while (player_iterator.next()) {
            player_datum *player = (player_datum *)player_iterator.datum;
            if (player->ticks_in_hill != 0 || player->ticks_outside_hill != 0) {
                short ticks;
                short points;
                if (player->ticks_in_hill != 0) {
                    ticks = player->ticks_in_hill;
                    points = g_king_variant->m_inside_hill_points;
                    if (uncontested)
                        points += g_king_variant->m_uncontested_hill_bonus;
                } else {
                    ticks = player->ticks_outside_hill;
                    points = g_king_variant->m_outside_hill_points;
                }
                if (ticks % tls_game_time_globals()->ticks_per_second == 0 && points != 0) {
                    long player_index = player_iterator.iterator.index;
                    game_results_insert_carry_event(player_index, _game_results_event_carry_type_king, -1);
                    game_engine_adjust_player_score(player_index, points);
                }
            }
        }
    }
    {
        c_player_with_unit_iterator iterator;
        king_engine_player_iterator_begin(&iterator);
        while (iterator.next()) {
            player_datum *player = (player_datum *)iterator.datum;
            const short &ticks = player->ticks_in_hill;
            if (ticks % tls_game_time_globals()->ticks_per_second == 0 && ticks != 0) {
                long absolute_index = datum_index_to_absolute_index(iterator.iterator.index);
                game_results_statistic_increment(absolute_index, king_engine_player_team(absolute_index), _game_results_statistic_king_unused_0, 1);
                if (uncontested)
                    game_results_statistic_increment(absolute_index, king_engine_player_team(absolute_index), _game_results_statistic_king_unused_1, 1);
            }
        }
    }
}
