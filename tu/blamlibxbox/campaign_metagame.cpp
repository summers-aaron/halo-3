// blamlibxbox/campaign_metagame.obj: from the records 0x118e90 0x1194c0 0x217650 0x217698 0x2176b0 0x217740 0x217770 0x217890 0x2179d8 0x217ad0 0x217bd0 0x217e10 0x217e98 0x217ee8 0x218020 0x2180e8 0x2187f0 0x218940 0x218a58 0x218b28 0x218bb0 0x218ce8 0x218da8 0x218f58 0x219140 0x219228 0x219288 0x2192b0 0x219360 0x219420 0x2195d8 0x2c8540 0x2c85d0 0x2c8640 0x2c86b0 0x4d8e00 0x4d8f08 0x4d8fb0 0x4d90b8 0x4d9118 0x4d9188 0x4d9270 0x4d92c8 0x4d9350 0x4d93d8 0x707f80 0x70bae0
// @flags /TP /O2
#include "game/campaign_metagame_globals.h"
#include "memory/tls_data_arrays.h"
#include "game/game_achievements_types.h"
#include "memory/data_types.h"
#include "interface/c_player_profile.h"
#include "interface/controller_types.h"
#include "interface/controller_inline.h"
#include "shell/shell.h"
#include "game/game_types.h"
#include "memory/static_arrays.h"
#include "units/unit_seat_flags.h"
// @address 0x1194c0 ?allocate@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_campaing_metagame_allocator@@YAXPAX@Z@@UAAPAXIPBD@Z
// @address 0x219360 ?reserve_memory@?$t_restricted_allocation_manager@$02$0A@$0A@$1?__tls_set_g_campaing_metagame_allocator@@YAXPAX@Z@@QAAPAXPBD0IJ@Z
// @address 0x217770 campaign_metagame_update_object_score
// @address 0x217ad0 campaign_metagame_set_profile_progress_bit
// @address 0x217890 campaign_metagame_set_profile_bit_and_grant_achievement
// @address 0x73e758 ?hs_function_table@@3PAPBUhs_function_definition@@A
// @address 0x807a0 ?game_is_cooperative@@YA_NXZ
// @address 0x1b7030 ?in_use@c_controller_interface@@QBA_NXZ
// @address 0x47e4f8 ?dirty_or@c_player_profile_interface@@AAAX_N@Z
// @address 0x187710 ?print@?$c_static_wchar_string@$0BAA@@@QAAPB_WPB_WZZ
// @address 0x217640 ?__tls_set_g_player_control_globals_allocator@@YAXPAX@Z
// @address 0x24dac0 ?hs_arguments_evaluate@@YAPAJJFPBF_N@Z
// @address 0xab8a0 ?hs_return@@YAXJJ@Z
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "game/game_time_types.h"
#include "game/game_time_access.h"
#include "interface/user_interface_messages.h"
#include "game/game.h"
#include "interface/gui_screens/scoreboard/gui_screen_scoreboard.h"
#include "interface/user_interface_memory.h"
#include "objects/damage_reporting_types.h"
#include "objects/objects.h"
#include "text/unicode.h"
#include "units/units.h"
#include "game/player_control.h"
#include "hs/hs_runtime.h"
#include "cache/restricted_memory_regions.h"
#include "game/game_time.h"
#include "interface/chud/chud_messaging.h"
#include "tag_files/tag_types.h"
#include "ai/ai_information_data.h"
#include "main/levels.h"
#include "game/campaign_metagame.h"
#include "saved_games/saved_film.h"
#include "interface/gui_screens/carnage_report/gui_screen_carnage_report.h"
#include "memory/data.h"
#include "memory/datum_index.h"
#include "game/game_globals_storage.h"
#include "game/players.h"
#include "objects/object_header.h"
#include "tag_files/tag_groups.h"
#include "game/game_allegiance.h"
#include "game/player_types.h"
#include "memory/data_types.h"
#include "memory/data_types.h"
#include "game/player_types.h"
#include "memory/data_types.h"
#include "memory/data_types.h"
#include "cache/cache_files.h"
#include "game/game_results.h"
#include "game/game_skulls.h"
#include "tag_files/string_ids.h"
#include "hs/hs_function.h"
#include "game/campaign_metagame_level_types.h"
#include "game/game_achievements.h"
#include "ai/ai_dialogue.h"

// Registers a field in restricted region 3 and publishes its thread-local address.
#include "cache/restricted_memory_layout.h"
#include "cache/restricted_memory_registration.h"
#include "multithreading/thread_tls.h"

// One scored kill, kept until the player's kill window closes.

#include "game/campaign_metagame_definitions.h"

#include "scenario/scenario_tag.h"

#include "game/game_globals.h"

#include "objects/object_datum.h"
#include "units/unit_types.h"

extern "C" {
// @address 0x7e8720 g_campaign_metagame_player_globals
s_campaign_metagame_player_globals g_campaign_metagame_player_globals;
}
#include "memory/tls_globals.h"
#include "game/player_mapping_types.h"
struct e_game_results_damage_type_description;
#include "scenario/scenario.h"

// 0x218020


#include "hs/hs_function_definition.h"
#include "hs/hs_value_types.h"

// Formats a multiplier ("2x", "0.75x", "1.5x"); 1.0 and 0.0 take the object's own text.
#include "interface/gui_widget.h"

#include <new>
#include "ai/actor_datum.h"
#include "units/unit_definitions.h"
#include "ai/character_definitions.h"
#include "creatures/creature_definitions.h"
#include "items/projectile_definitions.h"
#include "creatures/creature_datum.h"
#include "game/player_iterator.h"
#include "ai/actor_globals.h"
#include "ai/dialogue_event.h"
#include "game/game_globals_inline.h"
#include "game/game_time_inline.h"

enum { _string_id_multiplier_default = 0x105b6 };

template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *), void (*k_set_allocator_function)(void *)>
void *t_restricted_allocation_manager<k_region_index, k_adopt_function, k_post_copy_function, k_set_allocator_function>::allocate(
    unsigned int size, const char *name)
{
  return reserve_memory(name, "", size, 0);
}

template void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_campaing_metagame_allocator>::allocate(
    unsigned int size, const char *name);

// @address 0x217650 ?campaign_metagame_initialize@@YAXXZ

static __forceinline s_campaign_metagame_definition *campaign_metagame_scale_block_get(void)
{
  s_campaign_metagame_definition *block = 0;
  s_game_globals *globals = global_game_globals;
  if (globals && globals->campaign_metagame.s_tag_block::count == 1)
    block = (s_campaign_metagame_definition *)globals->campaign_metagame.address;
  return block;
}

void campaign_metagame_initialize(void)
{
  g_campaing_metagame_globals = (s_campaing_metagame_globals *)g_campaing_metagame_allocator.reserve_memory("campaign meta-game globals", 0, 0x2438, 0);
}

// Clears the thread's metagame globals pointer.
// @address 0x217698 ?campaign_metagame_dispose@@YAXXZ
void campaign_metagame_dispose(void)
{
  g_campaing_metagame_globals = 0;
}

// Clears the flags' bit 1 and byte 4 of the secondary globals.

// Clears the metagame globals, sets bit 1 and bit 5 from the campaign options, and marks the secondary globals.
// @address 0x2176b0 campaign_metagame_initialize_for_new_map
void campaign_metagame_initialize_for_new_map(void)
{
  s_campaing_metagame_globals *globals = g_campaing_metagame_globals;
  memset(globals, 0, sizeof(*globals));
  globals->flags |= 2;
  if (!saved_film_can_record(&tls_game_globals()->options))
    g_campaing_metagame_globals->flags |= 32;
  else
    g_campaing_metagame_globals->flags &= ~32u;
  memset(&g_campaign_metagame_player_globals, 0, sizeof(g_campaign_metagame_player_globals));
  g_campaign_metagame_player_globals.active = 1;
}

