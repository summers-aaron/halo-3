// blamlibxbox/multiplayer_game_hopper.obj: from the records 0x92b10 0x934a8 0x3c49a0 0x57aa80 0x57ac90 0x57ae00 0x57af18 0x57afa8 0x57b380 0x57b530 0x57b688 0x57b878 0x57b8e0 0x57b9b0 0x57ba88 0x57bb58 0x57bd80 0x57bea0 0x57c118 0x57c2d0 0x57c3d8 0x57c588 0x57c940 0x57cf28 0x57cf88 0x57de48 0x57df38 0x57e0f0 0x57e478 0x57e548 0x57e618
#include "networking/online/online_url_version.h"
#include "networking/online/online_cached_user.h"
#include "networking/online/online_cached_user_globals.h"
#include "networking/logic/network_banhammer.h"
#include "networking/logic/network_banhammer_controller_types.h"
#include "memory/static_arrays.h"
#include "networking/logic/storage/hopper_load_status_types.h"
#include "math/random_math.h"
#include "game/game_types.h"
#include "shell/shell.h"
#include "game/map_variant.h"
#include "cseries/crt_xdk.h"
#if !defined(_XBOX) || defined(__clang__)
#include "cseries/byte_swapping.h"
#endif
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "game/multiplayer_game_hopper.h"
#include "game/multiplayer_game_hopper_globals.h"
#include "game/multiplayer_game_hopper_playability.h"
#include "networking/online/online_xenon.h"
#include "interface/user_interface.h"
#include "networking/logic/logic_matchmaking_composition.h"
#include "networking/logic/network_life_cycle.h"
#include "scenario/scenario_map_variant.h"
#include "simulation/simulation.h"
#include "networking/session/network_session_membership.h"
#include "networking/session/network_session_parameters.h"
#include "game/game_variant.h"
#include "memory/bitstream.h"
#include "interface/utf8_types.h"
#include "networking/logic/network_leaderboard.h"
#include "networking/logic/network_leaderboard_globals_types.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "networking/network_configuration.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"
#include "cseries/language.h"
#include "networking/session/network_session_membership_types.h"
#include "networking/network_configuration_types.h"
#include "networking/tools/network_blf.h"
#include "networking/logic/storage/network_http_buffer_url.h"
#include "networking/logic/network_session_interface.h"
#include "game/multiplayer_game_hopper_download_storage.h"
#include "main/levels_globals.h"
#include "cseries/cseries.h"
#include "game/hopper_configuration.h"
#include "networking/tools/blf_authentication_types.h"
#include "game/multiplayer_hopper_services.h"
#include <new>
#include "xbox/system_time.h"
#include "networking/session/session_game_start_error_types.h"
#include "networking/logic/network_session_interface_globals.h"
#include "interface/controller_inline.h"

struct s_hopper_request_descriptor {
  c_static_string<256> name;
  long first;
  long second;
  long timeout;
  long fourth;
};

struct s_hopper_pick_filters { bool first; bool second; bool third; };

enum {
  multiplayer_game_hopper_download_complete = 2
};

struct multiplayer_hopper_check {
    unsigned long players;
        unsigned long guests;
        unsigned long statistics_valid;
        unsigned long status_valid;
        unsigned long hopper_valid;
        long hopper_error;

    unsigned long valid_players;
        unsigned long language;
        unsigned long country_code;
        unsigned long gamerzone;

    unsigned long experience;
    unsigned long experience_penalty;
    unsigned long skill;
    unsigned long skill_other;
    unsigned long access;
    unsigned long live_access;
    unsigned long content;
};

struct s_hopper_url_local : s_hopper_request_descriptor
{
    __forceinline s_hopper_url_local()
    {
        name.m_string[0] = 0;
        first = 0;
        second = 0;
        timeout = 600;
        fourth = 0;
    }
};

struct s_hopper_http_url : s_network_http_buffer_url
{
    __forceinline s_hopper_http_url()
    {
        service_type = 0;
        cache_mode = 0;
        lifetime_seconds = 600;
        queue_type = 0;
        name[0] = 0;
    }
};

struct s_multiplayer_game_hopper_manifest_file
{
    long expiry;
    void *data;
    bool failed;
};

// @flags /TP /O2 /Oc
// @address 0x2212e0 ?read_bits_internal@c_bitstream@@AAAXPAEJ@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x3c3100 ?reset@c_http_buffer_downloader@@QAAXXZ
// @address 0x932e8 ?get_data@c_http_buffer_downloader@@QAAJPAPBDPAJ@Z
// @address 0x57af18 ?multiplayer_game_hoppers_get_hopper_configuration@@YAPBVc_hopper_configuration@@G@Z
// @address 0x57bb58 ?multiplayer_game_hopper_compute_skill_update_weight@@YAJJJ@Z
// @address 0x57bd80 ?multiplayer_game_hopper_compute_quality_adjusted_skill_update_weight@@YAJJJ@Z
// @address 0x57cf28 ?multiplayer_game_hopper_get_description@@YAPBUutf8@@G@Z
// @address 0x57b878 ?multiplayer_game_hopper_compute_skill_match_delta@@YAJJ@Z
enum e_network_game_start_mode;


typedef char hopper_table_size[(sizeof(s_hopper_configuration_table_entry) == 0x260) ? 1 : -1];
typedef char hopper_table_hash_offset[(offsetof(s_hopper_configuration_table_entry, game_set_hash) == 0x20) ? 1 : -1];
typedef char hopper_table_sigma_offset[(offsetof(s_hopper_configuration_table_entry, runtime_sigma_multiplier) == 0x1e0) ? 1 : -1];

static __forceinline bool multiplayer_game_hopper_history_controller_valid(long controller_index)
{
    return (g_network_banhammer_globals.controllers[controller_index].flags & 1) != 0;
}
static __forceinline long multiplayer_game_hopper_history_controller_flags(long controller_index)
{
  long flags = 0;
  if (multiplayer_game_hopper_history_controller_valid(controller_index))
    flags = g_network_banhammer_globals.controllers[controller_index].ban_flags |
        g_network_banhammer_globals.ban_flags;
  return flags;
}
static __forceinline s_network_banhammer_repeated_play_list *multiplayer_game_hopper_history_controller_get_history(long controller_index)
{
  s_network_banhammer_repeated_play_list *history = 0;
  if (multiplayer_game_hopper_history_controller_valid(controller_index))
    history = &g_network_banhammer_globals.controllers[controller_index].repeated_play_list;
  return history;
}


static __forceinline long hopper_team_capacity(const s_hopper_team_configuration *team_configuration)
{
  return team_configuration->maximum_team_size * team_configuration->team_count;
}


static __forceinline s_hopper_update_globals &hopper_update_globals()
{
    return *(s_hopper_update_globals *)&g_multiplayer_game_hopper_globals;
}


// @address 0x3ada60 ?network_blf_find_chunk_in_file@@YAPAXPBDKJFW4e_blf_file_authentication_type@@PAK@Z

// @address 0x934a8 ?set_hash@c_http_buffer_downloader@@QAAXPBUs_network_http_request_hash@@@Z
void c_http_buffer_downloader::set_hash(const s_network_http_request_hash *hash)
{
  if ((!hash && m_hash_valid == true) || (hash && (!m_hash_valid || memcmp(hash, &m_hash, sizeof(m_hash)) != 0))) {
    reset();
    if (!hash) {
      m_hash_valid = false;
    } else {
      m_hash_valid = true;
      memcpy(&m_hash, hash, sizeof(m_hash));
    }
  }
}


// @address 0x3c49a0 multiplayer_game_hopper_initialize_game_set_request
extern "C" void multiplayer_game_hopper_initialize_game_set_request(s_hopper_request_descriptor *hopper_request_descriptor, unsigned short p1)
{
  online_url_set_base_path(hopper_request_descriptor);
  hopper_request_descriptor->name.append_print("%05u/game_set_%03u.bin", p1, 6);
  hopper_request_descriptor->second = 1;
  hopper_request_descriptor->fourth = 0;
}

// @address 0x57ae00 multiplayer_game_hopper_get_load_status
extern "C" long multiplayer_game_hopper_get_load_status(void)
{
  long result = 0;
  if (hopper_update_globals().configuration_failed)
    result = 3;
  else if (hopper_update_globals().configuration_valid)
    result = 2;
  else if (network_http_buffer_downloader_get_public_status(hopper_update_globals().configuration_downloader.m_internal_status) == 1)
    result = 1;
  return result;
}

const c_hopper_configuration *multiplayer_game_hoppers_get_hopper_configuration(unsigned short hopper_identifier)
{
  const c_hopper_configuration *configuration = 0;
  if (multiplayer_game_hoppers_get_current_hopper_configuration() &&
      multiplayer_game_hoppers_get_current_hopper_configuration()->get_hopper_identifier() == hopper_identifier)
    configuration = multiplayer_game_hoppers_get_current_hopper_configuration();
  else if (hopper_update_globals().configuration.count != 0) {
    long count = hopper_update_globals().configuration.count;
    for (long index = 0; index < count; ++index) {
      if (hopper_update_globals().configuration.entries[index].get_hopper_identifier() == hopper_identifier) {
        configuration = &hopper_update_globals().configuration.entries[index];
        break;
      } else { continue; }
    }
  }
  return configuration;
}


static __forceinline c_http_buffer_downloader *multiplayer_game_hopper_request_downloader()
{
  return &hopper_update_globals().game_set_downloader;
}

// @address 0x57ac90 multiplayer_game_hopper_set_active_hopper_and_request_game_set
extern "C" bool multiplayer_game_hopper_set_active_hopper_and_request_game_set(unsigned short hopper_identifier, bool choose_fallback, const void *membership)
{
  s_hopper_request_descriptor descriptor;
  descriptor.name.m_string[0] = 0;
  descriptor.first = 0;
  descriptor.second = 0;
  descriptor.timeout = 600;
  descriptor.fourth = 0;
  const c_hopper_configuration *configuration = multiplayer_game_hoppers_get_hopper_configuration(hopper_identifier);
  if (!configuration && choose_fallback) {
    long index;
    for (index = 0; index < hopper_update_globals().configuration.count; ++index) {
      configuration = &hopper_update_globals().configuration.entries[index];
      if (multiplayer_game_hopper_is_hopper_visible(configuration->get_hopper_identifier(), membership))
        break;
    }
    if (index == hopper_update_globals().configuration.count)
      configuration = &hopper_update_globals().configuration.entries[0];
    else if (!hopper_update_globals().configuration.count)
      configuration = 0;
  }
  if (configuration) {
    multiplayer_game_hopper_initialize_game_set_request(&descriptor, configuration->get_hopper_identifier());
    http_buffer_downloader_set_url(multiplayer_game_hopper_request_downloader(), (const s_network_http_buffer_url *)&descriptor);
    multiplayer_game_hopper_request_downloader()->set_hash(&configuration->m_configuration.game_set_hash);
  } else {
    descriptor.name.m_string[0] = 0;
    http_buffer_downloader_set_url(multiplayer_game_hopper_request_downloader(), (const s_network_http_buffer_url *)&descriptor);
    if (hopper_update_globals().game_set_downloader.m_hash_valid == 1) {
      multiplayer_game_hopper_request_downloader()->reset();
      hopper_update_globals().game_set_downloader.m_hash_valid = 0;
    }
  }
  multiplayer_game_hopper_request_downloader()->get_data();
  g_multiplayer_game_hopper_globals.m_current_configuration = configuration;
  return false;
}


