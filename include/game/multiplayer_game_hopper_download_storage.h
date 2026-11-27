#ifndef GAME_MULTIPLAYER_GAME_HOPPER_DOWNLOAD_STORAGE_H
#define GAME_MULTIPLAYER_GAME_HOPPER_DOWNLOAD_STORAGE_H

#include <stddef.h>
#include "interface/utf8_types.h"
#include "game/game_variant.h"
#include "game/map_variant.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"

struct s_hopper_configuration_category
{
  unsigned short identifier;
  unsigned short category_flags;
  utf8 name[32];
};

struct s_hopper_team_configuration
{
  long team_count;
  long minimum_team_size;
  long maximum_team_size;
  union
  {
    struct
    {
      long maximum_team_imbalance;
      long big_squad_size_threshold;
      long maximum_big_squad_imbalance;
      bool enable_big_squad_mixed_skill_restrictions;
    } ranked;
    struct
    {
      bool allow_uneven_teams;
      bool allow_parties_to_split;
      char spare_after_allow_parties_to_split[18];
    } unranked;
  } configuration;
};

struct s_hopper_configuration_table_entry
{
  utf8 name[32];
  s_network_http_request_hash game_set_hash;
  unsigned short hopper_identifier;
  long hopper_category;
  long hopper_type;
  union { long sort_key; long retail_image_index; };
  long image_index;
  unsigned char xlast_index;
  unsigned __int64 start_time;
  unsigned __int64 end_time;
  long hopper_regions;
  long minimum_skill;
  long maximum_skill;
  long minimum_experience;
  long maximum_experience;
  long minimum_party_size;
  long maximum_party_size;
  long hopper_access_bit;
  long account_type_access;
  bool require_all_party_members_meet_experience_requirements;
  bool require_all_party_members_meet_access_requirements;
  bool require_all_party_members_meet_live_account_access_requirements;
  bool hide_hopper_from_experience_restricted_players;
  bool hide_hopper_from_access_restricted_players;
  bool hide_hopper_from_live_account_access_restricted_players;
  bool requires_beta_rights;
  bool requires_all_downloadable_maps;
  bool veto_enabled;
  bool guests_allowed;
  union { bool require_hosts_on_multiple_teams; bool runtime_veto_enabled; };
  union { bool balance_network_bandwidth_between_teams; bool runtime_guests_allowed; };
  bool runtime_require_hosts_on_multiple_teams;
  long stats_write;
  long language_filter;
  long country_code_filter;
  long gamerzone_filter;
  long quitter_filter_percentage;
  long quitter_filter_maximum_party_size;
  long rematch_countdown_timer;
  long rematch_group_formation;
  long repeated_opponents_to_consider_for_penalty;
  long repeated_opponents_experience_threshold;
  long repeated_opponents_skill_throttle_start;
  long repeated_opponents_skill_throttle_stop;
  long maximum_total_matchmaking_seconds;
  long gather_start_game_early_seconds;
  long gather_give_up_seconds;
  unsigned char chance_of_gathering[16];
  long experience_points_per_win;
  long experience_penalty_per_drop;
  union { long minimum_mu_per_level[49]; float skill_thresholds[49]; };
  unsigned char maximum_skill_level_match_delta[50];
  union { long trueskill_sigma_multiplier; float runtime_sigma_multiplier; };
  union { long trueskill_beta_performance_variation; float runtime_beta_performance_variation; };
  union { long trueskill_tau_dynamics_factor; float runtime_tau_dynamics_factor; };
  bool trueskill_adjust_tau_with_update_weight;
  long trueskill_draw_probability;
  long trueskill_hillclimb_w0;
  long trueskill_hillclimb_w50;
  long trueskill_hillclimb_w100;
  long trueskill_hillclimb_w150;
  long skill_update_weight_s0;
  long skill_update_weight_s10;
  long skill_update_weight_s20;
  long skill_update_weight_s30;
  long skill_update_weight_s40;
  long skill_update_weight_s50;
  long quality_update_weight_q0;
  long quality_update_weight_q25;
  long quality_update_weight_q50;
  long quality_update_weight_q75;
  long quality_update_weight_q100;
  long runtime_stats_write;
  long runtime_language_filter;
  long runtime_country_code_filter;
  bool runtime_gamerzone_filter;
  s_hopper_team_configuration team_configuration;
  __forceinline bool is_ffa() const { return hopper_type == 0 || hopper_type == 1; }
  __forceinline bool is_ranked() const { return hopper_type == 1 || hopper_type == 3; }
};

