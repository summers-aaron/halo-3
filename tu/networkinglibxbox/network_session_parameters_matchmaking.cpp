// networkinglibxbox/network_session_parameters_matchmaking.obj: from the records 0x4136f0 0x4138a8 0x4139e0 0x413ab0 0x413d20 0x413f80 0x413f90 0x414010 0x4140e0 0x414208 0x4144d0 0x414520 0x414670 0x414788 0x4148e0 0x414a68 0x414b08 0x414d50 0x414d60 0x414db8 0x414e88 0x414f60 0x415078 0x415108 0x415298 0x4152e8
   // @compiler xdk6130
// @flags /TP /O2
// @address 0x413f80 ?matchmaking_requested_hopper_encode_function@@YAXPAVc_bitstream@@PBXJ@Z
// @address 0x4139e0 ?set@c_network_session_parameter_matchmaking_hopper_list@@QAA_NPBUs_network_session_matchmaking_hopper_list@@@Z
// @address 0x413ab0 ?matchmaking_hopper_list_encode_function@@YAXPAVc_bitstream@@PBXJ@Z
// @address 0x413f90 ?matchmaking_requested_hopper_decode_function@@YA_NPAVc_bitstream@@PAXJ@Z
// @address 0x414010 ?set@c_network_session_parameter_matchmaking_hopper@@QAA_NPBUs_network_session_matchmaking_hopper@@@Z
// @address 0x414db8 ?set@c_network_session_parameter_matchmaking_rematch_data@@QAA_NPBUs_network_session_parameter_matchmaking_rematch_data@@@Z
#include "cseries/cseries_types.h"
#include <string.h>
#include "memory/bitstream.h"
#include "math/integer_math.h"
#include "networking/session/network_session_parameters_matchmaking.h"


struct s_game_context;
struct s_owner;
extern "C" {
}
#include "networking/session/network_session_parameters.h"
#include "networking/session/network_session_parameter_access.h"
#include "networking/session/network_session_parameter_matchmaking_types.h"


struct s_network_session_matchmaking_requested_hopper
{
    unsigned short identifier;
};

struct s_matchmaking_criteria_basic {
    long search_stage;
    long searching_skill_min;
    long searching_skill_max;
    long contacted_session_count;
    long potential_session_count;
    long search_queries_submitted;
};

struct s_matchmaking_criteria_ranked {
    long state;
    long map_load_progress_percentage;
    long countdown_timer;
    long current_veto_count;
    long max_veto_count;
    long game_map_index;
    bool load_failure_encountered;
};

struct s_matchmaking_search_variant {
    long type;
    union {
        s_matchmaking_criteria_basic basic;
        struct { long give_up_timer_seconds, give_up_timer_maximum_seconds, qos_listener_data_replies_sent; } mixed;
        s_matchmaking_criteria_ranked ranked;
        struct { bool countdown_running; long countdown; } flagged;
    };
};

struct s_matchmaking_settings_9 {
    long party_skill_range_min;
    long party_skill_range_max;
    long searching_skill_range_min;
    long searching_skill_range_max;
    long searching_party_size_min;
    long searching_party_size_max;
    bool search_party_big_and_mixed;
    long hopper_required_player_count;
    long hopper_preferred_player_count;
};

// @address 0x4136f0 ?matchmaking_game_list_encode_function@@YAXPAVc_bitstream@@PBXJ@Z
void matchmaking_game_list_encode_function(c_bitstream *stream, const void *data, long size)
{
    const s_game_hopper_picked_game_collection *game_list = (const s_game_hopper_picked_game_collection *)data;
    for (unsigned long game_index = 0; game_index < 4; game_index++) {
        stream->write_integer("map-id", game_list->games[game_index].map_id, 32);
        stream->write_string("game-variant-name", game_list->games[game_index].game_variant_name, 32);
        stream->write_raw_data("game-variant-description", game_list->games[game_index].game_variant_hash.hash, 160);
        stream->write_string("map-variant-name", game_list->games[game_index].map_variant_name, 32);
        stream->write_raw_data("map-variant-description", game_list->games[game_index].map_variant_hash.hash, 160);
    }
}