// @address 0x217740 campaign_metagame_dispose_from_old_map
void campaign_metagame_dispose_from_old_map(void)
{
  g_campaing_metagame_globals->flags &= ~2u;
  g_campaign_metagame_player_globals.active = 0;
}

static __forceinline long campaign_metagame_seconds_to_ticks_round(float seconds)
{
    return (long)((float)tls_game_time_globals()->ticks_per_second * seconds + 0.5f);
}

void campaign_metagame_update_object_score(long object_index, short score_delta)
{
    if (object_index == -1)
        return;

    s_object_header *header = object_header_get(object_index);
    if (((1 << header->object_type) & 0x1003) == 0)
        return;

    long player_index = ((unit_datum *)header->datum)->unit.player_index;
    if (player_index == -1)
        return;

    s_campaing_metagame_player *slot = campaign_metagame_try_and_get_owner_slot(g_campaing_metagame_globals, player_index, 1);
    if (!slot)
        return;

    s_campaing_metagame_player *m = slot;
    s_campaign_metagame_definition *block = campaign_metagame_scale_block_get();
    int timer = campaign_metagame_seconds_to_ticks_round(block->window_seconds);
    float scale = campaign_metagame_get_score_scale();
    float f_delta = (float)score_delta;
    int delta = (int)(scale * f_delta);
    m->last_delta = delta;
    m->timer2 = timer;
    m->score += delta;
}

// @address 0x2a6618 ?game_grant_achievement_to_local_player@@YAXW4e_controller_index@@W4e_achievement@@_N@Z

// @address 0x2179d8 campaign_metagame_grant_map_achievement
extern "C" void campaign_metagame_grant_map_achievement(long player_index, long key)
{
    struct s_achievement_pair {
        long key;
        long achievement;
    };
    s_achievement_pair pairs[9] = {
        { 0xbc2, 0 }, { 0xbcc, 1 }, { 0xbd6, 2 }, { 0xbe0, 3 }, { 0xbea, 4 },
        { 0xbfe, 5 }, { 0xc1c, 6 }, { 0xc26, 7 }, { 0xc30, 8 },
    };
    for (unsigned long i = 0; i < 9; i++) {
        if (pairs[i].key == key) {
            long achievement = pairs[i].achievement;
            if (achievement != -1)
                game_grant_achievement_to_local_player((e_controller_index)player_index, (e_achievement)achievement, false);
            return;
        }
    }
}

void campaign_metagame_set_profile_progress_bit(long object_index, short p1)
{
    if (game_is_cooperative() || object_index == -1)
        return;

    s_object_header *header = object_header_get(object_index);
    if (((1 << header->object_type) & 0x1003) == 0)
        return;

    unit_datum *datum = (unit_datum *)header->datum;
    if (datum->unit.player_index == -1)
        return;
    long player_index = datum->unit.player_index;

    long absolute_player_index = player_index & 0xffff;
    long controller_index = tls_player_mapping_globals()->player_input_controller_mapping[absolute_player_index];
    if (controller_index == -1)
        return;

    c_player_profile_interface *profile = controller_get((e_controller_index)controller_index)->get_player_profile_interface();
    int old_flags = profile->campaign.secondary_skulls_found;
    short p1_short = (short)p1;
    if (p1_short >= 0 && p1_short < 5) {
        int new_flags = (1 << p1_short) | old_flags;
        if (new_flags != old_flags) {
            profile->dirty_or(old_flags - new_flags);
            profile->campaign.secondary_skulls_found = new_flags;
        }
    }
}

void campaign_metagame_set_profile_bit_and_grant_achievement(long object_index, short p1)
{
    if (game_is_cooperative() || object_index == -1)
        return;

    s_object_header *header = object_header_get(object_index);
    if (((1 << header->object_type) & 0x1003) == 0)
        return;

    unit_datum *datum = (unit_datum *)header->datum;
    if (datum->unit.player_index == -1)
        return;
    long player_index = datum->unit.player_index;

    long absolute_player_index = player_index & 0xffff;
    long controller_index = tls_player_mapping_globals()->player_input_controller_mapping[absolute_player_index];
    if (controller_index == -1)
        return;

    c_player_profile_interface *profile = controller_get((e_controller_index)controller_index)->get_player_profile_interface();
    int old_flags = profile->campaign.primary_skulls_found;
    short p1_short = (short)p1;
    if (p1_short >= 0 && p1_short < 9) {
        int new_flags = (1 << p1_short) | old_flags;
        if (new_flags != old_flags) {
            profile->dirty_or(old_flags - new_flags);
            profile->campaign.primary_skulls_found = new_flags;
        }
        long achievements[9] = { 22, 23, 24, 25, 26, 27, 28, 29, 30 };
        game_grant_achievement_to_local_player((e_controller_index)controller_index, (e_achievement)achievements[p1_short], false);
    }
}

// The highest valid level (0..3) among the active players' options, 4 when none.
// @address 0x217e10 campaign_metagame_get_highest_level
c_level campaign_metagame_get_highest_level(void)
{
  c_level result(4);
  s_game_options *options = &tls_game_globals()->options;
  for (long i = 0; i < 16; i++) {
    game_player_options *player = &options->players[i];
    if (player->valid) {
      unsigned char level = player->configuration.client.bungienet_user_role;
      bool valid = false;
      switch (player->configuration.client.bungienet_user_role) {
      case 0: case 1: case 2: case 3:
        valid = true;
        break;
      }
      if (valid && (result.value == 4 || (signed char)level > result.value))
        result.value = level;
    }
  }
  return result;
}

// @address 0x217e98 campaign_metagame_is_enabled
bool campaign_metagame_is_enabled(void)
{
  return tls_game_globals()->options.campaign_metagame_enabled && !(g_campaing_metagame_globals->flags & 0x10) && !(g_campaing_metagame_globals->flags & 0x20);
}

// @address 0x218940 campaign_metagame_compute_medal
int campaign_metagame_compute_medal(unsigned char flags)
{
  int result = -1;
  unsigned int index = flags >> 2;
  int type = 0;
  if (index <= 60) type = ((int *)g_game_results_damage_type_descriptions)[index];
  switch (flags & 3) {
  case 1:
    result = 1;
    break;
  case 2:
    if (index == 7) result = 6;
    else if (type == 4) result = 0;
    else if (type == 3) result = 3;
    break;
  case 3:
    switch (index) {
    case _damage_reporting_type_banshee:
    case _damage_reporting_type_ghost:
    case _damage_reporting_type_mongoose:
    case _damage_reporting_type_scorpion:
    case _damage_reporting_type_spectre_driver:
    case _damage_reporting_type_warthog_driver:
    case _damage_reporting_type_wraith:
    case _damage_reporting_type_tank:
    case _damage_reporting_type_chopper:
    case _damage_reporting_type_hornet:
    case _damage_reporting_type_mantis:
    case _damage_reporting_type_mauler:
      result = 4;
      break;
    }
    break;
  case 0:
    if (type == 3) result = 2;
    break;
  }
  return result;
}

// The scenario's curve value at the given game time (the first entry later than it), 1 when there is none.
// @address 0x218a58 campaign_metagame_get_time_multiplier
float campaign_metagame_get_time_multiplier(long ticks, float *time)
{
  float result = 1.0f;
  float minutes = tls_game_time_globals()->tick_length * (float)ticks * 0.016666668f;
  if (time)
    *time = -1.0f;
  scenario *scenario = global_scenario;
  if (scenario->time_curve.count && !tls_game_globals()->options.campaign_insertion_point) {
    s_campaign_metagame_time_curve *curve = (s_campaign_metagame_time_curve *)scenario->time_curve.address;
    if (curve && curve->entries.s_tag_block::count) {
      for (long i = 0; i < curve->entries.s_tag_block::count; i++) {
        s_campaign_metagame_time_entry *entries = (s_campaign_metagame_time_entry *)curve->entries.address;
        s_campaign_metagame_time_entry *entry = &entries[i];
        if (entry->time > minutes) {
          result = entry->value;
          if (time)
            *time = entry->time;
          break;
        }
      }
    }
  }
  return result;
}

