#ifndef EFFECTS_PLAYER_EFFECT_GLOBALS_TYPES_H
#define EFFECTS_PLAYER_EFFECT_GLOBALS_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "effects/player_effect_types.h"

struct player_effect_datum {
    real_vector3d direction;
    real_vector3d displacement;
    s_damage_response_screen_flash_definition flash;     // 24
    s_damage_response_screen_tint_definition tint;       // 56
    s_damage_response_camera_impulse_definition impulse;  // 76
    s_damage_response_camera_shake_definition shake;     // 100
    short flash_ticks;                                   // 128
    short tint_ticks;                                    // 130
    short impulse_ticks;
    short shake_ticks;                                   // 134
    unsigned char damage_indicator_values[4];
    unsigned char flags;                                 // 140
    unsigned char damage_indicator_flags;
    real_vector3d damage_indicator_direction;
    float shake_scale;                                   // 156
    float impulse_scale;                                 // 160
};
struct s_scripted_player_effect
{
    float position[3];
    float angles[3];
    float intensity;
    short ticks;
    short total_ticks;
};
struct s_player_effect_globals {
    long fade_start_time;
    short player_effect_index;
    bool fade_out;
    s_scripted_player_effect scripted_effect;
    unsigned long scripted_flags;
    long game_time;
    float enabled;
    player_effect_datum effects[4];                      // 52
};
typedef char player_effect_datum_size[(sizeof(player_effect_datum) == 164) ? 1 : -1];
typedef char player_effect_damage_indicator_values_offset[(offsetof(player_effect_datum, damage_indicator_values) == 136) ? 1 : -1];
typedef char player_effect_damage_indicator_direction_offset[(offsetof(player_effect_datum, damage_indicator_direction) == 144) ? 1 : -1];
typedef char player_effect_globals_effects_offset[(offsetof(s_player_effect_globals, effects) == 52) ? 1 : -1];

typedef char player_effect_globals_scripted_offset[(offsetof(s_player_effect_globals, scripted_effect) == 8) ? 1 : -1];
typedef char player_effect_globals_flags_offset[(offsetof(s_player_effect_globals, scripted_flags) == 0x28) ? 1 : -1];

#endif