// @address 0x4138a8 ?matchmaking_game_list_decode_function@@YA_NPAVc_bitstream@@PAXJ@Z
bool matchmaking_game_list_decode_function(c_bitstream *stream, void *data, long size)
{
    bool success = true;
    s_game_hopper_picked_game_collection *game_list = (s_game_hopper_picked_game_collection *)data;
    for (unsigned long game_index = 0; game_index < 4; game_index++) {
        game_list->games[game_index].map_id = stream->read_integer("map-id", 32);
        stream->read_string("game-variant-name", game_list->games[game_index].game_variant_name, 32);
        stream->read_raw_data("game-variant-description", game_list->games[game_index].game_variant_hash.hash, 160);
        stream->read_string("map-variant-name", game_list->games[game_index].map_variant_name, 32);
        stream->read_raw_data("map-variant-description", game_list->games[game_index].map_variant_hash.hash, 160);
    }
    success = success && !stream->error_occurred();
    return success;
}

bool c_network_session_parameter_matchmaking_hopper_list::set(const s_network_session_matchmaking_hopper_list *new_settings)
{
    bool success = false;
    if (this->set_allowed()) {
        if (memcmp(new_settings, &m_data, sizeof(m_data)) != 0 || !get_allowed()) {
            memcpy(&m_data, new_settings, sizeof(m_data));
            set_update_required();
        }
        success = true;
    }
    return success;
}

void matchmaking_hopper_list_encode_function(c_bitstream *stream,const void *data,long size) {
 const s_network_session_matchmaking_hopper_list *list=(const s_network_session_matchmaking_hopper_list *)data;
 stream->write_integer("status",list->hopper_load_status,2);
 stream->write_integer("version",list->membership_update_number,32);
 stream->write_integer("mask",list->category_populated_mask,16);
 stream->write_integer("category-count",list->category_count,3);
 for(long index=0;index<list->category_count;index++) {
  stream->write_integer("category-identifier",list->categories[index].identifier,16);
  stream->write_string("category-name",list->categories[index].name,32);
  stream->write_integer("category-count",list->categories[index].hopper_count,6);
 }
 stream->write_integer("hopper-count",list->hopper_count,6);
 for(long index=0;index<list->hopper_count;index++) {
  const s_network_session_matchmaking_hopper_description *h=&list->hoppers[index];
  stream->write_integer("hopper-identifier",h->identifier,16);
  stream->write_integer("hopper-category",h->category_identifier,16);
  stream->write_integer("hopper-presence",h->rich_presence_context_id,16);
  stream->write_integer("hopper-image",h->image_index,6);
  stream->write_bool("visible",h->is_visible);
  stream->write_bool("playable",h->is_playable);
  stream->write_bool("ranked",h->is_ranked);
  stream->write_bool("extra",h->experience_penalty_enabled);
  stream->write_string("hopper-name",h->name,32);
 }
}

