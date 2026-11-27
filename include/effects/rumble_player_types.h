#ifndef EFFECTS_RUMBLE_PLAYER_TYPES_H
#define EFFECTS_RUMBLE_PLAYER_TYPES_H

#include "effects/rumble_entry_types.h"
#include "math/real_math_types.h"
#include <stddef.h>

struct s_rumble_player { s_rumble_entry entries[8]; float priorities[8]; real_point2d transient_rumble; };

typedef char rumble_player_transient_offset[(offsetof(s_rumble_player, transient_rumble) == 0x80) ? 1 : -1];
typedef char rumble_player_size[(sizeof(s_rumble_player) == 0x88) ? 1 : -1];

#endif // EFFECTS_RUMBLE_PLAYER_TYPES_H
