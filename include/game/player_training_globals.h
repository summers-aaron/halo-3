#ifndef GAME_PLAYER_TRAINING_GLOBALS_H
#define GAME_PLAYER_TRAINING_GLOBALS_H
#include "game/player_training_tls.h"

#include "game/player_training_types.h"

static __forceinline void player_training_globals_set(s_player_training_entry_data *value) { player_training_globals = (s_player_training_globals *)value; }

static __forceinline s_player_training_entry_data *tls_player_training_entries() { return (s_player_training_entry_data *)player_training_globals; }

#endif // GAME_PLAYER_TRAINING_GLOBALS_H
