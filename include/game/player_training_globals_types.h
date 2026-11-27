#ifndef GAME_PLAYER_TRAINING_GLOBALS_TYPES_H
#define GAME_PLAYER_TRAINING_GLOBALS_TYPES_H

#include "game/player_training_types.h"
#include <stddef.h>

struct s_player_training_globals
{
    s_player_training_entry_data entries[4];
    long scripted_training_text;
    bool show_scripted_training_text;
    bool scripted_training_enabled;
};

typedef char player_training_script_text_offset[(offsetof(s_player_training_globals, scripted_training_text) == 0x850) ? 1 : -1];
typedef char player_training_script_show_offset[(offsetof(s_player_training_globals, show_scripted_training_text) == 0x854) ? 1 : -1];
typedef char player_training_script_enabled_offset[(offsetof(s_player_training_globals, scripted_training_enabled) == 0x855) ? 1 : -1];

#endif