static __forceinline bool multiplayer_game_hopper_level_exists(long map_id, s_datum *level)
{
  return levels_try_and_get_datum(g_level_globals.multiplayer_levels, map_id, level);
}
static __forceinline bool multiplayer_game_hopper_game_entry_excluded(const s_hopper_game_entry *entry,
    const s_game_hopper_picked_game_collection *collection, long picked_count, long map_mask, bool allow_duplicate_map,
    bool allow_duplicate_variant, bool allow_duplicate_both)
{
  bool excluded = false;
  bool map_picked = false;
  bool variant_picked = false;
  for (long index = 0; index < picked_count; ++index) {
    if (entry->map_id == collection->games[index].map_id)
      map_picked = true;
    if (strcmp(entry->game_variant_name, collection->games[index].game_variant_name) == 0)
      variant_picked = true;
  }
  bool in_mask = false;
  long map_index = -1;
  long index = 0;
  const s_network_configuration_map_entry *map = g_network_configuration.map_table;
  for (; index < 32; ++index, ++map) {
    if (map->map_id == entry->map_id) {
      map_index = index;
      break;
    }
  }
  if ((1u << map_index) & map_mask)
    in_mask = true;
  if (!in_mask)
    excluded = true;
  if (map_picked && !allow_duplicate_map)
    excluded = true;
  if (variant_picked && !allow_duplicate_variant)
    excluded = true;
  if (map_picked && variant_picked && !allow_duplicate_both)
    excluded = true;
  return excluded;
}

// @address 0x57afa8 multiplayer_game_hopper_pick_random_game_entry
extern "C" s_hopper_game_entry *multiplayer_game_hopper_pick_random_game_entry(s_game_hopper_picked_game_collection *collection,
    long picked_count, long player_count, s_hopper_game_set *game_set, long map_mask, bool picked, bool allow_duplicate_map,
    bool allow_duplicate_variant, bool allow_duplicate_both)
{
  s_hopper_game_entry *result = 0;
  long count = game_set->count;
  long total_weight = 0;
  for (long index = 0; index < count; ++index) {
    s_hopper_game_entry *entry = &game_set->entries[index];
    s_datum level;
    bool excluded = multiplayer_game_hopper_game_entry_excluded(
        entry, collection, picked_count, map_mask, allow_duplicate_map, allow_duplicate_variant, allow_duplicate_both);
    if ((!picked || !entry->optional) && player_count >= entry->minimum_player_count && !excluded &&
        multiplayer_game_hopper_level_exists(entry->map_id, &level))
      total_weight += entry->weight;
  }
  if (total_weight > 0) {
    long roll = random_range(0, (short)total_weight);
    for (long index = 0; index < count; ++index) {
      s_hopper_game_entry *entry = &game_set->entries[index];
      s_datum level;
      bool excluded = multiplayer_game_hopper_game_entry_excluded(
          entry, collection, picked_count, map_mask, allow_duplicate_map, allow_duplicate_variant, allow_duplicate_both);
      if ((!picked || !entry->optional) && player_count >= entry->minimum_player_count && !excluded &&
          multiplayer_game_hopper_level_exists(entry->map_id, &level)) {
        if (roll < entry->weight)
          { result = entry; break; }
        roll -= entry->weight;
      }
    }
  }
  return result;
}
static __forceinline long multiplayer_game_hopper_compute_displayed_skill_level(float skill_mean, float skill_sigma, long displayed_skill, long experience)
{
  const c_hopper_configuration *configuration = multiplayer_game_hoppers_get_current_hopper_configuration();
  long skill_level = -1;
  if (configuration) {
    const s_hopper_configuration_table_entry *values = configuration->get_hopper_configuration();
    skill_level = multiplayer_game_hopper_compute_skill_level(skill_mean, skill_sigma, experience);
    if (skill_level == displayed_skill - 1) {
      float skill = (skill_mean - configuration->get_trueskill_sigma_multiplier() * skill_sigma) * 8.3333f;
      float midpoint;
      if (displayed_skill == 50)
        midpoint = (float)((values->skill_thresholds[48] - values->skill_thresholds[47]) * 0.5 + values->skill_thresholds[48]);
      else
        midpoint = (float)((values->skill_thresholds[displayed_skill - 1] - values->skill_thresholds[skill_level - 1]) * 0.5 +
            values->skill_thresholds[skill_level - 1]);
      if (skill >= midpoint)
        skill_level = displayed_skill;
    }
    if (skill_level < 1)
      skill_level = 1;
    else if (skill_level > 50)
      skill_level = 50;
  }
  return skill_level;
}

// @address 0x57b380 ?multiplayer_game_hoppers_pick_random_game_collection@@YA_NJJPAUs_game_hopper_picked_game_collection@@@Z
bool multiplayer_game_hoppers_pick_random_game_collection(long player_count, long mode, struct s_game_hopper_picked_game_collection * collection)
{
  bool result = true;
  bool picked = false;
  memset(collection, 0, sizeof(*collection));
  s_hopper_game_set *game_set = &hopper_update_globals().game_set.decoded;
  if (game_set->count <= 0) {
    result = false;
  } else {
    for (long i = 0; i < game_set->count; ++i) {
      s_datum datum;
      levels_try_and_get_datum(g_level_globals.multiplayer_levels, game_set->entries[i].map_id, (s_datum *)&datum);
    }
    long pick_index = 0;
    s_hopper_pick_filters filters[4];
    filters[0].first = false;
    filters[0].second = false;
    filters[0].third = false;
    filters[1].first = false;
    filters[1].second = true;
    filters[1].third = false;
    filters[2].first = true;
    filters[2].second = false;
    filters[2].third = false;
    filters[3].first = true;
    filters[3].second = true;
    filters[3].third = true;
    for (; pick_index < 4; ++pick_index) {
      s_hopper_game_entry *entry = 0;
      for (unsigned int attempt = 0; attempt < 4; ++attempt) {
        entry = multiplayer_game_hopper_pick_random_game_entry(collection, pick_index, player_count, game_set, mode, picked,
                             filters[attempt].first, filters[attempt].second, filters[attempt].third);
        if (entry)
          break;
      }
      if (!entry) {
        result = false;
        break;
      }
      csstrnzcpy(collection->games[pick_index].game_variant_name, entry->game_variant_name, 32);
      collection->games[pick_index].game_variant_hash = entry->game_variant_hash;
      csstrnzcpy(collection->games[pick_index].map_variant_name, entry->map_variant_name, 32);
      collection->games[pick_index].map_variant_hash = entry->map_variant_hash;
      collection->games[pick_index].map_id = entry->map_id;
      picked = true;
    }
  }
  return result;
}
// @address 0x57b530 ?multiplayer_game_hopper_build_calculated_stats@@YAXPBUs_queried_player_global_statistics@@PBUs_queried_player_hopper_statistics@@PAUs_calculated_player_statistics@@@Z
void multiplayer_game_hopper_build_calculated_stats(const s_queried_player_global_statistics *global_statistics,
    const s_queried_player_hopper_statistics *hopper_statistics, s_calculated_player_statistics *calculated_statistics)
{
  if (global_statistics->valid) {
    multiplayer_game_hopper_compute_experience_rank(global_statistics->experience_base, global_statistics->experience_penalty, global_statistics->highest_skill,
                &calculated_statistics->experience, &calculated_statistics->experience_rank, &calculated_statistics->experience_grade);
    calculated_statistics->global_valid = true;
  }
  if (hopper_statistics->valid && multiplayer_game_hoppers_get_current_hopper_configuration() &&
      multiplayer_game_hoppers_get_current_hopper_configuration()->get_hopper_identifier() == hopper_statistics->hopper_identifier) {
    calculated_statistics->skill_level = multiplayer_game_hopper_compute_skill_level(
        hopper_statistics->mu, hopper_statistics->sigma, hopper_statistics->games_won);
    calculated_statistics->displayed_skill_level = multiplayer_game_hopper_compute_displayed_skill_level(
        hopper_statistics->mu, hopper_statistics->sigma, hopper_statistics->old_skill, hopper_statistics->games_won);
    calculated_statistics->skill_update_weight = multiplayer_game_hopper_compute_skill_update_weight(
        hopper_statistics->games_played, calculated_statistics->skill_level);
    calculated_statistics->hopper_valid = true;
  }
}
// @address 0x57b688 ?multiplayer_game_hopper_compute_skill_level@@YAJMMJ@Z
long multiplayer_game_hopper_compute_skill_level(float skill_mean, float skill_deviation, long experience)
{
  long skill_level = -1;
  const c_hopper_configuration *configuration = multiplayer_game_hoppers_get_current_hopper_configuration();
  if (configuration) {
    const s_hopper_configuration_table_entry *universal_configuration = configuration->get_hopper_configuration();
    float skill = (skill_mean - configuration->get_trueskill_sigma_multiplier() * skill_deviation) * 8.3333f;
    long index;
    for (index = 48; index >= 0 && universal_configuration->skill_thresholds[index] > skill; --index) {
    }
    long trueskill_level = index + 2;
    long experience_index = 49;
    const long *thresholds = g_network_configuration.skill_level_experience_thresholds;
    while (experience_index >= 0 && !(thresholds[experience_index] > 0 && experience >= thresholds[experience_index]))
      --experience_index;
    long experience_level = experience_index + 1;
    long level = trueskill_level > experience_level ? trueskill_level : experience_level;
    if (level > 1 && level < 50)
      skill_level = level;
    else if (level <= 1)
      skill_level = 1;
    else
      skill_level = 50;
  }
  return skill_level;
}

long multiplayer_game_hopper_compute_skill_match_delta(long index)
{
  long skill_match_delta = 0;
  if (multiplayer_game_hoppers_get_current_hopper_configuration()) {
    const c_hopper_configuration *configuration = multiplayer_game_hoppers_get_current_hopper_configuration();
    if (configuration->is_ranked())
      skill_match_delta = configuration->get_maximum_skill_level_match_delta(index);
    else
      skill_match_delta = 50;
  }
  return skill_match_delta;
}

// @address 0x57b8e0 multiplayer_game_hopper_compute_experience_rank
extern "C" void multiplayer_game_hopper_compute_experience_rank(
    long score, long baseline, long level_points, long *difference_out, long *rank_out, long *grade_out)
{
  long difference = score - baseline;
  *difference_out = difference;
  if (score == 0) {
    *rank_out = 0;
    *grade_out = 0;
    return;
  }
  if (difference <= 0) {
    *rank_out = 1;
    *grade_out = 0;
    return;
  }
  long rank = 0;
  for (long index = 14; index-- > 0; ) {
    if (g_network_configuration.rank_experience_thresholds[index] <= level_points) {
      rank = index;
      break;
    }
  }
  long step;
  for (step = (rank << 2) | 3; step > 0; --step) {
    long threshold = g_network_configuration.rank_grade_thresholds[step];
    if (threshold != 0 && threshold <= difference)
      break;
  }
  *rank_out = step >> 2;
  *grade_out = step & 3;
}


