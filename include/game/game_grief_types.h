#ifndef GAME_GAME_GRIEF_TYPES_H
#define GAME_GAME_GRIEF_TYPES_H

struct game_grief_timers
{
    long betrayal_decay_time;
    long quit_decay_time;
    long last_update_time;
    long reprieve_time;
    bool player_betrayed[4];
};

extern "C" {
extern game_grief_timers g_game_grief_globals;
}

#endif
