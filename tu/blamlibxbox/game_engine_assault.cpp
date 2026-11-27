// blamlibxbox/game_engine_assault.obj: from the records 0x2a0ad0 0x2a0b58 0x2a0be0 0x2a0c38 0x2a0cd0 0x2a0da0 0x2a12d8 0x2a1360 0x2a1508 0x2a1678 0x2a17a8 0x2a17b8 0x2a1868 0x2a18e8 0x2a1a28 0x2a1a88 0x2a1a98 0x2a1c70 0x2a1d20 0x2a2180 0x2a21e0 0x2a2258 0x2a2358 0x2a2430 0x2a26b0 0x2a28e8 0x2a2c28 0x2a2d00 0x2a2f60 0x2a30c0 0x2a31c8 0x2a34a0 0x2a3578 0x2a37f8 0x2a38a8 0x2e0e18 0x2e7150
// @compiler xdk6130
#include "tag_files/tag_groups.h"
#include "game/game_results_event_carry_types.h"
#include "cseries/cseries_unicode.h"
#include "game/game_engine_area_initialization.h"
#include "game/game_engine_player_traits.h"
#include "interface/chud/chud_user_data_types.h"
#include "objects/damage_data.h"
#include "math/real_math_globals.h"
#include "memory/tls_globals.h"
#include "game/player_mapping_types.h"
#include "memory/tls_data_arrays.h"
#include "game/game_time_types.h"
#include "interface/chud/chud_manager.h"
#include "objects/object_reference_types.h"
#include "math/real_math_types.h"
#include "math/real_math.h"
#include "memory/data_types.h"
#include "memory/datum_index.h"
#include "tag_files/tag_types.h"
#include "tag_files/string_ids.h"
#include "objects/object_types.h"
#include "game/player_types.h"
#include "game/game_types.h"
#include "game/player_traits.h"
#include "game/game_globals.h"
#include "game/game_variant.h"
#include "game/game_options.h"
#include "game/game_globals_storage.h"
#include "items/multiplayer_weapon_tracker.h"
#include <string.h>
#include "cseries/cseries.h"
#include "game/game_engine.h"
#include "game/players.h"
#include "objects/objects.h"
#include "simulation/game_interface/simulation_game_action.h"
#include "text/unicode.h"
#include "game/game_engine_assault.h"
#include "game/game_engine_ctf.h"
#include "game/game_engine_util.h"
#include "objects/multiplayer_game_objects.h"
#include "motor/action_system.h"
#include "memory/data.h"
#include "objects/object_datum.h"
#include "networking/network_utilities.h"
#include "game/game_engine_team.h"
#include "game/materials.h"
#include "game/multiplayer_powerup_flavor_types.h"
#include "cache/cache_files.h"
#include "objects/damage.h"
#include "networking/simulation/simulation_game_engine_globals.h"
#include "simulation/simulation_entity_types.h"
#include "effects/effect_vector_types.h"
#include "effects/effect_deterministic_types.h"
#include "game/game_engine_assault_variant.h"
#include "items/weapon_definitions.h"
#include "game/game_engine_settings_definitions.h"
#include "game/multiplayer_runtime_globals.h"
#include "game/game_engine_event_data.h"
#include "game/game_engine_scoring_globals.h"
#include "objects/multiplayer_object_types.h"
#include "game/game_engine_interface_state_types.h"
#include "game/game_results.h"
#include "units/unit_types.h"
#include "ai/weapon_datum_types.h"
#include "interface/chud/chud_navpoint_types.h"
#include "game/game_engine_area_set_types.h"
#include "game/multiplayer_constants.h"
#include "tag_files/tag_index.h"
#include "game/game_engine_assault_globals.h"
#include "scenario/scenario.h"
#include "game/game_engine_base_variant.h"
#include "game/game_results_types.h"
#include "game/player_in_game_iterator.h"
#include "text/string_list_hud.h"
#include "cseries/language.h"
#include "effects/effect_creation.h"
// @flags /TP /O2 /Oc
// @address 0x2a30c0 team_in_own_goal_area
// @address 0x2a21e0 ?player_killed_player@c_assault_engine@@UBAXJJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z
// @address 0x2a37f8 assault_initialize_area_set
// @address 0x2a2c28 ?assault_bomb_get_arming_theta@@YAMJ@Z
// @address 0x2a1678 ?allow_weapon_pickup@c_assault_engine@@UBA_NJJ@Z
// @address 0x2a1c70 ?apply_game_engine_traits_for_player@c_assault_engine@@UBAXJPAVc_player_traits@@@Z
// @address 0x2a38a8 ?find_selected_area@?$c_area_set@Vc_area@@$02@@QAAPAVc_area@@XZ
// @address 0x2e0e18 ?game_get_powerup_traits@@YAXW4e_multiplayer_powerup_flavor@@PAVc_player_traits@@@Z
// @address 0x2a2f60 ?assault_find_bomb_placed_post_team_designator@@YA?AW4e_multiplayer_team_designator@@JPAPAVc_area@@@Z

enum e_game_results_statistic;


static __forceinline bool assault_weapon_test_object_flag(const weapon_datum *weapon, long bit)
{
    return (weapon->item.object.object_flags & (1UL << bit)) != 0;
}









static __forceinline long ctf_get_initial_defensive_team_index() {
    long result = -1;
    for (long team = 0; team < 8; ++team) {
        if ((1 << team) & ((s_game_engine_globals *)tls_game_engine_globals())->active_teams) { result = team; break; }
    }
    return result;
}
// @address 0x8c429c g_assault_variant
extern "C" {
  c_game_engine_assault_variant *g_assault_variant;
}
static const float k_one = 1.0f;
static const float k_zero = 0.0f;


// @address 0x2a0ad0 ?get_object_definition_index@c_ctf_engine@@UBAJXZ
long c_ctf_engine::get_object_definition_index() const
{
    long result = -1;
    if (game_engine_running() && global_game_globals != 0 && global_game_globals->multiplayer_globals_index != -1)
    {
        const cache_file_tag_instance *instance = &global_tag_instances[datum_index_to_absolute_index(global_game_globals->multiplayer_globals_index)];
        if (instance->definition != 0)
        {
            const s_multiplayer_globals_definition *definition = (const s_multiplayer_globals_definition *)instance->definition;
            result = ((const s_multiplayer_runtime_globals_definition *)definition->runtime.address)->flag.index;
        }
    }
    return result;
}

// @address 0x41188 ??_7c_assault_engine@@6B@
// @address 0x793a48 ?internal_assault_engine@@3Vc_assault_engine@@A
c_assault_engine internal_assault_engine;

// @address 0x42e810 ?get_type@c_assault_engine@@UBAJXZ
long c_assault_engine::get_type() const
{
    return _game_engine_type_assault;
}

// @address 0x3fc598 ?get_simulation_entity_type@c_assault_engine@@UBA?AW4e_simulation_entity_type@@XZ
e_simulation_entity_type c_assault_engine::get_simulation_entity_type() const
{
    return _simulation_entity_type_assault_engine_globals;
}

// @address 0x5e15a8 ?assemble_spawn_influencers_for_player@c_assault_engine@@UBAXJPAUs_netgame_goal_influencer@@PAJ@Z
void c_assault_engine::assemble_spawn_influencers_for_player(long player_index, s_netgame_goal_influencer *influencers, long *influencer_count) const
{
    c_game_engine::assemble_spawn_influencers_for_player(player_index, influencers, influencer_count);
}

// @address 0x2a0b58 ?get_object_definition_index@c_assault_engine@@UBAJXZ
long c_assault_engine::get_object_definition_index() const
{
    long result = -1;
    if (game_engine_running() && global_game_globals != 0 && global_game_globals->multiplayer_globals_index != -1)
    {
        const cache_file_tag_instance *instance = &global_tag_instances[datum_index_to_absolute_index(global_game_globals->multiplayer_globals_index)];
        if (instance->definition != 0)
        {
            const s_multiplayer_globals_definition *definition = (const s_multiplayer_globals_definition *)instance->definition;
            result = ((const s_multiplayer_runtime_globals_definition *)definition->runtime.address)->bomb.index;
        }
    }
    return result;
}