// March multiplayer_game_hopper.obj passes table entries straight to
// c_hopper_configuration methods, so the table holds configurations, not bare entries.
class c_hopper_configuration {
public:
  s_hopper_configuration_table_entry m_configuration;
  long get_minimum_player_count() const
  {
    int result = 0;
    switch (m_configuration.hopper_type) {
    case 0:
    case 1:
      result = m_configuration.team_configuration.team_count;
      break;
    case 2:
    case 3:
      result = m_configuration.team_configuration.minimum_team_size * m_configuration.team_configuration.team_count;
      break;
    }
    if (result >= 16)
      result = 16;
    return result;
  }
  long get_maximum_player_count() const;
  __forceinline const s_hopper_configuration_table_entry *get_hopper_configuration() const { return &m_configuration; }
  __forceinline float get_trueskill_beta_performance_variation() const
  {
      return m_configuration.runtime_beta_performance_variation;
  }
  __forceinline float get_trueskill_sigma_multiplier() const { return m_configuration.runtime_sigma_multiplier; }
  __forceinline unsigned short get_hopper_identifier() const { return m_configuration.hopper_identifier; }
  __forceinline long get_trueskill_hillclimb_w0() const { return m_configuration.trueskill_hillclimb_w0; }
  __forceinline long get_trueskill_hillclimb_w50() const { return m_configuration.trueskill_hillclimb_w50; }
  __forceinline long get_trueskill_hillclimb_w100() const { return m_configuration.trueskill_hillclimb_w100; }
  __forceinline long get_trueskill_hillclimb_w150() const { return m_configuration.trueskill_hillclimb_w150; }
  __forceinline long get_skill_update_weight_s0() const { return m_configuration.skill_update_weight_s0; }
  __forceinline long get_skill_update_weight_s10() const { return m_configuration.skill_update_weight_s10; }
  __forceinline long get_skill_update_weight_s20() const { return m_configuration.skill_update_weight_s20; }
  __forceinline long get_skill_update_weight_s30() const { return m_configuration.skill_update_weight_s30; }
  __forceinline long get_skill_update_weight_s40() const { return m_configuration.skill_update_weight_s40; }
  __forceinline long get_skill_update_weight_s50() const { return m_configuration.skill_update_weight_s50; }
  __forceinline long get_game_quality_update_weight_q0() const { return m_configuration.quality_update_weight_q0; }
  __forceinline long get_game_quality_update_weight_q25() const { return m_configuration.quality_update_weight_q25; }
  __forceinline long get_game_quality_update_weight_q50() const { return m_configuration.quality_update_weight_q50; }
  __forceinline long get_game_quality_update_weight_q75() const { return m_configuration.quality_update_weight_q75; }
  __forceinline long get_game_quality_update_weight_q100() const { return m_configuration.quality_update_weight_q100; }
  __forceinline long get_hopper_type() const { return m_configuration.hopper_type; }
  __forceinline long get_maximum_skill_level_match_delta(long skill_level) const
  {
    return m_configuration.maximum_skill_level_match_delta[skill_level - 1 < 0 ? 0 : skill_level - 1];
  }
  __forceinline long get_minimum_party_size() const { return m_configuration.minimum_party_size; }
  __forceinline long get_maximum_party_size() const { return m_configuration.maximum_party_size; }
  __forceinline long get_gather_start_game_early_seconds() const { return m_configuration.gather_start_game_early_seconds; }
  __forceinline long get_gather_give_up_seconds() const { return m_configuration.gather_give_up_seconds; }
  __forceinline long get_hopper_category() const { return m_configuration.hopper_category; }
  __forceinline long get_image_index() const { return m_configuration.retail_image_index; }
  __forceinline long get_xlast_index() const { return m_configuration.image_index; }
  __forceinline unsigned char get_rich_presence_context_id() const { return m_configuration.xlast_index; }
  __forceinline long get_rematch_countdown_timer() const { return m_configuration.rematch_countdown_timer; }
  __forceinline long get_rematch_group_formation() const { return m_configuration.rematch_group_formation; }
  __forceinline long get_repeated_opponents_to_consider_for_penalty() const { return m_configuration.repeated_opponents_to_consider_for_penalty; }
  __forceinline long get_repeated_opponents_experience_threshold() const { return m_configuration.repeated_opponents_experience_threshold; }
  __forceinline long get_repeated_opponents_skill_throttle_start() const { return m_configuration.repeated_opponents_skill_throttle_start; }
  __forceinline long get_repeated_opponents_skill_throttle_stop() const { return m_configuration.repeated_opponents_skill_throttle_stop; }
  __forceinline long get_experience_points_per_win() const { return m_configuration.experience_points_per_win; }
  __forceinline long get_experience_penalty_per_drop() const { return m_configuration.experience_penalty_per_drop; }
  __forceinline long get_trueskill_draw_probability() const { return m_configuration.trueskill_draw_probability; }
  __forceinline const char *get_name() const { return (const char *)m_configuration.name; }
  __forceinline const utf8 *get_hopper_name() const { return m_configuration.name; }
  __forceinline bool is_ranked() const { return m_configuration.hopper_type == 1 || m_configuration.hopper_type == 3; }
  __forceinline bool is_team_based() const { return m_configuration.hopper_type == 2 || m_configuration.hopper_type == 3; }
  __forceinline bool is_veto_enabled() const { return m_configuration.runtime_veto_enabled; }
  __forceinline bool require_hosts_on_multiple_teams() const { return m_configuration.runtime_require_hosts_on_multiple_teams; }
  __forceinline float get_scaled_trueskill_beta_performance_variation() const { return m_configuration.runtime_beta_performance_variation / 8.3333f; }
  __forceinline float get_scaled_trueskill_tau_dynamics_factor() const { return m_configuration.runtime_tau_dynamics_factor / 8.3333f; }
  long get_maximum_team_imbalance() const;
  long get_team_count() const;
  long get_maximum_team_size() const;
  long get_big_squad_size_threshold() const;
};
typedef char hopper_configuration_wrapper_size[(sizeof(c_hopper_configuration) == 0x260) ? 1 : -1];