// @address 0x413d20 ?matchmaking_hopper_list_decode_function@@YA_NPAVc_bitstream@@PAXJ@Z
bool matchmaking_hopper_list_decode_function(c_bitstream *stream, void *data, long size)
{
    bool result = true;
    s_network_session_matchmaking_hopper_list *list = (s_network_session_matchmaking_hopper_list *)data;
    list->hopper_load_status = stream->read_integer("hopper_load_status", 2);
    list->membership_update_number = stream->read_integer("membership_update_number", 32);
    list->category_populated_mask = stream->read_integer("category_populated_mask", 16);
    list->category_count = stream->read_integer("category_count", 3);
    for (long category_index = 0; category_index < list->category_count; ++category_index) {
        list->categories[category_index].identifier = (unsigned short)stream->read_integer("peer-evict-mask", 16);
        stream->read_string("name", list->categories[category_index].name, 32);
        list->categories[category_index].hopper_count = (unsigned short)stream->read_integer("hopper-count", 6);
    }
    list->hopper_count = stream->read_integer("hopper_count", 6);
    for (long hopper_index = 0; hopper_index < list->hopper_count; ++hopper_index) {
        s_network_session_matchmaking_hopper_description *h = &list->hoppers[hopper_index];
        h->identifier = (unsigned short)stream->read_integer("peer-evict-mask", 16);
        h->category_identifier = (unsigned short)stream->read_integer("peer-evict-mask", 16);
        h->rich_presence_context_id = (unsigned char)stream->read_integer("hopper-presence", 16);
        h->image_index = stream->read_integer("image_index", 6);
        h->is_visible = stream->read_bool("is_visible");
        h->is_playable = stream->read_bool("is_playable");
        h->is_ranked = stream->read_bool("is_ranked");
        h->experience_penalty_enabled = stream->read_bool("experience_penalty_enabled");
        char *name = h->name;
        stream->read_string("name", name, 32);
    }
    result = result && list->hopper_load_status >= 0 && list->hopper_load_status < 4;
    result = result && !stream->error_occurred();
    return result;
}

void matchmaking_requested_hopper_encode_function(c_bitstream *stream, const void *data, long size)
{
    const s_network_session_matchmaking_requested_hopper *hopper = (const s_network_session_matchmaking_requested_hopper *)data;
    stream->write_integer("requested_hopper_identifier", hopper->identifier, 16);
}

bool matchmaking_requested_hopper_decode_function(c_bitstream *stream, void *data, long size)
{
  unsigned short *value = (unsigned short *)data;
  unsigned char overflow;
  *value = ((c_bitstream *)(stream))->read_integer("requested_hopper_identifier", 16);
  overflow = (stream->m_current.current_stream_bit_position > (stream->m_data_size_bytes << 3)) ? 1 : 0;
  if (stream->m_error_occurred)
    overflow = 1;
  return (!overflow && 1) ? 1 : 0;
}

bool c_network_session_parameter_matchmaking_hopper::set(const s_network_session_matchmaking_hopper *new_settings)
{
    bool success = false;
    if (this->set_allowed()) {
        if (memcmp(new_settings, &m_data, sizeof(m_data)) != 0 || !get_allowed()) {
            memcpy(&m_data, new_settings, sizeof(m_data));
            set_update_required();
        }
        success = true;
    }
    return success;
}

// 0x4140e0
// @address 0x4140e0 matchmaking_hopper_encode_function
void matchmaking_hopper_encode_function(c_bitstream *stream, const void *data, long size)
{
    const s_network_session_matchmaking_hopper *hopper = (const s_network_session_matchmaking_hopper *)data;
    stream->write_integer("hopper_identifier", hopper->hopper_identifier, 16);
    stream->write_integer("xlast_index", hopper->xlast_index, 5);
    stream->write_bool("is_ranked", hopper->is_ranked);
    stream->write_bool("veto_allowed", hopper->veto_allowed);
    stream->write_bool("rematch_allowed", hopper->rematch_allowed);
    stream->write_integer("rematch_timer", hopper->rematch_timer, 10);
    stream->write_bool("team_game", hopper->team_game);
    stream->write_integer("draw-probability", hopper->trueskill_draw_probability, 7);
    stream->write_raw_data("trueskill_beta_performance_variation", &hopper->trueskill_beta_performance_variation, 0x20);
    stream->write_raw_data("trueskill_tau_dynamics_factor", &hopper->trueskill_tau_dynamics_factor, 0x20);
    stream->write_integer("experience-base-increment", hopper->experience_base_increment, 30);
    stream->write_integer("experience-penalty-decrement", hopper->experience_penalty_decrement, 30);
    stream->write_bool("require-hosts-on-multiple-teams", hopper->require_hosts_on_multiple_teams);
    stream->write_raw_data("system_seconds", &hopper->system_seconds, 0x20);
    stream->write_integer("max-party-size", (unsigned int)hopper->maximum_party_size - 1, 4);
    stream->write_integer("repeated_opponents_to_consider_for_penalty", hopper->repeated_opponents_to_consider_for_penalty, 7);
    stream->write_integer("repeated_opponents_experience_threshold", hopper->repeated_opponents_experience_threshold, 4);
    stream->write_integer("repeated_opponents_skill_throttle_start", hopper->repeated_opponents_skill_throttle_start, 4);
    stream->write_integer("repeated_opponents_skill_throttle_stop", hopper->repeated_opponents_skill_throttle_stop, 4);
    stream->write_integer("stats_write", hopper->stats_write, 2);
    stream->write_integer("language_filter", hopper->language_filter, 2);
    stream->write_integer("country_code_filter", hopper->country_code_filter, 2);
    stream->write_bool("gamerzone_filter", hopper->gamerzone_filter);
}