namespace {
static __forceinline short assault_variant_get_sudden_death_time(const c_game_engine_assault_variant *variant)
{
    return variant->m_sudden_death_time;
}
}

// @address 0x2a17a8 ?get_sudden_death_time@c_assault_engine@@UBAFXZ
short c_assault_engine::get_sudden_death_time() const
{
    return assault_variant_get_sudden_death_time(g_assault_variant);
}

// @address 0x2a1868 ?initialize_object_data@c_assault_engine@@UBAXJ@Z
static __forceinline s_game_engine_globals *assault_object_globals()
{
    return (s_game_engine_globals *)tls_game_engine_globals();
}
void c_assault_engine::initialize_object_data(long object_index) const
{
    assault_object_globals()->bomb_reset_timers[object_index] = -1;
    assault_object_globals()->timeout_return_second_counter[object_index] = 0;
    simulation_action_game_engine_globals_update(0x2000);
    assault_object_globals()->bomb_arming_timers[object_index] = -1;
    simulation_action_game_engine_globals_update(0x1000);
    assault_object_globals()->bomb_weapon_flags[object_index] = 0;
    simulation_action_game_engine_globals_update(0x4000);
}

// @address 0x2a1a88 ?dispose_from_old_map@c_assault_engine@@UBAXXZ
void c_assault_engine::dispose_from_old_map() const
{
    g_assault_variant = 0;
}

static __forceinline player_datum *assault_player_get(long player_index)
{
    return &((player_datum *)player_data->data)[datum_index_to_absolute_index(player_index)];
}
static __forceinline weapon_datum *assault_weapon_get(long object_index)
{
    return (weapon_datum *)datum_get<s_object_header>(tls_object_header_data(), object_index)->datum;
}
static __forceinline short assault_weapon_get_multiplayer_type(long object_index)
{
    const weapon_datum *weapon = assault_weapon_get(object_index);
    const weapon_definition *definition = (const weapon_definition *)tag_get('weap', weapon->item.object.definition_index);
    return definition->multiplayer_weapon_type;
}
static __forceinline bool assault_player_has_bomb(long player_index)
{
    return game_engine_get_carried_multiplayer_weapon_index(player_index, 1) != -1;
}

struct s_game_engine_event_data;

// @address 0x2a0be0 ?emit_game_start_event@c_assault_engine@@UBAXJ@Z
void c_assault_engine::emit_game_start_event(long player_index) const
{
    struct {
        unsigned int f_0;
        unsigned int f_4;
        unsigned int f_8;
        unsigned int f_c;
        unsigned int f_10;
        unsigned int f_14;
        unsigned int f_18;
        unsigned int f_1c;
        unsigned int f_20;
        short f_24;
    } game_start_event_data;
    game_start_event_data.f_0 = 10;
    game_start_event_data.f_4 = 0;
    game_start_event_data.f_8 = -1;
    game_start_event_data.f_10 = -1;
    game_start_event_data.f_14 = -1;
    game_start_event_data.f_18 = -1;
    game_start_event_data.f_c = player_index;
    game_start_event_data.f_1c = -1;
    game_start_event_data.f_20 = 0;
    game_start_event_data.f_24 = -1;
    game_engine_send_event((s_game_engine_event_data *)&game_start_event_data);
}

// @address 0x2a0c38 ?emit_object_returned_event@c_assault_engine@@UBAXJ_N@Z
void c_assault_engine::emit_object_returned_event(long object_index, bool dropped) const
{
    s_game_engine_event_data request;
    unsigned char flag = (assault_object_globals()->assault_variant_flags >> 1) & 1;
    int mode;
    if (flag)
        mode = 17;
    else
        mode = (dropped == 0) + 3;
    request.event = mode;
    request.effect_team_index = object_index;
    request.identifier = -1;
    request.type = 10;
    request.flags = -1;
    request.cause_player_index = -1;
    request.cause_team_index = -1;
    request.effect_player_index = -1;
    request.data = 0;
    request.data_word = -1;
    game_engine_send_event((s_game_engine_event_data *)&request);
}

// @address 0x2a12d8 ?build_player_nav_point@c_assault_engine@@UBA_NJJJ_NPAUs_chud_navpoint@@@Z
bool c_assault_engine::build_player_nav_point(long user_index, long player_index, long target_player_index, bool target, s_chud_navpoint *navpoint) const
{
    bool result = c_game_engine::build_player_nav_point(user_index, player_index, target_player_index, target, navpoint);
    if (result && assault_player_has_bomb(player_index)) {
        navpoint->icon = 5;
        if (navpoint->type == -1)
            navpoint->type = 1;
        navpoint->flags = navpoint->flags | 1;
    }
    return result;
}

bool c_assault_engine::allow_weapon_pickup(long player_index, long object_index) const
{
    bool result = true;
    if (assault_weapon_get_multiplayer_type(object_index) != 1)
        result = c_game_engine::allow_weapon_pickup(player_index, object_index);
    if (result && player_index != -1 && object_index != -1) {
        player_datum *player = assault_player_get(player_index);
        weapon_datum *weapon = assault_weapon_get(object_index);
        long team_index = weapon->multiplayer_weapon_identifier;
        if (assault_weapon_get_multiplayer_type(object_index) == 1 && team_index != -1) {
            if ((assault_object_globals()->bomb_weapon_flags[team_index] & 4) != 0)
                result = false;
            else if (g_assault_variant->m_game_type == 2 && team_index == 8)
                result = true;
            else if (team_index != player->configuration.host.team_index)
                result = false;
        }
    }
    return result;
}

void c_assault_engine::apply_game_engine_traits_for_player(long player_index, c_player_traits *traits) const
{
    if (assault_player_has_bomb(player_index)) {
        long team_index = assault_player_get(player_index)->configuration.host.team_index;
        traits->set(&g_assault_variant->m_carrier_traits, false);
        if ((assault_object_globals()->bomb_weapon_flags[team_index] & 4) == 0 && assault_object_globals()->bomb_arming_timers[team_index] > 0)
            traits->set(&g_assault_variant->m_arming_traits, false);
    }
}

// @address 0x2a2358 ?build_simulation_baseline@c_assault_engine@@UBAXJPAX@Z
void c_assault_engine::build_simulation_baseline(long state_data_size, void *state_data) const {
    s_assault_engine_state_data *data = (s_assault_engine_state_data *)state_data;
    memset(data, 0, sizeof(*data));
    build_global_baseline((s_game_engine_state_data *)data);
    data->defensive_team = ctf_get_initial_defensive_team_index();
    memset(data->bomb_reset_timers, -1, sizeof(data->bomb_reset_timers));
    memset(data->player_that_placed_bombs, -1, sizeof(data->player_that_placed_bombs));
}

