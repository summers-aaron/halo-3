#ifndef GAME_GAME_ENGINE_RESPAWN_TYPES_H
#define GAME_GAME_ENGINE_RESPAWN_TYPES_H

struct s_respawn_globals
{
    unsigned long reserved_0000;
    long ejection_timestamp;
    unsigned long reserved_0008;
    long timestamp;
    bool player_betrayed[4];
};
extern "C" s_respawn_globals g_boot_respawn_globals;

#endif
