#ifndef INTERFACE_GUI_WIDGET_KEYFRAMES_H
#define INTERFACE_GUI_WIDGET_KEYFRAMES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

class c_function_definition;

// Keyframe blocks of the widget animation components. Each block starts with
// its time; transition_type 1 selects custom_function over the component's
// own function.
struct s_gui_color_keyframe
{
    long time;
    real_argb_color color;
    long transition_type;
    TAG_POINTER(const c_function_definition) custom_function;
};

struct s_gui_position_keyframe
{
    long time;
    real_point3d position;
    long transition_type;
    TAG_POINTER(const c_function_definition) custom_function;
};

struct s_gui_scale_keyframe
{
    long time;
    short anchoring;
    real_point2d origin;
    real_vector2d scale;
    long transition_type;
    TAG_POINTER(const c_function_definition) custom_function;
};

struct s_gui_rotation_keyframe
{
    long time;
    short anchoring;
    real_point2d origin;
    float rotation;
    long transition_type;
    TAG_POINTER(const c_function_definition) custom_function;
};

struct s_gui_texture_offset_keyframe
{
    long time;
    real_point2d offset;
    long transition_type;
    TAG_POINTER(const c_function_definition) custom_function;
};

struct s_gui_bitmap_keyframe
{
    long time;
    short sequence_index;
    short frame_index;
    long transition_type;
    TAG_POINTER(const c_function_definition) custom_function;
};

typedef char gui_texture_offset_keyframe_size[(sizeof(s_gui_texture_offset_keyframe) == 0x14) ? 1 : -1];
typedef char gui_texture_offset_keyframe_function_offset[(offsetof(s_gui_texture_offset_keyframe, custom_function) == 0x10) ? 1 : -1];
typedef char gui_bitmap_keyframe_size[(sizeof(s_gui_bitmap_keyframe) == 0x10) ? 1 : -1];
typedef char gui_bitmap_keyframe_function_offset[(offsetof(s_gui_bitmap_keyframe, custom_function) == 0xc) ? 1 : -1];

#endif // INTERFACE_GUI_WIDGET_KEYFRAMES_H