// @address 0x57b9b0 multiplayer_game_hopper_get_next_rank_experience
extern "C" long multiplayer_game_hopper_get_next_rank_experience(long experience, long experience_penalty, long highest_skill)
{
  long next_threshold = -1;
  long experience_difference;
  long rank;
  long grade;
  multiplayer_game_hopper_compute_experience_rank(experience, 0, highest_skill, &experience_difference, &rank, &grade);
  for (long rank_index = rank; rank_index < 14; ++rank_index) {
    if (g_network_configuration.rank_experience_thresholds[rank_index] > highest_skill)
      continue;
    for (long step = rank_index << 2; step < (rank_index + 1) << 2; ++step) {
      long threshold = g_network_configuration.rank_grade_thresholds[step];
      if (threshold != 0 && threshold > experience_difference) {
        if (next_threshold == -1 || threshold < next_threshold)
          next_threshold = threshold;
      }
    }
  }
  return next_threshold;
}
long multiplayer_game_hopper_compute_skill_update_weight(long skill_delta, long skill)
{
  const c_hopper_configuration *configuration = multiplayer_game_hoppers_get_current_hopper_configuration();
  long trueskill_hillclimb_w0 = configuration->get_trueskill_hillclimb_w0();
  long trueskill_hillclimb_w50 = configuration->get_trueskill_hillclimb_w50();
  long trueskill_hillclimb_w100 = configuration->get_trueskill_hillclimb_w100();
  long trueskill_hillclimb_w150 = configuration->get_trueskill_hillclimb_w150();
  long hillclimb_weight;
  if (skill_delta < 50)
    hillclimb_weight = trueskill_hillclimb_w0 + (skill_delta * trueskill_hillclimb_w50 - skill_delta * trueskill_hillclimb_w0) / 50;
  else if (skill_delta < 100)
    hillclimb_weight = trueskill_hillclimb_w50 + ((skill_delta - 50) * trueskill_hillclimb_w100 - (skill_delta - 50) * trueskill_hillclimb_w50) / 50;
  else if (skill_delta < 150)
    hillclimb_weight = trueskill_hillclimb_w100 + ((skill_delta - 100) * trueskill_hillclimb_w150 - (skill_delta - 100) * trueskill_hillclimb_w100) / 50;
  else
    hillclimb_weight = trueskill_hillclimb_w150;
  if (hillclimb_weight <= 0) hillclimb_weight = 1;
  else if (hillclimb_weight > 100) hillclimb_weight = 100;
  long skill_update_weight_s0 = configuration->get_skill_update_weight_s0();
  long skill_update_weight_s10 = configuration->get_skill_update_weight_s10();
  long skill_update_weight_s20 = configuration->get_skill_update_weight_s20();
  long skill_update_weight_s30 = configuration->get_skill_update_weight_s30();
  long skill_update_weight_s40 = configuration->get_skill_update_weight_s40();
  long skill_update_weight_s50 = configuration->get_skill_update_weight_s50();
  long skill_weight;
  if (skill < 10)
    skill_weight = skill_update_weight_s0 + (skill * skill_update_weight_s10 - skill * skill_update_weight_s0) / 10;
  else if (skill < 20)
    skill_weight = skill_update_weight_s10 + ((skill - 10) * skill_update_weight_s20 - (skill - 10) * skill_update_weight_s10) / 10;
  else if (skill < 30)
    skill_weight = skill_update_weight_s20 + ((skill - 20) * skill_update_weight_s30 - (skill - 20) * skill_update_weight_s20) / 10;
  else if (skill < 40)
    skill_weight = skill_update_weight_s30 + ((skill - 30) * skill_update_weight_s40 - (skill - 30) * skill_update_weight_s30) / 10;
  else if (skill < 50)
    skill_weight = skill_update_weight_s40 + ((skill - 40) * skill_update_weight_s50 - (skill - 40) * skill_update_weight_s40) / 10;
  else
    skill_weight = skill_update_weight_s50;
  if (skill_weight <= 0) skill_weight = 1;
  else if (skill_weight > 100) skill_weight = 100;
  long weight = hillclimb_weight * skill_weight / 100;
  if (weight <= 0) weight = 1;
  else if (weight > 100) weight = 100;
  return weight;
}

long multiplayer_game_hopper_compute_quality_adjusted_skill_update_weight(long skill_update_weight, long game_quality)
{
  const c_hopper_configuration *configuration = multiplayer_game_hoppers_get_current_hopper_configuration();
  long game_quality_update_weight_q0 = configuration->get_game_quality_update_weight_q0();
  long game_quality_update_weight_q25 = configuration->get_game_quality_update_weight_q25();
  long game_quality_update_weight_q50 = configuration->get_game_quality_update_weight_q50();
  long game_quality_update_weight_q75 = configuration->get_game_quality_update_weight_q75();
  long game_quality_update_weight_q100 = configuration->get_game_quality_update_weight_q100();
  long quality_weight;
  if (game_quality < 25)
    quality_weight = game_quality_update_weight_q0 + (game_quality * game_quality_update_weight_q25 - game_quality * game_quality_update_weight_q0) / 25;
  else if (game_quality < 50)
    quality_weight = game_quality_update_weight_q25 +
        ((game_quality - 25) * game_quality_update_weight_q50 - (game_quality - 25) * game_quality_update_weight_q25) / 25;
  else if (game_quality < 75)
    quality_weight = game_quality_update_weight_q50 +
        ((game_quality - 50) * game_quality_update_weight_q75 - (game_quality - 50) * game_quality_update_weight_q50) / 25;
  else if (game_quality < 100)
    quality_weight = game_quality_update_weight_q75 +
        ((game_quality - 75) * game_quality_update_weight_q100 - (game_quality - 75) * game_quality_update_weight_q75) / 25;
  else
    quality_weight = game_quality_update_weight_q100;
  if (quality_weight <= 0) quality_weight = 1;
  else if (quality_weight > 100) quality_weight = 100;
  long weight = skill_update_weight * quality_weight / 100;
  if (weight <= 0) weight = 1;
  else if (weight > 100) weight = 100;
  return weight;
}

// @address 0xb5108 exp

// @address 0x57bea0 ?multiplayer_game_hopper_compute_match_quality@@YAJPBVc_network_session_membership@@@Z
long multiplayer_game_hopper_compute_match_quality(const c_network_session_membership *membership)
{
  const c_hopper_configuration *configuration = multiplayer_game_hoppers_get_current_hopper_configuration();
  float beta = configuration->get_trueskill_beta_performance_variation();
  float beta_squared = beta * beta;
  beta_squared *= 0.014400116f;
  long minimum_quality = 100;
  long team_count = 0;
  long last_team = 0;
  long teams[16];
  float team_player_counts[16];
  float team_skill_means[16];
  float team_skill_variances[16];
  memset(team_player_counts, 0, sizeof(team_player_counts));
  memset(team_skill_means, 0, sizeof(team_skill_means));
  memset(team_skill_variances, 0, sizeof(team_skill_variances));
  for (long player_index = (membership)->get_first_player(); player_index != -1;
      player_index = (membership)->get_next_player(player_index))
  {
    const s_network_session_player *player = membership->get_player(player_index);
    long team_index;
    for (team_index = 0; team_index < team_count; ++team_index) {
      if (player->configuration.host.team_index == teams[team_index])
        break;
    }
    if (team_index == team_count) {
      teams[team_count] = player->configuration.host.team_index;
      ++team_count;
    }
    team_player_counts[team_index] += 1.0f;
    team_skill_means[team_index] += player->configuration.client.hopper_statistics.mu;
    team_skill_variances[team_index] += player->configuration.client.hopper_statistics.sigma * player->configuration.client.hopper_statistics.sigma;
  }
  last_team = team_count - 1;
  if (last_team > 0) {
    for (long a = 0; a < last_team; ++a) {
      for (long b = a + 1; b < team_count; ++b) {
        float total_players = team_player_counts[a] + team_player_counts[b];
        float c = total_players * beta_squared;
        float mean_delta = team_skill_means[a] - team_skill_means[b];
        float denominator = team_skill_variances[a] + team_skill_variances[b] + c;
        float quality = exp(mean_delta * mean_delta * -0.5 / denominator) * (float)sqrt(c / denominator);
        long quality_percent = (long)(quality * 100.0f);
        if (quality_percent < minimum_quality)
          minimum_quality = quality_percent;
      }
    }
    if (minimum_quality <= 0)
      return 1;
    if (minimum_quality > 100)
      return 100;
  }
  return minimum_quality;
}

// @address 0x57c118 multiplayer_game_hopper_client_compute_repeated_play_adjustment_weight
extern "C" void multiplayer_game_hopper_client_compute_repeated_play_adjustment_weight(long controller_index,
    unsigned short hopper_identifier, long recent_limit, long forbid_count, long penalty_start, long penalty_end,
    const unsigned __int64 *player_identifiers, long player_count, long *weight_out, bool *allowed_out)
{
  long weight = 100;
  bool allowed = true;
  long maximum_matches = 0;
  const s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[controller_index];
  unsigned long flags = 0;
  if (controller->flags & 1)
    flags = controller->ban_flags | g_network_banhammer_globals.ban_flags;
  const s_network_banhammer_repeated_play_list *history = multiplayer_game_hopper_history_controller_get_history(controller_index);
  long entry_count = history->entry_count;
  const s_network_banhammer_repeated_play_entry *entries = history->entries;
  for (long player_index = 0; player_index < player_count; ++player_index) {
    unsigned __int64 player_identifier = player_identifiers[player_index];
    long matches = 0;
    long examined = 0;
    for (long entry_index = entry_count; entry_index-- > 0; ) {
      const s_network_banhammer_repeated_play_entry *entry = &entries[entry_index];
      if (entry->hopper_identifier == hopper_identifier) {
        if (memcmp(&entry->player_identifier, &player_identifier, sizeof(player_identifier)) == 0)
          ++matches;
        ++examined;
        if (examined >= recent_limit)
          break;
      }
    }
    if (matches > maximum_matches)
      maximum_matches = matches;
  }
  if (flags & 256)
    weight = 0;
  else if (penalty_end != 0 && maximum_matches > 0 && maximum_matches >= penalty_end)
    weight = 0;
  else if (penalty_start != 0 && maximum_matches >= penalty_start)
    weight = (penalty_start - maximum_matches) * 100 / (penalty_end - penalty_start) + 100;
  if ((flags & 512) || (forbid_count != 0 && maximum_matches > 0 && maximum_matches >= forbid_count))
    allowed = false;
  *weight_out = weight;
  *allowed_out = allowed;
}

// @address 0x57c2d0 multiplayer_game_hopper_update_player_history
extern "C" void multiplayer_game_hopper_update_player_history(
    long controller_index, unsigned short hopper_identifier, const unsigned __int64 *player_identifiers, long player_count)
{
  s_network_banhammer_repeated_play_list *history = multiplayer_game_hopper_history_controller_get_history(controller_index);
  long entry_count = history->entry_count;
  s_network_banhammer_repeated_play_entry *entries = history->entries;
  long overflow = entry_count + player_count - 384;
  if (overflow > 0) {
    memmove(entries, &entries[overflow], (384 - overflow) * sizeof(s_network_banhammer_repeated_play_entry));
    entry_count -= overflow;
  }
  for (long player_index = 0; player_index < player_count; ++player_index) {
    entries[entry_count].hopper_identifier = hopper_identifier;
    memcpy(entries[entry_count].player_identifier, &player_identifiers[player_index], sizeof(player_identifiers[player_index]));
    ++entry_count;
  }
  history->entry_count = entry_count;
}

const utf8 *multiplayer_game_hopper_get_description(unsigned short hopper_identifier)
{
  const utf8 *description = 0;
  if (hopper_update_globals().description.count != 0) {
    for (long index = 0; index < hopper_update_globals().description.count; ++index) {
      const s_hopper_description *entry = &hopper_update_globals().description.entries[index];
      if (entry->hopper_identifier == hopper_identifier) {
        description = entry->description;
        break;
      } else { continue; }
    }
  }
  return description;
}

#if !defined(_XBOX) || defined(__clang__)
static __forceinline unsigned __int64 multiplayer_game_hopper_swap_time(unsigned __int64 value)
{
  return ((unsigned __int64)byte_swap_long((unsigned long)value) << 32) | byte_swap_long((unsigned long)(value >> 32));
}
#endif

