#ifndef GAME_PLAYER_DATA_H
#define GAME_PLAYER_DATA_H

struct s_data_array;

// Thread-local player datum array (TLS slot 0x831df660), owned by players.cpp (March 2007 debug name and type).
extern __declspec(thread) s_data_array *player_data;

#endif // GAME_PLAYER_DATA_H
