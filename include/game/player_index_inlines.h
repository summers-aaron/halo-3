#ifndef GAME_PLAYER_INDEX_INLINES_H
#define GAME_PLAYER_INDEX_INLINES_H

#include "memory/datum_index_inlines.h"
#include "memory/tls_globals.h"

static __forceinline long player_index_from_absolute_player_index(long absolute_index)
{
    return datum_absolute_index_to_index(tls_player_data(), absolute_index);
}

#endif // GAME_PLAYER_INDEX_INLINES_H
