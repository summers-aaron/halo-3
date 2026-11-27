// networkinglibxbox/network_leaderboard.obj: from the records 0x3c3070 0x3c4b20 0x3c4c78 0x3c4d80 0x3c4f00 0x3c4fd8 0x3c5360 0x3c54e8 0x3c5600 0x3c5a68 0x3c5c58 0x3c5e48 0x3c5ef0 0x3c6020 0x3c6108 0x3c6208 0x3c6600 0x3c6a10 0x3c6aa8 0x3c6d68 0x3c6f10 0x3c6ff8 0x3c70c0 0x57e6e8
#include <xtl.h>
#include "game/game_results_storage_types.h"
#include "cseries/crt_xdk.h"
#include "networking/logic/logic_matchmaking_seeker.h"
#include <time.h>
#include <string.h>
#include "memory/static_arrays.h"
#include "networking/logic/logic_managed_user_types.h"
#include "networking/logic/network_leaderboard.h"
#include "networking/online/online_statistics_xenon.h"
#include "networking/session/network_managed_session.h"
#include "networking/logic/storage/network_http_buffer_downloader.h"
#include "interface/online_leaderboard_id_types.h"
#include "networking/online/online_stats_query_status_types.h"
#include "networking/logic/network_session_interface.h"
#include "game/player_configuration.h"
#include "game/game_results_team_data_types.h"
#include "game/player_types.h"
#include "networking/logic/network_leaderboard_write.h"
#include "networking/logic/network_leaderboard_globals_types.h"
#include "interface/controller_types.h"
#include "networking/online/online_statistics_xenon_types.h"
#include "networking/online/online_statistics_tasks.h"
#include "networking/online/online_statistics_xenon_constants.h"
#include "networking/online/online_managed_session_types.h"
#include "networking/session/network_session.h"
#include "networking/session/network_session_parameters.h"
#include "networking/logic/life_cycle/life_cycle_handler_types.h"
#include "networking/logic/life_cycle/life_cycle_globals_types.h"
#include "networking/session/network_session_interface_types.h"
#include "networking/session/network_session_interface_user_externs.h"
#include "networking/network_time_access.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "networking/online/online_constants.h"
#include "networking/logic/network_life_cycle_session_helpers.h"
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x57e6e8 ??_Gc_http_buffer_downloader@@UAAPAXI@Z
// @address 0x3c3070 ??0c_http_buffer_downloader@@QAA@PADJ@Z
// @address 0x3c4b20 build_relative_player_scores_from_game_results
// @address 0x3c4c78 network_leaderboard_write_stats
// @address 0x3c4d80 network_leaderboard_write_initial_stats
// @address 0x3c4f00 network_leaderboard_get_write_status
// @address 0x3c5360 ?network_leaderboard_build_leaderboard_game_results_from_game_results@@YAXPBVc_game_results@@PAUs_network_leaderboard_game_results@@@Z
// @address 0x3c54e8 ?network_leaderboard_build_leaderboard_player_game_results_from_game_results@@YAXJPBUs_network_leaderboard_game_results@@PBVc_game_results@@JPAUs_network_leaderboard_player_game_results@@@Z
// @address 0x3c5a68 ?network_leaderboard_remove_users@@YAXXZ
// @address 0x3c5e48 ?network_leaderboard_build_query_from_statistic_list@@YAXPAUs_online_stat_query@@W4e_online_leaderboard_id@@JPBW4e_leaderboard_statistic@@@Z
// @address 0x3c6020 ?network_leaderboard_update_queries_refresh@@YAXXZ
// @address 0x3c6108 ?network_leaderboard_update_queries_initiate@@YAXXZ
// @address 0x3c6208 ?network_leaderboard_update_queries@@YAXXZ
// @address 0x3c6aa8 ?network_leaderboard_get_results@@YAXJ@Z
// @address 0x4084b8 ?online_stats_query_get_status@@YA?AW4e_online_stats_query_status@@J@Z
// @address 0x408530 ?online_stats_query_get_results@@YA_NJPAUs_online_stat_query_result@@@Z
// @address 0x3c6d68 ?network_leaderboard_update_writes_in_progress@@YAXXZ
// @address 0x3c6f10 network_leaderboard_get_property_id_by_statistic
// @address 0x3c6ff8 ?network_leaderboard_user_query_failed@@YAXJ@Z
// @address 0x3c6600 ?network_leaderboard_build_user_stats@@YAXJPAUs_queried_player_statistics@@@Z
// @address 0x3c6a10 ?network_leaderboard_notify_user_stats_updated@@YAXJ@Z
// @address 0x3c70c0 network_leaderboard_reset_user_stats
// @address 0x8af98 ?is_signed_in_to_live@c_controller_interface@@QBA_NXZ

enum e_controller_index;
struct s_player_configuration;
struct s_network_leaderboard_game_results
{
    union
    {
        __int64 words[5];
        struct
        {
            unsigned short playlist;
            long mode;
            float primary, secondary;
            long score, map;
            bool initial, ranked, flag;
            long team_count, player_count;
            unsigned long time;
        };
    };
};
struct s_network_leaderboard_player_game_results
{
    unsigned __int64 xuid;
    s_player_configuration_from_client stats;
    s_player_configuration_from_host properties;
    long score;
    bool exists, played, finished, initial;
};
typedef char leaderboard_player_results_size[(sizeof(s_network_leaderboard_player_game_results) == 0x120) ? 1 : -1];
typedef char leaderboard_player_results_host_offset[(offsetof(s_network_leaderboard_player_game_results, properties) == 0xd0) ? 1 : -1];

/* Scans 17 eight-byte slots: 1 if any slot is in state 1, else 2 if any is in state 2, else 0. */


