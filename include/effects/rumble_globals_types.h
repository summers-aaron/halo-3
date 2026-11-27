#ifndef EFFECTS_RUMBLE_GLOBALS_TYPES_H
#define EFFECTS_RUMBLE_GLOBALS_TYPES_H

#include "effects/rumble_player_types.h"

struct s_rumble_globals { s_rumble_player players[4]; float scripted_left; float scripted_right; float scripted_scale; };

#endif // EFFECTS_RUMBLE_GLOBALS_TYPES_H