struct s_hopper_description
{
  unsigned short hopper_identifier;
  long flag;
  utf8 description[256];
};

struct s_hopper_description_table { long count; s_hopper_description entries[64]; };

struct s_hopper_configuration_table
{
  long count;
  long category_count;
  s_hopper_configuration_category categories[4];
  c_hopper_configuration entries[32];
};

struct s_hopper_http_downloader : c_http_buffer_downloader
{
    __forceinline void clear_hash()
    {
        if (m_hash_valid == true)
        {
            reset();
            m_hash_valid = false;
        }
    }

};

struct s_hopper_game_entry {
  long weight;
  long minimum_player_count;
  bool optional;
  unsigned char optional_content;
  long map_id;
  char game_variant_name[32];
  s_network_http_request_hash game_variant_hash;
  char map_variant_name[32];
  s_network_http_request_hash map_variant_hash;
};
struct s_hopper_game_set { long count; s_hopper_game_entry entries[1]; };

struct s_hopper_game_set_table
{
    union
    {
        char data[0x3c0c];
        s_hopper_game_set decoded;
    };
};
typedef char hopper_game_entry_size_check[(sizeof(s_hopper_game_entry) == 120) ? 1 : -1];
typedef char hopper_game_set_table_size_check[(sizeof(s_hopper_game_set_table) == 0x3c0c) ? 1 : -1];

struct s_hopper_update_globals
{
    const c_hopper_configuration *current_configuration;
    long unused;
    bool configuration_valid;
    bool configuration_failed;
    long configuration_download_id;
    s_hopper_http_downloader configuration_downloader;   // (m_retry_count at 0x069c)
    char configuration_buffer[0x5be4 - 0x06a0];
    bool description_valid;
    bool description_failed;
    long description_download_id;
    s_hopper_http_downloader description_downloader;     // (m_retry_count at 0x6278)
    char description_buffer[0xad2c - 0x627c];
    bool game_set_valid;
    bool game_set_failed;
    long game_set_download_id;
    s_hopper_http_downloader game_set_downloader;        // (m_retry_count at 0xb3c0)
    char game_set_buffer[0xf868 - 0xb3c4];
    long game_set_parsed_download_id;
    void *game_set_parsed_data;
    unsigned int game_set_parsed_size;
    bool game_variant_valid;
    bool game_variant_failed;
    s_hopper_http_downloader game_variant_downloader;    // (m_hash_valid at 0xf98c, m_internal_status at 0xfee0)
    char game_variant_buffer[0x10a18 - 0xff08];
    bool map_variant_valid;
    bool map_variant_failed;
    s_hopper_http_downloader map_variant_downloader;     // (m_hash_valid at 0x10b30, m_internal_status at 0x11084)
    char map_variant_buffer[0x1f9e8 - 0x110ac];
    c_game_variant game_variant;
    c_map_variant map_variant;
    s_hopper_configuration_table configuration;
    s_hopper_description_table description;
    s_hopper_game_set_table game_set;
};

typedef char hopper_description_size_check[(sizeof(s_hopper_description) == 0x108) ? 1 : -1];
typedef char hopper_description_table_size_check[(sizeof(s_hopper_description_table) == 0x4204) ? 1 : -1];
typedef char hopper_configuration_entry_size_check[(sizeof(s_hopper_configuration_table_entry) == 0x260) ? 1 : -1];
typedef char hopper_description_storage_offset_check[
    (sizeof(void *) != 4 || offsetof(s_hopper_update_globals, description) == 0x32978) ? 1 : -1];
typedef char hopper_game_set_storage_offset_check[
    (sizeof(void *) != 4 || offsetof(s_hopper_update_globals, game_set) == 0x36b7c) ? 1 : -1];

#endif