enum e_leaderboard_statistic {
  k_leaderboard_statistic_invalid = -1,
  _leaderboard_statistic_hopper_mu,
  _leaderboard_statistic_hopper_sigma,
  _leaderboard_statistic_hopper_skill,
  _leaderboard_statistic_hopper_games_played,
  _leaderboard_statistic_hopper_games_won,
  _leaderboard_statistic_hopper_games_lost,
  _leaderboard_statistic_experience_base,
  _leaderboard_statistic_experience_penalty,
  _leaderboard_statistic_highest_skill,
  _leaderboard_statistic_ranked_games_played,
  _leaderboard_statistic_ranked_games_won,
  _leaderboard_statistic_ranked_games_lost,
  _leaderboard_statistic_kills,
  _leaderboard_statistic_deaths,
  _leaderboard_statistic_assists,
  _leaderboard_statistic_first_game_played_date,
  _leaderboard_statistic_last_game_played_date,
  _leaderboard_statistic_custom_games_played,
  _leaderboard_statistic_custom_games_won,
  k_leaderboard_statistic_count
};
struct s_online_stat_column { long statistic; long column; };
struct s_online_stat_table { long leaderboard_id; const s_online_stat_column *columns; long column_count; };
// @rdata 0x82026750..0x820270b0
static const s_online_stat_column k_global_statistic_columns[11] =
{
    { _leaderboard_statistic_experience_base, 4 },
    { _leaderboard_statistic_experience_penalty, 5 },
    { _leaderboard_statistic_highest_skill, 6 },
    { _leaderboard_statistic_kills, 11 },
    { _leaderboard_statistic_deaths, 10 },
    { _leaderboard_statistic_assists, 12 },
    { _leaderboard_statistic_first_game_played_date, 13 },
    { _leaderboard_statistic_last_game_played_date, 14 },
    { _leaderboard_statistic_ranked_games_played, 8 },
    { _leaderboard_statistic_ranked_games_won, 7 },
    { _leaderboard_statistic_ranked_games_lost, 9 },
};
static const s_online_stat_column k_custom_games_statistic_columns[2] =
{
    { _leaderboard_statistic_custom_games_played, 2 },
    { _leaderboard_statistic_custom_games_won, 3 },
};
static const s_online_stat_column k_hopper_skill_statistic_columns[2] =
{
    { _leaderboard_statistic_hopper_sigma, 1 },
    { _leaderboard_statistic_hopper_mu, 0 },
};
static const s_online_stat_column k_hopper_statistic_columns[32][4] =
{
    {
        { _leaderboard_statistic_hopper_skill, 15 },
        { _leaderboard_statistic_hopper_games_played, 17 },
        { _leaderboard_statistic_hopper_games_won, 16 },
        { _leaderboard_statistic_hopper_games_lost, 18 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 19 },
        { _leaderboard_statistic_hopper_games_played, 21 },
        { _leaderboard_statistic_hopper_games_won, 20 },
        { _leaderboard_statistic_hopper_games_lost, 22 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 23 },
        { _leaderboard_statistic_hopper_games_played, 25 },
        { _leaderboard_statistic_hopper_games_won, 24 },
        { _leaderboard_statistic_hopper_games_lost, 26 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 27 },
        { _leaderboard_statistic_hopper_games_played, 29 },
        { _leaderboard_statistic_hopper_games_won, 28 },
        { _leaderboard_statistic_hopper_games_lost, 30 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 31 },
        { _leaderboard_statistic_hopper_games_played, 33 },
        { _leaderboard_statistic_hopper_games_won, 32 },
        { _leaderboard_statistic_hopper_games_lost, 34 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 35 },
        { _leaderboard_statistic_hopper_games_played, 37 },
        { _leaderboard_statistic_hopper_games_won, 36 },
        { _leaderboard_statistic_hopper_games_lost, 38 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 39 },
        { _leaderboard_statistic_hopper_games_played, 41 },
        { _leaderboard_statistic_hopper_games_won, 40 },
        { _leaderboard_statistic_hopper_games_lost, 42 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 43 },
        { _leaderboard_statistic_hopper_games_played, 45 },
        { _leaderboard_statistic_hopper_games_won, 44 },
        { _leaderboard_statistic_hopper_games_lost, 46 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 47 },
        { _leaderboard_statistic_hopper_games_played, 49 },
        { _leaderboard_statistic_hopper_games_won, 48 },
        { _leaderboard_statistic_hopper_games_lost, 50 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 51 },
        { _leaderboard_statistic_hopper_games_played, 53 },
        { _leaderboard_statistic_hopper_games_won, 52 },
        { _leaderboard_statistic_hopper_games_lost, 54 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 55 },
        { _leaderboard_statistic_hopper_games_played, 57 },
        { _leaderboard_statistic_hopper_games_won, 56 },
        { _leaderboard_statistic_hopper_games_lost, 58 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 59 },
        { _leaderboard_statistic_hopper_games_played, 61 },
        { _leaderboard_statistic_hopper_games_won, 60 },
        { _leaderboard_statistic_hopper_games_lost, 62 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 63 },
        { _leaderboard_statistic_hopper_games_played, 65 },
        { _leaderboard_statistic_hopper_games_won, 64 },
        { _leaderboard_statistic_hopper_games_lost, 66 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 67 },
        { _leaderboard_statistic_hopper_games_played, 69 },
        { _leaderboard_statistic_hopper_games_won, 68 },
        { _leaderboard_statistic_hopper_games_lost, 70 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 71 },
        { _leaderboard_statistic_hopper_games_played, 73 },
        { _leaderboard_statistic_hopper_games_won, 72 },
        { _leaderboard_statistic_hopper_games_lost, 74 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 75 },
        { _leaderboard_statistic_hopper_games_played, 77 },
        { _leaderboard_statistic_hopper_games_won, 76 },
        { _leaderboard_statistic_hopper_games_lost, 78 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 79 },
        { _leaderboard_statistic_hopper_games_played, 81 },
        { _leaderboard_statistic_hopper_games_won, 80 },
        { _leaderboard_statistic_hopper_games_lost, 82 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 83 },
        { _leaderboard_statistic_hopper_games_played, 85 },
        { _leaderboard_statistic_hopper_games_won, 84 },
        { _leaderboard_statistic_hopper_games_lost, 86 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 87 },
        { _leaderboard_statistic_hopper_games_played, 89 },
        { _leaderboard_statistic_hopper_games_won, 88 },
        { _leaderboard_statistic_hopper_games_lost, 90 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 91 },
        { _leaderboard_statistic_hopper_games_played, 93 },
        { _leaderboard_statistic_hopper_games_won, 92 },
        { _leaderboard_statistic_hopper_games_lost, 94 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 95 },
        { _leaderboard_statistic_hopper_games_played, 97 },
        { _leaderboard_statistic_hopper_games_won, 96 },
        { _leaderboard_statistic_hopper_games_lost, 98 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 99 },
        { _leaderboard_statistic_hopper_games_played, 101 },
        { _leaderboard_statistic_hopper_games_won, 100 },
        { _leaderboard_statistic_hopper_games_lost, 102 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 103 },
        { _leaderboard_statistic_hopper_games_played, 105 },
        { _leaderboard_statistic_hopper_games_won, 104 },
        { _leaderboard_statistic_hopper_games_lost, 106 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 107 },
        { _leaderboard_statistic_hopper_games_played, 109 },
        { _leaderboard_statistic_hopper_games_won, 108 },
        { _leaderboard_statistic_hopper_games_lost, 110 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 111 },
        { _leaderboard_statistic_hopper_games_played, 113 },
        { _leaderboard_statistic_hopper_games_won, 112 },
        { _leaderboard_statistic_hopper_games_lost, 114 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 115 },
        { _leaderboard_statistic_hopper_games_played, 117 },
        { _leaderboard_statistic_hopper_games_won, 116 },
        { _leaderboard_statistic_hopper_games_lost, 118 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 119 },
        { _leaderboard_statistic_hopper_games_played, 121 },
        { _leaderboard_statistic_hopper_games_won, 120 },
        { _leaderboard_statistic_hopper_games_lost, 122 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 123 },
        { _leaderboard_statistic_hopper_games_played, 125 },
        { _leaderboard_statistic_hopper_games_won, 124 },
        { _leaderboard_statistic_hopper_games_lost, 126 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 127 },
        { _leaderboard_statistic_hopper_games_played, 129 },
        { _leaderboard_statistic_hopper_games_won, 128 },
        { _leaderboard_statistic_hopper_games_lost, 130 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 131 },
        { _leaderboard_statistic_hopper_games_played, 133 },
        { _leaderboard_statistic_hopper_games_won, 132 },
        { _leaderboard_statistic_hopper_games_lost, 134 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 135 },
        { _leaderboard_statistic_hopper_games_played, 137 },
        { _leaderboard_statistic_hopper_games_won, 136 },
        { _leaderboard_statistic_hopper_games_lost, 138 }
    },
    {
        { _leaderboard_statistic_hopper_skill, 139 },
        { _leaderboard_statistic_hopper_games_played, 141 },
        { _leaderboard_statistic_hopper_games_won, 140 },
        { _leaderboard_statistic_hopper_games_lost, 142 }
    },
};
static const s_online_stat_table k_online_statistic_tables[98] =
{
    { 1, k_global_statistic_columns, 11 },
    { 2, k_custom_games_statistic_columns, 2 },
    { 4, k_hopper_skill_statistic_columns, 2 },
    { 5, k_hopper_skill_statistic_columns, 2 },
    { 3, k_hopper_statistic_columns[0], 4 },
    { 7, k_hopper_skill_statistic_columns, 2 },
    { 8, k_hopper_skill_statistic_columns, 2 },
    { 6, k_hopper_statistic_columns[1], 4 },
    { 10, k_hopper_skill_statistic_columns, 2 },
    { 11, k_hopper_skill_statistic_columns, 2 },
    { 9, k_hopper_statistic_columns[2], 4 },
    { 13, k_hopper_skill_statistic_columns, 2 },
    { 14, k_hopper_skill_statistic_columns, 2 },
    { 12, k_hopper_statistic_columns[3], 4 },
    { 16, k_hopper_skill_statistic_columns, 2 },
    { 17, k_hopper_skill_statistic_columns, 2 },
    { 15, k_hopper_statistic_columns[4], 4 },
    { 19, k_hopper_skill_statistic_columns, 2 },
    { 20, k_hopper_skill_statistic_columns, 2 },
    { 18, k_hopper_statistic_columns[5], 4 },
    { 22, k_hopper_skill_statistic_columns, 2 },
    { 23, k_hopper_skill_statistic_columns, 2 },
    { 21, k_hopper_statistic_columns[6], 4 },
    { 25, k_hopper_skill_statistic_columns, 2 },
    { 26, k_hopper_skill_statistic_columns, 2 },
    { 24, k_hopper_statistic_columns[7], 4 },
    { 28, k_hopper_skill_statistic_columns, 2 },
    { 29, k_hopper_skill_statistic_columns, 2 },
    { 27, k_hopper_statistic_columns[8], 4 },
    { 31, k_hopper_skill_statistic_columns, 2 },
    { 32, k_hopper_skill_statistic_columns, 2 },
    { 30, k_hopper_statistic_columns[9], 4 },
    { 34, k_hopper_skill_statistic_columns, 2 },
    { 35, k_hopper_skill_statistic_columns, 2 },
    { 33, k_hopper_statistic_columns[10], 4 },
    { 37, k_hopper_skill_statistic_columns, 2 },
    { 38, k_hopper_skill_statistic_columns, 2 },
    { 36, k_hopper_statistic_columns[11], 4 },
    { 40, k_hopper_skill_statistic_columns, 2 },
    { 41, k_hopper_skill_statistic_columns, 2 },
    { 39, k_hopper_statistic_columns[12], 4 },
    { 43, k_hopper_skill_statistic_columns, 2 },
    { 44, k_hopper_skill_statistic_columns, 2 },
    { 42, k_hopper_statistic_columns[13], 4 },
    { 46, k_hopper_skill_statistic_columns, 2 },
    { 47, k_hopper_skill_statistic_columns, 2 },
    { 45, k_hopper_statistic_columns[14], 4 },
    { 49, k_hopper_skill_statistic_columns, 2 },
    { 50, k_hopper_skill_statistic_columns, 2 },
    { 48, k_hopper_statistic_columns[15], 4 },
    { 52, k_hopper_skill_statistic_columns, 2 },
    { 53, k_hopper_skill_statistic_columns, 2 },
    { 51, k_hopper_statistic_columns[16], 4 },
    { 55, k_hopper_skill_statistic_columns, 2 },
    { 56, k_hopper_skill_statistic_columns, 2 },
    { 54, k_hopper_statistic_columns[17], 4 },
    { 58, k_hopper_skill_statistic_columns, 2 },
    { 59, k_hopper_skill_statistic_columns, 2 },
    { 57, k_hopper_statistic_columns[18], 4 },
    { 61, k_hopper_skill_statistic_columns, 2 },
    { 62, k_hopper_skill_statistic_columns, 2 },
    { 60, k_hopper_statistic_columns[19], 4 },
    { 64, k_hopper_skill_statistic_columns, 2 },
    { 65, k_hopper_skill_statistic_columns, 2 },
    { 63, k_hopper_statistic_columns[20], 4 },
    { 67, k_hopper_skill_statistic_columns, 2 },
    { 68, k_hopper_skill_statistic_columns, 2 },
    { 66, k_hopper_statistic_columns[21], 4 },
    { 70, k_hopper_skill_statistic_columns, 2 },
    { 71, k_hopper_skill_statistic_columns, 2 },
    { 69, k_hopper_statistic_columns[22], 4 },
    { 73, k_hopper_skill_statistic_columns, 2 },
    { 74, k_hopper_skill_statistic_columns, 2 },
    { 72, k_hopper_statistic_columns[23], 4 },
    { 76, k_hopper_skill_statistic_columns, 2 },
    { 77, k_hopper_skill_statistic_columns, 2 },
    { 75, k_hopper_statistic_columns[24], 4 },
    { 79, k_hopper_skill_statistic_columns, 2 },
    { 80, k_hopper_skill_statistic_columns, 2 },
    { 78, k_hopper_statistic_columns[25], 4 },
    { 82, k_hopper_skill_statistic_columns, 2 },
    { 83, k_hopper_skill_statistic_columns, 2 },
    { 81, k_hopper_statistic_columns[26], 4 },
    { 85, k_hopper_skill_statistic_columns, 2 },
    { 86, k_hopper_skill_statistic_columns, 2 },
    { 84, k_hopper_statistic_columns[27], 4 },
    { 88, k_hopper_skill_statistic_columns, 2 },
    { 89, k_hopper_skill_statistic_columns, 2 },
    { 87, k_hopper_statistic_columns[28], 4 },
    { 91, k_hopper_skill_statistic_columns, 2 },
    { 92, k_hopper_skill_statistic_columns, 2 },
    { 90, k_hopper_statistic_columns[29], 4 },
    { 94, k_hopper_skill_statistic_columns, 2 },
    { 95, k_hopper_skill_statistic_columns, 2 },
    { 93, k_hopper_statistic_columns[30], 4 },
    { 97, k_hopper_skill_statistic_columns, 2 },
    { 98, k_hopper_skill_statistic_columns, 2 },
    { 96, k_hopper_statistic_columns[31], 4 },
};
static const e_leaderboard_statistic k_network_leaderboard_submit_statistics[] = {
  _leaderboard_statistic_hopper_mu, _leaderboard_statistic_hopper_sigma,
};
static const e_leaderboard_statistic k_skill_statistics[] = {
  _leaderboard_statistic_hopper_skill, _leaderboard_statistic_hopper_games_played, _leaderboard_statistic_hopper_games_won,
      _leaderboard_statistic_hopper_games_lost,
};
static const e_leaderboard_statistic k_global_statistics[] = {
  _leaderboard_statistic_experience_base, _leaderboard_statistic_experience_penalty, _leaderboard_statistic_highest_skill,
      _leaderboard_statistic_ranked_games_played, _leaderboard_statistic_ranked_games_won, _leaderboard_statistic_ranked_games_lost,
      _leaderboard_statistic_kills, _leaderboard_statistic_deaths, _leaderboard_statistic_assists,
      _leaderboard_statistic_first_game_played_date, _leaderboard_statistic_last_game_played_date,
};
static const e_leaderboard_statistic k_custom_statistics[] = {
  _leaderboard_statistic_custom_games_played, _leaderboard_statistic_custom_games_won,
};
// @rdata-end

