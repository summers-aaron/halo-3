#ifndef INCLUDE_GAME_PLAYER_TRAINING_TLS_H
#define INCLUDE_GAME_PLAYER_TRAINING_TLS_H

struct s_player_training_globals;


// March 2007 TLS slot 0x831df6d4.
extern __declspec(thread) s_player_training_globals *player_training_globals;

#endif // INCLUDE_GAME_PLAYER_TRAINING_TLS_H
