#ifndef GAME_PLAYER_ACCESS_H
#define GAME_PLAYER_ACCESS_H

#include "game/player_types.h"
#include "memory/tls_globals.h"

static __forceinline player_datum *player_get(long player_index)
{
    return &((player_datum *)player_data->data)[DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index)];
}

#endif