// @address 0x57cf88 ?multiplayer_game_hopper_decode@@YA_NPAVc_bitstream@@PAUs_hopper_configuration_table@@@Z
bool multiplayer_game_hopper_decode(c_bitstream *stream, s_hopper_configuration_table *table)
{
  bool valid = true;
  table->category_count = stream->read_integer("hopper-category-count", 3);
  valid = valid && table->category_count >= 0 && (unsigned int)table->category_count <= 4;
  for (long category_index = 0; valid && category_index < table->category_count; ++category_index) {
    s_hopper_configuration_category *category = &table->categories[category_index];
    category->identifier = (unsigned short)stream->read_integer("category-identifier", 16);
    category->category_flags = (unsigned short)stream->read_integer("category-flags", 6);
    stream->read_string("category-name", (char *)category->name, 32);
  }
  table->count = stream->read_integer("hopper-configuration-count", 6);
  valid = valid && table->count >= 0 && (unsigned int)table->count <= 32;
  for (long index = 0; valid && index < table->count; ++index) {
    s_hopper_configuration_table_entry *entry = &table->entries[index].m_configuration;
    stream->read_string("hopper-name", (char *)entry->name, 32);
    stream->read_raw_data("hopper-data", &entry->game_set_hash, 160);
    entry->hopper_identifier = (unsigned short)stream->read_integer("hopper-identifier", 16);
    entry->hopper_category = stream->read_integer("hopper-category", 16);
    entry->hopper_type = stream->read_integer("hopper-type", 2);
    entry->sort_key = stream->read_integer("sort-key", 6);
    entry->image_index = stream->read_integer("image-index", 5);
    entry->xlast_index = (unsigned char)stream->read_integer("xlast-index", 16);
    stream->read_raw_data("start-time", &entry->start_time, 64);
    stream->read_raw_data("end-time", &entry->end_time, 64);
#if !defined(_XBOX) || defined(__clang__)
    // Served hopper files store FILETIMEs in Xbox byte order.
    entry->start_time = __builtin_bswap64(entry->start_time);
    entry->end_time = __builtin_bswap64(entry->end_time);
#endif
    entry->hopper_regions = stream->read_integer("hopper-regions", 32);
    valid = valid && entry->hopper_type >= 0 && entry->hopper_type < 4;
    entry->minimum_skill = stream->read_integer("hopper-field-64", 17);
    entry->maximum_skill = stream->read_integer("hopper-field-68", 17);
    entry->minimum_experience = stream->read_integer("hopper-field-6c", 17);
    entry->maximum_experience = stream->read_integer("hopper-field-70", 17);
    entry->minimum_party_size = stream->read_integer("minimum-party-size", 4) + 1;
    entry->maximum_party_size = stream->read_integer("maximum-party-size", 4) + 1;
    entry->hopper_access_bit = stream->read_integer("hopper-access-bit", 4) - 1;
    entry->account_type_access = stream->read_integer("account-type-access", 2);
    valid = valid && (entry->hopper_access_bit == -1 || (entry->hopper_access_bit >= 0 && entry->hopper_access_bit < 8));
    valid = valid && entry->account_type_access >= 0 && entry->account_type_access < 3;
    entry->require_all_party_members_meet_experience_requirements = stream->read_bool("require_all_party_members_meet_experience_requirements");
    entry->require_all_party_members_meet_access_requirements = stream->read_bool("require_all_party_members_meet_access_requirements");
    entry->require_all_party_members_meet_live_account_access_requirements = stream->read_bool(
        "require_all_party_members_meet_live_account_access_requirements");
    entry->hide_hopper_from_experience_restricted_players = stream->read_bool("hide_hopper_from_experience_restricted_players");
    entry->hide_hopper_from_access_restricted_players = stream->read_bool("hide_hopper_from_access_restricted_players");
    entry->hide_hopper_from_live_account_access_restricted_players = stream->read_bool("hide_hopper_from_live_account_access_restricted_players");
    entry->requires_beta_rights = stream->read_bool("requires_beta_rights");
    entry->requires_all_downloadable_maps = stream->read_bool("requires_all_downloadable_maps");
    entry->veto_enabled = stream->read_bool("veto_enabled");
    entry->runtime_stats_write = stream->read_integer("hopper-field-230", 2);
    entry->runtime_language_filter = stream->read_integer("hopper-field-234", 2);
    entry->runtime_country_code_filter = stream->read_integer("hopper-field-238", 2);
    entry->runtime_gamerzone_filter = stream->read_bool("hopper-field-23c");
    valid = valid && entry->runtime_stats_write >= 0 && entry->runtime_stats_write < 4;
    valid = valid && entry->runtime_language_filter >= 0 && entry->runtime_language_filter < 4;
    valid = valid && entry->runtime_country_code_filter >= 0 && entry->runtime_country_code_filter < 4;
    entry->guests_allowed = stream->read_bool("guests_allowed");
    entry->require_hosts_on_multiple_teams = stream->read_bool("require_hosts_on_multiple_teams");
    entry->balance_network_bandwidth_between_teams = stream->read_bool("balance_network_bandwidth_between_teams");
    entry->runtime_require_hosts_on_multiple_teams = stream->read_bool("hopper-field-90");
    entry->stats_write = stream->read_integer("stats_write", 2);
    valid = valid && entry->stats_write >= 0 && entry->stats_write < 3;
    entry->language_filter = stream->read_integer("language_filter", 2);
    entry->country_code_filter = stream->read_integer("country_code_filter", 2);
    entry->gamerzone_filter = stream->read_integer("gamerzone_filter", 2);
    valid = valid && entry->language_filter >= 0 && entry->language_filter < 3;
    valid = valid && entry->country_code_filter >= 0 && entry->country_code_filter < 3;
    valid = valid && entry->gamerzone_filter >= 0 && entry->gamerzone_filter < 3;
    entry->quitter_filter_percentage = stream->read_integer("quitter_filter_percentage", 7);
    entry->quitter_filter_maximum_party_size = stream->read_integer("quitter_filter_maximum_party_size", 4);
    valid = valid && entry->quitter_filter_percentage >= 0 && entry->quitter_filter_percentage < 101;
    entry->rematch_countdown_timer = stream->read_integer("rematch_countdown_timer", 10);
    entry->rematch_group_formation = stream->read_integer("rematch_group_formation", 2);
    entry->repeated_opponents_to_consider_for_penalty = stream->read_integer("repeated_opponents_to_consider_for_penalty", 7);
    entry->repeated_opponents_experience_threshold = stream->read_integer("repeated_opponents_experience_threshold", 4);
    entry->repeated_opponents_skill_throttle_start = stream->read_integer("repeated_opponents_skill_throttle_start", 4);
    entry->repeated_opponents_skill_throttle_stop = stream->read_integer("repeated_opponents_skill_throttle_stop", 4);
    valid = valid && entry->rematch_countdown_timer >= 0 && entry->rematch_countdown_timer < 1001;
    valid = valid && entry->rematch_group_formation >= 0 && entry->rematch_group_formation < 3;
    entry->maximum_total_matchmaking_seconds = stream->read_integer("maximum_total_matchmaking_seconds", 10);
    entry->gather_start_game_early_seconds = stream->read_integer("gather_start_game_early_seconds", 10);
    entry->gather_give_up_seconds = stream->read_integer("gather_give_up_seconds", 10);
    for (unsigned long chance_index = 0; chance_index < 16; ++chance_index) {
      entry->chance_of_gathering[chance_index] = (unsigned char)stream->read_integer("chance_of_gathering", 7);
      valid = valid && entry->chance_of_gathering[chance_index] >= 0 && entry->chance_of_gathering[chance_index] < 101;
    }
    entry->experience_points_per_win = stream->read_integer("experience_points_per_win", 2);
    entry->experience_penalty_per_drop = stream->read_integer("experience_penalty_per_drop", 2);
    for (unsigned long level_index = 0; level_index < 49; ++level_index)
      entry->minimum_mu_per_level[level_index] = stream->read_integer("minimum_mu_per_level", 32);
    for (unsigned long delta_index = 0; delta_index < 50; ++delta_index)
      entry->maximum_skill_level_match_delta[delta_index] = (unsigned char)stream->read_integer("maximum_skill_level_match_delta", 6);
    entry->trueskill_sigma_multiplier = stream->read_integer("trueskill_sigma_multiplier", 32);
    entry->trueskill_beta_performance_variation = stream->read_integer("trueskill_beta_performance_variation", 32);
    entry->trueskill_tau_dynamics_factor = stream->read_integer("trueskill_tau_dynamics_factor", 32);
    entry->trueskill_adjust_tau_with_update_weight = stream->read_bool("trueskill_adjust_tau_with_update_weight");
    entry->trueskill_draw_probability = stream->read_integer("trueskill_draw_probability", 7);
    entry->trueskill_hillclimb_w0 = stream->read_integer("trueskill_hillclimb_w0", 7);
    entry->trueskill_hillclimb_w50 = stream->read_integer("trueskill_hillclimb_w50", 7);
    entry->trueskill_hillclimb_w100 = stream->read_integer("trueskill_hillclimb_w100", 7);
    entry->trueskill_hillclimb_w150 = stream->read_integer("trueskill_hillclimb_w150", 7);
    entry->skill_update_weight_s0 = stream->read_integer("skill_update_weight_s0", 7);
    entry->skill_update_weight_s10 = stream->read_integer("skill_update_weight_s10", 7);
    entry->skill_update_weight_s20 = stream->read_integer("skill_update_weight_s20", 7);
    entry->skill_update_weight_s30 = stream->read_integer("skill_update_weight_s30", 7);
    entry->skill_update_weight_s40 = stream->read_integer("skill_update_weight_s40", 7);
    entry->skill_update_weight_s50 = stream->read_integer("skill_update_weight_s50", 7);
    entry->quality_update_weight_q0 = stream->read_integer("quality_update_weight_q0", 7);
    entry->quality_update_weight_q25 = stream->read_integer("quality_update_weight_q25", 7);
    entry->quality_update_weight_q50 = stream->read_integer("quality_update_weight_q50", 7);
    entry->quality_update_weight_q75 = stream->read_integer("quality_update_weight_q75", 7);
    entry->quality_update_weight_q100 = stream->read_integer("quality_update_weight_q100", 7);
    valid = valid && entry->trueskill_draw_probability >= 0 && entry->trueskill_draw_probability < 101;
    valid = valid && entry->trueskill_hillclimb_w0 >= 0 && entry->trueskill_hillclimb_w0 < 101;
    valid = valid && entry->trueskill_hillclimb_w50 >= 0 && entry->trueskill_hillclimb_w50 < 101;
    valid = valid && entry->trueskill_hillclimb_w100 >= 0 && entry->trueskill_hillclimb_w100 < 101;
    valid = valid && entry->trueskill_hillclimb_w150 >= 0 && entry->trueskill_hillclimb_w150 < 101;
    valid = valid && entry->skill_update_weight_s0 >= 0 && entry->skill_update_weight_s0 < 101;
    valid = valid && entry->skill_update_weight_s10 >= 0 && entry->skill_update_weight_s10 < 101;
    valid = valid && entry->skill_update_weight_s20 >= 0 && entry->skill_update_weight_s20 < 101;
    valid = valid && entry->skill_update_weight_s30 >= 0 && entry->skill_update_weight_s30 < 101;
    valid = valid && entry->skill_update_weight_s40 >= 0 && entry->skill_update_weight_s40 < 101;
    valid = valid && entry->skill_update_weight_s50 >= 0 && entry->skill_update_weight_s50 < 101;
    valid = valid && entry->quality_update_weight_q0 >= 0 && entry->quality_update_weight_q0 < 101;
    valid = valid && entry->quality_update_weight_q25 >= 0 && entry->quality_update_weight_q25 < 101;
    valid = valid && entry->quality_update_weight_q50 >= 0 && entry->quality_update_weight_q50 < 101;
    valid = valid && entry->quality_update_weight_q75 >= 0 && entry->quality_update_weight_q75 < 101;
    valid = valid && entry->quality_update_weight_q100 >= 0 && entry->quality_update_weight_q100 < 101;
    if (entry->is_ffa()) {
      entry->team_configuration.team_count = stream->read_integer("minimum_player_count", 4) + 1;
      entry->team_configuration.minimum_team_size = stream->read_integer("maximum_player_count", 4) + 1;
      valid = valid && entry->team_configuration.team_count >= 2 && entry->team_configuration.team_count <= 16;
      valid = valid && entry->team_configuration.minimum_team_size >= 2 && entry->team_configuration.minimum_team_size <= 16;
    } else {
      if (entry->is_ranked()) {
        s_hopper_team_configuration *team_configuration = &entry->team_configuration;
        team_configuration->team_count = stream->read_integer("team_count", 3) + 1;
        team_configuration->minimum_team_size = stream->read_integer("minimum_team_size", 3) + 1;
        team_configuration->maximum_team_size = stream->read_integer("maximum_team_size", 3) + 1;
        team_configuration->configuration.ranked.maximum_team_imbalance = stream->read_integer("maximum_team_imbalance", 3);
        team_configuration->configuration.ranked.big_squad_size_threshold = stream->read_integer("big_squad_size_threshold", 4) + 1;
        team_configuration->configuration.ranked.maximum_big_squad_imbalance = stream->read_integer("maximum_big_squad_imbalance", 3);
        team_configuration->configuration.ranked.enable_big_squad_mixed_skill_restrictions = stream->read_bool("enable_big_squad_mixed_skill_restrictions");
        valid = valid && team_configuration->team_count >= 2 && team_configuration->team_count <= 8;
        valid = valid && team_configuration->minimum_team_size >= 1 && team_configuration->minimum_team_size <= 8;
        valid = valid && team_configuration->maximum_team_size >= 1 && team_configuration->minimum_team_size <= 8;
        valid = valid && team_configuration->configuration.ranked.big_squad_size_threshold >= 1 &&
            team_configuration->configuration.ranked.big_squad_size_threshold <= 16;
        valid = valid && team_configuration->configuration.ranked.maximum_big_squad_imbalance >= 0 &&
            team_configuration->configuration.ranked.maximum_big_squad_imbalance <= 6;
        valid = valid && team_configuration->minimum_team_size <= team_configuration->maximum_team_size;
        valid = valid && team_configuration->team_count * team_configuration->maximum_team_size <= 16;
      } else {
        s_hopper_team_configuration *team_configuration = &entry->team_configuration;
        team_configuration->team_count = stream->read_integer("team_count", 3) + 1;
        team_configuration->minimum_team_size = stream->read_integer("minimum_team_size", 3) + 1;
        team_configuration->maximum_team_size = stream->read_integer("maximum_team_size", 3) + 1;
        team_configuration->configuration.unranked.allow_uneven_teams = stream->read_bool("allow_uneven_teams");
        team_configuration->configuration.unranked.allow_parties_to_split = stream->read_bool("allow_parties_to_split");
        valid = valid && team_configuration->team_count >= 2 && team_configuration->team_count <= 8;
        valid = valid && team_configuration->minimum_team_size >= 1 && team_configuration->minimum_team_size <= 8;
        valid = valid && team_configuration->maximum_team_size >= 1 && team_configuration->minimum_team_size <= 8;
        valid = valid && team_configuration->minimum_team_size <= team_configuration->maximum_team_size;
        valid = valid && team_configuration->maximum_team_size * entry->team_configuration.team_count <= 16;
      }
    }
  }
  return valid;
}

