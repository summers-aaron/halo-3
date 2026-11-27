#ifndef GAME_PLAYER_ABSOLUTE_INDEX_H
#define GAME_PLAYER_ABSOLUTE_INDEX_H

#include "memory/data_absolute_index.h"
#include "memory/tls_globals.h"

static __forceinline long player_index_from_absolute_player_index(long absolute_index)
{
    return datum_absolute_index_to_index(tls_player_data(), absolute_index);
}

#endif
