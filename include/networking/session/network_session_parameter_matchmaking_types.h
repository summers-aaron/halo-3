#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_MATCHMAKING_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETER_MATCHMAKING_TYPES_H

#include "networking/logic/storage/network_http_request_hash.h"
#include "networking/transport/transport_security_types.h"
#include <stddef.h>

// Rematch codec and post-match handler agree on the per-team join payload.
struct s_network_session_parameter_matchmaking_rematch_team_data
{
    long host_peer_index;
    unsigned long rematch_peer_mask;
    bool join_data_valid;
    s_transport_session_description host_description;
};

struct s_network_session_parameter_matchmaking_rematch_data
{
    unsigned long team_rematch_mask;
    s_network_session_parameter_matchmaking_rematch_team_data teams[16];
};
typedef char network_session_parameter_matchmaking_rematch_data_size_check[(sizeof(s_network_session_parameter_matchmaking_rematch_data) == 1156) ? 1 : -1];
typedef char network_session_parameter_matchmaking_rematch_teams_offset[(offsetof(s_network_session_parameter_matchmaking_rematch_data, teams) == 4) ? 1 : -1];

struct s_network_session_matchmaking_hopper_category
{
    unsigned short identifier;
    unsigned short hopper_count;
    char name[32];
};
typedef char network_session_hopper_category_count_offset[(offsetof(s_network_session_matchmaking_hopper_category, hopper_count) == 2) ? 1 : -1];

struct s_network_session_matchmaking_hopper_description
{
    unsigned short identifier;
    unsigned short category_identifier;
    unsigned char rich_presence_context_id;
    long image_index;
    bool is_visible;
    bool is_playable;
    bool is_ranked;
    bool experience_penalty_enabled;
    char name[32];
};

struct s_network_session_matchmaking_hopper_list
{
    long hopper_load_status;
    long membership_update_number;
    unsigned long category_populated_mask;
    long category_count;
    s_network_session_matchmaking_hopper_category categories[4];
    long hopper_count;
    s_network_session_matchmaking_hopper_description hoppers[32];
};
typedef char network_session_matchmaking_hopper_list_size_check[(sizeof(s_network_session_matchmaking_hopper_list) == 1700) ? 1 : -1];

// Picked games: four entries pairing map and variant names with their content hashes.
struct s_game_hopper_picked_game
{
    long map_id;
    char game_variant_name[32];
    s_network_http_request_hash game_variant_hash;
    char map_variant_name[32];
    s_network_http_request_hash map_variant_hash;
};

struct s_game_hopper_picked_game_collection
{
    s_game_hopper_picked_game games[4];
};
typedef char game_hopper_picked_game_collection_size_check[(sizeof(s_game_hopper_picked_game_collection) == 432) ? 1 : -1];


// Hopper settings: rules, skill tuning, and filters for one matchmaking hopper.
struct s_network_session_matchmaking_hopper
{
    unsigned short hopper_identifier;
    unsigned char xlast_index;
    bool is_ranked;
    bool veto_allowed;
    bool rematch_allowed;
    bool team_game;
    long trueskill_draw_probability;
    float trueskill_beta_performance_variation;
    float trueskill_tau_dynamics_factor;
    long experience_base_increment;
    long experience_penalty_decrement;
    bool require_hosts_on_multiple_teams;
    unsigned long system_seconds;
    long maximum_party_size;
    long rematch_timer;
    long repeated_opponents_to_consider_for_penalty;
    long repeated_opponents_experience_threshold;
    long repeated_opponents_skill_throttle_start;
    long repeated_opponents_skill_throttle_stop;
    long stats_write;
    long language_filter;
    long country_code_filter;
    bool gamerzone_filter;
};
typedef char network_session_matchmaking_hopper_size_check[(sizeof(s_network_session_matchmaking_hopper) == 76) ? 1 : -1];


// matchmaking progress: one progress type, then the state's own data. Names and offsets from March's encoders
// (network_session_parameters_matchmaking.cpp: matchmaking_progress_encode_function and the four per-type encoders
// it switches to on progress-type 1 searching, 2 assembling, 4 configuring, 5 post match; their bitstream strings)
struct s_replicated_life_cycle_matchmaking_progress_searching_for_match
{
    long search_stage;
    long searching_skill_min;
    long searching_skill_max;
    long contacted_session_count;
    long potential_session_count;
    long search_queries_submitted;
};

struct s_replicated_life_cycle_matchmaking_progress_assembling_match
{
    long give_up_timer_seconds;
    long give_up_timer_maximum_seconds;
    long qos_listener_data_replies_sent;
};

struct s_replicated_life_cycle_matchmaking_progress_configuring_match
{
    long state;
    long map_load_progress_percentage;
    long countdown_timer;
    long current_veto_count;
    long max_veto_count;
    long game_map_index;
    bool load_failure_encountered;
};

struct s_replicated_life_cycle_progress_post_match
{
    bool countdown_running;                         // March encode "countdown-running"
    long countdown;                                 // March encode "countdown"
};

struct s_replicated_life_cycle_matchmaking_progress
{
    long type;                                      // March "progress-type"
    union
    {
        s_replicated_life_cycle_matchmaking_progress_searching_for_match searching_for_match;
        s_replicated_life_cycle_matchmaking_progress_assembling_match assembling_match;
        s_replicated_life_cycle_matchmaking_progress_configuring_match configuring_match;
        s_replicated_life_cycle_progress_post_match post_match;
    };
};
typedef char replicated_life_cycle_matchmaking_progress_size_check[(sizeof(s_replicated_life_cycle_matchmaking_progress) == 32) ? 1 : -1];

// search criteria: names and offsets from March's matchmaking_progress_search_criteria_encode_function
struct s_replicated_life_cycle_matchmaking_progress_search_criteria
{
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
typedef char replicated_life_cycle_matchmaking_progress_search_criteria_size_check[(sizeof(s_replicated_life_cycle_matchmaking_progress_search_criteria)
    == 36) ? 1 : -1];
struct s_matchmaking_single_hopper_statistics
{
    long total_population;                          // user_interface_matchmaking.cpp, gui_screen_pregame_lobby_matchmaking.cpp
    long hopper_population;                         // the postgame lobby shows it as the hopper's player count
};

#endif