struct s_queried_player_statistics {
 s_queried_player_global_statistics global;
 s_player_displayed_statistics displayed;
 s_queried_player_hopper_statistics hopper;
};
typedef char leaderboard_queried_statistics_size[(sizeof(s_queried_player_statistics) == 0x58) ? 1 : -1];
typedef char leaderboard_queried_statistics_hopper_offset[(offsetof(s_queried_player_statistics, hopper) == 0x3c) ? 1 : -1];

struct s_leaderboard_stat_array
{
 s_managed_user_property_data data[19];
 __forceinline s_managed_user_property_data &operator[](long index) { return data[index]; }
};

__forceinline void build_game_results(unsigned short playlist, bool ranked, bool flag, long mode, float primary, float secondary, long map,
    unsigned long time, s_network_leaderboard_game_results *game)
{
 memset(game, 0, sizeof(*game));
 game->playlist = playlist;
 game->flag = flag;
 game->time = time;
 game->initial = true;
 game->ranked = ranked;
 game->mode = mode;
 game->primary = primary;
 game->secondary = secondary;
 game->score = 0;
 game->map = map;
}
__forceinline void build_player_results(const game_player_options *options,s_network_leaderboard_player_game_results *player) {
 memset(player, 0, sizeof(*player));
 player->initial = true;
 player->xuid = options->configuration.client.xuid;
 memcpy(&player->stats, &options->configuration, sizeof(s_player_configuration));
 player->exists = true;
 player->finished = false;
 player->played = false;
 player->score = 0;
}
struct s_network_leaderboard_globals;
// @address 0x1090d40 ?network_leaderboard_globals@@3Us_network_leaderboard_globals@@A
s_network_leaderboard_globals network_leaderboard_globals;
// @address 0x8c447c g_network_time_globals
struct s_network_configuration;