// @address 0x217ee8 campaign_metagame_update_scores_before_revert
extern "C" void campaign_metagame_update_scores_before_revert(void)
{
    s_game_globals *metagame_definitions;
    s_campaign_metagame_definition *kill_config;
    int player_unit_index;
    int scored_player_index;
    s_campaign_metagame_player_slot *kill_player_slot;
    float revert_score_scale;
    player_datum *current_datum;
    c_data_iterator<player_datum> metagame_iterator;
    if (tls_game_globals()->options.game_mode != 1) return;
    if ((g_campaing_metagame_globals->flags & 1) != 0) return;
    metagame_definitions = global_game_globals;
    if (!metagame_definitions) return;
    if (metagame_definitions->campaign_metagame.s_tag_block::count != 1) return;
    kill_config = (s_campaign_metagame_definition *)metagame_definitions->campaign_metagame.address;
    if (!kill_config) return;
    metagame_iterator.m_iterator.absolute_index = -1;
    metagame_iterator.m_iterator.index = -1;
    metagame_iterator.m_iterator.data = tls_player_data();
    current_datum = (player_datum *)::data_iterator_next(&metagame_iterator.m_iterator);
    metagame_iterator.m_datum = current_datum;
    while (current_datum) {
        player_unit_index = current_datum->unit_index;
        if (player_unit_index != -1) {
            s_object_header *header = object_header_get(player_unit_index);
            unit_datum *unit = (unit_datum *)header->datum;
            if ((unsigned char)((unit->object.damage_flags >> 2) & 1) == 0) {
                scored_player_index = metagame_iterator.m_iterator.index;
                kill_player_slot = campaign_metagame_try_and_get_player_slot(
                    &g_campaign_metagame_player_globals, scored_player_index, 1);
                campaign_metagame_update_player_score(scored_player_index,
                    campaign_metagame_get_score_scale() * (float)(double)(int)kill_config->suicide_points);
                kill_player_slot->deaths++;
            }
        }
        current_datum = (player_datum *)::data_iterator_next(&metagame_iterator.m_iterator);
        metagame_iterator.m_datum = current_datum;
    }
}

// @address 0x218020 campaign_metagame_update_scores_after_revert
extern "C" void campaign_metagame_update_scores_after_revert(void)
{
    if (tls_game_globals()->options.game_mode == 1 && (g_campaing_metagame_globals->flags & 1) == 0) {
        s_game_globals *game_globals = global_game_globals;
        if (game_globals && game_globals->campaign_metagame.s_tag_block::count == 1 && game_globals->campaign_metagame.address != 0) {
            c_data_iterator<player_datum> iterator;
            iterator.m_iterator.absolute_index = -1;
            iterator.m_iterator.index = -1;
            iterator.m_iterator.data = tls_player_data();
            iterator.m_datum = (player_datum *)::data_iterator_next(&iterator.m_iterator);
            while (iterator.m_datum) {
                campaign_metagame_update_player_score(iterator.m_iterator.index, 0.0f);
                iterator.m_datum = (player_datum *)::data_iterator_next(&iterator.m_iterator);
            }
        }
    }
}

static __forceinline short campaign_metagame_player_count_get(void)
{
  long count;
  if (tls_game_globals()->options.game_mode == 1)
    count = tls_game_globals()->options.campaign_difficulty;
  else
    count = 1;
  return count;
}

// @address 0x218b28 campaign_metagame_get_player_count_scale
float campaign_metagame_get_player_count_scale(void)
{
  s_campaign_metagame_definition *block = campaign_metagame_scale_block_get();
  long count = campaign_metagame_player_count_get();
  float result = 1.0f;
  if (block && count >= 0 && count < block->player_count_multipliers.s_tag_block::count)
    result = block->player_count_multipliers.begin()[count];
  return result;
}


static __forceinline unsigned char campaign_metagame_skull_is_active_primary(int skull)
{
  int skulls = tls_game_globals()->options.game_mode == 1 ? tls_game_globals()->options.primary_skulls : 0;
  bool result = false;
  if (!skull_out_of_range(skull, 9))
    result = (((1 << skull) & skulls) != 0);
  return result;
}

static __forceinline unsigned char campaign_metagame_skull_is_active_secondary(int skull)
{
  int skulls = tls_game_globals()->options.game_mode == 1 ? tls_game_globals()->options.secondary_skulls : 0;
  bool result = false;
  if (!skull_out_of_range(skull, 5))
    result = (((1 << skull) & skulls) != 0);
  return result;
}

// The sum of the active skulls' score multipliers.
// @address 0x218bb0 campaign_metagame_get_skull_bonus
float campaign_metagame_get_skull_bonus(void)
{
  s_campaign_metagame_definition *block = campaign_metagame_scale_block_get();
  float total = 0.0f;
  for (long skull = 0; skull < 9; skull++) {
    if (campaign_metagame_skull_is_active_primary(skull) && skull < block->primary_skull_multipliers.s_tag_block::count)
      total += block->primary_skull_multipliers.begin()[skull];
  }
  for (long skull = 0; skull < 5; skull++) {
    if (campaign_metagame_skull_is_active_secondary(skull) && skull < block->secondary_skull_multipliers.s_tag_block::count)
      total += block->secondary_skull_multipliers.begin()[skull];
  }
  return total;
}

// The player-count scale for the game mode: the larger of the 1.0 floor and the skull bonus, times the count scale.
// @address 0x218ce8 campaign_metagame_get_score_scale
float campaign_metagame_get_score_scale(void)
{
  short count = campaign_metagame_player_count_get();
  float scale = campaign_metagame_get_player_count_scale();
  float bonus = campaign_metagame_get_skull_bonus();
  float result = 0.0f;
  switch (count) {
  case 0:
  case 1:
    result = (bonus < 1.0f ? 1.0f : bonus) * scale;
    break;
  case 2:
  case 3:
    result = bonus + scale;
    break;
  }
  return result;
}

// @address 0x218f58 campaign_metagame_grant_score_achievements
// 0x218f58
extern "C" void campaign_metagame_grant_score_achievements(void)
{
    game_globals_storage *g = tls_game_globals();
    bool enabled = g->options.campaign_metagame_enabled != 0 && !(g_campaing_metagame_globals->flags & 0x10) && !(g_campaing_metagame_globals->flags & 0x20);
    if (enabled && !game_is_cooperative()) {
        int amount = 0;
        long value = -1;
        scenario *current_scenario = global_scenario;
        if (current_scenario->time_curve.count != 0 && current_scenario->time_curve.address != 0)
            amount = (int)((s_campaign_metagame_time_curve *)current_scenario->time_curve.address)->score_threshold;
        if (g->options.map_id != -1) {
            long table[11];
            table[0] = -1;
            table[10] = -1;
            table[1] = 13; table[2] = 14; table[3] = 15; table[4] = 16; table[5] = 17;
            table[6] = 18; table[7] = 19; table[8] = 20; table[9] = 21;
            int index = levels_get_campaign_map_index(g->options.campaign_id, g->options.map_id);
            if (index >= 0 && (unsigned)index < 11)
                value = table[index];
        }
        if (amount > 0 && value != -1) {
            int i = 0;
            do {
                if (controller_get((e_controller_index)i)->in_use() && i != -1) {
                    long handle = tls_player_mapping_globals()->user_controller_mapping[i];
                    if (handle != -1) {
                        s_campaing_metagame_player *object = campaign_metagame_try_and_get_owner_slot(g_campaing_metagame_globals, handle, 1);
                        if (object != 0 && object->scaled >= amount)
                            game_grant_achievement_to_local_player((e_controller_index)i, (e_achievement)value, false);
                    }
                }
                int next = -1;
                if ((unsigned)i <= 2)
                    next = i + 1;
                i = next;
            } while (i != -1);
        }
    }
}

