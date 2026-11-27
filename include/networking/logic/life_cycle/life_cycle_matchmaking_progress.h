#ifndef NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_MATCHMAKING_PROGRESS_H
#define NETWORKING_LOGIC_LIFE_CYCLE_LIFE_CYCLE_MATCHMAKING_PROGRESS_H

#include <stddef.h>
#include "networking/session/network_session_parameter_matchmaking_types.h"

enum e_life_cycle_matchmaking_progress_type
{
    _life_cycle_matchmaking_progress_preparing_map = 4,
};


struct s_matchmaking_search_criteria
{
    long game_type;
    long game_mode;
    long skill_minimum;
    long skill_maximum;
    long size_minimum;
    long size_maximum;
    long party_size;
    long need_player_count;
    long want_player_count;
};

struct s_matchmaking_search_statistics
{
    long progress_total;
    long pong_current;
    long games_found;
    long possible_matches;
};

struct s_life_cycle_matchmaking_progress
{
    union { long type; e_life_cycle_matchmaking_progress_type progress_type; };
    union { wchar_t name[32]; unsigned short map_name[32]; unsigned short hopper_name[32]; };
    long squad_player_count;
    long group_player_count;
    union { bool has_strict_search_criteria; unsigned char map_flags; unsigned char minimum_players; unsigned char game_found; bool teams_enabled; };
    union { unsigned char map_options; unsigned char maximum_players; unsigned char teams; };
    union
    {
        struct
        {
            long search_preference;
            union
            {
                s_matchmaking_search_criteria search_criteria;
                s_replicated_life_cycle_matchmaking_progress_search_criteria replicated_search_criteria;
            };
            union
            {
                struct
                {
                    long give_up_time;
                    s_matchmaking_search_statistics search_statistics;
                    long ping_current;
                };
                s_replicated_life_cycle_matchmaking_progress_assembling_match assemble;
                s_replicated_life_cycle_matchmaking_progress_searching_for_match searching_for_match;
            };
        };
        struct
        {
            union
            {
                struct
                {
                    s_replicated_life_cycle_progress_post_match post_match;
                    bool rematch_enabled;
                    bool rematch_ready;
                };
                struct
                {
                    bool countdown_started;
                    long countdown_timer;
                };
            };
            unsigned long rematch_team_player_mask;
            unsigned long rematch_requested_player_mask;
            unsigned long rematch_party_player_mask;
            bool rematch_requested;
        };
        struct
        {
            union
            {
                s_replicated_life_cycle_matchmaking_progress_configuring_match configuring_match;
            };
            unsigned long ready_player_mask;
            long lobby_type;
            wchar_t map_variant_name[32];
            long game_engine_index;
            wchar_t game_variant_name[32];
        };
    };
};

typedef char life_cycle_matchmaking_progress_group_offset[
    (offsetof(s_life_cycle_matchmaking_progress, group_player_count) == 0x48) ? 1 : -1];
typedef char life_cycle_matchmaking_progress_preference_offset[
    (offsetof(s_life_cycle_matchmaking_progress, search_preference) == 0x50) ? 1 : -1];
typedef char life_cycle_matchmaking_progress_give_up_offset[
    (offsetof(s_life_cycle_matchmaking_progress, give_up_time) == 0x78) ? 1 : -1];
typedef char life_cycle_matchmaking_progress_ping_offset[
    (offsetof(s_life_cycle_matchmaking_progress, ping_current) == 0x8c) ? 1 : -1];
typedef char life_cycle_matchmaking_progress_size[
    (sizeof(s_life_cycle_matchmaking_progress) == 0xf8) ? 1 : -1];

typedef char life_cycle_matchmaking_progress_map_options_offset[
    (offsetof(s_life_cycle_matchmaking_progress, map_options) == 0x4d) ? 1 : -1];

#endif