// 0x3c3070

c_http_buffer_downloader::c_http_buffer_downloader(char *buffer, long buffer_size) :
    m_hash_valid(false),
    m_has_extra_headers(false),
    m_internal_status(0),
    m_request_cookie(0),
    m_cache_index(-1),
    m_buffer(buffer),
    m_buffer_size(buffer_size),
    m_bytes_received(0),
    m_automatic_retry_mode(0),
    m_retry_delay_msec(2000),
    m_next_retry_time(-1),
    m_retry_count(0)
{
}

#define STEP(index) \
  { long state = network_leaderboard_globals.slots[index].state; \
    if (state == 1) result = 1; \
    else if (state == 2 && result == 0) result = 2; }

extern "C" void build_relative_player_scores_from_game_results(
    // match: 0x3c4b20: without __restrict on scores, c2 reloads leaderboard->team_count (lwz 1Ch) after the scores stores
    const c_game_results *results, const s_network_leaderboard_game_results *leaderboard, long *__restrict scores, long)
{
 memset(scores,-1,16*sizeof(long));
 if(leaderboard->flag) {
  long current_team_relative_score=leaderboard->team_count-1;
  for(long relative_place=0;relative_place<leaderboard->team_count;++relative_place) {
   long tie_count=0;
   for(long team_index=0;team_index<16U;++team_index) {
    if(results->teams.team[team_index].team_exists
      && results->teams.team[team_index].standing==relative_place) {
     ++tie_count;
     for(long player_index=0;player_index<16U;++player_index) {
      const s_game_results_player_data *player=&results->players[player_index];
      if(player->player_exists
      && player->configuration.host.team_index==team_index)
       scores[player_index]=current_team_relative_score;
     }
    }
   }
   current_team_relative_score-=tie_count;
  }
 } else {
  long current_player_relative_score=leaderboard->player_count-1;
  for(long relative_place=0;relative_place<leaderboard->player_count;++relative_place) {
   long tie_count=0;
   for(long player_index=0;player_index<16U;++player_index) {
    const s_game_results_player_data *player=&results->players[player_index];
    if(player->player_exists
      && player->standing==relative_place) {
     scores[player_index]=current_player_relative_score;
     ++tie_count;
    }
   }
   current_player_relative_score-=tie_count;
  }
 }
}

extern "C" void network_leaderboard_write_stats(long session,const c_game_results *results) {
 s_network_leaderboard_game_results game;
 s_network_leaderboard_player_game_results players[16];
 long relative[16];
 memset(&game,0,sizeof(game));
 memset(players,0,sizeof(players));
 network_leaderboard_build_leaderboard_game_results_from_game_results(results,&game);
 build_relative_player_scores_from_game_results(results,&game,relative,16);
 for (unsigned long i=0;i<16;i++) {
  const s_game_results_player_data *player=&results->players[i];
  if (player->player_exists)
   network_leaderboard_build_leaderboard_player_game_results_from_game_results(i,&game,results,relative[i],&players[i]);
 }
 network_leaderboard_write_stats_internal(session,&game,players,true);
}

void network_leaderboard_write_initial_stats(long session, unsigned short playlist, bool ranked, bool flag, long mode, float primary,
    float secondary, long map, unsigned long time, const game_player_options *options)
{
 s_network_leaderboard_game_results game; s_network_leaderboard_player_game_results players[16];
 memset(&game,0,sizeof(game)); memset(players,0,sizeof(players));
 build_game_results(playlist,ranked,flag,mode,primary,secondary,map,time,&game);
 for (unsigned long i = 0; i < 16; i++) {
  const game_player_options *player = &options[i];
  if (player->valid) build_player_results(player, &players[i]);
 }
 network_leaderboard_write_stats_internal(session,&game,players,false);
}

int network_leaderboard_get_write_status(void)
{
  int result = 0;
  if (network_leaderboard_globals.slots[0].state == 1)
    result = 1;
  for (int i = 2; i < 18; i += 4) {
    STEP(i - 1)
    STEP(i)
    STEP(i + 1)
    STEP(i + 2)
  }
  return result;
}

// match: 0x3c5360: without __restrict on game, c2 reloads out->flag (lbz 18h(r4)) after the game reads instead of reusing clrlwi
void network_leaderboard_build_leaderboard_game_results_from_game_results(const c_game_results *__restrict game,s_network_leaderboard_game_results *out) {
 memset(&out->playlist,0,sizeof(*out));
 out->playlist=game->hopper_identifier;
 out->flag=game->teams_enabled;
 out->time=game->end_reason;
 out->initial=game->hopper_identifier!=0xffff;
 if(out->flag) {
  for(unsigned long team_index=0;team_index<16;team_index++)
   if(game->teams.team[team_index].team_exists) out->team_count++;
 } else {
  for(unsigned long player_index=0;player_index<16;player_index++)
   if(game->players[player_index].player_exists) out->player_count++;
 }
 if(out->initial) {
  out->ranked=game->hopper_ranked;
  out->mode=game->hopper_mode;
  out->primary=game->hopper_primary;
  out->secondary=game->hopper_secondary;
  out->score=game->hopper_score;
  out->map=game->hopper_map;
 }
}

void network_leaderboard_build_leaderboard_player_game_results_from_game_results(long player_index,
    const s_network_leaderboard_game_results *leaderboard, const c_game_results *results, long relative_score,
    s_network_leaderboard_player_game_results *player_game_results_out)
{
 memset(player_game_results_out,0,sizeof(*player_game_results_out));
 player_game_results_out->initial=true;
 player_game_results_out->xuid=results->players[player_index].configuration.client.xuid;
 memcpy(&player_game_results_out->stats, &results->players[player_index].configuration, sizeof(s_player_configuration));
 player_game_results_out->exists=(results->statistics.player[player_index].values[0] >> 1)==1;
 player_game_results_out->finished=(results->statistics.player[player_index].values[2] >> 1)==1;
 player_game_results_out->played=(results->statistics.player[player_index].values[1] >> 1)==1;
 if (player_game_results_out->played) {
  player_game_results_out->score=relative_score;
  if (results->teams_enabled) {
   if (relative_score>=leaderboard->team_count/2) player_game_results_out->finished=true;
  } else if (relative_score>=leaderboard->player_count/2) {
   player_game_results_out->finished=true;
  }
 } else {
  player_game_results_out->finished=false;
  player_game_results_out->score=0;
 }
}