// @address 0x57de48 multiplayer_game_hopper_decode_descriptions
extern "C" bool multiplayer_game_hopper_decode_descriptions(c_bitstream *stream, s_hopper_description_table *table)
{
  table->count = stream->read_integer("count", 6);
  bool valid = table->count >= 0 && table->count < 64u;
  for (long index = 0; valid && index < table->count; ++index) {
    s_hopper_description *entry = &table->entries[index];
    entry->hopper_identifier = (unsigned short)stream->read_integer("hopper-identifier", 16);
    entry->flag = stream->read_integer("flag", 1);
    stream->read_string("description", (char *)entry->description, 256);
  }
  return valid;
}

// @address 0x57df38 multiplayer_game_hopper_decode_game_set
extern "C" unsigned char multiplayer_game_hopper_decode_game_set(c_bitstream *stream, s_hopper_game_set *game_set)
{
    long entry_count = stream->read_integer("game-count", 6);
    game_set->count = entry_count;
    bool valid = entry_count >= 0 && (unsigned long)entry_count < 128;
    long entry_index = 0;
    if (!valid)
        return valid;
    while (valid) {
        if (entry_index >= game_set->count)
            break;
        s_hopper_game_entry *entry = &game_set->entries[entry_index];
        entry->weight = stream->read_integer("weight", 32);
        entry->minimum_player_count = stream->read_integer("minimum-player-count", 4);
        entry->optional = stream->read_bool("optional");
        entry->optional_content = stream->read_bool("optional-content");
        entry->map_id = stream->read_integer("map-id", 32);
        stream->read_string("game-variant-name", entry->game_variant_name, sizeof(entry->game_variant_name));
        stream->read_raw_data("data", &entry->game_variant_hash, 160);
        stream->read_string("map-variant-name", entry->map_variant_name, sizeof(entry->map_variant_name));
        stream->read_raw_data("data", &entry->map_variant_hash, 160);
        valid = valid && entry->minimum_player_count >= 0 && entry->minimum_player_count < 16;
        ++entry_index;
    }
    return valid;
}


// 0x57e478: returns the state of the downloaded file and, once it is complete, the parsed 'mhcf' block from it
// (parsed again only when the download changed)
#define block (&((s_http_download<21813> *)storage)->parsed)
// @address 0x57e478 multiplayer_game_hopper_get_configuration_data
extern "C" int multiplayer_game_hopper_get_configuration_data(char *storage, void **out_data, unsigned int *out_size)
{
  int status;
  void *data;
  unsigned int size;

  size = 0;
  data = 0;
  status = ((c_http_buffer_downloader *)storage)->get_data((const char **)&data, (long *)&size);
  if (status == multiplayer_game_hopper_download_complete && data != 0) {
    if (block->download_id != ((c_http_buffer_downloader *)storage)->m_retry_count) {
      block->data = network_blf_find_chunk_in_file((const char *)data, size, 0x6d686366, 11, _blf_file_authentication_none, (unsigned long *)&block->size);
    }
  }
  else {
    block->data = 0;
    block->size = 0;
  }
  if (out_data != 0) {
    *out_data = block->data;
  }
  if (out_size != 0) {
    *out_size = block->data != 0 ? block->size : 0;
  }
  return status;
}
#undef block

// 0x57e548: returns the state of the downloaded file and, once it is complete, the parsed 'mhdf' block from it
// (parsed again only when the download changed)
#define block (&((s_http_download<19105> *)storage)->parsed)
// @address 0x57e548 multiplayer_game_hopper_get_description_data
extern "C" int multiplayer_game_hopper_get_description_data(char *storage, void **out_data, unsigned int *out_size)
{
  int status;
  void *data;
  unsigned int size;

  size = 0;
  data = 0;
  status = ((c_http_buffer_downloader *)storage)->get_data((const char **)&data, (long *)&size);
  if (status == multiplayer_game_hopper_download_complete && data != 0) {
    if (block->download_id != ((c_http_buffer_downloader *)storage)->m_retry_count) {
      block->data = network_blf_find_chunk_in_file((const char *)data, size, 0x6d686466, 3, _blf_file_authentication_none, (unsigned long *)&block->size);
    }
  }
  else {
    block->data = 0;
    block->size = 0;
  }
  if (out_data != 0) {
    *out_data = block->data;
  }
  if (out_size != 0) {
    *out_size = block->data != 0 ? block->size : 0;
  }
  return status;
}
#undef block

// 0x57e618: returns the state of the downloaded file and, once it is complete, the parsed 'gset' block from it
// (parsed again only when the download changed)
#define block (&((s_http_download<17569> *)storage)->parsed)
// @address 0x57e618 multiplayer_game_hopper_get_game_set_data
extern "C" int multiplayer_game_hopper_get_game_set_data(char *storage, void **out_data, unsigned int *out_size)
{
  int status;
  void *data;
  unsigned int size;

  size = 0;
  data = 0;
  status = ((c_http_buffer_downloader *)storage)->get_data((const char **)&data, (long *)&size);
  if (status == multiplayer_game_hopper_download_complete && data != 0) {
    if (block->download_id != ((c_http_buffer_downloader *)storage)->m_retry_count) {
      block->data = network_blf_find_chunk_in_file((const char *)data, size, 0x67736574, 6, _blf_file_authentication_none, (unsigned long *)&block->size);
    }
  }
  else {
    block->data = 0;
    block->size = 0;
  }
  if (out_data != 0) {
    *out_data = block->data;
  }
  if (out_size != 0) {
    *out_size = block->data != 0 ? block->size : 0;
  }
  return status;
}
#undef block

// ---------------------------------------------------------------------------
// 0x57aa80: constructs the global multiplayer-game-hopper download storage at
// 0x8315fc60 (five stored-buffer file downloaders, a zeroed hopper scratch
// block and the embedded map variant)
typedef char hopper_storage_description_valid_offset[
    (sizeof(void *) != 4 || ((offsetof(s_multiplayer_game_hopper_global_storage, m_description_valid)
    == offsetof(s_hopper_update_globals, description_valid)))) ? 1 : -1];
typedef char hopper_storage_game_set_valid_offset[
    (sizeof(void *) != 4 || ((offsetof(s_multiplayer_game_hopper_global_storage, m_game_set_valid)
    == offsetof(s_hopper_update_globals, game_set_valid)))) ? 1 : -1];
typedef char hopper_storage_game_variant_valid_offset[
    (sizeof(void *) != 4 || ((offsetof(s_multiplayer_game_hopper_global_storage, m_game_variant_valid)
    == offsetof(s_hopper_update_globals, game_variant_valid)))) ? 1 : -1];
typedef char hopper_storage_map_variant_valid_offset[
    (sizeof(void *) != 4 || ((offsetof(s_multiplayer_game_hopper_global_storage, m_map_variant_valid)
    == offsetof(s_hopper_update_globals, map_variant_valid)))) ? 1 : -1];
typedef char multiplayer_game_hopper_globals_size_check[(sizeof(void *) != 4 || ((sizeof(s_multiplayer_game_hopper_global_storage) == 0x3a788))) ? 1 : -1];

// @address 0x57aa80 ??0s_multiplayer_game_hopper_global_storage@@QAA@XZ
s_multiplayer_game_hopper_global_storage::s_multiplayer_game_hopper_global_storage() :
  s_multiplayer_game_hopper_global_storage_base(this)
{
}

s_multiplayer_game_hopper_global_storage g_multiplayer_game_hopper_globals;
// @address 0x57ba88 multiplayer_game_hopper_get_next_rank_skill
extern "C" long multiplayer_game_hopper_get_next_rank_skill(long experience, long experience_penalty, long highest_skill)
{
  const s_network_configuration *configuration = &g_network_configuration;
  long next_threshold = -1;
  long experience_difference;
  long rank;
  long grade;
  multiplayer_game_hopper_compute_experience_rank(experience, 0, highest_skill, &experience_difference, &rank, &grade);
  for (long rank_index = rank + 1; rank_index < 14; ++rank_index) {
    long threshold = configuration->rank_experience_thresholds[rank_index];
    if (threshold > highest_skill) {
      for (long step = rank_index << 2; step < (rank_index + 1) << 2; ++step) {
        long required_experience = configuration->rank_grade_thresholds[step];
        if (required_experience != 0 && required_experience <= experience_difference) {
          next_threshold = configuration->rank_experience_thresholds[rank_index];
          break;
        }
      }
    }
    if (next_threshold != -1)
      break;
  }
  return next_threshold;
}


static __forceinline bool multiplayer_game_hopper_status_is_guest(unsigned __int64 xuid)
{
    unsigned int account_flags = (unsigned int)(xuid >> 48);
    bool result = (account_flags & 15) == 9 && (account_flags & 192) != 0;
    return result;
}

