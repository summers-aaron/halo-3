#ifndef GAME_GAME_ENGINE_KING_SETTINGS_TYPES_H
#define GAME_GAME_ENGINE_KING_SETTINGS_TYPES_H

#include "game/game_engine_king_variant.h"

// The active king variant (March asserts "king_variant==NULL" in
// game_engine_king.obj), set by c_king_engine::initialize_for_new_map via
// can_be_cast_to. Declared here (rather than game_engine_king.h) so
// game_engine_koth_traits.cpp can use it without dragging in the legacy
// king_get_hill_move_in_seconds signature; image keeps it and
// game_engine_king.cpp names it with // @address.
extern "C" { extern c_game_engine_king_variant *g_king_variant; }

#endif