static __forceinline unsigned __int64 network_leaderboard_controller_get_player_xuid(const c_controller_interface *controller)
{
 unsigned __int64 xuid=0;
 if (controller->get_user_index()>=0
      && controller->get_user_index()<4) xuid=g_network_session_interface_local_users[controller->get_user_index()].xuid;
 return xuid;
}
void network_leaderboard_remove_users(void)
{
 for (long user_index=0;user_index<4;user_index++) {
  s_leaderboard_user *user=&network_leaderboard_globals.users[user_index];
  bool remove=false;
  if (user->valid) {
   c_controller_interface *controller=(c_controller_interface *)&g_controller_globals.controllers[user->controller_index];
   if (!controller->is_signed_in_to_live()) {
    remove=true;
   } else if (user->xuid!=network_leaderboard_controller_get_player_xuid(controller)) {
    remove=true;
   }
  }
  if (remove) memset(user,0,sizeof(*user));
 }
}

void network_leaderboard_build_query_from_statistic_list(
    s_online_stat_query *query, e_online_leaderboard_id leaderboard_id, long statistic_count, const e_leaderboard_statistic *statistics)
{
 query->leaderboard_id=leaderboard_id;
 query->column_count=statistic_count;
 for (long statistic_index=0;statistic_index<statistic_count;statistic_index++) {
  long column=-1;
  for (unsigned long table_index=0;table_index<98;table_index++) {
   const s_online_stat_table *table=&k_online_statistic_tables[table_index];
   if (table->leaderboard_id==leaderboard_id) {
    for (long column_index=0;column_index<table->column_count;column_index++) {
     const s_online_stat_column *entry=&table->columns[column_index];
     if (entry->statistic==statistics[statistic_index]) {
      column=entry->column;
      break;
     }
    }
   }
  }
  query->columns[statistic_index]=column;
 }
}

void network_leaderboard_update_queries_refresh(void)
{
 if (network_leaderboard_globals.refresh_pending) {
  if (network_time_since_inline(network_leaderboard_globals.refresh_time) >=
      g_network_configuration.leaderboard_refresh_msec)
  {
   network_leaderboard_globals.refresh_time=network_time_get();
   network_leaderboard_globals.refresh_pending=false;
   for (long user_index=0;user_index<4;user_index++) {
    s_leaderboard_user *user=&network_leaderboard_globals.users[user_index];
    if (user->state==3) {
     user->state=4;
    }
   }
  }
 }
}

void network_leaderboard_update_queries_initiate(void)
{
 for (long user_index=0;user_index<4;user_index++) {
  s_leaderboard_user *user=&network_leaderboard_globals.users[user_index];
  if (!user->valid) continue;
  if (user->state!=1
      && user->state!=4) continue;
  if (online_stats_query_get_status(user_index)==_online_stats_query_status_pending) {
   if (!user->initiate_flag) user->initiate_flag=1;
   continue;
  }
  if (user->initiate_flag) user->initiate_flag=0;
  unsigned short flags = (unsigned short)(user->xuid >> 48);
  bool notify=(flags & 15)==9
      && (flags & 192)!=0;
  if (notify) {
   user->state=3;
   user->failure_count=0;
   network_leaderboard_notify_user_stats_updated(user_index);
  } else {
   network_leaderboard_submit_query_for_user(user_index);
   user->query_time=network_time_get();
  }
 }
}

static __forceinline bool network_leaderboard_board_is_valid(long board_id)
{
 bool result=false;
 result=board_id!=-1;
 result = result
      && (board_id == network_leaderboard_globals.global_leaderboard
      || board_id == network_leaderboard_globals.custom_leaderboard
      || board_id == network_leaderboard_globals.playlist_leaderboard
      || board_id == network_leaderboard_globals.playlist_skill_leaderboard);
 return result;
}
static __forceinline long network_leaderboard_find_user_by_xuid(unsigned __int64 xuid)
{
 long result=-1;
 for (long i=0;i<4;i++) {
  s_leaderboard_user *user=&network_leaderboard_globals.users[i];
  if (user->valid
      && xuid==user->xuid) {
   result=i;
   break;
  }
 }
 return result;
}
static __forceinline long network_leaderboard_find_statistic(long column_id,long leaderboard_id)
{
 long result=-1;
 if (column_id!=-1) {
  for (unsigned long i=0;i<98;i++) {
   const s_online_stat_table *table=&k_online_statistic_tables[i];
   if (table->leaderboard_id==leaderboard_id) {
    for (long j=0;j<table->column_count;j++) {
     const s_online_stat_column *entry=&table->columns[j];
     if (entry->column==column_id) {
      result=entry->statistic;
      break;
     }
    }
   }
  }
 }
 return result;
}
void network_leaderboard_get_results(long user_index)
{
 s_leaderboard_user *user=&network_leaderboard_globals.users[user_index];
 s_online_stat_query_result results;
 results.leaderboard_count = 0;
 memset(results.leaderboard_results, 0, sizeof(results.leaderboard_results));
 bool success=false;
 if (online_stats_query_get_results(user_index, &results)) {
  bool error=false;
  s_leaderboard_stat_array stats;
  memset(&stats,0,sizeof(stats));
  for (long i=0;i<results.leaderboard_count;i++) {
   s_online_stat_query_leaderboard *board=&results.leaderboard_results[i];
   if (!network_leaderboard_board_is_valid(board->leaderboard_id)) {
    error=true;
   } else {
    for (long j=0;j<board->player_count;j++) {
     s_online_stat_query_player *row=&board->player_stats[j];
     long index=network_leaderboard_find_user_by_xuid(row->xuid);
     if (index==-1) {
      error=true;
     } else if (index!=user_index) {
      error=true;
     } else {
      for (long k=0;k<row->stat_count;k++) {
       s_online_stat_query_stat *column=&row->stats[k];
       long statistic=network_leaderboard_find_statistic(column->column_id,board->leaderboard_id);
       if (statistic==-1) {
        error=true;
       } else if (column->data.type != 255) {
        s_online_data data;
        memcpy(&data, &column->data, sizeof(data));
        stats[statistic].data = data;
        stats[statistic].valid=true;
       }
      }
     }
    }
   }
  }
  if (!error) {
   memcpy(user->statistics,&stats,sizeof(stats));
   success=true;
  }
 }
 if (!success) {
  network_leaderboard_user_query_failed(user_index);
 } else {
  user->state=3;
  user->failure_count=0;
  network_leaderboard_notify_user_stats_updated(user_index);
 }
}

void network_leaderboard_update_queries(void)
{
 network_leaderboard_update_queries_refresh();
 for (long user_index=0;user_index<4;user_index++) {
  s_leaderboard_user *user=&network_leaderboard_globals.users[user_index];
  if (!user->valid) continue;
  if (user->state!=0) {
   if (user->state!=6) continue;
   long wait = user->failure_count <= g_network_configuration.leaderboard_query_failure_count
       ? g_network_configuration.leaderboard_query_retry_delay_msec
       : g_network_configuration.leaderboard_query_maximum_retry_delay_msec;
   if (network_time_since_inline(user->query_time)<=wait) continue;
  }
  user->state=1;
 }
 network_leaderboard_update_queries_initiate();
 for (long user_index=0;user_index<4;user_index++) {
  s_leaderboard_user *user=&network_leaderboard_globals.users[user_index];
  if (!user->valid) continue;
  if (user->state!=2
      && user->state!=5) continue;
  bool failed=false;
  e_online_stats_query_status status=online_stats_query_get_status(user_index);
  if (status!=_online_stats_query_status_pending) {
   if (status!=_online_stats_query_status_success) failed=true;
   else network_leaderboard_get_results(user_index);
  }
  if (failed) network_leaderboard_user_query_failed(user_index);
 }
}

