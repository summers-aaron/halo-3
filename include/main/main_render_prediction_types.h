#ifndef MAIN_MAIN_RENDER_PREDICTION_TYPES_H
#define MAIN_MAIN_RENDER_PREDICTION_TYPES_H
#include <stddef.h>
#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

// Decorator tag layouts shared by visibility rendering and resource prediction.
struct s_decorator_group
{
    unsigned short instance_count;
    unsigned char decorator_set_index;
    unsigned char instance_buffer_index;
    unsigned long instance_buffer_offset;
    real_vector4d instance_shader_parameters;
    real_vector3d instance_shader_parameters_tail;
    real_point3d bounding_center;
};

struct s_decorator_set
{
    s_tag_reference render_model;
    char spare_before_texture[16];
    s_tag_reference texture;
    unsigned char flags;
    unsigned char render_type;
    unsigned short unused;
    real_vector4d vertex_constants_246;
    real_vector4d vertex_constants_249;
    real_vector4d pixel_constants_13;
    float fade_start;
    float fade_end;
    float early_cull_fraction;
};

typedef char decorator_group_size[(sizeof(s_decorator_group) == 0x30) ? 1 : -1];
typedef char decorator_group_center_offset[(offsetof(s_decorator_group, bounding_center) == 0x24) ? 1 : -1];
typedef char decorator_set_texture_offset[(offsetof(s_decorator_set, texture.index) == 0x2c) ? 1 : -1];
typedef char decorator_set_fade_offset[(offsetof(s_decorator_set, fade_start) == 0x64) ? 1 : -1];
#endif