// @address 0x2a2430 ?build_simulation_update@c_assault_engine@@UBAXPAKJPAX@Z
void c_assault_engine::build_simulation_update(unsigned long *flags, long state_data_size, void *state_data) const
{
  s_assault_engine_state_data *cache = (s_assault_engine_state_data *)state_data;
  unsigned long changed = 0;
  if ((*flags & 0x3f) != 0)
    build_global_update(*flags & 0x3f, &changed, (s_game_engine_state_data *)state_data);
  if ((*flags & 0x40) != 0) {
    long value = ((s_game_engine_globals *)tls_game_engine_globals())->defensive_team;
    if (cache->defensive_team != value) {
      changed |= 0x40;
      cache->defensive_team = value;
    }
  }
  if ((*flags & 0x2000) != 0) {
    for (unsigned int i = 0; i < 9; i++) {
      short value = ((s_game_engine_globals *)tls_game_engine_globals())->bomb_reset_timers[i];
      if (cache->bomb_reset_timers[i] != value) {
        changed |= 0x2000;
        cache->bomb_reset_timers[i] = value;
      }
    }
  }
  if ((*flags & 0x1000) != 0) {
    short *dst = cache->bomb_arming_timers;
    unsigned int count = 9;
    long *src = ((s_game_engine_globals *)tls_game_engine_globals())->bomb_arming_timers;
    do {
      long value = *src;
      if (*dst != value) {
        changed |= 0x1000;
        *dst = (short)value;
      }
      count--;
      src++;
      dst++;
    } while (count != 0);
  }
  if ((*flags & 0x4000) != 0) {
    for (unsigned int i = 0; i < 9; i++) {
      unsigned char value = ((s_game_engine_globals *)tls_game_engine_globals())->bomb_weapon_flags[i];
      if (cache->bomb_weapon_flags[i] != value) {
        changed |= 0x4000;
        cache->bomb_weapon_flags[i] = value;
      }
    }
  }
  if ((*flags & 0x8000) != 0) {
    for (unsigned int i = 0; i < 9; i++) {
      long handle = ((s_game_engine_globals *)tls_game_engine_globals())->player_that_placed_bombs[i];
      if (handle != -1)
        handle &= 0xffff;
      if ((long)cache->player_that_placed_bombs[i] != handle) {
        changed |= 0x8000;
        cache->player_that_placed_bombs[i] = handle;
      }
    }
  }
  if ((*flags & 0x800) != 0) {
    unsigned short value = ((s_game_engine_globals *)tls_game_engine_globals())->assault_helper_flags;
    if (cache->helper_flags != value) {
      changed |= 0x800;
      cache->helper_flags = value;
    }
  }
  *flags = changed;
}

static __forceinline void *assault_datum_try_and_get_absolute(const s_data_array *data, long absolute_index)
{
    void *result = 0;
    if (absolute_index != -1) {
        if (absolute_index >= 0 && absolute_index < data->first_unallocated) {
            unsigned short *identifier = (unsigned short *)((char *)data->data + data->size * absolute_index);
            if (*identifier != 0)
                result = identifier;
        }
    }
    return result;
}
static __forceinline long assault_datum_absolute_index_to_index(const s_data_array *data, long absolute_index)
{
    long result = -1;
    if (absolute_index != -1)
        result = (*(unsigned short *)((char *)data->data + data->size * absolute_index) << 16) | absolute_index;
    return result;
}
// @address 0x2a26b0 ?apply_simulation_update@c_assault_engine@@UBA_NKJPBX@Z
bool c_assault_engine::apply_simulation_update(unsigned long update_mask, long state_data_size, const void *state_data) const
{
    const s_assault_engine_state_data *state = (const s_assault_engine_state_data *)state_data;
    bool success = true;
    unsigned long base_mask = update_mask & 0x3f;
    if (base_mask)
        success = success && apply_global_update(base_mask, (const s_game_engine_state_data *)state);
    if (update_mask & 0x40) {
        if (assault_object_globals()->defensive_team != state->defensive_team)
            side_switch_ctf_switch_sides((e_game_team)state->defensive_team);
    }
    if (update_mask & 0x2000) {
        for (long i = 0; i < 9; i++)
            assault_object_globals()->bomb_reset_timers[i] = state->bomb_reset_timers[i];
    }
    if (update_mask & 0x1000) {
        for (long i = 0; i < 9; i++)
            assault_object_globals()->bomb_arming_timers[i] = state->bomb_arming_timers[i];
    }
    if (update_mask & 0x4000) {
        for (long i = 0; i < 9; i++)
            assault_object_globals()->bomb_weapon_flags[i] = state->bomb_weapon_flags[i];
    }
    if (update_mask & 0x8000) {
        for (long i = 0; i < 9; i++) {
            void *player = assault_datum_try_and_get_absolute(player_data, state->player_that_placed_bombs[i]);
            long player_index = -1;
            if (player)
                player_index = assault_datum_absolute_index_to_index(player_data, state->player_that_placed_bombs[i]);
            assault_object_globals()->player_that_placed_bombs[i] = player_index;
        }
    }
    if (update_mask & 0x800)
        assault_object_globals()->assault_helper_flags = state->helper_flags;
    return success;
}

float assault_bomb_get_arming_theta(long unit_index)
{
  float divisor = (float)g_assault_variant->m_bomb_arming_time;
  long player_index = ((weapon_datum *)datum_get<s_object_header>(tls_object_header_data(), unit_index)->datum)->multiplayer_weapon_identifier;
  float result = k_zero;
  if (!(divisor > k_one))
    divisor = k_one;
  if (player_index != -1) {
    s_game_engine_globals *game = (s_game_engine_globals *)tls_game_engine_globals();
    if (game->bomb_weapon_flags[player_index] & 4)
      result = k_one;
    else {
      long count = game->bomb_arming_timers[player_index] > 0 ? game->bomb_arming_timers[player_index] : 0;
      result = tls_game_time_globals()->tick_length / divisor * (float)count;
    }
  }
  return result;
}

static __forceinline s_game_engine_globals *assault_shared_globals()
{
    return (s_game_engine_globals *)tls_game_engine_globals();
}
static __forceinline bool assault_neutral_flag()
{
    return (assault_shared_globals()->assault_variant_flags >> 1) & 1;
}
static __forceinline bool assault_side_switch()
{
    return assault_shared_globals()->assault_variant_flags & 1;
}
// @address 0x2a2258 ?object_should_exist@c_assault_engine@@UBA_NJ@Z
bool c_assault_engine::object_should_exist(long team) const
{
    bool result = false;
    if (team >= 0 && team < 8) {
        if (!assault_neutral_flag() && ::game_engine_is_team_active((e_game_team)team)) {
            if (assault_side_switch()) {
                result = (assault_shared_globals()->defensive_team - team) != 0;
            } else {
                result = true;
            }
        }
    } else if (team == 8) {
        result = assault_neutral_flag();
    }
    return result;
}

// @address 0x2a2180 ?player_killed_player_get_kill_message@c_assault_engine@@UBAJJJ_N@Z
long c_assault_engine::player_killed_player_get_kill_message(long killer_player_index, long killed_player_index, bool is_betrayal) const
{
    int result = -1;
    if (killer_player_index != -1 && killed_player_index != -1 && !is_betrayal &&
        game_engine_get_carried_multiplayer_weapon_index(killed_player_index, 1) != -1) {
        result = 0x29;
    }
    return result;
}

// @address 0x2a21e0 ?player_killed_player@c_assault_engine@@UBAXJJJ_NV?$c_flags@W4e_game_engine_kill_flags@@J$0BI@@@J@Z
void c_assault_engine::player_killed_player(long killer_player_index, long object_index, long victim_player_index, bool betrayal,
    c_flags<e_game_engine_kill_flags, long, 24> kill_flags, long damage) const
{
    c_ctf_engine::player_killed_player(killer_player_index, object_index, victim_player_index, betrayal, kill_flags, damage);
    if (killer_player_index != -1 && victim_player_index != -1 && !betrayal &&
        game_engine_get_carried_multiplayer_weapon_index(victim_player_index, 1) != -1) {
        int time_message_data = 0;
        ::game_engine_handle_time_messages(47, (unsigned short)killer_player_index, killer_player_index, victim_player_index, (int *)(&time_message_data));
    }
}

extern "C" inline c_area_set<c_area, 3> *assault_initialize_area_set(c_area_set<c_area, 3> *area_set)
{
    area_set->c_area_set<c_area, 3>::c_area_set();
    return area_set;
}

struct s_multiplayer_object_boundary_geometry_data;