// 0x3c6600
#define NETWORK_LEADERBOARD_MIN(a,b) ((a)<(b)?(a):(b))
#define NETWORK_LEADERBOARD_MAX(a,b) ((a)>(b)?(a):(b))
#define network_leaderboard_pin(value,lower,upper) NETWORK_LEADERBOARD_MIN(NETWORK_LEADERBOARD_MAX(value,lower),upper)
void network_leaderboard_build_user_stats(long user_index,s_queried_player_statistics *statistics)
{
 s_leaderboard_user *user=&network_leaderboard_globals.users[user_index];
 memset(statistics,0,sizeof(*statistics));
 if (network_leaderboard_globals.playlist != 0xffff
      && network_leaderboard_globals.playlist_leaderboard != -1
      && network_leaderboard_globals.playlist_skill_leaderboard != -1)
 {
  statistics->hopper.hopper_identifier=network_leaderboard_globals.playlist;
  statistics->hopper.mu=user->statistics[0].valid ? (float)user->statistics[0].data.value.real64 : 3.0f;
  statistics->hopper.sigma=user->statistics[1].valid ? (float)user->statistics[1].data.value.real64 : 1.0f;
  statistics->hopper.old_skill=network_leaderboard_pin(user->statistics[2].data.value.integer,1,50);
  statistics->hopper.games_played=network_leaderboard_pin(user->statistics[3].data.value.integer,0,0x1ffff);
  statistics->hopper.games_completed=network_leaderboard_pin(user->statistics[4].data.value.integer,0,0x1ffff);
  statistics->hopper.games_won=network_leaderboard_pin(user->statistics[5].data.value.integer,0,0x1ffff);
  statistics->hopper.valid=true;
 } else {
  memset(&statistics->hopper,0,sizeof(statistics->hopper));
  statistics->hopper.valid=false;
 }
 statistics->global.experience_base=network_leaderboard_pin(user->statistics[6].data.value.integer,0,0x1ffff);
 statistics->global.experience_penalty=network_leaderboard_pin(user->statistics[7].data.value.integer,-0x1ffff,0x1ffff);
 statistics->global.highest_skill=network_leaderboard_pin(user->statistics[8].data.value.integer,1,50);
 if (user->highest_skill_valid==1) statistics->global.highest_skill=NETWORK_LEADERBOARD_MAX(statistics->global.highest_skill,user->highest_skill);
 statistics->displayed.ranked_played=network_leaderboard_pin(user->statistics[9].data.value.integer,0,0x1ffff);
 statistics->displayed.ranked_completed=network_leaderboard_pin(user->statistics[10].data.value.integer,0,0x1ffff);
 statistics->displayed.ranked_won=network_leaderboard_pin(user->statistics[11].data.value.integer,0,0x1ffff);
 statistics->displayed.unranked_played=network_leaderboard_pin(user->statistics[12].data.value.integer,0,0x1ffff);
 statistics->displayed.unranked_completed=network_leaderboard_pin(user->statistics[13].data.value.integer,0,0x1ffff);
 statistics->displayed.unranked_won=network_leaderboard_pin(user->statistics[14].data.value.integer,0,0x1ffff);
 if (user->statistics[15].valid) statistics->displayed.first_played=user->statistics[15].data.value.integer;
 else statistics->displayed.first_played=(long)_time64(0);
 if (user->statistics[16].valid) statistics->displayed.last_played=user->statistics[16].data.value.integer;
 else statistics->displayed.last_played=(long)_time64(0);
 statistics->displayed.custom_completed=network_leaderboard_pin(user->statistics[17].data.value.integer,0,0x1ffff);
 statistics->displayed.custom_won=network_leaderboard_pin(user->statistics[18].data.value.integer,0,0x1ffff);
 statistics->global.valid=true;
 statistics->displayed.valid=true;
}

static __forceinline long network_leaderboard_write_status_from_state(short state)
{
 long status;
 switch (state) {
 case 0: status=0; break;
 case 1: case 2: case 3: status=1; break;
 case 4: status=2; break;
 case 5: status=3; break;
 default: status=0; break;
 }
 return status;
}
static __forceinline bool network_leaderboard_write_status_is_finished(long status)
{
 bool finished=false;
 switch (status) {
 case 0: finished=true; break;
 case 1: break;
 case 2: finished=true; break;
 case 3: finished=true; break;
 default: finished=true; break;
 }
 return finished;
}
static __forceinline void network_leaderboard_update_write(long write_index, c_statistics_write_overlapped_task *task)
{
 s_network_leaderboard_write_slot *slot=&network_leaderboard_globals.slots[write_index];
 if (slot->state==1) {
  long status=network_leaderboard_write_status_from_state(task->get_task_state());
  if (network_leaderboard_write_status_is_finished(status)) slot->state=2;
 }
}
void network_leaderboard_update_writes_in_progress(void)
{
 network_leaderboard_update_write(0, g_online_statistics_globals.global_write_task);
 for (unsigned long write_index=1;write_index<17;write_index++) {
  network_leaderboard_update_write(write_index, g_online_statistics_globals.write_tasks[write_index - 1]);
 }
}

extern "C" int network_leaderboard_get_property_id_by_statistic(int requested_statistic) {
    switch (requested_statistic) {
    case 2: return 10;
    case 3: return 11;
    case 4: return 12;
    case 5: return 13;
    case 6: return 14;
    case 7: return 15;
    case 8: return 16;
    case 9: return 17;
    case _leaderboard_statistic_ranked_games_won: return 18;
    case _leaderboard_statistic_ranked_games_lost: return 19;
    case _leaderboard_statistic_kills: return 20;
    case _leaderboard_statistic_deaths: return 21;
    case _leaderboard_statistic_assists: return 22;
    case _leaderboard_statistic_custom_games_played: return 24;
    case _leaderboard_statistic_custom_games_won: return 25;
    case _leaderboard_statistic_first_game_played_date: return 26;
    case _leaderboard_statistic_last_game_played_date: return 27;
    // match: 0x3c6f10: without __assume(0), c2 emits the switch range check (cmplwi 16; bgtlr)
    default: __assume(0);
    }
}

void network_leaderboard_user_query_failed(long user_index)
{
 s_leaderboard_user *user=&network_leaderboard_globals.users[user_index];
 s_player_configuration configuration;
 e_controller_index controller_index;
 unsigned long voice_settings;
 if (::network_session_interface_get_local_user_properties(user_index, &controller_index,
         &configuration, &voice_settings))
 {
  memset(&configuration.client.global_statistics, 0, sizeof(s_queried_player_statistics));
  network_session_interface_set_local_user_properties(user_index,(e_controller_index)controller_index,&configuration,voice_settings);
 }
 user->state=6;
 user->failure_count++;
}
// @address 0x3c6a10 ?network_leaderboard_notify_user_stats_updated@@YAXJ@Z
void network_leaderboard_notify_user_stats_updated(long user_index)
{
 s_player_configuration configuration;
 e_controller_index controller_index;
 unsigned long voice_settings;
 if (::network_session_interface_get_local_user_properties(user_index, &controller_index,
         &configuration, &voice_settings))
 {
  network_leaderboard_build_user_stats(user_index,(s_queried_player_statistics *)(&configuration.client.global_statistics));
  network_session_interface_set_local_user_properties(user_index,(e_controller_index)controller_index,&configuration,voice_settings);
 }
}

