#ifndef GAME_NETWORK_PLAYER_COLORS_H
#define GAME_NETWORK_PLAYER_COLORS_H

#include "cseries/cseries.h"
#include "game/game_types.h"
#include "game/player_color_types.h"
#include "math/real_math_types.h"
extern "C" void network_get_player_colors(e_game_team team_index, bool unused, const c_enum<e_player_color_index, char, -1, 32> *color_indices,
    real_rgb_color *colors);

#endif // GAME_NETWORK_PLAYER_COLORS_H