// @address 0x219228 campaign_metagame_reset_finished
// 0x219228
extern "C" void campaign_metagame_reset_finished(void)
{
    bool call = true;
    g_campaing_metagame_globals->flags &= 0xfffffff7;
    signed char simulation = tls_game_globals()->options.game_simulation;
    if (simulation == 2 || simulation == 4)
        call = false;
    if (call)
        game_time_set_pause_reason(0, 8);
}

// @address 0x219288 campaign_metagame_set_disabled_flag
void campaign_metagame_set_disabled_flag(unsigned int flags)
{
  if ((int)(flags & 0x400) == 0)
    return;
  g_campaing_metagame_globals->flags |= 0x10;
}

// @address 0x2192b0 campaign_metagame_update_player_score
extern "C" void campaign_metagame_update_player_score(long player_index, float points)
{
    s_campaing_metagame_player *owner_slot = campaign_metagame_try_and_get_owner_slot(g_campaing_metagame_globals, player_index, 0);
    s_campaign_metagame_player_slot *player_slot = campaign_metagame_try_and_get_player_slot(
        &g_campaign_metagame_player_globals, player_index, 1);
    int limit = owner_slot->score + owner_slot->awarded_bonus;
    int total = (int)(points + 0.5f) + player_slot->score;
    player_slot->score = total;
    if (total > limit)
        player_slot->score = limit;
    else
        player_slot->score = total;
}

template<long k_region_index, void (*k_adopt_function)(void *), void (*k_post_copy_function)(void *), void (*k_set_allocator_function)(void *)>
void *t_restricted_allocation_manager<k_region_index, k_adopt_function, k_post_copy_function, k_set_allocator_function>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits)
{
  int field_index = restricted_memory_register_member(&g_restricted_regions[3], name, type, size, 0,
                                __tls_set_g_player_control_globals_allocator, 0, 0);
  unsigned int offset = g_restricted_regions[3].get_member_offset(field_index);
  void *address = restricted_region_get_base_address(3) + offset;
  int owner_thread = get_current_thread_index();
  m_thread_id = owner_thread;
  m_member_index = field_index;
  g_campaing_metagame_globals = (s_campaing_metagame_globals *)address;
  return restricted_region_get_base_address(3) + g_restricted_regions[3].get_member_offset(field_index);
}

template void *t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    __tls_set_g_campaing_metagame_allocator>::reserve_memory(
    const char *name, const char *type, unsigned int size, long alignment_bits);

// Finds the owner's slot whose key equals the player's; when asked and none matches, claims the first slot no active
// player's key is using and copies the player's key into it (0 when none is free).
// @address 0x219420 campaign_metagame_try_and_get_owner_slot
s_campaing_metagame_player *campaign_metagame_try_and_get_owner_slot(s_campaing_metagame_globals *globals, long index, unsigned char allocate)
{
    s_data_array *players = (s_data_array *)tls_player_data();
    player_datum *player = &((player_datum *)players->data)[datum_index_to_absolute_index(index)];
    long found = -1;
    long i;
    for (i = 0; i < 4; i++) {
        if (memcmp(&player->player_identifier, globals->players[i].identifier, sizeof(globals->players[i].identifier)) == 0)
            found = i;
    }
    if (allocate) {
        if (found == -1) {
            c_data_iterator<player_datum> iterator;
            unsigned long used = 0;
            iterator.m_iterator.data = players;
            iterator.m_iterator.absolute_index = -1;
            iterator.m_iterator.index = -1;
            iterator.m_datum = (player_datum *)::data_iterator_next(&iterator.m_iterator);
            while (iterator.m_datum) {
                unsigned long flags = iterator.m_datum->flags & 2;
                for (i = 0; i < 4; i++) {
                    if (flags == 0 && memcmp(&iterator.m_datum->player_identifier, globals->players[i].identifier, sizeof(globals->players[i].identifier)) == 0)
                        used |= 1 << i;
                }
                iterator.m_datum = (player_datum *)::data_iterator_next(&iterator.m_iterator);
            }
            for (i = 0; ; i++) {
                if (i >= 4)
                    return 0;
                if (((1 << i) & used) == 0) {
                    found = i;
                    memcpy(globals->players[i].identifier, &player->player_identifier, sizeof(globals->players[i].identifier));
                    break;
                }
            }
        }
    }
    if (found == -1)
        return 0;
    return &globals->players[found];
}

// Finds the player's slot in the global slot table by key; when none matches, claims the first slot no active player's
// key is using and copies the player's key into it.
// @address 0x2195d8 campaign_metagame_try_and_get_player_slot
extern "C" s_campaign_metagame_player_slot *campaign_metagame_try_and_get_player_slot(s_campaign_metagame_player_globals *, long player_index, int)
{
    s_data_array *players = (s_data_array *)tls_player_data();
    player_datum *player = &((player_datum *)players->data)[datum_index_to_absolute_index(player_index)];
    long found = -1;
    for (long i = 0; i < 4; i++) {
        if (memcmp(&player->player_identifier, g_campaign_metagame_player_globals.slots[i].key, sizeof(g_campaign_metagame_player_globals.slots[i].key)) == 0)
            found = i;
    }
    if (found == -1) {
        unsigned long used = 0;
        c_data_iterator<player_datum> iterator;
        iterator.m_iterator.data = players;
        iterator.m_iterator.absolute_index = -1;
        iterator.m_iterator.index = -1;
        iterator.m_datum = (player_datum *)::data_iterator_next(&iterator.m_iterator);
        while (iterator.m_datum) {
            for (long i = 0; i < 4; i++) {
                if ((iterator.m_datum->flags & 2) == 0 && memcmp(&iterator.m_datum->player_identifier,
                    g_campaign_metagame_player_globals.slots[i].key, sizeof(g_campaign_metagame_player_globals.slots[i].key)) == 0)
                    used |= 1 << i;
            }
            iterator.m_datum = (player_datum *)::data_iterator_next(&iterator.m_iterator);
        }
        for (long i = 0; i < 4; i++) {
            if ((used & (1 << i)) == 0) {
                found = i;
                memcpy(g_campaign_metagame_player_globals.slots[found].key, &player->player_identifier,
                    sizeof(g_campaign_metagame_player_globals.slots[found].key));
                break;
            }
        }
    }
    return (found != -1) ? &g_campaign_metagame_player_globals.slots[found] : 0;
}

// 0x2c8540: enables or disables campaign metagame scoring from the script argument.
// @address 0x2c8540 campaign_metagame_set_enabled_evaluate
extern "C" void campaign_metagame_set_enabled_evaluate(short function_index, long thread_index, bool initialize)
{
    const hs_function_definition *definition = hs_function_table[function_index];
    hs_value *arguments = (hs_value *)hs_arguments_evaluate(thread_index, definition->formal_parameter_count,
        definition->formal_parameters, initialize);
    if (arguments) {
        if (arguments[0].boolean_value)
            g_campaing_metagame_globals->flags |= 1;
        else
            g_campaing_metagame_globals->flags &= ~1u;
        hs_return(thread_index, 0);
    }
}

// 0x2c85d0: adds the first argument's score delta to its object.
// @address 0x2c85d0 ?campaign_metagame_update_object_score_evaluate@@YAXFJ_N@Z
void campaign_metagame_update_object_score_evaluate(short function_index, long thread_index, bool initialize)
{
    const hs_function_definition *definition = hs_function_table[function_index];
    hs_value *arguments = (hs_value *)hs_arguments_evaluate(thread_index, definition->formal_parameter_count,
        definition->formal_parameters, initialize);
    if (arguments) {
        campaign_metagame_update_object_score(arguments[0].long_value, arguments[1].short_value);
        hs_return(thread_index, 0);
    }
}