// 0x3c70c0
extern "C" void network_leaderboard_reset_user_stats(long user_index, unsigned long selection)
{
 s_player_configuration configuration;
 e_controller_index controller_index;
 unsigned long voice_settings;
 if (::network_session_interface_get_local_user_properties(user_index, &controller_index,
         &configuration, &voice_settings))
 {
  s_queried_player_statistics *statistics = (s_queried_player_statistics *)(&configuration.client.global_statistics);
  bool equals_one = selection == 1;
  if (selection >= 1) {
   if (!equals_one)
    memset(statistics, 0, sizeof(s_queried_player_statistics));
   else
    memset(statistics, 0, sizeof(statistics->global));
  } else {
   int *word = (int *)&statistics->hopper;
   int remaining = 7;
   do {
    *word = 0;
    ++word;
    --remaining;
   } while (remaining != 0);
  }
  network_session_interface_set_local_user_properties(user_index, (e_controller_index)controller_index, &configuration, voice_settings);
 }
}

// @address 0xf283c8 g_network_session_interface_local_users
// @address 0x11401e8 g_controller_globals
static __forceinline unsigned char network_leaderboard_property_type(long id)
{
 return (unsigned char)k_online_property_data_type_map[id];
}
// @address 0x3c4fd8 ?network_leaderboard_build_property_from_statistic_and_results@@YAXW4e_leaderboard_statistic@@PBUs_network_leaderboard_game_results@@PBUs_network_leaderboard_player_game_results@@PAU_XUSER_PROPERTY@@@Z
void network_leaderboard_build_property_from_statistic_and_results(e_leaderboard_statistic statistic,
    const s_network_leaderboard_game_results *game, const s_network_leaderboard_player_game_results *player, XUSER_PROPERTY *property)
{
 memset(property, 0, sizeof(*property));
 property->dwPropertyId=network_leaderboard_get_property_id_by_statistic(statistic);
 property->value.type=network_leaderboard_property_type(property->dwPropertyId);
 switch (statistic) {
 case _leaderboard_statistic_hopper_skill: property->value.nData=player->properties.hopper_skill; break;
 case _leaderboard_statistic_hopper_games_played: property->value.nData=player->exists!=0; break;
 case _leaderboard_statistic_hopper_games_won: property->value.nData=player->played!=0; break;
 case _leaderboard_statistic_hopper_games_lost: property->value.nData=player->finished!=0; break;
 case _leaderboard_statistic_experience_base:
  if (player->played && player->finished && !player->stats.experience_growth_banned
      && !(player->stats.player_flags & (1<<9))) property->value.nData=game->score;
  if (player->played && player->stats.displayed_statistics.ranked_completed == 0
      && player->stats.displayed_statistics.unranked_completed == 0 && player->stats.global_statistics.experience_base == 0
      && player->stats.global_statistics.experience_penalty == 0)
      property->value.nData = property->value.nData < 1 ? 1 : property->value.nData;
  break;
 case _leaderboard_statistic_experience_penalty: if (!player->played) property->value.nData=game->map; break;
 case _leaderboard_statistic_highest_skill: property->value.nData=game->ranked ? player->properties.hopper_skill : 1; break;
 case _leaderboard_statistic_ranked_games_played: if (game->ranked
      && player->exists) property->value.nData=1; break;
 case _leaderboard_statistic_ranked_games_won: if (game->ranked
      && player->played) property->value.nData=1; break;
 case _leaderboard_statistic_ranked_games_lost: if (game->ranked
      && player->finished) property->value.nData=1; break;
 case _leaderboard_statistic_kills: if (!game->ranked
      && player->exists) property->value.nData=1; break;
 case _leaderboard_statistic_deaths: if (!game->ranked
      && player->played) property->value.nData=1; break;
 case _leaderboard_statistic_assists: if (!game->ranked
      && player->finished) property->value.nData=1; break;
 case _leaderboard_statistic_first_game_played_date:
 case _leaderboard_statistic_last_game_played_date: property->value.nData=game->time; break;
 case _leaderboard_statistic_custom_games_played: if (!game->initial
      && player->played) property->value.nData=1; break;
 case _leaderboard_statistic_custom_games_won: if (!game->initial
      && player->finished) property->value.nData=1; break;
 // match: 0x3c4fd8: unreachable hint omits dead branch check
 default: __assume(0);
 }
}

// @address 0x3c5ef0 ?network_leaderboard_submit_query_for_user@@YAXJ@Z
void network_leaderboard_submit_query_for_user(long user_index)
{
 s_online_stat_query queries[4];
 memset(queries, 0, sizeof(queries));
 network_leaderboard_build_query_from_statistic_list(&queries[0], network_leaderboard_globals.global_leaderboard, 11, k_global_statistics);
 network_leaderboard_build_query_from_statistic_list(&queries[1], network_leaderboard_globals.custom_leaderboard, 2, k_custom_statistics);
 long query_count = 2;
 if (network_leaderboard_globals.playlist_skill_leaderboard != -1) {
  network_leaderboard_build_query_from_statistic_list(
      &queries[2], network_leaderboard_globals.playlist_skill_leaderboard, 4, k_skill_statistics);
  query_count = 3;
 }
 if (network_leaderboard_globals.playlist_leaderboard != -1) {
  network_leaderboard_build_query_from_statistic_list(
      &queries[query_count], network_leaderboard_globals.playlist_leaderboard, 2, k_network_leaderboard_submit_statistics);
  query_count++;
 }
 s_leaderboard_user *user = &network_leaderboard_globals.users[user_index];
 unsigned long time;
 if (g_network_time_globals.enabled)
  time = g_network_time_globals.time;
 else
  time = GetTickCount();
 user->query_time = time;
 if (!online_stats_query_initiate(user_index, 1, &user->xuid, query_count, queries)) {
  network_leaderboard_user_query_failed(user_index);
  return;
 }
 long status;
 if (user->state == 4)
  status = 5;
 else
  status = 2;
 user->state = status;
}


static __forceinline bool managed_session_get_handle(long index, void **handle)
{
 bool result=false;
 if (index!=-1) {
  s_online_managed_session *session=&online_session_manager_globals.sessions[index];
  if (session->actual.handle
      && session->actual.handle!=(void *)-1) {
   *handle=session->actual.handle;
   result=true;
  }
 }
 return result;
}

static __forceinline void network_leaderboard_stat_write_add_extra_property(s_online_stat_write *write)
{
 XUSER_PROPERTY *property=&write->properties[write->property_count];
 property->dwPropertyId=58;
 unsigned char *type=&property->value.type;
 *type=network_leaderboard_property_type(58);
 property->value.i64Data=0;
 write->property_count++;
}

static __forceinline void network_leaderboard_build_stat_write(s_online_stat_write *write, long leaderboard_id, long statistic_count,
    const e_leaderboard_statistic *statistics, const s_network_leaderboard_game_results *game,
    const s_network_leaderboard_player_game_results *player)
{
 write->leaderboard_id=leaderboard_id;
 write->property_count=statistic_count;
 for (long i=0;i<write->property_count;i++)
  network_leaderboard_build_property_from_statistic_and_results(statistics[i],game,player,&write->properties[i]);
 network_leaderboard_stat_write_add_extra_property(write);
}