// @address 0x414208 matchmaking_hopper_decode
bool matchmaking_hopper_decode(c_bitstream *stream, void *data, long size)
{
    s_network_session_matchmaking_hopper *hopper = (s_network_session_matchmaking_hopper *)data;
    hopper->hopper_identifier = stream->read_integer("hopper_identifier", 16);
    hopper->xlast_index = stream->read_integer("xlast_index", 5);
    hopper->is_ranked = stream->read_bool("is_ranked");
    hopper->veto_allowed = stream->read_bool("veto_allowed");
    hopper->rematch_allowed = stream->read_bool("rematch_allowed");
    hopper->rematch_timer = stream->read_integer("rematch_timer", 10);
    hopper->team_game = stream->read_bool("team_game");
    hopper->trueskill_draw_probability = stream->read_integer("draw-probability", 7);
    read_bytes32(stream, (unsigned char *)&hopper->trueskill_beta_performance_variation);
    read_bytes32(stream, (unsigned char *)&hopper->trueskill_tau_dynamics_factor);
    hopper->experience_base_increment = stream->read_integer("experience-base-increment", 30);
    hopper->experience_penalty_decrement = stream->read_integer("experience-penalty-decrement", 30);
    hopper->require_hosts_on_multiple_teams = stream->read_bool("require-hosts-on-multiple-teams");
    read_bytes32(stream, (unsigned char *)&hopper->system_seconds);
    hopper->maximum_party_size = stream->read_integer("max-party-size", 4) + 1;
    hopper->repeated_opponents_to_consider_for_penalty = stream->read_integer("repeated_opponents_to_consider_for_penalty", 7);
    hopper->repeated_opponents_experience_threshold = stream->read_integer("repeated_opponents_experience_threshold", 4);
    hopper->repeated_opponents_skill_throttle_start = stream->read_integer("repeated_opponents_skill_throttle_start", 4);
    hopper->repeated_opponents_skill_throttle_stop = stream->read_integer("repeated_opponents_skill_throttle_stop", 4);
    hopper->stats_write = stream->read_integer("stats_write", 2);
    hopper->language_filter = stream->read_integer("language_filter", 2);
    hopper->country_code_filter = stream->read_integer("country_code_filter", 2);
    hopper->gamerzone_filter = stream->read_bool("gamerzone_filter");
    bool success = true;
    success = success && !stream->error_occurred();
    success = success && hopper->stats_write >= 0 && hopper->stats_write < 4;
    success = success && hopper->language_filter >= 0 && hopper->language_filter < 4;
    success = success && hopper->country_code_filter >= 0 && hopper->country_code_filter < 4;
    return success;
}

// 0x4144d0
// @address 0x4144d0 matchmaking_search_preference_decode
bool matchmaking_search_preference_decode(c_bitstream *stream, void *data, long size)
{
    long *status = (long *)data;
    *status = stream->read_integer("search-preference", 2);
    return *status >= 0 && *status < 4;
}