// 0x2c8640: adds the first argument's score delta to its object.
// @address 0x2c8640 campaign_metagame_grant_profile_achievement_evaluate
extern "C" void campaign_metagame_grant_profile_achievement_evaluate(short function_index, long thread_index, bool initialize)
{
    const hs_function_definition *definition = hs_function_table[function_index];
    hs_value *arguments = (hs_value *)hs_arguments_evaluate(thread_index, definition->formal_parameter_count,
        definition->formal_parameters, initialize);
    if (arguments) {
        campaign_metagame_set_profile_bit_and_grant_achievement(arguments[0].long_value, arguments[1].short_value);
        hs_return(thread_index, 0);
    }
}

// 0x2c86b0: adds the first argument's score delta to its object, non-cooperative.
// @address 0x2c86b0 ?campaign_metagame_set_profile_bit_evaluate@@YAXFJ_N@Z
void campaign_metagame_set_profile_bit_evaluate(short function_index, long thread_index, bool initialize)
{
    const hs_function_definition *definition = hs_function_table[function_index];
    hs_value *arguments = (hs_value *)hs_arguments_evaluate(thread_index, definition->formal_parameter_count,
        definition->formal_parameters, initialize);
    if (arguments) {
        campaign_metagame_set_profile_progress_bit(arguments[0].long_value, arguments[1].short_value);
        hs_return(thread_index, 0);
    }
}


// The elapsed campaign time as minutes:seconds.
// @address 0x4d8e00 parse_xml_campaign_time
extern "C" bool parse_xml_campaign_time(long unused, wchar_t *buffer, unsigned long buffer_length)
{
    bool result = false;
    if (tls_game_globals()->options.game_mode == 1) {
        s_campaign_metagame_player_globals *globals = 0;
        if (g_campaign_metagame_player_globals.active)
            globals = &g_campaign_metagame_player_globals;
        c_static_wchar_string<256> string;
        long ticks = globals->ticks;
        if (ticks <= 0)
            ticks = 0;
        long seconds = (long)(tls_game_time_globals()->tick_length * (float)ticks);
        string.print(L"%2i:%02i", seconds / 60, seconds % 60);
        string.copy_to(buffer, buffer_length);
        result = true;
    }
    return result;
}

// The number of primary skulls the campaign options turn on.
// @address 0x4d8f08 parse_xml_campaign_skull_count
extern "C" bool parse_xml_campaign_skull_count(long unused, wchar_t *buffer, unsigned long buffer_length)
{
    bool result = false;
    if (tls_game_globals()->options.game_mode == 1) {
        long skulls = tls_game_globals()->options.primary_skulls;
        c_static_wchar_string<256> string;
        short count = 0;
        for (short skull = 0; skull < 9; skull++) {
            if ((1 << skull) & skulls)
                count++;
        }
        string.print(L"%i", count);
        string.copy_to(buffer, buffer_length);
        result = true;
    }
    return result;
}

// @address 0x484d20 ?print@?$c_static_wchar_string@$0EAA@@@QAAPB_WPB_WZZ

// @address 0x4d8fb0 campaign_metagame_set_multiplier_string
extern "C" void campaign_metagame_set_multiplier_string(
    c_gui_widget *source, float multiplier, wchar_t *buffer, unsigned long buffer_length)
{
    c_static_wchar_string<1024> string;
    if (fabs(multiplier - 1.0f) >= 0.0001f && fabs(multiplier) >= 0.0001f)
    {
        if (multiplier >= 100.0f)
            string.print(L"%ix", (long)multiplier);
        else if (multiplier < 1.0f)
            string.print(L"%.2gx", multiplier);
        else
            string.print(L"%.3gx", multiplier);
    }
    else
    {
        source->get_string_by_string_id(_string_id_multiplier_default, &string);
    }
    string.copy_to(buffer, buffer_length);
}

// 0x4d90b8: the player-count score multiplier, formatted.
// @address 0x4d90b8 parse_xml_campaign_player_scale
extern "C" bool parse_xml_campaign_player_scale(c_gui_widget *source, wchar_t *buffer, unsigned long buffer_length)
{
    bool result = false;
    if (tls_game_globals()->options.game_mode == 1) {
        float multiplier = campaign_metagame_get_player_count_scale();
        campaign_metagame_set_multiplier_string(source, multiplier, buffer, buffer_length);
        result = true;
    }
    return result;
}

// @address 0x4d9118 parse_xml_campaign_skull_bonus
// 0x4d9118: the skull bonus score multiplier, formatted.
extern "C" bool parse_xml_campaign_skull_bonus(c_gui_widget *source, wchar_t *buffer, unsigned long buffer_length)
{
    bool result = false;
    if (tls_game_globals()->options.game_mode == 1) {
        float multiplier = campaign_metagame_get_skull_bonus();
        campaign_metagame_set_multiplier_string(source, multiplier, buffer, buffer_length);
        result = true;
    }
    return result;
}

// @address 0x4d9188 campaign_metagame_get_difficulty_scale

static __forceinline short campaign_metagame_difficulty_level_get(void)
{
    return (tls_game_globals()->options.game_mode == 1) ? tls_game_globals()->options.campaign_difficulty : _campaign_difficulty_level_normal;
}

extern "C" float campaign_metagame_get_difficulty_scale(void)
{
    float result = 0.0f;
    float bonus = campaign_metagame_get_skull_bonus();
    float scale_bonus;
    if (fabs(bonus) < 0.0001f)
        scale_bonus = 1.0f;
    else
        scale_bonus = bonus;

    switch (campaign_metagame_difficulty_level_get()) {
    case _campaign_difficulty_level_easy:
        result = campaign_metagame_get_player_count_scale() * scale_bonus;
        break;
    case _campaign_difficulty_level_normal:
        result = scale_bonus;
        break;
    case _campaign_difficulty_level_heroic:
    case _campaign_difficulty_level_legendary:
        result = campaign_metagame_get_player_count_scale() + bonus;
        break;
    default:
        break;
    }
    return result;
}

// 0x4d9270: the campaign difficulty's score multiplier, formatted.
// @address 0x4d9270 parse_xml_campaign_difficulty_scale
extern "C" bool parse_xml_campaign_difficulty_scale(c_gui_widget *source, wchar_t *buffer, unsigned long buffer_length)
{
    bool result = false;
    if (tls_game_globals()->options.game_mode == 1) {
        float multiplier = campaign_metagame_get_difficulty_scale();
        campaign_metagame_set_multiplier_string(source, multiplier, buffer, buffer_length);
        result = true;
    }
    return result;
}

// 0x4d92c8: the current metagame multiplier, formatted.
// @address 0x4d92c8 parse_xml_campaign_time_multiplier
extern "C" bool parse_xml_campaign_time_multiplier(c_gui_widget *source, wchar_t *buffer, unsigned long buffer_length)
{
    bool result = false;
    if (tls_game_globals()->options.game_mode == 1) {
        s_campaing_metagame_globals *globals = 0;
        s_campaing_metagame_globals *metagame = g_campaing_metagame_globals;
        if (metagame && (metagame->flags & 2))
            globals = metagame;
        campaign_metagame_set_multiplier_string(source, globals->time_multiplier, buffer, buffer_length);
        result = true;
    }
    return result;
}