template <typename t_area_type, long k_area_count>
t_area_type *c_area_set<t_area_type, k_area_count>::find_selected_area()
{
    t_area_type *result = 0;
    for (t_area_type *area = begin(); area != end(); area++) {
        if (area->is_selected()) {
            result = area;
            break;
        }
    }
    return result;
}
template c_area *c_area_set<c_area, 3>::find_selected_area();

template <typename t_area_type, long k_area_count>
bool c_area_set<t_area_type, k_area_count>::player_in_set(long player_index, t_area_type **area_out)
{
    bool result = false;
    if (game_engine_in_round()) {
        t_area_type *selected_area = find_selected_area();
        if (selected_area != 0) {
            result = player_in_multiplayer_object_boundary(player_index, (const s_multiplayer_object_boundary_geometry_data *)selected_area);
        } else {
            for (long index = 0; index < m_area_count && !result; index++) {
                if (player_in_multiplayer_object_boundary(player_index, (const s_multiplayer_object_boundary_geometry_data *)&m_areas[index])) {
                    result = true;
                    if (area_out != 0)
                        *area_out = &m_areas[index];
                }
            }
        }
    }
    return result;
}
template bool c_area_set<c_area, 3>::player_in_set(long, c_area **);

e_multiplayer_team_designator assault_find_bomb_placed_post_team_designator(long object_index, c_area **placed_area)
{
    e_multiplayer_team_designator result = _multiplayer_team_designator_invalid;
    short team_index = assault_weapon_get(object_index)->multiplayer_weapon_identifier;
    if (team_index != -1 && (assault_object_globals()->bomb_weapon_flags[team_index] & 4) != 0) {
        real_point3d point;
        object_get_origin(object_index, &point);
        c_area_set<c_area, 3> *team_areas = assault_object_globals()->assault_team_areas();
        for (long team = 0; team < 9; team++, team_areas++) {
            if (!team_areas->exist())
                continue;
            if (!team_areas->point_in_set(&point, placed_area))
                continue;
            result = (e_multiplayer_team_designator)team;
            break;
        }
    }
    return result;
}

static __forceinline bool assault_game_is_predicted()
{
    bool result = false;
    if (tls_game_globals()->options.simulation == 4)
        result = true;
    return result;
}
static __forceinline void assault_event_initialize(long type, long index, s_game_engine_event_data *event)
{
    event->type = type;
    event->event = index;
    event->identifier = -1;
    event->flags = -1;
    event->cause_player_index = -1;
    event->cause_team_index = -1;
    event->effect_player_index = -1;
    event->effect_team_index = -1;
    event->data = 0;
    event->data_word = -1;
}
static __forceinline void assault_event_set_cause_player_and_team(long player_index, s_game_engine_event_data *event)
{
    event->cause_player_index = player_index;
    event->cause_team_index = assault_player_get(player_index)->configuration.host.team_index;
}

// @address 0x2a1360 ?multiplayer_weapon_picked_up@c_assault_engine@@UBAXJJ@Z
void c_assault_engine::multiplayer_weapon_picked_up(long weapon_index, long unit_index) const
{
    if (!assault_game_is_predicted() && assault_weapon_get_multiplayer_type(weapon_index) == 1) {
        weapon_datum *weapon = assault_weapon_get(weapon_index);
        unit_datum *unit = (unit_datum *)assault_weapon_get(unit_index);
        long team = weapon->multiplayer_weapon_identifier;
        player_datum *player = assault_player_get(unit->unit.player_index);
        if (team != -1) {
            assault_object_globals()->bomb_weapon_flags[team] |= 1;
            assault_object_globals()->bomb_weapon_flags[team] &= ~4;
            simulation_action_game_engine_globals_update(0x4000);
            if ((unsigned long)team <= 8) {
                assault_object_globals()->bomb_reset_timers[team] = -1;
                assault_object_globals()->timeout_return_second_counter[team] = 0;
                simulation_action_game_engine_globals_update(0x2000);
            }
            game_results_statistic_increment(
                unit->unit.player_index & 0xffff, (e_game_team)player->configuration.host.team_index, _game_results_statistic_assault_bomb_grabs, 1);
            if (unit->unit.player_index != -1 && (unsigned long)team <= 8) {
                s_game_engine_event_data event;
                assault_event_initialize(10, 1, &event);
                assault_event_set_cause_player_and_team(unit->unit.player_index, &event);
                event.effect_team_index = team;
                game_engine_send_event((s_game_engine_event_data *)&event);
            }
        }
    }
}

// @address 0x2a1508 ?multiplayer_weapon_dropped@c_assault_engine@@UBAXJJ@Z
void c_assault_engine::multiplayer_weapon_dropped(long weapon_index, long unit_index) const
{
    if (!assault_game_is_predicted() && assault_weapon_get_multiplayer_type(weapon_index) == 1) {
        weapon_datum *weapon = assault_weapon_get(weapon_index);
        long team = (short)(unsigned short)weapon->multiplayer_weapon_identifier;
        if ((unsigned long)team <= 8) {
            unit_datum *unit = (unit_datum *)assault_weapon_get(unit_index);
            if ((assault_object_globals()->bomb_weapon_flags[team] & 1) != 0) {
                assault_object_globals()->bomb_reset_timers[team] = -1;
                simulation_action_game_engine_globals_update(0x2000);
                assault_object_globals()->timeout_return_second_counter[team] = 0;
            }
            if (unit->unit.player_index != -1 && (assault_object_globals()->assault_helper_flags & 1) == 0) {
                s_game_engine_event_data event;
                assault_event_initialize(10, 2, &event);
                assault_event_set_cause_player_and_team(unit->unit.player_index, &event);
                event.effect_team_index = team;
                game_engine_send_event((s_game_engine_event_data *)&event);
            }
        }
    }
}

// @address 0x2a18e8 ?initialize_for_new_map@c_assault_engine@@UBA_NXZ
bool c_assault_engine::initialize_for_new_map() const
{
    game_options *options = &tls_game_globals()->options;
    const c_game_variant *variant = options->game_mode == 2 ? &options->game_variant : 0;
    variant->get_active_variant()->can_be_cast_to(_game_engine_assault, (const void **)&g_assault_variant);

    s_game_engine_globals *globals = assault_object_globals();
    c_area_set<c_area, 3> *team_areas = globals->assault_team_areas();
    if (team_areas != 0)
        game_engine_initialize_team_area_sets(team_areas);
    // Zero the area sets and per-bomb state through timeout_return_second_counter (size is 4-aligned).
    memset(team_areas, 0, offsetof(s_game_engine_globals, timeout_return_second_counter) + sizeof(globals->timeout_return_second_counter) + 2
        - offsetof(s_game_engine_globals, variant_globals));
    for (long i = 0; i < 9; i++)
        globals->bomb_reset_timers[i] = -1;
    for (long i = 0; i < 9; i++)
        globals->bomb_arming_timers[i] = -1;
    for (long i = 0; i < 9; i++)
        globals->player_that_placed_bombs[i] = -1;

    short game_type = g_assault_variant->m_game_type;
    switch (game_type) {
    case 1:
        globals->assault_variant_flags |= 1;
        break;
    case 2:
        globals->assault_variant_flags |= 2;
        break;
    }
    globals->assault_bomb_exploded = 0;
    return true;
}

static __forceinline void game_engine_assault_initialize_area_array(c_area_set<c_area, 3> *area_sets)
{
    long count = 9;
    while (--count >= 0) {
        assault_initialize_area_set(area_sets);
        ++area_sets;
    }
}

// @address 0x2a1a28 game_engine_initialize_team_area_sets
extern "C" c_area_set<c_area, 3> *game_engine_initialize_team_area_sets(c_area_set<c_area, 3> *area_sets)
{
    game_engine_assault_initialize_area_array(area_sets);
    game_engine_assault_initialize_area_array(area_sets + 9);
    return area_sets;
}

static __forceinline bool game_powerups_game_is_multiplayer()
{
    return tls_game_globals()->options.game_mode == 2;
}
static __forceinline const c_game_variant *game_powerups_current_game_variant()
{
    game_options *options = &tls_game_globals()->options;
    return options->game_mode == 2 ? &options->game_variant : 0;
}

