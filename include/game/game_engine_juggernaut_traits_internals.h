#ifndef GAME_GAME_ENGINE_JUGGERNAUT_TRAITS_INTERNALS_H
#define GAME_GAME_ENGINE_JUGGERNAUT_TRAITS_INTERNALS_H

#include "game/game_engine_variant_definitions.h"
#include "game/game_engine_traits.h"

class c_game_engine_vip_variant;

// VIP settings extend the base variant definition with scoring, selection and player traits.
struct s_game_engine_vip_settings_definition
{
    s_game_engine_base_variant_definition base;
    unsigned long flags;
    short score_to_win;
    short selection;
    short movement;
    short order;
    signed char kill_points;
    signed char takedown_points;
    signed char kill_as_vip_points;
    signed char vip_death_points;
    signed char destination_arrival_points;
    signed char suicide_points;
    signed char vip_suicide_points;
    signed char betrayal_points;
    signed char influence_radius;
    long team_traits;
    long influence_traits;
    long vip_traits;
};

extern "C" {
void game_engine_territories_variant_set_from_definition(
    c_game_engine_vip_variant *variant, const s_game_engine_vip_settings_definition *definition);
}

#endif