// 0x4d9350: the campaign difficulty's score multiplier times the metagame's, formatted.
// @address 0x4d9350 parse_xml_campaign_total_multiplier
extern "C" bool parse_xml_campaign_total_multiplier(c_gui_widget *source, wchar_t *buffer, unsigned long buffer_length)
{
    bool result = false;
    if (tls_game_globals()->options.game_mode == 1) {
        float multiplier = campaign_metagame_get_difficulty_scale();
        s_campaing_metagame_globals *globals = 0;
        s_campaing_metagame_globals *metagame = g_campaing_metagame_globals;
        if (metagame && (metagame->flags & 2))
            globals = metagame;
        campaign_metagame_set_multiplier_string(source, globals->time_multiplier * multiplier, buffer, buffer_length);
        result = true;
    }
    return result;
}

// The metagame's score scale, as a whole number.
// @address 0x4d93d8 parse_xml_campaign_total_score
extern "C" bool parse_xml_campaign_total_score(long unused, wchar_t *buffer, unsigned long buffer_length)
{
    bool result = false;
    if (tls_game_globals()->options.game_mode == 1) {
        c_static_wchar_string<256> string;
        s_campaing_metagame_globals *globals = 0;
        s_campaing_metagame_globals *metagame = g_campaing_metagame_globals;
        if (metagame && (metagame->flags & 2))
            globals = metagame;
        string.print(L"%i", (long)globals->final_score);
        string.copy_to(buffer, buffer_length);
        result = true;
    }
    return result;
}

// @address 0x707f80 campaign_metagame_initialize_atexit
extern "C" void campaign_metagame_initialize_atexit(void)
{
    atexit(campaign_metagame_dispose_atexit);
}

// @address 0x70bae0 campaign_metagame_dispose_atexit
extern "C" void campaign_metagame_dispose_atexit(void)
{
}

// @address 0x218da8 campaign_metagame_update_final_scores
extern "C" void campaign_metagame_update_final_scores(void)
{
    s_campaing_metagame_globals *globals = g_campaing_metagame_globals;
    if ((globals->flags & 4) == 0) {
        float time;
        float total = 0.0f;
        globals->time_multiplier = campaign_metagame_get_time_multiplier(g_campaign_metagame_player_globals.ticks, &time);
        short player;
        for (player = 0; player < 4; player = (short)(player + 1)) {
            s_campaing_metagame_player *scores =
                &globals->players[player];
            s_campaign_metagame_player_slot *other =
                &g_campaign_metagame_player_globals.slots[player];
            short player_total = 0;
            short i;
            for (i = 0; i < 8; i = (short)(i + 1)) {
                short sum = 0;
                short j;
                for (j = 0; j < 28; j = (short)(j + 1))
                    sum = (short)(scores->values[j][i] + sum);
                scores->sums[i] = sum;
                player_total = (short)(sum + player_total);
            }
            scores->valid = 1;
            scores->total_kills = player_total;
            short extra_total = 0;
            short k;
            for (k = 0; k < 8; k = (short)(k + 1)) {
                if (k != 2)
                    extra_total = (short)(scores->extra[k] + extra_total);
            }
            scores->extra_total = extra_total;
            int delta = scores->score - other->score + scores->awarded_bonus;
            float rate = globals->time_multiplier;
            scores->scaled = (int)((float)delta * rate);
            total = (float)scores->scaled + total;
        }
        (g_campaing_metagame_globals)->final_score = total;
        (g_campaing_metagame_globals)->flags |= 4;
        campaign_metagame_grant_score_achievements();
    }
}

// @address 0x219140 campaign_metagame_handle_game_won
// 0x219140

extern "C" void campaign_metagame_handle_game_won(void)
{
    if (g_campaing_metagame_globals == 0 || (g_campaing_metagame_globals->flags & 2) == 0)
        return;
    campaign_metagame_update_final_scores();
    void *memory = ::user_interface_malloc_tracked(sizeof(c_load_screen_message), (char const *)(0), 0);
    if (memory == 0)
        return;
    c_load_screen_message *message = new (memory)
        c_load_screen_message(0x100b9, k_any_controller, _console_window, 0x1000c);
    if (message == 0)
        return;
    ::user_interface_messaging_post(message);
    bool call = true;
    g_campaing_metagame_globals->flags |= 8;
    signed char simulation = tls_game_globals()->options.game_simulation;
    if (simulation == 2 || simulation == 4)
        call = false;
    if (call)
        game_time_set_pause_reason(1, 8);
}
static __forceinline int campaign_metagame_progress_out_of_range(long index, long count)
{
    if (index < 0) return 1;
    if (index >= count) return 1;
    return 0;
}
static __forceinline bool campaign_metagame_progress_controller_active(const c_controller_interface *controller)
{
    return ((const c_controller_interface *)controller)->is_signed_in_to_machine() || (_rotl((unsigned short)controller->flags.m_storage, 30) & 1) != 0;
}
static __forceinline bool campaign_metagame_progress_test(const c_player_profile_interface *profile, long table, long level, long difficulty)
{
    bool result = false;
    if ((level == -1 || level >= 0 && level < sizeof(profile->campaign.map_completed_flags[table])) &&
        !campaign_metagame_progress_out_of_range(difficulty, 4))
        result = level != -1 ? (profile->campaign.map_completed_flags[table][level] & (1 << difficulty)) != 0 : false;
    return result;
}
static __forceinline void campaign_metagame_progress_or(unsigned long *bits, unsigned long bit)
{
    *bits |= bit;
}
static __forceinline void campaign_metagame_progress_or_ref(unsigned long *bits, const unsigned long &bit)
{
    *bits |= bit;
}

// @address 0x217bd0 campaign_metagame_get_highest_completed_difficulty
extern "C" long campaign_metagame_get_highest_completed_difficulty(long controller_index)
{
    long last_level = (short)levels_get_campaign_map_index(1, 0xc30);
    c_controller_interface *controller =
        controller_get((e_controller_index)controller_index);
    long result = -1;
    if (campaign_metagame_progress_controller_active(controller)) {
        if (last_level >= 0) {
            unsigned long first[4] = {};
            unsigned long second[4] = {};
            unsigned long mask = (1i64 << (last_level + 1)) - 1;
            const c_player_profile_interface *profile =
                (const c_player_profile_interface *)controller_get((e_controller_index)controller_index)->get_player_profile_interface();
            for (long difficulty = 0; difficulty < 4; difficulty++) {
                for (long level = 0; level <= last_level; level++) {
                    bool a = campaign_metagame_progress_test(profile, 0, level, difficulty);
                    bool b = campaign_metagame_progress_test(profile, 1, level, difficulty);
                    if (a) campaign_metagame_progress_or(&first[difficulty], 1UL << level);
                    else first[difficulty] &= ~(1UL << level);
                    if (b) campaign_metagame_progress_or_ref(&second[difficulty], 1UL << level);
                    else second[difficulty] &= ~(1UL << level);
                }
            }
            for (long difficulty = 3; difficulty >= 0; difficulty--) {
                unsigned long completed = 0;
                for (long higher = difficulty; higher < 4; higher++)
                    completed = second[higher] | first[higher] | completed;
                if ((completed & mask) == mask) {
                    result = difficulty;
                    break;
                }
            }
        }
    }
    return result;
}