static __forceinline void multiplayer_game_hopper_status_apply_filter(long filter, unsigned long player_mask, unsigned long *status)
{
    if (filter == 2)
        *status |= player_mask;
    else
        *status |= player_mask;
}

static __forceinline long multiplayer_game_hopper_status_content_index(long map_id, bool search = true)
{
    long result = -1;
    if (search)
    {
        for (long content_index = 0; (unsigned long)content_index < 32; content_index++)
        {
            if (g_network_configuration.map_table[content_index].map_id == map_id)
            {
                result = content_index;
                break;
            }
        }
    }
    return result;
}


// @address 0x57e0f0
void multiplayer_game_hopper_get_players_status(const c_hopper_configuration *hopper,
    // match: 0x57e0f0: pointer __restrict qualifier prevents alias reload
    const c_network_session_membership *session_membership, multiplayer_hopper_check *__restrict check)
{
    unsigned long required_content_mask = 0;
    if (network_session_matchmaking_composition_session_state_valid() == 2)
    {
        const s_hopper_game_set *game_set = &hopper_update_globals().game_set.decoded;
        for (long game_index = 0; game_index < game_set->count; game_index++)
        {
            if (!game_set->entries[game_index].optional_content)
            {
                long content_index = multiplayer_game_hopper_status_content_index(game_set->entries[game_index].map_id);
                if (content_index != -1)
                    required_content_mask |= 1 << content_index;
            }
        }
    }
    memset(check, 0, sizeof(*check));
    long player_index = session_membership->get_first_player();
    if (player_index != -1)
    {
        const s_hopper_configuration_table_entry *configuration = (const s_hopper_configuration_table_entry *)hopper;
        long hopper_identifier = hopper->get_hopper_identifier();
        long hopper_regions = configuration->hopper_regions;
        long language_filter = configuration->language_filter;
        long country_code_filter = configuration->country_code_filter;
        long gamerzone_filter = configuration->gamerzone_filter;
        long minimum_skill = configuration->minimum_skill;
        long maximum_skill = configuration->maximum_skill;
        long minimum_experience = configuration->minimum_experience;
        long maximum_experience = configuration->maximum_experience;
        long hopper_access_bit = configuration->hopper_access_bit;
        long account_type_access = configuration->account_type_access;
        do
        {
            const s_network_session_player *player = session_membership->get_player(player_index);
            const s_network_session_peer *peer = session_membership->get_peer(player->peer_index);
            long player_mask = 1 << player_index;
            check->players |= player_mask;
            if (multiplayer_game_hopper_status_is_guest(session_membership->get_player_xuid(player_index)))
                check->guests |= player_mask;
            if (!(player->configuration.client.player_flags & 2))
                check->status_valid |= player_mask;
            if (player->configuration.client.global_statistics.valid && player->configuration.client.hopper_statistics.valid &&
                player->configuration.host.global_stats_valid && player->configuration.host.hopper_stats_valid)
                check->statistics_valid |= player_mask;
            long peer_hopper_identifier = peer->properties.hopper_identifier;
            if (peer_hopper_identifier == hopper_identifier)
                check->hopper_valid |= player_mask;
            if (peer_hopper_identifier == 0xffff)
                check->hopper_error = _session_game_start_error_player_missing_files;
            if (peer->properties.hopper_error != 0 && (check->hopper_error == 0 || peer->properties.hopper_error < check->hopper_error))
                check->hopper_error = peer->properties.hopper_error;
            if ((peer->properties.map_content_mask & required_content_mask) == required_content_mask)
                check->content |= player_mask;
            if (hopper_regions == -1)
                check->valid_players |= player_mask;
            else
            {
                // match: 0x57e0f0: without it the regions compare reads r29 not r11 and two ors swap operands
                unsigned long regions = _rotl(hopper_regions, 0);
                unsigned long player_region = player->configuration.client.gamer_region;
                for (; regions; regions >>= 8)
                {
                    if ((unsigned char)regions == (unsigned char)player_region)
                    {
                        check->valid_players |= player_mask;
                        break;
                    }
                }
            }
            multiplayer_game_hopper_status_apply_filter(language_filter, player_mask, &check->language);
            multiplayer_game_hopper_status_apply_filter(country_code_filter, player_mask, &check->country_code);
            multiplayer_game_hopper_status_apply_filter(gamerzone_filter, player_mask, &check->gamerzone);
            long skill = player->configuration.client.global_statistics.experience_base;
            if (skill >= minimum_skill)
            {
                unsigned long *skill_status = &check->skill;
                *skill_status |= player_mask;
            }
            if (maximum_skill == 0 || skill <= maximum_skill)
                check->skill_other |= player_mask;
            long experience = player->configuration.client.displayed_statistics.ranked_played
                + player->configuration.client.displayed_statistics.unranked_played;
            if (experience >= minimum_experience)
                check->experience |= player_mask;
            if (maximum_experience == 0 || experience <= maximum_experience)
            {
                unsigned long *experience_status = &check->experience_penalty;
                *experience_status |= player_mask;
            }
            if (hopper_access_bit == -1 || (player->configuration.client.hopper_access_flags & (1UL << hopper_access_bit)))
            {
                unsigned long *access_status = &check->access;
                *access_status |= player_mask;
            }
            unsigned long *live_status = &check->live_access;
            if (account_type_access == 2)
                *live_status |= player_mask;
            else
            {
                unsigned char account_type = player->configuration.client.is_free_live_gold_account;
                if (account_type_access == 0)
                {
                    if (!account_type)
                        *live_status |= player_mask;
                }
                else if (account_type_access == 1)
                {
                    if (account_type)
                        *live_status |= player_mask;
                }
            }
            player_index = session_membership->get_next_player(player_index);
        } while (player_index != -1);
    }
}


// @address 0x57c3d8 multiplayer_game_hopper_is_hopper_visible
bool multiplayer_game_hopper_is_hopper_visible(unsigned short identifier, const void *membership)
{
    bool visible = false;
    const c_hopper_configuration *configuration = multiplayer_game_hoppers_get_hopper_configuration(identifier);
    if (configuration)
    {
        multiplayer_hopper_check check;
        multiplayer_game_hopper_get_players_status((const c_hopper_configuration *)configuration, (const c_network_session_membership *)membership, &check);
        visible = true;
        if ((check.valid_players ^ check.players) != 0)
            visible = false;
        if (configuration->m_configuration.hide_hopper_from_access_restricted_players)
        {
            if (configuration->m_configuration.require_all_party_members_meet_experience_requirements)
            {
                if (((check.experience & check.experience_penalty) ^ check.players) != 0)
                    visible = false;
            }
            else
            {
                if ((check.experience & check.experience_penalty) == 0)
                    visible = false;
            }
        }
        if (configuration->m_configuration.hide_hopper_from_live_account_access_restricted_players)
        {
            if (configuration->m_configuration.require_all_party_members_meet_access_requirements)
            {
                if (((check.skill & check.skill_other) ^ check.players) != 0)
                    visible = false;
            }
            else
            {
                if ((check.skill & check.skill_other) == 0)
                    visible = false;
            }
        }
        if (configuration->m_configuration.requires_beta_rights)
        {
            if (configuration->m_configuration.require_all_party_members_meet_live_account_access_requirements)
            {
                if ((check.access ^ check.players) != 0)
                    visible = false;
            }
            else
            {
                if (check.access == 0)
                    visible = false;
            }
        }
        if (configuration->m_configuration.requires_all_downloadable_maps)
        {
            if (configuration->m_configuration.hide_hopper_from_experience_restricted_players)
            {
                if ((check.live_access ^ check.players) != 0)
                    visible = false;
            }
            else
            {
                if (check.live_access == 0)
                    visible = false;
            }
        }
        if (configuration->m_configuration.veto_enabled)
        {
            unsigned __int64 time;
            GetSystemTimeAsFileTime(&time);
            if (configuration->m_configuration.start_time
                && configuration->m_configuration.start_time > time)
                visible = false;
            if (configuration->m_configuration.end_time
                && configuration->m_configuration.end_time < time)
                visible = false;
        }
    }
    return visible;
}


