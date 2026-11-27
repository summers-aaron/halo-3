#ifndef EFFECTS_LENS_FLARE_DEFINITIONS_H
#define EFFECTS_LENS_FLARE_DEFINITIONS_H

#include <stddef.h>
#include "cseries/cseries.h"
#include "tag_files/tag_types.h"
#include "math/real_math_types.h"
#include "math/function_definitions.h"

struct s_lens_flare_color_function
{
    string_id input_variable;
    string_id range_variable;
    short output_modifier;
    short pad10;
    string_id output_modifier_input;
    c_function_definition function;
};

struct s_lens_flare_reflection
{
    word flags;
    short pad2;
    short bitmap_index;
    short pad6;
    float rotation_offset;
    float radius_min;
    float radius_max;
    float brightness_min;
    float brightness_max;
    float tint_modulation_factor;
    real_rgb_color tint_color;
    float animation_speed;
};

struct s_lens_flare_definition
{
    float falloff_angle;
    float cutoff_angle;
    union { float cos_falloff_angle; float direction_offset; };
    union { float cos_cutoff_angle; struct { short orientation; short exposure; }; };
    float near_fade_begin;
    float near_fade_end;
    float far_fade_begin;
    float far_fade_end;
    s_tag_reference bitmap;
    word occlusion_flags;
    short pad32;
    short rotation_function;
    short pad36;
    float rotation_function_scale;
    short falloff_function;
    short pad3e;
    s_tag_block reflections;
    word flags;
    short pad4e;
    s_tag_block time_brightness;
    s_tag_block age_brightness;
    s_tag_block time_color;
    s_tag_block age_color;
    s_tag_block time_rotation;
    s_tag_block age_rotation;
};

typedef char lens_flare_falloff_offset[(offsetof(s_lens_flare_definition, falloff_function) == 0x3c) ? 1 : -1];
typedef char lens_flare_definition_size[(sizeof(s_lens_flare_definition) == 0x98) ? 1 : -1];

#endif
