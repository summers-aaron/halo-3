#ifndef GAME_GAME_ENGINE_KILL_FLAGS_TYPES_H
#define GAME_GAME_ENGINE_KILL_FLAGS_TYPES_H

enum e_game_engine_kill_flags
{
    _game_engine_kill_flag_melee = 0,
    _game_engine_kill_flag_assassination,
    _game_engine_kill_flag_splatter,
    _game_engine_kill_flag_sticky,
    _game_engine_kill_flag_headshot,
    _game_engine_kill_flag_leader,
    _game_engine_kill_flag_team_elimination,
    _game_engine_kill_flag_solo_elimination,

    k_number_of_game_engine_kill_flags = 24
};

#endif // GAME_GAME_ENGINE_KILL_FLAGS_TYPES_H