static __forceinline void game_powerups_player_traits_clear(c_player_traits *traits)
{
    traits->m_shield_vitality_traits.m_damage_resistance = _damage_resistance_percentage_setting_unchanged;
    traits->m_shield_vitality_traits.m_shield_recharge_rate = _shield_recharge_rate_percentage_setting_unchanged;
    traits->m_shield_vitality_traits.m_vampirism = _vampirism_percentage_setting_unchanged;
    traits->m_shield_vitality_traits.m_headshot_immunity = _headshot_immunity_setting_unchanged;
    traits->m_shield_vitality_traits.m_shield_multiplier = _shield_multiplier_setting_unchanged;
    memset(traits->m_shield_vitality_traits.m_padding, 0, sizeof(traits->m_shield_vitality_traits.m_padding));
    traits->m_weapon_traits.m_damage_modifier = _damage_modifier_percentage_setting_unchanged;
    traits->m_weapon_traits.m_initial_primary_weapon = -2;
    traits->m_weapon_traits.m_initial_secondary_weapon = -2;
    traits->m_weapon_traits.m_grenade_count = _grenade_count_setting_unchanged;
    traits->m_weapon_traits.m_recharging_grenades = _recharging_grenades_setting_unchanged;
    traits->m_weapon_traits.m_infinite_ammo = _infinite_ammo_setting_unchanged;
    traits->m_weapon_traits.m_weapon_pickup = _weapon_pickup_setting_unchanged;
    traits->m_movement_traits.m_speed = _player_speed_setting_unchanged;
    traits->m_movement_traits.m_gravity = _player_gravity_setting_unchanged;
    traits->m_movement_traits.m_vehicle_usage = _vehicle_usage_setting_unchanged;
    traits->m_movement_traits.m_padding = 0;
    traits->m_appearance_traits.m_active_camo = _active_camo_setting_unchanged;
    traits->m_appearance_traits.m_waypoint = _waypoint_setting_unchanged;
    traits->m_appearance_traits.m_aura = _aura_setting_unchanged;
    traits->m_appearance_traits.m_forced_change_color = _forced_change_color_setting_unchanged;
    traits->m_sensor_traits.m_motion_tracker = _motion_tracker_setting_unchanged;
    traits->m_sensor_traits.m_motion_tracker_range = _motion_tracker_range_setting_unchanged;
}

void game_get_powerup_traits(e_multiplayer_powerup_flavor flavor, c_player_traits *powerup_traits)
{
    game_powerups_player_traits_clear(powerup_traits);
    if (game_powerups_game_is_multiplayer() && game_engine_running()) {
        const c_game_variant *game_variant = game_powerups_current_game_variant();
        const c_game_engine_base_variant *active_variant = game_variant->get_active_variant();
        switch (flavor) {
        case _multiplayer_powerup_flavor_red:
            powerup_traits->set(active_variant->get_map_override_options()->get_red_powerup_traits(), false);
            break;
        case _multiplayer_powerup_flavor_blue:
            powerup_traits->set(active_variant->get_map_override_options()->get_blue_powerup_traits(), false);
            break;
        case _multiplayer_powerup_flavor_yellow:
            powerup_traits->set(active_variant->get_map_override_options()->get_yellow_powerup_traits(), false);
            break;
        }
    }
}

// the player iterator and player view as accepted in players.cpp
static __forceinline long assault_object_get_ultimate_parent(long object_index)
{
    long result = -1;
    while (object_index != -1) {
        result = object_index;
        object_index = assault_weapon_get(object_index)->item.object.parent_object_index;
    }
    return result;
}
bool team_in_own_goal_area(const s_multiplayer_object_boundary_geometry_data *boundary, e_multiplayer_team_designator team, long *out)
{
    bool result = false;
    c_player_in_game_iterator players;
    players.begin();
    while (players.next()) {
        player_datum *player = (player_datum *)players.get_datum();
        if (game_engine_team_index_to_team_designator((e_game_team)player->configuration.host.team_index) == team) {
            long player_index = players.get_index();
            player_datum *current = (player_datum *)assault_player_get(player_index);
            if (boundary != 0 && player_in_multiplayer_object_boundary(player_index, boundary) && current->unit_index != -1
                && assault_object_get_ultimate_parent(current->unit_index) == current->unit_index) {
                result = true;
                *out = player_index;
                break;
            }
        }
    }
    return result;
}

static __forceinline const s_multiplayer_constants *assault_get_constants()
{
    const s_multiplayer_globals_definition *definition =
        (const s_multiplayer_globals_definition *)global_tag_instances[datum_index_to_absolute_index(global_game_globals->multiplayer_globals_index)]
            .definition;
    return (const s_multiplayer_constants *)((const s_multiplayer_runtime_globals_definition *)definition->runtime.address)->multiplayer_constants.address;
}

// @address 0x2a0cd0 ?is_team_near_object@c_assault_engine@@UBA_NJM@Z
bool c_assault_engine::is_team_near_object(long object_index, float distance) const
{
    float range = distance;
    long target = object_index;
    bool include_all = true;
    bool check_all = false;
    bool result;
    bool near = false;
    if (team_near_object(target, range, include_all))
        near = true;
    long owner = -1;
    s_game_engine_globals *globals = assault_shared_globals();
    unsigned char assault_only = (globals->assault_variant_flags >> 1) & 1;
    if (assault_only && team_near_object(target, range, check_all))
        near = true;
    owner = assault_weapon_get(object_index)->multiplayer_weapon_identifier;
    if (owner == -1) {
        result = near;
    } else {
        if ((globals->bomb_weapon_flags[owner] & 4) != 0)
            return true;
        result = near;
    }
    return result;
}

static __forceinline short assault_variant_get_respawn(const c_game_engine_assault_variant *variant)
{
    return variant->m_respawn;
}
static __forceinline void assault_adjust_player_results_statistic(long player_index, bool include_team, e_game_results_statistic statistic, long amount)
{
    long absolute_player_index = player_index & 0xffff;
    e_game_team team = include_team ? (e_game_team)assault_player_get(player_index)->configuration.host.team_index : _game_team_none;
    game_results_statistic_increment(absolute_player_index, team, statistic, amount);
}

// @address 0x2a34a0 assault_reset_bomb
extern "C" void assault_reset_bomb(long bomb_index)
{
    const s_multiplayer_constants *constants = assault_get_constants();
    const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)bomb_index);
    if (tracker)
    {
        long weapon_index = tracker->weapon_index;
        if (weapon_index != -1)
        {
            object_detach(weapon_index);
            real_point3d origin;
            object_get_origin(weapon_index, &origin);
            origin.z += 0.1f;
            object_set_position_direct(weapon_index, &origin, 0, 0, 0, false);
            simulation_action_object_update(weapon_index, 0x802);
            assault_trigger_bomb_effect(weapon_index, constants->bomb_reset_effect.index, -1, -1, 1);
        }
    }
}

