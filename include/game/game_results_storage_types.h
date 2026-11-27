#ifndef GAME_GAME_RESULTS_STORAGE_TYPES_H
#define GAME_GAME_RESULTS_STORAGE_TYPES_H
#include <stddef.h>
#include <string.h>
#include "math/real_math_types.h"
#include "game/game_results_player_data.h"
#include "game/game_results_team_data_types.h"
#include "game/game_results_statistics_types.h"
#include "game/game_variant.h"
struct s_game_results_event {
    union
    {
        long data[9];
        struct
        {
            unsigned char type;
            signed char player_indices[2];
            union
            {
                struct
                {
                    real_point3d positions[2];
                    long damage_type;
                    long time;
                } kill;
                struct
                {
                    union { real_point3d position; real_point3d score_position; };
                    long carry_value;
                    long carry_type;
                    char carry_type_padding[8];
                    long time;
                } carry;
            };
        };
    };
};
class c_game_results
{
public:
    __forceinline c_game_results() {}

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
    union
    {
        struct { unsigned char first_final_flags[2]; bool invalid; };
        unsigned char final_flags[3];
    };
    s_game_results_player_data players[16];
    s_game_results_teams teams;
    s_game_results_statistics statistics;
    s_game_results_event events[1000];
    s_entry entries[16];
    long get_first_unreferenced_machine_index();
    long add_machine(const s_machine_identifier *identifier);
    long get_machine_index(const s_machine_identifier *identifier) const;
    long get_host_machine_index() const;
    void fixup_player_machine_references(void);
};

typedef char results_event_size[(sizeof(s_game_results_event) == 0x24) ? 1 : -1];
typedef char results_event_kill_offset[(offsetof(s_game_results_event, kill) == 4) ? 1 : -1];
typedef char results_hopper_identifier_offset[(offsetof(c_game_results, hopper_identifier) == 4) ? 1 : -1];
typedef char results_hopper_mode_offset[(offsetof(c_game_results, hopper_mode) == 0x4c) ? 1 : -1];
typedef char results_unique_id_offset[(offsetof(c_game_results, unique_id) == 0x68) ? 1 : -1];
typedef char results_game_variant_offset[(offsetof(c_game_results, game_variant) == 0x70) ? 1 : -1];
typedef char results_map_id_offset[(offsetof(c_game_results, map_id) == 0x314) ? 1 : -1];
typedef char results_scenario_path_offset[(offsetof(c_game_results, scenario_path) == 0x318) ? 1 : -1];
typedef char results_finish_value_offset[(offsetof(c_game_results, finish_value) == 0x428) ? 1 : -1];
typedef char results_players_offset[(offsetof(c_game_results, players) == 0x430) ? 1 : -1];
#endif