// 0x414520
// @address 0x414520 matchmaking_progress_decode_basic_criteria
bool matchmaking_progress_decode_basic_criteria(c_bitstream *stream, s_matchmaking_criteria_basic *criteria)
{
    criteria->search_stage = stream->read_integer("search_stage", 2);
    criteria->searching_skill_min = stream->read_integer("searching_skill_min", 6);
    criteria->searching_skill_max = stream->read_integer("searching_skill_max", 6);
    criteria->contacted_session_count = stream->read_integer("contacted_session_count", 10);
    criteria->potential_session_count = stream->read_integer("potential_session_count", 6);
    criteria->search_queries_submitted = stream->read_integer("search_queries_submitted", 6);
    bool success = true;
    success = success && !stream->error_occurred();
    success = success && criteria->search_stage >= 0 && criteria->search_stage <= 3;
    success = success && criteria->searching_skill_min >= 1 && criteria->searching_skill_min <= 50;
    success = success && criteria->searching_skill_max >= 1 && criteria->searching_skill_max <= 50;
    return success;
}

// 0x414670
// @address 0x414670 matchmaking_progress_decode_ranked_criteria
bool matchmaking_progress_decode_ranked_criteria(c_bitstream *stream, s_matchmaking_criteria_ranked *criteria)
{
    criteria->state = stream->read_integer("state", 3);
    criteria->map_load_progress_percentage = stream->read_integer("map_load_progress_percentage", 7);
    criteria->countdown_timer = stream->read_integer("countdown_timer", 10);
    criteria->current_veto_count = stream->read_integer("current_veto_count", 5);
    criteria->max_veto_count = stream->read_integer("max_veto_count", 5);
    criteria->game_map_index = stream->read_integer("game_map_index", 2);
    criteria->load_failure_encountered = stream->read_bool("load_failure_encountered");
    bool success = true;
    success = success && !stream->error_occurred();
    success = success && criteria->map_load_progress_percentage >= 0 && criteria->map_load_progress_percentage <= 100;
    return success;
}

// 0x414788
// @address 0x414788 matchmaking_progress_encode
void matchmaking_progress_encode(c_bitstream *stream, const void *data, long size)
{
    const s_matchmaking_search_variant *criteria = (const s_matchmaking_search_variant *)data;
    stream->write_integer("progress-type", criteria->type, 3);
    switch (criteria->type) {
    case 1:
        stream->write_integer("search_stage", criteria->basic.search_stage, 2);
        stream->write_integer("searching_skill_min", criteria->basic.searching_skill_min, 6);
        stream->write_integer("searching_skill_max", criteria->basic.searching_skill_max, 6);
        stream->write_integer("contacted_session_count", criteria->basic.contacted_session_count, 10);
        stream->write_integer("potential_session_count", criteria->basic.potential_session_count, 6);
        stream->write_integer("search_queries_submitted", criteria->basic.search_queries_submitted, 6);
        break;
    case 2:
        stream->write_integer("give_up_timer_seconds", criteria->mixed.give_up_timer_seconds, 10);
        stream->write_integer("give_up_timer_maximum_seconds", criteria->mixed.give_up_timer_maximum_seconds, 10);
        stream->write_integer("qos_listener_data_replies_sent", criteria->mixed.qos_listener_data_replies_sent, 8);
        break;
    case 4:
        stream->write_integer("state", criteria->ranked.state, 3);
        stream->write_integer("map_load_progress_percentage", criteria->ranked.map_load_progress_percentage, 7);
        stream->write_integer("countdown_timer", criteria->ranked.countdown_timer, 10);
        stream->write_integer("current_veto_count", criteria->ranked.current_veto_count, 5);
        stream->write_integer("max_veto_count", criteria->ranked.max_veto_count, 5);
        stream->write_integer("game_map_index", criteria->ranked.game_map_index, 2);
        stream->write_bool("load_failure_encountered", criteria->ranked.load_failure_encountered);
        break;
    case 5:
        stream->write_bool("countdown_running", criteria->flagged.countdown_running);
        stream->write_integer("countdown", criteria->flagged.countdown, 10);
        break;
    }
}