// @address 0x2a3578 ?explode_bomb@@YAXJ@Z
void explode_bomb(long bomb_index)
{
    const s_multiplayer_constants *constants = assault_get_constants();
    long weapon_index = -1;
    const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)bomb_index);
    if (tracker)
        weapon_index = tracker->weapon_index;
    if (bomb_index != -1 && weapon_index != -1)
    {
        e_multiplayer_team_designator designator = assault_find_bomb_placed_post_team_designator(weapon_index, 0);
        if (designator != 9)
        {
            long team = ((s_game_engine_globals *)tls_game_engine_globals())->team_mapping[designator];
            long planter_index = assault_object_globals()->player_that_placed_bombs[bomb_index];
            const player_datum *planter = (const player_datum *)datum_try_and_get(player_data, planter_index);
            assault_trigger_bomb_effect(weapon_index, constants->bomb_explosion_effect.index,
        constants->bomb_explosion_sound.index, constants->bomb_explosion_damage.index, 0);
            if (team != -1 && planter_index != -1 && datum_try_and_get(player_data, planter_index))
            {
                if (game_engine_adjust_player_score(planter_index, 1))
                    assault_adjust_player_results_statistic(planter_index, true, _game_results_statistic_assault_bomb_detonations, 1);
                s_game_engine_event_data event_data;
                assault_event_initialize(10, 5, &event_data);
                assault_event_set_cause_player_and_team(planter_index, &event_data);
                event_data.effect_team_index = team;
                game_engine_send_event((s_game_engine_event_data *)&event_data);
                assault_object_globals()->player_that_placed_bombs[bomb_index] = -1;
                simulation_action_game_engine_globals_update(0x8000);
                if (assault_variant_get_respawn(g_assault_variant) != 0)
                {
                    c_player_in_game_iterator players;
                    players.begin();
                    while (players.next())
                    {
                        const player_datum *player = (const player_datum *)players.get_datum();
                        if (!(player->flags & 2) && (player->flags & 1) && player->unit_index == -1)
                        {
                            bool respawn = false;
                            switch (assault_variant_get_respawn(g_assault_variant))
                            {
                            case 1:
                                if (player->configuration.host.team_index == planter->configuration.host.team_index)
                                    respawn = true;
                                break;
                            case 2:
                                if (player->configuration.host.team_index != planter->configuration.host.team_index)
                                    respawn = true;
                                break;
                            case 3:
                                respawn = true;
                                break;
                            }
                            if (respawn)
                                player_force_spawn(players.get_index());
                        }
                    }
                }
            }
        }
    }
}

// @address 0x2a17b8 ?verify_state@c_assault_engine@@UBA_NXZ
bool c_assault_engine::verify_state() const
{
    bool result = ensure_all_flags_exist();
    for (long team_index = 0; team_index < 8; team_index++) {
        const s_multiplayer_weapon_tracker *weapon = game_engine_multiplayer_weapon_get((short)team_index);
        if (weapon && weapon->weapon_index != -1 && weapon->carrier_player_index != -1 && weapon->carrier_unit_index != -1 &&
            assault_player_get(weapon->carrier_player_index)->configuration.host.team_index != team_index)
            action_submit_event(weapon->carrier_unit_index, 46);
    }
    return result;
}

// @address 0x2a1a98 ?process_player_holding_object@c_assault_engine@@UBAXJJ@Z
void c_assault_engine::process_player_holding_object(long player_index, long object_index) const
{
    c_area *area;
    if (player_in_enemy_area(player_index, &area)) {
        if (!(assault_shared_globals()->bomb_weapon_flags[object_index] & 4)) {
            if (assault_shared_globals()->bomb_arming_timers[object_index] == -1) {
                s_game_engine_event_data event_data;
                assault_event_initialize(10, 14, &event_data);
                assault_event_set_cause_player_and_team(player_index, &event_data);
                game_engine_send_event((s_game_engine_event_data *)&event_data);
            }
            long arming_ticks = g_assault_variant->m_bomb_arming_time * tls_game_time_globals()->ticks_per_second;
            assault_shared_globals()->bomb_arming_timers[object_index]++;
            simulation_action_game_engine_globals_update(0x1000);
            if (assault_shared_globals()->bomb_arming_timers[object_index] >= arming_ticks) {
                assault_shared_globals()->bomb_weapon_flags[object_index] |= 4;
                assault_shared_globals()->bomb_arming_timers[object_index] = g_assault_variant->m_bomb_disarming_time *
                    tls_game_time_globals()->ticks_per_second;
                s_game_engine_event_data event_data2;
                assault_event_initialize(10, 15, &event_data2);
                assault_event_set_cause_player_and_team(player_index, &event_data2);
                game_engine_send_event((s_game_engine_event_data *)&event_data2);
                simulation_action_game_engine_globals_update(0x4000);
            }
        } else {
            const s_multiplayer_weapon_tracker *weapon = game_engine_multiplayer_weapon_get((short)object_index);
            if (weapon)
                bomb_place_on_enemy_post(weapon->weapon_index, player_index, area);
        }
    }
}

static __forceinline s_multiplayer_object_properties *assault_object_try_and_get_multiplayer(long object_index)
{
    object_datum *object = (object_datum *)object_try_and_get_and_verify_type(object_index, 0xffffffff);
    if (object && object->multiplayer_data_offset != -1)
        return (s_multiplayer_object_properties *)((char *)object + object->multiplayer_data_offset);
    return 0;
}

void bomb_place_on_enemy_post(long weapon_index, long player_index, c_area *area)
{
    short team = assault_weapon_get(weapon_index)->multiplayer_weapon_identifier;
    if (team != -1)
    {
        long absolute_player_index = player_index & 0xffff;
        game_results_statistic_increment(absolute_player_index,
            (e_game_team)assault_player_get(player_index)->configuration.host.team_index, _game_results_statistic_assault_bomb_arms, 1);
        int time_message_data = 0;
        ::game_engine_handle_time_messages(46, absolute_player_index, player_index, -1, (int *)(&time_message_data));
        {
            real_point3d center; area->get_center(&center);
            assault_shared_globals()->assault_helper_flags |= 1;
            game_engine_multiplayer_weapon_reset(weapon_index, &center);
        }
        assault_shared_globals()->assault_helper_flags &= ~1;
        object_attach_to_marker_with_type(area->get_object_index(), 0x40004, weapon_index, 0x40004);
        simulation_action_object_update(weapon_index, 0x800);
        assault_shared_globals()->player_that_placed_bombs[team] = player_index;
        simulation_action_game_engine_globals_update(0x8000);
        assault_shared_globals()->bomb_arming_timers[team] = tls_game_time_globals()->ticks_per_second * g_assault_variant->m_bomb_disarming_time;
        simulation_action_game_engine_globals_update(0x1000);
        assault_shared_globals()->bomb_reset_timers[team] = -1;
        simulation_action_game_engine_globals_update(0x2000);
        assault_shared_globals()->timeout_return_second_counter[team] = -1;
        simulation_action_game_engine_globals_update(0x2000);
        {
            s_game_engine_event_data event_data;
            assault_event_initialize(10, 13, &event_data);
            assault_event_set_cause_player_and_team(player_index, &event_data);
            s_multiplayer_object_properties *properties = assault_object_try_and_get_multiplayer(area->get_object_index());
            event_data.effect_team_index = ((s_game_engine_globals *)tls_game_engine_globals())->team_mapping[properties->team_designator];
            game_engine_send_event((s_game_engine_event_data *)&event_data);
        }
        s_damage_owner owner;
        memset(&owner, 0, sizeof(owner));
        owner.object_index = -1;
        owner.player_index = -1;
        owner.team = -1;
        object_set_damage_owner(weapon_index, &owner, true);
    }
}

enum e_game_results_event_carry_type;

