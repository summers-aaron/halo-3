#ifndef GAME_GAME_ENGINE_TRAITS_H
#define GAME_GAME_ENGINE_TRAITS_H

#include "game/game_types.h"

// Functions defined in tu/blamlibxbox/game_engine_traits.cpp.

extern "C" void game_engine_social_options_copy(short *destination, const long *source);

class c_game_engine_base_variant;
struct s_game_engine_base_variant_definition;

extern "C" void game_engine_base_variant_set(
    c_game_engine_base_variant *variant, const s_game_engine_base_variant_definition *definition, e_game_engine_type engine_type);

struct s_game_engine_zone_variant_definition;
struct s_game_engine_zone_variant;
extern "C" void game_engine_zone_variant_set(const s_game_engine_zone_variant_definition *definition, s_game_engine_zone_variant *variant);

struct s_assault_unit_variant;
struct s_assault_unit_definition;
extern "C" void game_engine_juggernaut_variant_set_from_definition(
    s_assault_unit_variant *variant, const s_assault_unit_definition *definition);

class c_game_engine_assault_variant;
struct s_game_engine_assault_variant_definition;
extern "C" void game_engine_assault_variant_set_from_definition(
    c_game_engine_assault_variant *variant, const s_game_engine_assault_variant_definition *definition);

extern "C" const short k_game_variant_score_time_values[];

class c_game_engine_map_override_options;
extern "C" void game_engine_map_override_options_copy(
    c_game_engine_map_override_options *destination, const c_game_engine_map_override_options *source);

#endif // GAME_GAME_ENGINE_TRAITS_H