// 0x4148e0
static __forceinline bool network_session_parameters_matchmaking_progress_overflowed(c_bitstream *stream)
{
    bool overflowed = stream->m_current.current_stream_bit_position > (stream->m_data_size_bytes << 3);
    if (stream->m_error_occurred)
        overflowed = true;
    return overflowed;
}
// @address 0x4148e0 matchmaking_progress_decode_function
bool matchmaking_progress_decode_function(c_bitstream *stream, void *data, long size)
{
    s_matchmaking_search_variant *progress = (s_matchmaking_search_variant *)data;
    bool success = true;
    unsigned int progress_type = stream->read_integer("progress-type", 3);
    int criteria_result = progress_type - 1;
    progress->type = progress_type;
    if ((unsigned int)criteria_result <= 4) {
        do {
            switch (criteria_result) {
            case 0:
                criteria_result = (unsigned char)matchmaking_progress_decode_basic_criteria(stream, &progress->basic);
                break;
            case 1:
                progress->mixed.give_up_timer_seconds = stream->read_integer("give_up_timer_seconds", 10);
                progress->mixed.give_up_timer_maximum_seconds = stream->read_integer("give_up_timer_maximum_seconds", 10);
                progress->mixed.qos_listener_data_replies_sent = stream->read_integer("qos_listener_data_replies_sent", 8);
                criteria_result = !network_session_parameters_matchmaking_progress_overflowed(stream);
                break;
            case 3:
                criteria_result = (unsigned char)matchmaking_progress_decode_ranked_criteria(stream, &progress->ranked);
                break;
            case 4:
                progress->flagged.countdown_running = stream->read_bool("countdown_running");
                progress->flagged.countdown = stream->read_integer("countdown", 10);
                criteria_result = !network_session_parameters_matchmaking_progress_overflowed(stream);
                break;
            case 2:
                continue;
            }
            success = success && criteria_result != 0;
        } while (false);
    }
    return success && !network_session_parameters_matchmaking_progress_overflowed(stream);
}

// @address 0x414a68 matchmaking_progress_search_criteria_encode
void matchmaking_progress_search_criteria_encode(c_bitstream *stream, const void *data, long size)
{
    const s_matchmaking_settings_9 *settings = (const s_matchmaking_settings_9 *)data;
    stream->write_integer("party_skill_range_min", settings->party_skill_range_min, 6);
    stream->write_integer("party_skill_range_max", settings->party_skill_range_max, 6);
    stream->write_integer("searching_skill_range_min", settings->searching_skill_range_min, 6);
    stream->write_integer("searching_skill_range_max", settings->searching_skill_range_max, 6);
    stream->write_integer("searching_party_size_min", settings->searching_party_size_min - 1, 4);
    stream->write_integer("searching_party_size_max", settings->searching_party_size_max - 1, 4);
    stream->write_bool("search_party_big_and_mixed", settings->search_party_big_and_mixed);
    stream->write_integer("hopper_required_player_count", settings->hopper_required_player_count - 1, 4);
    stream->write_integer("hopper_preferred_player_count", settings->hopper_preferred_player_count - 1, 4);
}

// 0x414b08
// @address 0x414b08 matchmaking_progress_search_criteria_decode
bool matchmaking_progress_search_criteria_decode(c_bitstream *stream, void *data, long size)
{
    bool success = true;
    s_matchmaking_settings_9 *criteria = (s_matchmaking_settings_9 *)data;
    criteria->party_skill_range_min = stream->read_integer("party_skill_range_min", 6);
    criteria->party_skill_range_max = stream->read_integer("party_skill_range_max", 6);
    criteria->searching_skill_range_min = stream->read_integer("searching_skill_range_min", 6);
    criteria->searching_skill_range_max = stream->read_integer("searching_skill_range_max", 6);
    criteria->searching_party_size_min = stream->read_integer("searching_party_size_min", 4) + 1;
    criteria->searching_party_size_max = stream->read_integer("searching_party_size_max", 4) + 1;
    criteria->search_party_big_and_mixed = stream->read_bool("search_party_big_and_mixed");
    criteria->hopper_required_player_count = stream->read_integer("hopper_required_player_count", 4) + 1;
    criteria->hopper_preferred_player_count = stream->read_integer("hopper_preferred_player_count", 4) + 1;
    success = success && !stream->error_occurred();
    success = success && criteria->party_skill_range_min >= 1 && criteria->party_skill_range_min <= 50;
    success = success && criteria->party_skill_range_max >= 1 && criteria->party_skill_range_max <= 50;
    success = success && criteria->searching_skill_range_min >= 1 && criteria->searching_skill_range_min <= 50;
    success = success && criteria->searching_skill_range_max >= 1 && criteria->searching_skill_range_max <= 50;
    success = success && criteria->searching_party_size_min >= 1 && criteria->searching_party_size_min <= 16;
    success = success && criteria->searching_party_size_max >= 1 && criteria->searching_party_size_max <= 16;
    success = success && criteria->hopper_required_player_count >= 1 && criteria->hopper_required_player_count <= 16;
    success = success && criteria->hopper_preferred_player_count >= 1 && criteria->hopper_preferred_player_count <= 16;
    return success;
}