// @address 0x2187f0 campaign_metagame_try_and_get_kill_definition
extern "C" s_campaign_metagame_damage_definition *campaign_metagame_try_and_get_kill_definition(long object_index)
{
    s_campaign_metagame_damage_definition *result = 0;
    s_object_header *header = object_header_get(object_index);
    long type_0 = header->object_type;
    long type = result ? type_0 : type_0;
    long type_mask = 1 << type;
    if (type_mask & 0x1003) {
        unit_datum *unit = (unit_datum *)header->datum;
        unit_definition *definition = (unit_definition *)tag_get('unit', unit->object.definition_index);
        if (definition->campaign_metagame.count == 1)
            result = (s_campaign_metagame_damage_definition *)definition->campaign_metagame.address;
        if (unit->unit.actor_index != -1) {
            actor_datum *actor_0 = &((actor_datum *)tls_actor_data()->data)[datum_index_to_absolute_index(unit->unit.actor_index)];
            actor_datum *actor = (type_mask & 0x1003) ? actor_0 : actor_0;
            long character_index = actor->character_index;
            if (character_index != -1) {
                character_definition *character = (character_definition *)tag_get('char', character_index);
                for (;;) {
                    if (character->campaign_metagame.count == 1) {
                        result = (s_campaign_metagame_damage_definition *)character->campaign_metagame.address;
                        break;
                    }
                    if (character->parent_character.index == -1)
                        break;
                    character = (character_definition *)tag_get('char', character->parent_character.index);
                }
            }
        }
    } else if (type_mask & 0x800) {
        creature_definition *definition = (creature_definition *)tag_get('crea',
            ((unit_datum *)header->datum)->object.definition_index);
        if (definition->campaign_metagame.count == 1)
            result = (s_campaign_metagame_damage_definition *)definition->campaign_metagame.address;
    } else if (type_mask & 0x400) {
        projectile_definition *definition = (projectile_definition *)tag_get('proj',
            ((unit_datum *)header->datum)->object.definition_index);
        if (definition->campaign_metagame.count == 1)
            result = (s_campaign_metagame_damage_definition *)definition->campaign_metagame.address;
    }
    return result;
}

// 0x2180e8: score a campaign kill for the responsible player and record it in the kill log
static __forceinline long campaign_metagame_object_type_mask(long object_index)
{
    return 1 << ((s_object_header *)tls_object_header_data()->data)[datum_index_to_absolute_index(object_index)].object_type;
}
static __forceinline float campaign_metagame_medal_multiplier(const s_campaign_metagame_definition *config, long medal)
{
    float multiplier = 1.0f;
    multiplier *= (medal != -1) ? config->style_multipliers.begin()[medal] : 1.0f;
    if (multiplier == 0.0f)
        multiplier = 1.0f;
    return multiplier;
}
static __forceinline void campaign_metagame_set_points(s_campaing_metagame_player *player, float points)
{
    player->points = (long)points;
    player->timer1 = points > 0.0f ? tls_game_time_globals()->ticks_per_second * 2 : 0;
}
// @address 0x2180e8 campaign_metagame_handle_kill
extern "C" void campaign_metagame_handle_kill(
    long object_index, long player_index, long damage_owner_index, long, unsigned char damage_flags, long, long)
{
    const s_campaign_metagame_definition *config = (const s_campaign_metagame_definition *)campaign_metagame_scale_block_get();
    if (tls_game_globals()->options.game_mode != 1 || !config || config->style_multipliers.s_tag_block::count != 8)
        return;
    if (g_campaing_metagame_globals->flags & 1)
        return;

    if (damage_owner_index != -1 && object_try_and_get_and_verify_type(damage_owner_index, 0x1003))
    {
        unit_datum *unit = OBJECT_GET(damage_owner_index, unit_datum, _object_mask_unit);
        if (unit->unit.actor_index != -1 && (unsigned short)unit->unit.parent_seat_index != 0xffff && unit->object.parent_object_index != -1 &&
            !unit_seat_test_flag(unit->object.parent_object_index, unit->unit.parent_seat_index, _unit_seat_boarding_seat_bit))
        {
            long parent_index = unit->object.parent_object_index;
            long vehicle_index;
            do
            {
                vehicle_index = parent_index;
                parent_index = OBJECT_GET(vehicle_index, unit_datum, _object_mask_unit)->object.parent_object_index;
            } while (parent_index != -1);
            if (campaign_metagame_object_type_mask(vehicle_index) & 0x1003)
            {
                long driver_seat = unit_get_driver_seat(vehicle_index);
                if (driver_seat != -1)
                {
                    long driver_index = unit_seat_get_occupant(vehicle_index, driver_seat);
                    if (driver_index != -1 && (campaign_metagame_object_type_mask(driver_index) & 0x1003))
                    {
                        unit_datum *driver = OBJECT_GET(driver_index, unit_datum, _object_mask_unit);
                        if (driver->unit.player_index != -1 && game_team_is_ally((e_game_team)driver->unit.team, (e_game_team)unit->unit.team))
                            player_index = driver->unit.player_index;
                    }
                }
            }
        }
    }

    player_datum *player = player_index != -1 ? (player_datum *)datum_try_and_get(tls_player_data(), player_index) : 0;
    if (player && player->unit_index != -1 && (campaign_metagame_object_type_mask(object_index) & 0x1803))
    {
        long victim_type = ((s_object_header *)tls_object_header_data()->data)[datum_index_to_absolute_index(object_index)].object_type;
        unit_datum *killer = OBJECT_GET(player->unit_index, unit_datum, _object_mask_unit);
        long victim_team = -1;
        if ((1 << victim_type) & 0x1003)
            victim_team = OBJECT_GET(object_index, unit_datum, _object_mask_unit)->unit.team;
        else if ((1 << victim_type) & 0x800)
            victim_team = OBJECT_GET(object_index, creature_datum, _object_mask_creature)->creature.team;
        bool enemy = game_team_is_enemy((e_game_team)killer->unit.team, (e_game_team)victim_team) &&
            !game_team_is_ally((e_game_team)killer->unit.team, (e_game_team)victim_team);
        long medal = campaign_metagame_compute_medal(damage_flags);
        if (!enemy)
        {
            if (campaign_metagame_object_type_mask(object_index) & 0x1003)
            {
                unit_datum *victim = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
                long victim_player_index = victim->unit.player_index;
                long victim_actor_index = victim->unit.actor_index;
                if (victim_player_index != -1)
                {
                    s_campaing_metagame_player *owner = campaign_metagame_try_and_get_owner_slot(g_campaing_metagame_globals, player_index, 1);
                    s_campaign_metagame_player_slot *killer_slot = campaign_metagame_try_and_get_player_slot(
                        &g_campaign_metagame_player_globals, player_index, 1);
                    float multiplier = campaign_metagame_medal_multiplier(config, medal);
                    float scale = campaign_metagame_get_score_scale() * multiplier;
                    s_campaign_metagame_player_slot *victim_slot = campaign_metagame_try_and_get_player_slot(
                        &g_campaign_metagame_player_globals, victim_player_index, 1);
                    victim_slot->deaths++;
                    if (victim_player_index != player_index)
                    {
                        float points = (float)config->betrayal_points * scale;
                        campaign_metagame_set_points(owner, points);
                        campaign_metagame_update_player_score(player_index, points);
                        killer_slot->betrayals++;
                    }
                }
                else if (victim_actor_index != -1)
                {
                    s_campaing_metagame_player *owner = campaign_metagame_try_and_get_owner_slot(g_campaing_metagame_globals, player_index, 1);
                    owner->ai_kills++;
                    const s_campaign_metagame_damage_definition *definition =
                        (const s_campaign_metagame_damage_definition *)campaign_metagame_try_and_get_kill_definition(object_index);
                    if (definition)
                    {
                        float scale = campaign_metagame_get_score_scale();
                        long points = (long)((float)abs(definition->score) + 0.5f);
                        float award = scale * (float)points;
                        campaign_metagame_set_points(owner, award);
                        campaign_metagame_update_player_score(player_index, award);
                    }
                }
            }
        }
        if (enemy || victim_team == -1)
        {
            const s_campaign_metagame_damage_definition *definition = campaign_metagame_try_and_get_kill_definition(object_index);
            if (definition && (!(definition->flags & 1) || victim_team != -1))
            {
                s_campaing_metagame_globals *globals = g_campaing_metagame_globals;
                long event_index = globals->event_count;
                if (event_index < 256)
                {
                    globals->event_count = event_index + 1;
                    s_campaing_metagame_damage_event *event = &globals->events[event_index];
                    event->definition = *definition;
                    event->player_index = player_index;
                    event->time = tls_game_time_globals()->time;
                    event->style = (signed char)medal;
                    event->multikill = (campaign_metagame_object_type_mask(object_index) & 0x1003)
                        ? OBJECT_GET(object_index, unit_datum, _object_mask_unit)->unit.emp_shield_stun_ticks != 0
                        : false;
                    player_iterator iterator;
                    iterator.player_index = player_index;
                    iterator.controller_index = -1;
                    while (player_iterate_output_users(&iterator))
                    {
                        long controller_index = iterator.controller_index;
                        if (medal != -1 && controller_index >= 0)
                            chud_messaging_post_medal_for_user(controller_index, medal);
                        if (event->multikill && controller_index >= 0)
                            chud_messaging_post_medal_for_user(controller_index, 7);
                    }
                }
            }
        }
    }

    if (campaign_metagame_object_type_mask(object_index) & 0x1003)
    {
        unit_datum *victim = OBJECT_GET(object_index, unit_datum, _object_mask_unit);
        long medal = campaign_metagame_compute_medal(damage_flags);
        if (victim->unit.player_index != -1 && (player_index == -1 || player_index == victim->unit.player_index))
        {
            s_campaing_metagame_player *owner = campaign_metagame_try_and_get_owner_slot(g_campaing_metagame_globals, victim->unit.player_index, 1);
            s_campaign_metagame_player_slot *victim_slot = campaign_metagame_try_and_get_player_slot(
                &g_campaign_metagame_player_globals, victim->unit.player_index, 1);
            float multiplier = campaign_metagame_medal_multiplier(config, medal);
            float scale = campaign_metagame_get_score_scale() * multiplier;
            float points = scale * (float)config->suicide_points;
            campaign_metagame_set_points(owner, points);
            campaign_metagame_update_player_score(victim->unit.player_index, points);
            victim_slot->deaths++;
        }
    }
}

