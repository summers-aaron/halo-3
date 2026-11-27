#ifndef GAME_GAME_ENGINE_LOCAL_GLOBALS_H
#define GAME_GAME_ENGINE_LOCAL_GLOBALS_H

#include <stddef.h>
#include "memory/static_arrays.h"

struct s_user_talker_data { short talk_time[16]; };
struct s_player_state_data
{
    long index;
    long reserved; // Reset to zero when index changes.
    long state_response_index; // element of the multiplayer runtime state_responses block, or -1
};
struct s_local_game_engine_globals
{
    long next_stats_report_time;
    c_static_array<float, 4> score_offsets;
    c_static_array<s_player_state_data, 4> player_states;
    s_user_talker_data user_talker_data[4];
};
typedef char local_game_engine_score_offsets_layout[(offsetof(s_local_game_engine_globals, score_offsets) == 4 &&
    offsetof(s_local_game_engine_globals, player_states) == 0x14) ? 1 : -1];
typedef char local_game_engine_globals_size[(sizeof(s_local_game_engine_globals) == 196) ? 1 : -1];
extern "C" { extern __declspec(thread) s_local_game_engine_globals *g_local_game_engine_globals; }

#endif
