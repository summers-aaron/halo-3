#ifndef GAME_GAME_ENGINE_DEFAULT_HELPERS_H
#define GAME_GAME_ENGINE_DEFAULT_HELPERS_H

#include "game/player_types.h"
#include "game/game_engine_globals.h"
#include "game/player_mapping_types.h"
#include "memory/tls_globals.h"

static __forceinline const player_datum *player_get(const s_data_array *array, long player_index)
{
    int absolute_index = player_index & 0xffff;
    const player_datum *result = &((const player_datum *)((const s_data_array *)array)->data)[absolute_index];
    return result;
}

static __forceinline long player_index_from_user_index_render(long user_index)
{
    long result = -1;
    if (user_index != -1)
        result = tls_player_mapping_globals()->player_indices[user_index];
    return result;
}

// Preserve the accepted legacy caller ABI until its owner and callers are unified.
extern "C" void game_results_try_and_grant_achievement(long player_index, long achievement);


#endif
