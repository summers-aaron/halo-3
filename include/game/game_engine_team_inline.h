#ifndef GAME_GAME_ENGINE_TEAM_INLINE_H
#define GAME_GAME_ENGINE_TEAM_INLINE_H

#include "memory/tls_globals.h"
#include "game/game_globals_storage.h"
#include "game/game_engine_scoring_globals.h"

static __forceinline e_game_team game_engine_team_designator_to_team_index(e_multiplayer_team_designator designator)
{
    return (e_game_team)tls_game_engine_globals()->team_mapping[designator];
}

#include "game/game_variant_inline.h"

#endif