// @address 0x3c5600 ?network_leaderboard_write_stats_internal@@YAXJPBUs_network_leaderboard_game_results@@PBUs_network_leaderboard_player_game_results@@_N@Z
void network_leaderboard_write_stats_internal(long session_index, const s_network_leaderboard_game_results *game,
    const s_network_leaderboard_player_game_results *players, bool final)
{
 void *session_handle;
 bool should_write=false;
 if (managed_session_get_handle(session_index,&session_handle)) {
  if (game->playlist!=0xffff) {
   if (network_leaderboard_globals.global_leaderboard!=-1
      && network_leaderboard_globals.playlist_skill_leaderboard!=-1
      && network_leaderboard_globals.playlist_leaderboard!=-1
      && network_leaderboard_globals.playlist==game->playlist)
    should_write=true;
  } else if (network_leaderboard_globals.custom_leaderboard!=-1) {
   should_write=true;
  }
 }
 if (!should_write)
  return;
 if (game->initial) {
  s_online_stat_write write;
  memset(&write,0,sizeof(write));
  write.leaderboard_id=0;
  write.property_count=3;
  write.properties[0].dwPropertyId=4;
  write.properties[0].value.type=1;
  write.properties[0].value.nData=game->mode;
  write.properties[1].dwPropertyId=5;
  write.properties[1].value.type=3;
  write.properties[1].value.dblData=game->primary;
  double sigma = ((game->secondary > (double)0.010001f ? game->secondary : (double)0.010001f) < (double)0.049999f)
      ? (game->secondary > (double)0.010001f ? game->secondary : (double)0.010001f)
      : (double)0.049999f;
  write.properties[2].dwPropertyId=6;
  write.properties[2].value.type=3;
  write.properties[2].value.dblData=sigma;
  network_leaderboard_globals.slots[0].value=network_time_get();
  network_leaderboard_globals.slots[0].state=online_global_stats_write_initiate(session_handle,1,&write) ? 1 : 2;
 }
 for (long player_index=0;player_index<16;player_index++) {
  const s_network_leaderboard_player_game_results *player=&players[player_index];
  if (player->initial
      && player->exists) {
   unsigned short identity_flags = (unsigned short)(player->stats.xuid >> 48);
   bool is_guest=(identity_flags & 15)==9
      && (identity_flags & 192)!=0;
   if (!is_guest && (!final
      || player->played)) {
    if (game->initial) {
     s_online_stat_write writes[3];
     memset(writes,0,sizeof(writes));
     writes[0].leaderboard_id=0;
     writes[0].property_count=3;
     writes[0].properties[0].dwPropertyId=7;
     writes[0].properties[0].value.type=1;
     writes[0].properties[0].value.nData=player->score;
     writes[0].properties[1].dwPropertyId=8;
     writes[0].properties[1].value.type=1;
     writes[0].properties[1].value.nData=player->properties.team_index;
     writes[0].properties[2].dwPropertyId=9;
     writes[0].properties[2].value.type=1;
     writes[0].properties[2].value.nData=player->properties.hopper_weight;
     network_leaderboard_build_stat_write(&writes[1],network_leaderboard_globals.playlist_skill_leaderboard,4,k_skill_statistics,game,player);
     network_leaderboard_build_stat_write(&writes[2],network_leaderboard_globals.global_leaderboard,11,k_global_statistics,game,player);
     network_leaderboard_globals.slots[player_index + 1].state =
         online_stats_write_initiate(player_index, session_handle, player->xuid, 3, writes) ? 1 : 2;
    } else {
     s_online_stat_write write;
     memset(&write,0,sizeof(write));
     network_leaderboard_build_stat_write(&write,network_leaderboard_globals.custom_leaderboard,2,k_custom_statistics,game,player);
     network_leaderboard_globals.slots[player_index + 1].state =
         online_stats_write_initiate(player_index, session_handle, player->xuid, 1, &write) ? 1 : 2;
    }
   }
  }
 }
}

static __forceinline bool network_leaderboard_join_to_squad_in_progress()
{
  c_network_session *session = 0;
  if (network_life_cycle_globals_get()->initialized)
    session = network_life_cycle_globals_get()->manager.m_target_squad_session;
  return !session->disconnected()
      && session->session_type() == 1;
}

// match: 0x3c5c58: early inlining retains both get_allowed calls.
static __forceinline const s_network_session_matchmaking_hopper *network_leaderboard_get_hopper(
    const c_network_session_parameter_matchmaking_hopper *parameter)
{
  const s_network_session_matchmaking_hopper *result = 0;
  if (parameter->get_allowed())
    result = &parameter->m_data;
  return result;
}

static __forceinline e_online_leaderboard_id online_constant_get_hopper_leaderboard_id_by_xlast_index(long xlast_index)
{
  e_online_leaderboard_id hopper_leaderboard_id = k_online_leaderboard_id_invalid;
  hopper_leaderboard_id = (e_online_leaderboard_id)(xlast_index * 3 + 3);
  return hopper_leaderboard_id;
}
static __forceinline e_online_leaderboard_id online_constant_get_hopper_skill_leaderboard_id_by_xlast_index(long xlast_index, bool ranked)
{
  e_online_leaderboard_id hopper_skill_leaderboard_id = k_online_leaderboard_id_invalid;
  if (ranked)
    hopper_skill_leaderboard_id = (e_online_leaderboard_id)(xlast_index * 3 + 4);
  else
    hopper_skill_leaderboard_id = (e_online_leaderboard_id)(xlast_index * 3 + 5);
  return hopper_skill_leaderboard_id;
}
// @address 0x3c5c58 ?network_leaderboard_update_leaderboards@@YAXXZ
void network_leaderboard_update_leaderboards(void)
{
 if (network_leaderboard_join_to_squad_in_progress())
  return;
 c_network_session *session = 0;
 unsigned short playlist = 0xffff;
 long playlist_skill_leaderboard = -1;
 long playlist_leaderboard = -1;
 bool update = true;
 if (network_life_cycle_in_squad_session(&session)
      && session->session_class() == 2) {
  if (session->current_local_state() == 10) {
   update = false;
  } else if (session->established()) {
   const c_network_session_parameters *parameters = session->get_session_parameters();
   if (parameters->m_parameters.matchmaking_hopper.get_allowed()) {
    const s_network_session_matchmaking_hopper *hopper = network_leaderboard_get_hopper(&parameters->m_parameters.matchmaking_hopper);
    if (hopper->hopper_identifier != 0xffff) {
     playlist = hopper->hopper_identifier;
     playlist_skill_leaderboard = online_constant_get_hopper_leaderboard_id_by_xlast_index(hopper->xlast_index);
     playlist_leaderboard = online_constant_get_hopper_skill_leaderboard_id_by_xlast_index(hopper->xlast_index, hopper->is_ranked);
    }
   }
  }
 }
 s_network_leaderboard_globals *globals = &network_leaderboard_globals;
 if (update
      && (playlist != globals->playlist || playlist_skill_leaderboard != globals->playlist_skill_leaderboard
      || playlist_leaderboard != globals->playlist_leaderboard))
 {
  for (long user_index = 0; user_index < 4; user_index++) {
   s_leaderboard_user *user = &globals->users[user_index];
   if (user->valid) {
    network_leaderboard_reset_user_stats(user_index, 0);
    user->state = 0;
   }
  }
  globals->playlist = playlist;
  globals->playlist_skill_leaderboard = (e_online_leaderboard_id)playlist_skill_leaderboard;
  globals->playlist_leaderboard = (e_online_leaderboard_id)playlist_leaderboard;
 }
}
