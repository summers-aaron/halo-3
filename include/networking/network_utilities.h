#ifndef NETWORKING_NETWORK_UTILITIES_H
#define NETWORKING_NETWORK_UTILITIES_H

#include "game/game_types.h"
#include "math/real_math_types.h"

// Functions defined in tu/networkinglibxbox/network_utilities.cpp that other units call.

// Tests whether one player name is a random name.
extern "C" bool network_player_name_is_random(const wchar_t *name);
extern "C" bool network_get_random_player_name(wchar_t *name, long size);
real_rgb_color network_get_team_color(e_game_team team_index);

// Data defined in tu/networkinglibxbox/network_utilities.cpp that other units use.

extern const long k_network_compatible_version;

#endif // NETWORKING_NETWORK_UTILITIES_H