// @address 0x414d50 matchmaking_assembly_peer_evict_mask_encode
void matchmaking_assembly_peer_evict_mask_encode(c_bitstream *stream, const void *data, long size)
{
    const long *value = (const long *)data;
    stream->write_integer("peer-evict-mask", *value, 16);
}

// @address 0x414d60 matchmaking_assembly_peer_evict_mask_decode
bool matchmaking_assembly_peer_evict_mask_decode(c_bitstream *stream, void *data, long size)
{
    long *value = (long *)data;
    unsigned long identifier = stream->read_integer("peer-evict-mask", 16);
    *value = identifier;
    return identifier != 0 && count_bits(identifier) < 16;
}

bool c_network_session_parameter_matchmaking_rematch_data::set(const s_network_session_parameter_matchmaking_rematch_data *new_settings)
{
    bool success = false;
    if (this->set_allowed()) {
        if (memcmp(new_settings, &m_data, sizeof(m_data)) != 0 || !get_allowed()) {
            memcpy(&m_data, new_settings, sizeof(m_data));
            set_update_required();
        }
        success = true;
    }
    return success;
}


// @address 0x414e88 ?handle_change_request_payload@c_network_session_parameter_matchmaking_rematch_data@@EAA_NPBX@Z
bool c_network_session_parameter_matchmaking_rematch_data::handle_change_request_payload(const void *payload)
{
    bool result = true;
    const s_network_session_parameter_matchmaking_rematch_data *requested = (const s_network_session_parameter_matchmaking_rematch_data *)payload;
    if (change_request_data_valid(requested)) {
        for (long team_index = 0; team_index < 16; team_index++) {
            if ((requested->team_rematch_mask & (1 << team_index)) == 0)
                continue;
            s_network_session_parameter_matchmaking_rematch_data current =
                m_data;
            if ((current.team_rematch_mask & (1 << team_index)) == 0)
                break;
            const s_network_session_parameter_matchmaking_rematch_team_data *requested_team = &requested->teams[team_index];
            s_network_session_parameter_matchmaking_rematch_team_data *team = &current.teams[team_index];
            if (team->host_peer_index != requested_team->host_peer_index)
                break;
            const s_transport_session_description *host_description = &requested_team->host_description;
            team->join_data_valid = true;
            memcpy(&team->host_description, host_description, sizeof(team->host_description));
            result = set(&current);
            break;
        }
    }
    return result;
}

