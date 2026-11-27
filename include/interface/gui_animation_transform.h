#ifndef INTERFACE_GUI_ANIMATION_TRANSFORM_H
#define INTERFACE_GUI_ANIMATION_TRANSFORM_H

#include <stddef.h>
#include "math/real_math_types.h"

// Widget animation transform.
struct s_animation_transform {
    s_animation_transform();
    unsigned long start_time;
    long screen_transition_start_time;
    long previous_control_start_time;
    long next_control_start_time;
    long indicator_start_time;
    unsigned __int64 animation_state_flags;
    real_argb_color color;
    real_point3d position;
    real_vector2d scale;
    real_point2d rotation_origin;
    float rotation_angle;
    float rotation_sine;
    float rotation_cosine;
    real_point2d rotation_pivot;
    real_point2d texture_offset;
    short bitmap_sequence_index;
    short bitmap_frame_index;
    long font;
};

typedef char gui_anim_xform_size[(sizeof(s_animation_transform) == 0x70) ? 1 : -1];
typedef char gui_anim_xform_flags_offset[(offsetof(s_animation_transform, animation_state_flags) == 0x18) ? 1 : -1];
typedef char gui_anim_xform_sine_offset[(offsetof(s_animation_transform, rotation_sine) == 0x50) ? 1 : -1];
typedef char gui_anim_xform_cosine_offset[(offsetof(s_animation_transform, rotation_cosine) == 0x54) ? 1 : -1];
typedef char gui_anim_xform_pivot_offset[(offsetof(s_animation_transform, rotation_pivot) == 0x58) ? 1 : -1];
typedef char gui_anim_xform_texture_offset[(offsetof(s_animation_transform, texture_offset) == 0x60) ? 1 : -1];
typedef char gui_anim_xform_font_offset[(offsetof(s_animation_transform, font) == 0x6c) ? 1 : -1];

#endif // INTERFACE_GUI_ANIMATION_TRANSFORM_H