// 0x118e90: the owner slot as the kill scoring sees it.

#define BIT_VECTOR_TEST_FLAG(vector, index) (((vector)[(index) >> 5] & (1 << ((index) & 31))) != 0)
#define BIT_VECTOR_SET_FLAG(vector, index, value) \
    ((value) ? ((vector)[(index) >> 5] |= (1 << ((index) & 31))) : ((vector)[(index) >> 5] &= ~(1 << ((index) & 31))))

static __forceinline bool campaign_metagame_finished(void)
{
    bool result = false;
    s_campaing_metagame_globals *globals = g_campaing_metagame_globals;
    if (globals && (globals->flags & 2))
        result = ((unsigned long)globals->flags >> 3) & 1;
    return result;
}

// Ages every player's timers, scores the kills whose window has closed, then drops the spent events.
// @address 0x118e90 ?campaign_metagame_update@@YAXXZ
void campaign_metagame_update()
{
    if (campaign_metagame_finished())
        return;
    s_campaing_metagame_globals *globals = g_campaing_metagame_globals;
    if (!globals || !(globals->flags & 2) || (globals->flags & 4))
        return;
    s_campaign_metagame_definition *config = (s_campaign_metagame_definition *)campaign_metagame_scale_block_get();
    if (game_is_campaign() && config && config->style_multipliers.s_tag_block::count == 8) {
        long time = game_time_get();
        long cutoff = campaign_metagame_seconds_to_ticks_round(config->window_seconds);
        short remaining[256];
        unsigned long seen[8];
        memset(seen, 0, sizeof(seen));
        unsigned long removed[8];
        memset(removed, 0, sizeof(removed));
        c_data_iterator<player_datum> iterator;
        iterator.begin((s_data_array *)tls_player_data());
        if (!(globals->flags & 1))
            g_campaign_metagame_player_globals.ticks++;
        while (iterator.next()) {
            s_campaing_metagame_player *slot = campaign_metagame_try_and_get_owner_slot(
                g_campaing_metagame_globals, iterator.get_index(), 1);
            bool expired = false;
            long count = 0;
            if (slot->timer0 > 0)
                --slot->timer0;
            if (slot->timer1 > 0)
                --slot->timer1;
            if (slot->timer2 > 0)
                --slot->timer2;
            campaign_metagame_try_and_get_player_slot(&g_campaign_metagame_player_globals, iterator.get_index(), 1);
            for (long i = 0; i < g_campaing_metagame_globals->event_count; ++i) {
                s_campaing_metagame_damage_event &event = g_campaing_metagame_globals->events[i];
                if (g_campaing_metagame_globals->events[i].player_index == iterator.get_index()) {
                    if (!BIT_VECTOR_TEST_FLAG(seen, i)) {
                        BIT_VECTOR_SET_FLAG(seen, i, true);
                        ++count;
                    }
                    if (event.time < time - cutoff || event.time > time)
                        expired = true;
                }
            }
            slot->pending = 0;
            bool enough = count >= config->minimum_kills;
            float base = enough ? config->style_multipliers.begin()[5] : 0.0f;
            if (expired && enough) {
                slot->timer0 = tls_game_time_globals()->ticks_per_second * 2;
                slot->style_kills[5]++;
                if (tls_actor_globals()->enabled) {
                    player_datum *record = &((player_datum *)tls_player_data()->data)[datum_index_to_absolute_index(iterator.get_index())];
                    c_dialogue_event event(0x6f, record->unit_index, -1, -1, -1, 0);
                    ai_dialogue_event(&event);
                }
            }
            for (long i = 0; i < g_campaing_metagame_globals->event_count; ++i) {
                s_campaing_metagame_damage_event &event = g_campaing_metagame_globals->events[i];
                if (g_campaing_metagame_globals->events[i].player_index == iterator.get_index()) {
                    float multiplier = base;
                    long style = event.style;
                    if (style != -1) {
                        float style_multiplier = config->style_multipliers.begin()[style];
                        multiplier += style_multiplier;
                        if (expired)
                            slot->style_kills[style]++;
                    }
                    if (event.multikill) {
                        float multikill_multiplier = config->style_multipliers.begin()[7];
                        multiplier += multikill_multiplier;
                        if (expired)
                            slot->style_kills[7]++;
                    }
                    if (multiplier == 0.0f)
                        multiplier = 1.0f;
                    float points = (float)event.score * multiplier;
                    if (expired)
                        slot->bucket_kills[event.category][event.kind]++;
                    float scale = campaign_metagame_get_score_scale();
                    long rounded = (long)(points + 0.5f);
                    long score = (long)(scale * (float)rounded);
                    if (expired) {
                        slot->awarded_bonus = score + slot->awarded_bonus;
                        float bonus_score = (float)score;
                        float reroll = campaign_metagame_get_score_scale();
                        long base_points = (long)((float)event.score + 0.5f);
                        long residual = (long)(bonus_score - reroll * (float)base_points);
                        slot->bonus_difference = residual + slot->bonus_difference;
                        BIT_VECTOR_SET_FLAG(removed, i, true);
                    } else {
                        slot->pending = score + slot->pending;
                    }
                }
            }
        }
        s_campaing_metagame_globals *metagame = g_campaing_metagame_globals;
        long event_count = metagame->event_count;
        long retained = 0;
        for (long i = 0; i < event_count; ++i) {
            if (!BIT_VECTOR_TEST_FLAG(seen, i))
                BIT_VECTOR_SET_FLAG(removed, i, true);
            if (!BIT_VECTOR_TEST_FLAG(removed, i))
                remaining[retained++] = (short)i;
        }
        for (long i = 0; i < retained; ++i)
            metagame->events[i] = metagame->events[remaining[i]];
        metagame->event_count = retained;
    }
}

t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_campaing_metagame_allocator> g_campaing_metagame_allocator;

// @address 0x11df6c4 g_campaing_metagame_globals
__declspec(thread) s_campaing_metagame_globals *g_campaing_metagame_globals;
