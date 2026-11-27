#ifndef GAME_GAME_SAVE_GLOBALS_H
#define GAME_GAME_SAVE_GLOBALS_H

struct s_game_save_globals
{
    long state;
    long safe_check_ticks;
    long start_time;
    long safe_count;
    long last_save_time;
    long dangerous_object_index;
};

extern "C" __declspec(thread) s_game_save_globals *g_game_save_globals;

#endif
