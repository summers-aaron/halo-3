#ifndef GAME_PLAYER_MAPPING_INLINES_H
#define GAME_PLAYER_MAPPING_INLINES_H

#include "game/player_mapping_types.h"
#include "memory/tls_globals.h"

static __forceinline long player_index_from_user_index_render(long user_index)
{
    long result = -1;
    if (user_index != -1)
        result = tls_player_mapping_globals()->player_indices[user_index];
    return result;
}

#endif