// @address 0x57c940 ?multiplayer_game_is_playable@@YA?AW4e_session_game_start_error@@G_N0PBVc_network_session_membership@@PAG@Z
e_session_game_start_error multiplayer_game_is_playable(unsigned short hopper_identifier, bool is_matchmaking, bool check_hopper,
    const c_network_session_membership *membership, unsigned short *out_player_error_mask)
{
    long error = 0;
    unsigned short player_error_mask = 0;
    const c_hopper_configuration *configuration = multiplayer_game_hoppers_get_hopper_configuration(hopper_identifier);
    if (check_hopper && !is_matchmaking)
    {
        if (!online_local_xuid_is_online_enabled())
        {
            bool found_gold = false;
            for (long i = 0; i < 4; i++)
            {
                if (g_online_cached_users[i].silver_or_gold)
                {
                    found_gold = true;
                    break;
                }
            }
            if (found_gold)
                error = _session_game_start_error_must_connect_to_live_for_match;
        }
        if (error == 0)
        {
            bool all_signed_in = true;
            for (long i = 0; i < 4; i++)
            {
                if (g_online_cached_users[i].signed_in != 0 && g_online_cached_users[i].online_enabled == 0)
                {
                    all_signed_in = false;
                    break;
                }
            }
            if (!all_signed_in)
            {
                for (long player_index = (membership)->get_first_player(); player_index != -1;
                    player_index = (membership)->get_next_player(player_index))
                {
                    if (!membership->get_player(player_index)->configuration.client.silver)
                        player_error_mask |= (unsigned short)(1 << player_index);
                }
                error = _session_game_start_error_must_have_online_enabled_profiles_for_match;
            }
            else
            {
                error = _session_game_start_error_must_have_live_for_match;
            }
        }
    }
    if (error == 0 && check_hopper)
        error = multiplayer_game_hopper_compute_start_error(check_hopper, (0xffff - hopper_identifier) != 0);
    if (error == 0 && check_hopper && (hopper_identifier == 0xffff || configuration == NULL))
        error = _session_game_start_error_invalid_hopper;
    if (error == 0 && is_matchmaking)
    {
        for (long player_index = (membership)->get_first_player(); player_index != -1;
            player_index = (membership)->get_next_player(player_index))
        {
            if (!membership->get_player(player_index)->configuration.client.silver)
            {
                error = _session_game_start_error_account_not_online_enabled;
                player_error_mask |= (unsigned short)(1 << player_index);
            }
        }
    }
    if (error == 0 && ((long)::network_squad_session_get_start_mode()) == 1 && get_alpha_custom_games_disabled())
        error = _session_game_start_error_custom_games_are_disabled_for_alpha;
    if (error == 0)
    {
        for (long peer_index = (membership)->get_first_peer(); peer_index != -1;
            peer_index = (membership)->get_next_peer(peer_index))
        {
            const s_network_session_peer *peer = &membership->m_current_membership.peers[peer_index];
            if (peer->player_count > g_network_configuration.maximum_players_per_peer)
            {
                for (long user_index = 0; user_index < 4; user_index++)
                {
                    long user_player_index = peer->user_player_indices[user_index];
                    if (user_player_index != -1)
                        player_error_mask |= (unsigned short)(1 << user_player_index);
                }
            }
            if (player_error_mask != 0)
                error = _session_game_start_error_maximum_multiplayer_split_screen_exceeded;
        }
    }
    unsigned __int64 current_time;
    GetSystemTimeAsFileTime(&current_time);
    if (error == 0 && hopper_identifier != 0xffff)
    {
        unsigned __int64 start_time = configuration->m_configuration.start_time;
        if (start_time != 0 && start_time > current_time)
            error = _session_game_start_error_not_yet_start_time;
    }
    if (error == 0 && hopper_identifier != 0xffff)
    {
        unsigned __int64 end_time = configuration->m_configuration.end_time;
        if (end_time != 0 && end_time < current_time)
            error = _session_game_start_error_end_time_has_passed;
    }
    if (error == 0 && hopper_identifier != 0xffff)
    {
        long player_count = membership->get_player_count();
        if (player_count < ((const s_hopper_configuration_table_entry *)configuration)->minimum_party_size)
            error = _session_game_start_error_squad_too_small;
        else if (player_count > ((const s_hopper_configuration_table_entry *)configuration)->maximum_party_size)
            error = _session_game_start_error_squad_too_large;
    }
    {
        while (error == 0 && configuration != NULL)
        {
            multiplayer_hopper_check check;
            multiplayer_game_hopper_get_players_status(configuration, membership, &check);
            if ((check.hopper_valid ^ check.players) != 0)
            {
                error = check.hopper_error;
                player_error_mask = (unsigned short)(check.players & ~check.hopper_valid);
                break;
            }
            if ((long)check.hopper_error != 0)
            {
                error = check.hopper_error;
                break;
            }
            if ((check.content ^ check.players) != 0)
            {
                error = _session_game_start_error_player_missing_required_maps;
                player_error_mask = (unsigned short)(check.players & ~check.content);
                break;
            }
            if ((check.status_valid ^ check.players) != 0)
            {
                error = _session_game_start_error_player_banned_from_matchmaking;
                player_error_mask = (unsigned short)(check.players & ~check.status_valid);
                break;
            }
            if ((check.statistics_valid ^ check.players) != 0)
            {
                error = _session_game_start_error_player_stats_pending;
                player_error_mask = (unsigned short)(check.players & ~check.statistics_valid);
                break;
            }
            if (((const s_hopper_configuration_table_entry *)configuration)->require_all_party_members_meet_experience_requirements)
            {
                if ((check.experience ^ check.players) != 0)
                {
                    error = _session_game_start_error_games_played_too_low;
                    player_error_mask = (unsigned short)(check.players & ~check.experience);
                    break;
                }
                if ((check.experience_penalty ^ check.players) != 0)
                {
                    error = _session_game_start_error_games_played_too_high;
                    player_error_mask = (unsigned short)(check.players & ~check.experience_penalty);
                    break;
                }
            }
            else
            {
                if (check.experience == 0)
                {
                    error = _session_game_start_error_games_played_too_low;
                    player_error_mask = (unsigned short)check.players;
                    break;
                }
                if (check.experience_penalty == 0)
                {
                    error = _session_game_start_error_games_played_too_high;
                    player_error_mask = (unsigned short)check.players;
                    break;
                }
            }
            if (((const s_hopper_configuration_table_entry *)configuration)->require_all_party_members_meet_access_requirements)
            {
                if ((check.skill ^ check.players) != 0)
                {
                    error = _session_game_start_error_experience_too_low;
                    player_error_mask = (unsigned short)(check.players & ~check.skill);
                    break;
                }
                if ((check.skill_other ^ check.players) != 0)
                {
                    error = _session_game_start_error_experience_too_high;
                    player_error_mask = (unsigned short)(check.players & ~check.skill_other);
                    break;
                }
            }
            else
            {
                if (check.skill == 0)
                {
                    error = _session_game_start_error_experience_too_low;
                    player_error_mask = (unsigned short)check.players;
                    break;
                }
                if (check.skill_other == 0)
                {
                    error = _session_game_start_error_experience_too_high;
                    player_error_mask = (unsigned short)check.players;
                    break;
                }
            }
            if (((const s_hopper_configuration_table_entry *)configuration)->hopper_access_bit != -1)
            {
                if (((const s_hopper_configuration_table_entry *)configuration)->require_all_party_members_meet_live_account_access_requirements)
                {
                    if ((check.access ^ check.players) != 0)
                    {
                        error = _session_game_start_error_access_bit_not_set;
                        player_error_mask = (unsigned short)(check.players & ~check.access);
                        break;
                    }
                }
                else if (check.access == 0)
                {
                    error = _session_game_start_error_access_bit_not_set;
                    player_error_mask = (unsigned short)check.players;
                    break;
                }
            }
            if (((const s_hopper_configuration_table_entry *)configuration)->hide_hopper_from_experience_restricted_players)
            {
                if ((check.live_access ^ check.players) != 0)
                {
                    error = _session_game_start_error_unpaid_in_paid_hopper +
                        (((const s_hopper_configuration_table_entry *)configuration)->account_type_access != 0);
                    player_error_mask = (unsigned short)(check.players & ~check.live_access);
                    break;
                }
            }
            else if (check.live_access == 0)
            {
                error = _session_game_start_error_unpaid_in_paid_hopper +
                    (((const s_hopper_configuration_table_entry *)configuration)->account_type_access != 0);
                player_error_mask = check.players;
                break;
            }
            if ((check.valid_players ^ check.players) != 0)
            {
                error = _session_game_start_error_player_not_in_required_region;
                player_error_mask = (unsigned short)(check.players & ~check.valid_players);
                break;
            }
            if (!((const s_hopper_configuration_table_entry *)configuration)->balance_network_bandwidth_between_teams && check.guests != 0)
            {
                error = _session_game_start_error_guest_player_when_not_allowed;
                player_error_mask = (unsigned short)check.guests;
                break;
            }
            configuration = NULL;
        }
    }
    if (out_player_error_mask != NULL)
        *out_player_error_mask = player_error_mask;
    return (e_session_game_start_error)error;
}


extern "C" {
// @address 0x11db098 g_hopper_last_manifest_version
long g_hopper_last_manifest_version;
}
// @rdata 0x82045234..0x82045250
static const char k_hopper_configuration_url_format[] = "matchmaking_hopper_%03u.bin";
// @rdata-end


typedef char hopper_chk_configuration_downloader[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, configuration_downloader) == 0x0010)) ? 1 : -1];
typedef char hopper_chk_configuration_downloader_m_retry_count[
    (sizeof(void *) != 4 || (((unsigned long)&((s_hopper_update_globals *)0)->configuration_downloader.m_retry_count == 0x069c) )) ? 1 : -1];
typedef char hopper_chk_description_downloader[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, description_downloader) == 0x5bec)) ? 1 : -1];
typedef char hopper_chk_description_downloader_m_retry_count[
    (sizeof(void *) != 4 || (((unsigned long)&((s_hopper_update_globals *)0)->description_downloader.m_retry_count == 0x6278) )) ? 1 : -1];
typedef char hopper_chk_game_set_downloader[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, game_set_downloader) == 0xad34)) ? 1 : -1];
typedef char hopper_chk_game_set_downloader_m_retry_count[
    (sizeof(void *) != 4 || (((unsigned long)&((s_hopper_update_globals *)0)->game_set_downloader.m_retry_count == 0xb3c0) )) ? 1 : -1];
typedef char hopper_chk_game_set_parsed_download_id[
    (sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, game_set_parsed_download_id) == 0xf868)) ? 1 : -1];
typedef char hopper_chk_game_variant_downloader[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, game_variant_downloader) == 0xf878)) ? 1 : -1];
typedef char hopper_chk_game_variant_downloader_m_internal_status[
    (sizeof(void *) != 4 || (((unsigned long)&((s_hopper_update_globals *)0)->game_variant_downloader.m_internal_status == 0xfee0) )) ? 1 : -1];
typedef char hopper_chk_map_variant_downloader[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, map_variant_downloader) == 0x10a1c)) ? 1 : -1];
typedef char hopper_chk_map_variant_downloader_m_internal_status[
    (sizeof(void *) != 4 || (((unsigned long)&((s_hopper_update_globals *)0)->map_variant_downloader.m_internal_status == 0x11084) )) ? 1 : -1];
typedef char hopper_chk_game_variant[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, game_variant) == 0x1f9e8)) ? 1 : -1];
typedef char hopper_chk_map_variant[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, map_variant) == 0x1fc50)) ? 1 : -1];
typedef char hopper_chk_configuration[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, configuration) == 0x2dce0)) ? 1 : -1];
typedef char hopper_chk_description[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, description) == 0x32978)) ? 1 : -1];
typedef char hopper_chk_game_set[(sizeof(void *) != 4 || (offsetof(s_hopper_update_globals, game_set) == 0x36b7c)) ? 1 : -1];


static __forceinline const s_network_http_request_hash *hopper_configuration_get_game_set_hash(const c_hopper_configuration *configuration)
{
    return &configuration->m_configuration.game_set_hash;
}


static __forceinline bool multiplayer_game_hopper_unpack_hopper_description(const void *buffer, long buffer_size, s_hopper_description_table *description)
{
    bool success = buffer != 0;
    if (success)
    {
        c_bitstream stream;
        bitstream_initialize(&stream, (const unsigned char *)buffer, buffer_size);
        stream.reset(3);
        success = multiplayer_game_hopper_decode_descriptions(&stream, description);
    }
    return success;
}

static __forceinline unsigned char multiplayer_game_hopper_unpack_game_set(const void *buffer, long buffer_size, s_hopper_game_set_table *game_set)
{
    unsigned char success = buffer != 0;
    if (success)
    {
        c_bitstream stream;
        bitstream_initialize(&stream, (const unsigned char *)buffer, buffer_size);
        stream.reset(3);
        success = multiplayer_game_hopper_decode_game_set(&stream, &game_set->decoded);
    }
    return success;
}

static __forceinline bool multiplayer_game_hopper_unpack_game_variant(const void *buffer, long buffer_size, c_game_variant *game_variant)
{
    bool success = buffer != 0;
    if (success)
    {
        c_bitstream stream;
        bitstream_initialize(&stream, (const unsigned char *)buffer, buffer_size);
        stream.reset(3);
        game_variant_decode(game_variant, &stream);
        stream.finish_reading();
    }
    return success;
}

static __forceinline bool multiplayer_game_hopper_unpack_map_variant(const void *buffer, long buffer_size, c_map_variant *map_variant)
{
    bool success = buffer != 0;
    if (success)
    {
        c_bitstream stream;
        bitstream_initialize(&stream, (const unsigned char *)buffer, buffer_size);
        stream.reset(3);
        map_variant->create_default(_map_id_none);
        map_variant_decode(map_variant, &stream);
    }
    return success;
}

