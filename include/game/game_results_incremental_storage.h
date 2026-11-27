#ifndef GAME_GAME_RESULTS_INCREMENTAL_STORAGE_H
#define GAME_GAME_RESULTS_INCREMENTAL_STORAGE_H

#include <stddef.h>
#include "game/game_results_player_data.h"
#include "game/game_results_team_data_types.h"
#include "game/game_results_statistics_types.h"
#include "game/game_variant.h"

// Full-layout view of g_current_game_results used by incremental replication.
// Same object (0xa33f80), same member names and offsets as c_game_results.
struct s_game_results_incremental_storage
{
    bool initialized;
    bool finalized;
    bool aborted;
    unsigned short hopper_identifier;
    unsigned short hopper_name[32];
    bool hopper_ranked;
    bool hopper_team_based;
    unsigned char hopper_unknown48;
    union { long hopper_unknown4c; long hopper_mode; };
    union { float hopper_unknown50; float hopper_primary; };
    union { float hopper_unknown54; float hopper_secondary; };
    union { long hopper_unknown58; long hopper_score; };
    union { long hopper_unknown5c; long hopper_map; };
    bool teams_enabled;
    unsigned __int64 unique_id;
    c_game_variant game_variant;
    unsigned short map_name[32];
    long map_id;
    union { char scenario_path[260]; char map_path[260]; };
    bool summary_valid;
    long end_reason;
    bool finish_valid;
    long finish_value;
    unsigned char final_flags[3];
    s_game_results_player_data players[16];
    s_game_results_teams teams;
    s_game_results_statistics statistics;
    char events[36000];
    s_entry entries[16];
};

extern "C" s_game_results_incremental_storage g_game_results_incremental_storage;

typedef char incremental_hopper_identifier_offset[(offsetof(s_game_results_incremental_storage, hopper_identifier) == 4) ? 1 : -1];
typedef char incremental_hopper_mode_offset[(offsetof(s_game_results_incremental_storage, hopper_mode) == 0x4c) ? 1 : -1];
typedef char incremental_unique_id_offset[(offsetof(s_game_results_incremental_storage, unique_id) == 0x68) ? 1 : -1];
typedef char incremental_game_variant_offset[(offsetof(s_game_results_incremental_storage, game_variant) == 0x70) ? 1 : -1];
typedef char incremental_map_id_offset[(offsetof(s_game_results_incremental_storage, map_id) == 0x314) ? 1 : -1];
typedef char incremental_scenario_path_offset[(offsetof(s_game_results_incremental_storage, scenario_path) == 0x318) ? 1 : -1];
typedef char incremental_finish_value_offset[(offsetof(s_game_results_incremental_storage, finish_value) == 0x428) ? 1 : -1];
typedef char incremental_players_offset[(offsetof(s_game_results_incremental_storage, players) == 0x430) ? 1 : -1];

#endif
