#ifndef GAME_PLAYER_MAPPING_TYPES_H
#define GAME_PLAYER_MAPPING_TYPES_H

#include "game/player_masks_types.h"

// The thread-local player mapping globals (memory/tls_globals.h: slot player_mapping_globals, tls_player_mapping_globals()).
// 0xe8 bytes. Layout and names as players.cpp (which owns the allocator), units.cpp and render_objects.cpp all
// declare them; each player's controller is player_input_controller_mapping[absolute player index].
struct s_user_globals
{
    short player_user_count;
    short player_controller_count;
    long player_user_mapping[4];
    long player_controller_mapping[4];
    long user_controller_mapping[4];
    long player_input_controller_mapping[16];
    long player_input_user_mapping[16];
    short local_player_count;
    long player_indices[4];
    long unit_indices[4];
    s_player_masks player_masks;
};

typedef char user_globals_size[(sizeof(s_user_globals) == 0xe8) ? 1 : -1];

#endif // GAME_PLAYER_MAPPING_TYPES_H
