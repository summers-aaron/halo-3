#ifndef INTERFACE_SESSION_GAME_MODE_TYPES_H
#define INTERFACE_SESSION_GAME_MODE_TYPES_H

enum e_session_game_mode
{
    _session_game_mode_none = 0,
    _session_game_mode_matchmaking = 1,
    _session_game_mode_pregame = 2,
    _session_game_mode_in_game = 3,
    _session_game_mode_postgame = 4,
    k_session_game_mode_count = 5
};

#endif
