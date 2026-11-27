#ifndef EFFECTS_PLAYER_EFFECT_TYPES_H
#define EFFECTS_PLAYER_EFFECT_TYPES_H

#include <stddef.h>

#include "math/real_math_types.h"

struct s_damage_response_camera_shake_definition
{
    float duration;
    short transition;
    short pad6;
    float translation;
    float rotation;
    short wave;
    short pad18;
    float period;
    float wobble;
};
struct s_damage_response_camera_impulse_definition {
    float duration;
    short transition;
    short pad6;
    float rotation;
    float pushback;
    real_bounds jitter;
};
struct s_damage_response_screen_flash_definition
{
    short type, priority;
    float duration;
    unsigned short transition;
    short pad10;
    float intensity;
    real_argb_color color;
};
struct s_damage_response_screen_tint_definition { float duration; short priority; unsigned short transition; float amount[3]; };

typedef char damage_response_impulse_size[(sizeof(s_damage_response_camera_impulse_definition) == 24) ? 1 : -1];
typedef char damage_response_shake_size[(sizeof(s_damage_response_camera_shake_definition) == 28) ? 1 : -1];
typedef char damage_response_shake_translation_offset[(offsetof(s_damage_response_camera_shake_definition, translation) == 8) ? 1 : -1];
typedef char damage_response_shake_period_offset[(offsetof(s_damage_response_camera_shake_definition, period) == 20) ? 1 : -1];
typedef char damage_response_flash_size[(sizeof(s_damage_response_screen_flash_definition) == 32) ? 1 : -1];

#endif