// @address 0x2a1d20 ?update_object@c_assault_engine@@UBA_NPBUs_multiplayer_weapon_tracker@@@Z
bool c_assault_engine::update_object(const s_multiplayer_weapon_tracker *object_tracker) const
{
    bool result = false;
    weapon_datum *object = assault_weapon_get(object_tracker->weapon_index);
    long type = object->multiplayer_weapon_identifier;
    long variant_value = g_assault_variant->m_bomb_reset_time;
    if ((unsigned int)type <= 8)
    {
        bool has_carrier = object_tracker->carrier_player_index + 1 != 0;
        bool flag_bit0_clear = (assault_shared_globals()->bomb_weapon_flags[type] & 1) == 0;
        bool armed = (assault_shared_globals()->bomb_weapon_flags[type] & 4) != 0;
        if (!assault_game_is_predicted())
        {
            if (has_carrier)
                game_results_insert_carry_event(object_tracker->carrier_player_index, _game_results_event_carry_type_assault, object_tracker->id);
            if (!armed)
            {
                if (has_carrier || flag_bit0_clear)
                {
                    if (assault_shared_globals()->bomb_reset_timers[type] != -1)
                    {
                        assault_shared_globals()->timeout_return_second_counter[type] = -1;
                        assault_shared_globals()->bomb_reset_timers[type] = -1;
                        simulation_action_game_engine_globals_update(0x2000);
                    }
                }
                if (!has_carrier && !flag_bit0_clear)
                {
                    if (variant_value > 0)
                    {
                        assault_shared_globals()->timeout_return_second_counter[type]++;
                        if (assault_shared_globals()->timeout_return_second_counter[type] >= tls_game_time_globals()->ticks_per_second)
                        {
                            assault_shared_globals()->bomb_reset_timers[type]++;
                            simulation_action_game_engine_globals_update(0x2000);
                            assault_shared_globals()->timeout_return_second_counter[type] = -1;
                            if (assault_shared_globals()->bomb_reset_timers[type] >= variant_value)
                                result = true;
                        }
                    }
                    if (result && !assault_weapon_test_object_flag(object, 8) && object->item.object.parent_node_index == -1)
                        result = false;
                }
                if ((has_carrier && !player_in_enemy_area(object_tracker->carrier_player_index, 0)) || !has_carrier)
                {
                    if (assault_shared_globals()->bomb_arming_timers[type] > 0)
                    {
                        assault_shared_globals()->bomb_arming_timers[type]--;
                        simulation_action_game_engine_globals_update(0x1000);
                    }
                }
            }
            else
            {
                short fuse_time = g_assault_variant->m_bomb_fuse_time;
                assault_shared_globals()->timeout_return_second_counter[type]++;
                if (assault_shared_globals()->timeout_return_second_counter[type] >= tls_game_time_globals()->ticks_per_second)
                {
                    assault_shared_globals()->bomb_reset_timers[type]++;
                    simulation_action_game_engine_globals_update(0x2000);
                    assault_shared_globals()->timeout_return_second_counter[type] = -1;
                    if (assault_shared_globals()->bomb_reset_timers[type] >= fuse_time)
                    {
                        result = true;
                        explode_bomb(type);
                        assault_shared_globals()->bomb_weapon_flags[type] &= 0xFB;
                        simulation_action_game_engine_globals_update(0x4000);
                        assault_shared_globals()->assault_bomb_exploded = 1;
                    }
                }
                c_area *area = 0;
                e_multiplayer_team_designator team = assault_find_bomb_placed_post_team_designator(object_tracker->weapon_index, &area);
                long player_out = 0;
                if (team_in_own_goal_area((const s_multiplayer_object_boundary_geometry_data *)area, team, &player_out) != 0)
                {
                    if (assault_shared_globals()->bomb_arming_timers[type] > 0)
                    {
                        assault_shared_globals()->bomb_arming_timers[type]--;
                        simulation_action_game_engine_globals_update(0x1000);
                    }
                    else
                    {
                        assault_shared_globals()->bomb_weapon_flags[type] &= 0xFB;
                        simulation_action_game_engine_globals_update(0x4000);
                        player_datum *disarmer = assault_player_get(player_out);
                        if (disarmer != 0)
                            game_results_statistic_increment(player_out & 0xffff, (e_game_team)disarmer->configuration.host.team_index,
                                _game_results_statistic_assault_bomb_disarms, 1);
                        s_game_engine_event_data event;
                        assault_event_initialize(10, 12, &event);
                        assault_event_set_cause_player_and_team(player_out, &event);
                        game_engine_send_event((s_game_engine_event_data *)&event);
                        assault_reset_bomb(type);
                        if (g_assault_variant->m_assault_flags.test(0))
                            result = true;
                    }
                }
                else
                {
                    if (assault_shared_globals()->bomb_arming_timers[type] <
                    tls_game_time_globals()->ticks_per_second * g_assault_variant->m_bomb_disarming_time)
                    {
                        assault_shared_globals()->bomb_arming_timers[type]++;
                        simulation_action_game_engine_globals_update(0x1000);
                    }
                }
            }
        }
    }
    return result;
}


static __forceinline long assault_navpoint_player_from_user(long user_index)
{
    long player_index = NONE;
    if (user_index != NONE)
        player_index = tls_player_mapping_globals()->player_indices[user_index];
    return player_index;
}
static __forceinline void assault_chud_submit_navpoint(long user_index, s_chud_navpoint *navpoint)
{
    if ((unsigned long)user_index <= 3)
        c_chud_manager::x_persistent_user_data[user_index].m_navpoint_manager.submit_navpoint(navpoint);
}

static __forceinline bool assault_weapon_tracker_dropped(const s_multiplayer_weapon_tracker *tracker)
{
    return tracker->carrier_player_index == NONE;
}
static __forceinline bool assault_bomb_armed(const s_game_engine_globals *globals, long bomb_index)
{
    return (globals->bomb_weapon_flags[bomb_index] & 4) != 0;
}

// @address 0x2a0da0 ?submit_nav_points@c_assault_engine@@UBAXJJ@Z
void c_assault_engine::submit_nav_points(long user_index, long target_player_index) const
{
    c_game_engine::submit_nav_points(user_index, target_player_index);
    if (game_engine_in_round())
    {
        long player_index = assault_navpoint_player_from_user(user_index);
        player_datum *player = assault_player_get(player_index);
        bool has_offensive_bomb = false;
        bool has_defensive_bomb = false;
        s_game_engine_globals *initial_globals = assault_shared_globals();
        bool defending_player = assault_side_switch() && initial_globals->defensive_team == player->configuration.host.team_index;
        if (game_engine_ctf_team_has_flag_carrier(player->configuration.host.team_index))
        {
            if ((initial_globals->assault_variant_flags & 1) != 0)
                has_defensive_bomb = true;
            else
                has_offensive_bomb = true;
        }
        for (long team_index = 0; team_index < 8; ++team_index)
        {
            if (game_engine_is_team_active((e_game_team)team_index))
            {
                const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)team_index);
                s_game_engine_globals *globals = assault_shared_globals();
                bool same_team = team_index == player->configuration.host.team_index;
                bool defending_team = team_index == globals->defensive_team;
                if ((!same_team && has_offensive_bomb)
                    || (!same_team && has_defensive_bomb && defending_team)
                    || (same_team && defending_player))
                {
                    e_multiplayer_team_designator designator = game_engine_team_index_to_team_designator((e_game_team)team_index);
                    if (designator != (e_multiplayer_team_designator)NONE)
                    {
                        c_area_set<c_area, 3> *areas = &globals->assault_team_areas()[designator];
                        for (c_area *area = areas->begin(); area != areas->end(); ++area)
                        {
                            real_point3d center; area->get_center(&center);
                            if (same_team)
                            {
                                bool bomb_placed = false;
                                for (long bomb_index = 0; bomb_index < 8; ++bomb_index)
                                {
                                    const s_multiplayer_weapon_tracker *bomb = game_engine_multiplayer_weapon_get((short)bomb_index);
                                    if (bomb != 0 && assault_bomb_armed(globals, bomb->id))
                                    {
                                        real_point3d position;
                                        object_get_origin(bomb->weapon_index, &position);
                                        if (point_in_multiplayer_object_boundary(&position, (const s_multiplayer_object_boundary_geometry_data *)area))
                                        {
                                            bomb_placed = true;
                                            break;
                                        }
                                    }
                                }
                                if (bomb_placed)
                                    continue;
                            }
                            {
                                s_chud_navpoint navpoint;
                                navpoint.object_index = area->get_object_index();
                                navpoint.flags = 193;
                                navpoint.type = 2;
                                navpoint.icon = NONE;
                                navpoint.state = !assault_player_has_bomb(player_index);
                                navpoint.scale = 1.0f;
                                navpoint.name[0] = 0;
                                navpoint.position = center;
                                assault_chud_submit_navpoint(user_index, &navpoint);
                            }
                        }
                    }
                }
                if (tracker != 0 && assault_weapon_tracker_dropped(tracker))
                {
                    s_chud_navpoint navpoint;
                    bool enemy_team = team_index != player->configuration.host.team_index;
                    if (ctf_engine_build_flag_nav_point(&navpoint, tracker->weapon_index, false, enemy_team, false))
                    {
                        if (assault_bomb_armed(globals, team_index))
                        {
                            c_static_wchar_string<256> label;
                            label.print(L"%d", g_assault_variant->m_bomb_fuse_time - globals->bomb_reset_timers[team_index]);
                            ustrnzcpy(navpoint.name, label.get_string(), 4);
                        }
                        assault_chud_submit_navpoint(user_index, &navpoint);
                    }
                }
            }
        }
        s_game_engine_globals *globals = assault_shared_globals();
        if (assault_neutral_flag())
        {
            const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get(8);
            if (tracker != 0 && assault_weapon_tracker_dropped(tracker))
            {
                s_chud_navpoint navpoint;
                if (ctf_engine_build_flag_nav_point(&navpoint, tracker->weapon_index, false, false, false))
                {
                    navpoint.state = 1;
                    e_multiplayer_team_designator placed_team = assault_find_bomb_placed_post_team_designator(tracker->weapon_index, 0);
                    if (placed_team != _multiplayer_team_designator_invalid)
                    {
                        long team = ((s_game_engine_globals *)globals)->team_mapping[placed_team];
                        navpoint.state = player->configuration.host.team_index == team ? 2 : 0;
                    }
                    if (assault_bomb_armed(globals, 8))
                    {
                        c_static_wchar_string<256> label;
                        label.print(L"%d", g_assault_variant->m_bomb_fuse_time - globals->bomb_reset_timers[8]);
                        ustrnzcpy(navpoint.name, label.get_string(), 4);
                    }
                    assault_chud_submit_navpoint(user_index, &navpoint);
                }
            }
        }
    }
}

