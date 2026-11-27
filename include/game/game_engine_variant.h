#ifndef GAME_GAME_ENGINE_VARIANT_H
#define GAME_GAME_ENGINE_VARIANT_H

#include "game/game_types.h"

// Functions defined in tu/blamlibxbox/game_engine_variant.cpp that other units call.

class c_game_variant;

#include "game/game_engine_variant_validation.h"
void game_engine_variant_describe_invalidity(const c_game_variant *variant);
bool game_engine_variant_validate(c_game_variant *variant);
extern "C" c_game_variant *game_engine_variant_initialize_default(c_game_variant *output_variant, int game_engine_index);
#include "game/game_engine_variant_lookup.h"

#endif // GAME_GAME_ENGINE_VARIANT_H
