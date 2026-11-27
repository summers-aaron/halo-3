#ifndef GAME_GAME_ENGINE_KING_TRAITS_INTERNALS_H
#define GAME_GAME_ENGINE_KING_TRAITS_INTERNALS_H
#include "game/game_engine_base_variant.h"
#include "game/game_engine_variant_definitions.h"
#include "game/game_engine_traits.h"

// match: 0x2696b8, 0x2698a8: copy/set_from_definition storage holds the base
// variant, packed hill bytes, and four trait blocks.
struct s_king_variant_copy_storage
{
    c_game_engine_base_variant base;
    char base_padding[4];
    unsigned char hill_flags;
    signed char team_scoring;
    signed char moving_hill;
    signed char moving_hill_order;
    short score_to_win;
    signed char points[6];
    c_player_traits traits[4];
};
struct s_king_variant_copy_definition
{
    s_game_engine_base_variant_definition base;
    unsigned long flags;
    short mode;
    short type;
    short count;
    short time;
    signed char values[6];
    long references[4];
};
extern "C" {
void game_engine_king_variant_copy(s_king_variant_copy_storage *, const s_king_variant_copy_storage *);
void game_engine_king_variant_set_from_definition(s_king_variant_copy_storage *, const s_king_variant_copy_definition *);
}
#endif