static __forceinline unsigned long assault_interface_color_to_pixel32(const real_rgb_color *color)
{
    unsigned long red = (long)(color->red * 255.0f);
    unsigned long green = (long)(color->green * 255.0f);
    unsigned long blue = (long)(color->blue * 255.0f);
    return ((((0xff00 | red) << 8) | green) << 8) | blue;
}

// @address 0x2a28e8 ?get_hud_interface_state@c_assault_engine@@UBAXJPAUgame_engine_interface_state@@@Z
void c_assault_engine::get_hud_interface_state(long player_index, game_engine_interface_state *state) const
{
    player_datum *player = assault_player_get(player_index);
    long team_index = player->configuration.host.team_index;
    long bomb_index = team_index;
    game_engine_ctf_update_sudden_death_state(player_index, state);
    if (g_assault_variant->m_game_type == 2)
        bomb_index = 8;
    const s_multiplayer_weapon_tracker *tracker = game_engine_multiplayer_weapon_get((short)bomb_index);
    long bomb_time = g_assault_variant->m_bomb_arming_time;
    bool return_visible = false;
    if (tracker != 0 && assault_shared_globals()->bomb_arming_timers[bomb_index] > 0
        && (assault_shared_globals()->bomb_weapon_flags[tracker->id] & 4) == 0
        && game_engine_get_carried_multiplayer_weapon_index(player_index, 1) != -1)
        return_visible = true;
    s_multiplayer_globals_definition *multiplayer_globals =
        (s_multiplayer_globals_definition *)global_tag_instances[datum_index_to_absolute_index(global_game_globals->multiplayer_globals_index)].definition;
    s_multiplayer_universal_globals_definition *universal = (s_multiplayer_universal_globals_definition *)multiplayer_globals->universal.address;
    c_static_wchar_string<256> label;
    long string_list_index = universal->strings_index;
    long language = get_current_language();
    string_list_get_normal_string_by_language(string_list_index, 0x40007, language, &label);
    e_multiplayer_team_designator designator = game_engine_team_index_to_team_designator((e_game_team)team_index);
    for (long bomb = 0; bomb < 9; bomb++)
    {
        if (bomb != team_index && (assault_shared_globals()->bomb_weapon_flags[bomb] & 4) != 0)
        {
            const s_multiplayer_weapon_tracker *other = game_engine_multiplayer_weapon_get((short)bomb);
            if (other != 0)
            {
                c_area *area = 0;
                if (assault_find_bomb_placed_post_team_designator(other->weapon_index, &area) == designator
                    && player_in_multiplayer_object_boundary(player_index, (const s_multiplayer_object_boundary_geometry_data *)area)
                    && assault_object_get_ultimate_parent(player->unit_index) == player->unit_index)
                {
                    return_visible = true;
                    bomb_index = bomb;
                    bomb_time = g_assault_variant->m_bomb_disarming_time;
                    string_list_get_normal_string(universal->strings_index, 0x40008, &label);
                }
            }
        }
    }
    if (return_visible && assault_shared_globals()->bomb_arming_timers[bomb_index] > 0)
    {
        usnzprintf(state->flag_return_text, 64, label.get_string());
        state->flag_return_visible = true;
        state->flag_return_progress = (float)assault_shared_globals()->bomb_arming_timers[bomb_index]
            / (float)(tls_game_time_globals()->ticks_per_second * bomb_time);
        real_rgb_color color = network_get_team_color((e_game_team)team_index);
        state->flag_return_color = assault_interface_color_to_pixel32(&color);
    }
}

static __forceinline const s_damage_owner *assault_object_get_damage_owner(long object_index)
{
    return &((const object_datum *)assault_weapon_get(object_index))->damage_owner;
}

// @address 0x2a31c8 assault_trigger_bomb_effect
extern "C" void assault_trigger_bomb_effect(long weapon_index, long effect_index, long sound_index, long damage_index, long effect_type)
{
    if (effect_index != -1)
    {
        real_vector3d up = *global_up3d;
        real_point3d origin;
        object_get_origin(weapon_index, &origin);
        s_damage_owner owner = *assault_object_get_damage_owner(weapon_index);
        s_effect_vector vectors[5];
        vectors[0].point = vectors[1].point = vectors[2].point = vectors[3].point = vectors[4].point = origin;
        vectors[0].vector = up;
        vectors[1].vector = *global_down3d;
        vectors[2].vector = up;
        vectors[3].vector.i = -up.i;
        vectors[3].vector.j = -up.j;
        vectors[3].vector.k = -up.k;
        vectors[4].vector = up;
        vectors[0].marker_name = _string_id_forward;
        vectors[1].marker_name = _string_id_gravity;
        vectors[2].marker_name = _string_id_up;
        vectors[3].marker_name = _string_id_backward;
        vectors[4].marker_name = _string_id_root;
        effect_new_with_damage_owner(effect_index, &owner, 0, 5, vectors, 0, 0, _effect_deterministic);
        simulation_action_weapon_effect(weapon_index, &origin, effect_type);
        if (sound_index != -1)
        {
            effect_new_with_damage_owner(sound_index, &owner, 0, 5, vectors, 0, 0, _effect_deterministic);
            simulation_action_weapon_effect(weapon_index, &origin, effect_type);
        }
    }
    if (damage_index != -1)
    {
        s_damage_data damage_data;
        real_point3d origin;
        object_get_origin(weapon_index, &origin);
        damage_data_new(&damage_data, damage_index);
        damage_data.owner_data = *assault_object_get_damage_owner(weapon_index);
        damage_data.scale[0] = 1.0f;
        object_get_location(weapon_index, &damage_data.location);
        damage_data.position = origin;
        damage_data.origin = damage_data.position;
        damage_data.direction = *global_up3d;
        damage_data.reporting_info = 132;
        area_of_effect_cause_damage(&damage_data, -1, -1);
    }
}
