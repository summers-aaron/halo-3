#ifndef GAME_PLAYER_MAPPING_GLOBALS_H
#define GAME_PLAYER_MAPPING_GLOBALS_H

struct s_user_globals;

// Thread-local player mapping, owned by players.cpp.
extern __declspec(thread) s_user_globals *player_mapping_globals;

#endif // GAME_PLAYER_MAPPING_GLOBALS_H
