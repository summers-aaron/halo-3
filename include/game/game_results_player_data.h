#ifndef GAME_GAME_RESULTS_PLAYER_DATA_H
#define GAME_GAME_RESULTS_PLAYER_DATA_H

#include "game/player_configuration.h"
#include "objects/object_reference_types.h"
#include <stddef.h>

struct s_entry {
    union { unsigned char key[6]; s_machine_identifier identifier; };
    unsigned char valid;
    unsigned char joined;
    unsigned char host;
    unsigned char initial_host;
    unsigned char excluded;
    short statistics[5];
};
struct s_game_results_player_data
{
    bool player_exists;
    signed char machine_index;
    unsigned char identifier[8];
    s_player_configuration configuration;
    signed char standing;
    char alignment_121[300 - 289];
    short final_score;
};

typedef char results_player_data_size[(sizeof(s_game_results_player_data) == 0x130) ? 1 : -1];
typedef char results_player_standing_offset[(offsetof(s_game_results_player_data, standing) == 0x120) ? 1 : -1];
typedef char results_player_configuration_offset[(offsetof(s_game_results_player_data, configuration) == 0x10) ? 1 : -1];
typedef char results_player_final_score_offset[(offsetof(s_game_results_player_data, final_score) == 0x12c) ? 1 : -1];
typedef char results_machine_entry_size[(sizeof(s_entry) == 22) ? 1 : -1];
typedef char results_machine_joined_offset[(offsetof(s_entry, joined) == 7) ? 1 : -1];
typedef char results_machine_excluded_offset[(offsetof(s_entry, excluded) == 10) ? 1 : -1];
typedef char results_machine_statistics_offset[(offsetof(s_entry, statistics) == 12) ? 1 : -1];

#endif
