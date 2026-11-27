#ifndef GAME_PLAYER_MAPPING_INLINE_H
#define GAME_PLAYER_MAPPING_INLINE_H

#include "game/player_mapping_types.h"
#include "memory/tls_globals.h"

static __forceinline bool players_user_is_active(long user_index)
{
    bool active = false;
    if (user_index != -1)
        active = player_mapping_globals->player_indices[user_index] != -1;
    return active;
}
static __forceinline long player_mapping_get_player_for_user(long user_index)
{
    long player_index = -1;
    if (user_index != -1)
        player_index = player_mapping_globals->player_user_mapping[user_index];
    return player_index;
}
#endif