// @address 0x414f60 ?change_request_data_valid@c_network_session_parameter_matchmaking_rematch_data@@SA_NPBUs_network_session_parameter_matchmaking_rematch_data@@@Z
bool c_network_session_parameter_matchmaking_rematch_data::change_request_data_valid(const s_network_session_parameter_matchmaking_rematch_data *rematch)
{
    bool valid = false;
    if (count_bits(rematch->team_rematch_mask) == 1) {
        bool found = false;
        for (long team_index = 0; team_index < 16; team_index++) {
            if (rematch->team_rematch_mask & (1 << team_index)) {
                const s_network_session_parameter_matchmaking_rematch_team_data *team = &rematch->teams[team_index];
                if (team->join_data_valid && team->host_peer_index >= 0 && team->host_peer_index < 16 && team->rematch_peer_mask != 0)
                    valid = true;
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }
    return valid;
}

// @address 0x415078 ?matchmaking_rematch_encode_function@@YAXPAVc_bitstream@@PBXJ@Z
void matchmaking_rematch_encode_function(c_bitstream *stream, const void *data, long size)
{
    const s_network_session_parameter_matchmaking_rematch_data *rematch = (const s_network_session_parameter_matchmaking_rematch_data *)data;
    stream->write_integer("team-rematch-mask", rematch->team_rematch_mask, 16);
    for (long team_index = 0; team_index < 16; team_index++) {
        if (rematch->team_rematch_mask & (1 << team_index)) {
            const s_network_session_parameter_matchmaking_rematch_team_data *team = &rematch->teams[team_index];
            stream->write_integer("host-peer-index", team->host_peer_index, 4);
            stream->write_integer("rematch-peer-mask", team->rematch_peer_mask, 16);
            stream->write_bool("join-data-valid", team->join_data_valid);
            if (team->join_data_valid)
                stream->write_raw_data("host-description", &team->host_description, 480);
        }
    }
}

// @address 0x415108 ?matchmaking_rematch_decode_function@@YA_NPAVc_bitstream@@PAXJ@Z
bool matchmaking_rematch_decode_function(c_bitstream *stream, void *data, long size)
{
    bool success = true;
    s_network_session_parameter_matchmaking_rematch_data *rematch = (s_network_session_parameter_matchmaking_rematch_data *)data;
    rematch->team_rematch_mask = stream->read_integer("team-rematch-mask", 16);
    for (long team_index = 0; team_index < 16; team_index++) {
        if (rematch->team_rematch_mask & (1 << team_index)) {
            s_network_session_parameter_matchmaking_rematch_team_data *team = &rematch->teams[team_index];
            team->host_peer_index = stream->read_integer("host-peer-index", 4);
            team->rematch_peer_mask = stream->read_integer("rematch-peer-mask", 16);
            team->join_data_valid = stream->read_bool("join-data-valid");
            if (team->join_data_valid)
                stream->read_raw_data("host-description", &team->host_description, 480);
        }
    }
    success = success && !stream->error_occurred();
    return success;
}

// 0x415298
// @address 0x415298 matchmaking_messaging_encode
void matchmaking_messaging_encode(c_bitstream *stream, const void *data, long size)
{
    const s_network_session_parameter_matchmaking_messaging *values = (const s_network_session_parameter_matchmaking_messaging *)data;
    stream->write_integer("message_type", values->message_type, 3);
    stream->write_integer("display_cookie", values->display_cookie, 4);
    stream->write_integer("message_data", values->message_data, 0x14);
}

// 0x4152e8
static __forceinline bool network_session_parameters_matchmaking_valid_type(const long *type)
{
    return *type >= 0 && *type < 5;
}
static __forceinline bool network_session_parameters_matchmaking_stream_overflow(c_bitstream *stream)
{
    bool overflow = stream->m_current.current_stream_bit_position > (stream->m_data_size_bytes << 3);
    if (stream->m_error_occurred)
        overflow = true;
    return overflow;
}
// @address 0x4152e8 matchmaking_messaging_decode
bool matchmaking_messaging_decode(c_bitstream *stream, void *data, long size)
{
    s_network_session_parameter_matchmaking_messaging *values = (s_network_session_parameter_matchmaking_messaging *)data;
    values->message_type = stream->read_integer("message_type", 3);
    values->display_cookie = stream->read_integer("display_cookie", 4);
    unsigned int message_data = stream->read_integer("message_data", 20);
    values->message_data = message_data;
    return network_session_parameters_matchmaking_valid_type(&values->message_type) && !network_session_parameters_matchmaking_stream_overflow(stream);
}
