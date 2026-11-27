#ifndef INCLUDE_GAME_PLAYER_CONTROL_GLOBALS_H
#define INCLUDE_GAME_PLAYER_CONTROL_GLOBALS_H

struct s_player_control_globals;


// March 2007 TLS slot 0x831df678.
extern __declspec(thread) s_player_control_globals *player_control_globals;

#endif // INCLUDE_GAME_PLAYER_CONTROL_GLOBALS_H
