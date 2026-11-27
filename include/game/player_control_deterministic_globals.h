#ifndef GAME_PLAYER_CONTROL_DETERMINISTIC_GLOBALS_H
#define GAME_PLAYER_CONTROL_DETERMINISTIC_GLOBALS_H

struct s_player_control_deterministic_globals
{
    union
    {
        unsigned long action_test_flags;
        unsigned char action_test_bytes[4];
    };
};

extern "C" __declspec(thread) s_player_control_deterministic_globals *g_player_control_globals_deterministic;

#endif // GAME_PLAYER_CONTROL_DETERMINISTIC_GLOBALS_H
