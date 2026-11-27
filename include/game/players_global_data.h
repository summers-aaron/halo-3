#ifndef GAME_PLAYERS_GLOBAL_DATA_H
#define GAME_PLAYERS_GLOBAL_DATA_H

struct s_players_globals;

// Thread-local players globals, owned by players.cpp.
extern __declspec(thread) s_players_globals *players_globals;

#endif // GAME_PLAYERS_GLOBAL_DATA_H
