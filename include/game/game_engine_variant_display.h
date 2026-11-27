#ifndef GAME_GAME_ENGINE_VARIANT_DISPLAY_H
#define GAME_GAME_ENGINE_VARIANT_DISPLAY_H

#include "game/game_engine.h"
#include "game/game_engine_base_variant.h"
#include "memory/static_arrays.h"

// March 82c89dd8: the public wrapper dispatches through the protected virtual.
__forceinline void c_game_engine_base_variant::get_game_engine_name(c_static_wchar_string<1024> *name) const
{
    game_engine_get_multiplayer_string(get_game_engine_name_string_id(), (c_static_wchar_string_1024 *)name);
}

#endif
