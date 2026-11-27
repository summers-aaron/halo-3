#ifndef GAME_GAME_ENGINE_ODDBALL_DEFINITION_H
#define GAME_GAME_ENGINE_ODDBALL_DEFINITION_H

#include "game/game_engine_variant_definitions.h"

struct s_game_engine_oddball_variant_definition : s_game_engine_base_variant_definition
{
    long flags;
    short team_scoring;
    short score_to_win;
    char carrying_points;
    char kill_points;
    char ball_kill_points;
    char carrier_kill_points;
    char ball_count;
    char alignment_bytes[3];
    short ball_spawn_delay;
    short ball_inactive_respawn_delay;
    long carrier_traits;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char oddball_definition_check_22[(offsetof(s_game_engine_oddball_variant_definition, flags) == 0x38) ? 1 : -1];
typedef char oddball_definition_check_23[(offsetof(s_game_engine_oddball_variant_definition, ball_spawn_delay) == 0x48) ? 1 : -1];
typedef char oddball_definition_check_24[(offsetof(s_game_engine_oddball_variant_definition, carrier_traits) == 0x4c) ? 1 : -1];
typedef char oddball_definition_check_25[(sizeof(s_game_engine_oddball_variant_definition) == 0x50) ? 1 : -1];
#endif

#endif