// @address 0x92b10 multiplayer_game_hopper_update
extern "C" void multiplayer_game_hopper_update(void)
{
    s_hopper_update_globals &globals = hopper_update_globals();

    bool changed = false;
    if (g_hopper_last_manifest_version != g_online_url_version)
    {
        g_hopper_last_manifest_version = g_online_url_version;
        changed = true;
    }
    if (changed)
    {
        s_hopper_url_local url;
        online_url_set_base_path(&url);
        url.name.append_print(k_hopper_configuration_url_format, 11);
        url.fourth = 0;
        url.second = 1;
        http_buffer_downloader_set_url(&globals.configuration_downloader, (const s_network_http_buffer_url *)&url);

        online_url_set_base_path(&url);
        e_language language = g_current_language;
        if (language == -1)
        {
            language = get_e_language_from_xbox_language(XTLGetLanguage());
            g_current_language = language;
        }
        url.name.append_print("%s/matchmaking_hopper_descriptions_%03u.bin", get_language_suffix((e_language)language, false), 3);
        url.fourth = 0;
        url.second = 2;
        http_buffer_downloader_set_url(&globals.description_downloader, (const s_network_http_buffer_url *)&url);

        globals.game_set_valid = false;
        globals.game_set_failed = false;
        globals.game_set_downloader.reset();
        globals.game_set_parsed_download_id = globals.game_set_downloader.m_retry_count;
        if (globals.current_configuration != 0)
        {
            multiplayer_game_hopper_initialize_game_set_request(&url, globals.current_configuration->get_hopper_identifier());
            http_buffer_downloader_set_url(&globals.game_set_downloader, (const s_network_http_buffer_url *)&url);
            globals.game_set_downloader.set_hash(hopper_configuration_get_game_set_hash(globals.current_configuration));
        }
        else
        {
            url.name.m_string[0] = 0;
            http_buffer_downloader_set_url(&globals.game_set_downloader, (const s_network_http_buffer_url *)&url);
            globals.game_set_downloader.clear_hash();
        }
    }

    globals.configuration_downloader.get_data();
    globals.description_downloader.get_data();
    globals.game_set_downloader.get_data();
    globals.game_variant_downloader.get_data();
    globals.map_variant_downloader.get_data();

    void *buffer;
    unsigned long buffer_size;

    long configuration_download_id = globals.configuration_downloader.m_retry_count;
    if (globals.configuration_download_id != configuration_download_id && network_life_cycle_can_download_files())
    {
        long status = multiplayer_game_hopper_get_configuration_data((char *)&globals.configuration_downloader, &buffer, (unsigned int *)&buffer_size);
        if (status == 2)
        {
            const unsigned char *hopper_buffer = (const unsigned char *)buffer;
            bool unpacked = hopper_buffer != 0;
            unsigned short active_hopper = 0xffff;
            if (globals.current_configuration != 0)
            {
                active_hopper = globals.current_configuration->get_hopper_identifier();
            }
            if (unpacked)
            {
                c_bitstream stream;
                bitstream_initialize(&stream, hopper_buffer, buffer_size);
                stream.reset(3);
                unpacked = multiplayer_game_hopper_decode(&stream, &globals.configuration);
            }
            if (unpacked)
            {
                multiplayer_game_hopper_set_active_hopper_and_request_game_set(active_hopper, false, 0);
                globals.configuration_valid = true;
                globals.configuration_failed = false;
            }
            else
            {
                globals.configuration_valid = false;
                globals.configuration_failed = true;
            }
            globals.configuration_download_id = configuration_download_id;
        }
        else if (status != 1)
        {
            globals.configuration_failed = true;
            globals.configuration_download_id = configuration_download_id;
        }
    }

    long description_download_id = globals.description_downloader.m_retry_count;
    if (globals.description_download_id != description_download_id && network_life_cycle_can_download_files())
    {
        long status = multiplayer_game_hopper_get_description_data((char *)&globals.description_downloader, &buffer, (unsigned int *)&buffer_size);
        if (status == 2)
        {
            if (multiplayer_game_hopper_unpack_hopper_description(buffer, buffer_size, &globals.description))
            {
                globals.description_valid = true;
                globals.description_failed = false;
            }
            else
            {
                globals.description_valid = false;
                globals.description_failed = true;
            }
            globals.description_download_id = description_download_id;
        }
        else if (status != 1)
        {
            globals.description_failed = true;
            globals.description_download_id = description_download_id;
        }
    }

    long game_set_download_id = globals.game_set_downloader.m_retry_count;
    if (globals.game_set_download_id != game_set_download_id && network_life_cycle_can_download_files())
    {
        long status = multiplayer_game_hopper_get_game_set_data((char *)&globals.game_set_downloader, &buffer, (unsigned int *)&buffer_size);
        if (status == 2)
        {
            if (multiplayer_game_hopper_unpack_game_set(buffer, buffer_size, &globals.game_set))
            {
                globals.game_set_valid = true;
                globals.game_set_failed = false;
            }
            else
            {
                globals.game_set_valid = false;
                globals.game_set_failed = true;
            }
            globals.game_set_download_id = game_set_download_id;
        }
        else if (status != 1)
        {
            globals.game_set_valid = false;
            globals.game_set_failed = true;
            globals.game_set_download_id = game_set_download_id;
        }
    }

    long game_variant_status = network_http_buffer_downloader_get_game_variant(&globals.game_variant_downloader, &buffer, &buffer_size);
    if (game_variant_status == 2)
    {
        if (globals.game_variant_downloader.m_internal_status != 4)
        {
            if (multiplayer_game_hopper_unpack_game_variant(buffer, buffer_size, &globals.game_variant))
            {
                globals.game_variant_valid = true;
                globals.game_variant_failed = false;
            }
            else
            {
                globals.game_variant_valid = false;
                globals.game_variant_failed = true;
            }
            http_buffer_downloader_set_url(&globals.game_variant_downloader, &s_hopper_http_url());
            globals.game_variant_downloader.clear_hash();
        }
    }
    else if (game_variant_status != 1 && globals.game_variant_downloader.m_internal_status != 4)
    {
        globals.game_variant_failed = true;
        http_buffer_downloader_set_url(&globals.game_variant_downloader, &s_hopper_http_url());
        globals.game_variant_downloader.clear_hash();
    }

    long map_variant_status = network_http_buffer_downloader_get_map_variant(&globals.map_variant_downloader, &buffer, &buffer_size);
    if (map_variant_status == 2)
    {
        if (globals.map_variant_downloader.m_internal_status != 4)
        {
            if (multiplayer_game_hopper_unpack_map_variant(buffer, buffer_size, &globals.map_variant))
            {
                globals.map_variant_valid = true;
                globals.map_variant_failed = false;
            }
            else
            {
                globals.map_variant_valid = false;
                globals.map_variant_failed = true;
            }
            http_buffer_downloader_set_url(&globals.map_variant_downloader, &s_hopper_http_url());
            globals.map_variant_downloader.clear_hash();
        }
    }
    else if (map_variant_status != 1 && globals.map_variant_downloader.m_internal_status != 4)
    {
        globals.map_variant_failed = true;
        http_buffer_downloader_set_url(&globals.map_variant_downloader, &s_hopper_http_url());
        globals.map_variant_downloader.clear_hash();
    }
}


extern "C"
{

}
struct s_network_leaderboard_globals;

static __forceinline e_hopper_load_status multiplayer_game_hopper_manifest_file_load_status(const s_multiplayer_game_hopper_manifest_file *manifest)
{
    e_hopper_load_status status = _hopper_load_status_loading;
    if (manifest->data)
        status = _hopper_load_status_loaded;
    else if (manifest->failed)
        status = _hopper_load_status_failed;
    return status;
}

static __forceinline e_hopper_load_status multiplayer_game_hopper_manifest_get_load_status()
{
    e_hopper_load_status result = _hopper_load_status_none;
    const s_multiplayer_game_hopper_manifest_file *manifest = network_storage_manifest_get();
    if (manifest)
        result = multiplayer_game_hopper_manifest_file_load_status(manifest);
    return result;
}


static __forceinline e_hopper_load_status multiplayer_game_hopper_user_file_loaded_for_controller(e_controller_index controller_index)
{
    const s_network_banhammer_controller_data *controller = &g_network_banhammer_globals.controllers[controller_index];
    long status = 0;
    if (controller->flags & 1)
    {
        bool loaded = true;
        loaded = loaded && ((controller->flags >> 4) & 1);
        if (!loaded)
        {
            if (g_network_banhammer_globals.flags & 2)
                status = 3;
            else
                status = 1;
        }
        else
        {
            status = 2;
            g_network_banhammer_globals.flags &= ~2;
        }
    }
    return (e_hopper_load_status)status;
}

// @address 0x57c588 multiplayer_game_hopper_compute_start_error
extern "C" e_session_game_start_error multiplayer_game_hopper_compute_start_error(bool check_hopper, bool hopper_valid)
{
    long error = _session_game_start_error_none;
    long failure = _session_game_start_error_none;

    e_hopper_load_status manifest_status = multiplayer_game_hopper_manifest_get_load_status();
    if (!error && manifest_status == _hopper_load_status_loading)
        error = _session_game_start_error_network_configuration_pending;
    else if (!failure && manifest_status == _hopper_load_status_failed)
        failure = _session_game_start_error_network_configuration_unavailable;

    long configuration_status = 0;
    if (g_network_configuration_state.download_failed)
        configuration_status = 1;
    else if (g_network_configuration_state.downloaded_data_valid)
        configuration_status = 2;
    if (!error && configuration_status == 0)
        error = _session_game_start_error_network_configuration_pending;
    else if (!failure && configuration_status == 1)
        failure = _session_game_start_error_network_configuration_unavailable;

    long signatures_status = 1;
    if (g_network_banhammer_globals.map_signatures.disabled)
        signatures_status = 3;
    else if (g_network_banhammer_globals.map_signatures.valid)
        signatures_status = 2;
    if (!error && signatures_status == 1)
        error = _session_game_start_error_banhammer_pending;
    else if (!failure && signatures_status == 3)
        failure = _session_game_start_error_banhammer_unavailable;

    long banhammer_status = 2;
    long banhammer_count = 0;
    for (e_controller_index controller_index = _controller0; controller_index != k_no_controller;
        controller_index = next_controller(controller_index))
    {
        if (multiplayer_game_hopper_history_controller_valid(controller_index))
        {
            e_hopper_load_status load_status = network_banhammer_loaded_for_controller(controller_index, 0);
            if (load_status == _hopper_load_status_failed)
            {
                banhammer_status = 3;
                break;
            }
            if (load_status == _hopper_load_status_loading)
                banhammer_status = 1;
            banhammer_count++;
        }
    }
    if (banhammer_status == 2 && banhammer_count == 0)
        banhammer_status = 3;
    if (!error && banhammer_status == 1)
        error = _session_game_start_error_banhammer_pending;
    else if (!failure && banhammer_status == 3)
        failure = _session_game_start_error_banhammer_unavailable;

    if (check_hopper)
    {
        long hopper_status = multiplayer_game_hopper_get_load_status();
        if (!error && hopper_status == 1)
            error = _session_game_start_error_master_hopper_file_pending;
        else if (!failure && hopper_status == 3)
            failure = _session_game_start_error_master_hopper_file_unavailable;

        if (hopper_valid)
        {
            long composition_status = network_session_matchmaking_composition_session_state_valid();
            if (!error && composition_status == 1)
                error = _session_game_start_error_hopper_specific_file_pending;
            else if (!failure && composition_status == 3)
                failure = _session_game_start_error_hopper_specific_file_unavailable;

            long leaderboard_status = 2;
            for (long user_index = 0; user_index < 4; user_index++)
            {
                const s_leaderboard_user *user =
                    &network_leaderboard_globals.users[user_index];
                if (user->valid)
                {
                    if (user->failure_count > 0)
                    {
                        leaderboard_status = 3;
                        break;
                    }
                    if (user->state != 3 && user->state != 4 && user->state != 5)
                        leaderboard_status = 1;
                }
            }
            if (!error && leaderboard_status == 1)
                error = _session_game_start_error_player_stats_pending;
            else if (!failure && leaderboard_status == 3)
                failure = _session_game_start_error_player_stats_unavailable;

            long user_file_status = 2;
            long user_file_count = 0;
            for (e_controller_index controller_index = _controller0; controller_index != k_no_controller;
                controller_index = next_controller(controller_index))
            {
                if (multiplayer_game_hopper_history_controller_valid(controller_index))
                {
                    e_hopper_load_status load_status = multiplayer_game_hopper_user_file_loaded_for_controller(controller_index);
                    if (load_status == _hopper_load_status_failed)
                    {
                        user_file_status = 3;
                        break;
                    }
                    if (load_status == _hopper_load_status_loading)
                        user_file_status = 1;
                    user_file_count++;
                }
            }
            if (user_file_status == 2 && user_file_count == 0)
                user_file_status = 3;
            if (!error && user_file_status == 1)
                error = _session_game_start_error_banhammer_pending;
            else if (!failure && user_file_status == 3)
                failure = _session_game_start_error_banhammer_unavailable;
        }
    }

    return (e_session_game_start_error)(failure ? failure : error);
}
