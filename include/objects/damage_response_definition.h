#ifndef OBJECTS_DAMAGE_RESPONSE_DEFINITION_H
#define OBJECTS_DAMAGE_RESPONSE_DEFINITION_H
#include "effects/player_effect_types.h"
#include "math/function_definitions.h"
#include "tag_files/tag_types.h"

// The serialized response class is 192 bytes.
struct s_damage_response_rumble_frequency
{
    float range;
    c_function_definition function;
};
struct s_damage_response_directional_flash
{
    float duration;
    short fade_function;
    float size;
    float inner_scale;
    float outer_scale;
    real_argb_color color;
};
struct s_damage_response_class
{
    short type;
    unsigned short flags;
    s_damage_response_screen_flash_definition screen_flash;
    s_damage_response_screen_tint_definition motion_blur;
    s_damage_response_directional_flash directional_flash;
    s_damage_response_rumble_frequency tracks[2];
    s_damage_response_camera_impulse_definition camera_impulse;
    s_damage_response_camera_shake_definition camera_shake;
};
struct s_damage_response_definition
{
    c_typed_tag_block<s_damage_response_class> classes;
};
typedef char damage_response_class_size[(sizeof(s_damage_response_class) == 0xc0) ? 1 : -1];
typedef char damage_response_rumble_offset[(offsetof(s_damage_response_class, tracks) == 0x5c) ? 1 : -1];
#endif
