#ifndef RENDER_RENDER_TEXTURE_CAMERA_GLOBALS_H
#define RENDER_RENDER_TEXTURE_CAMERA_GLOBALS_H

#include "math/real_math_types.h"

struct s_render_texture_camera_globals
{
    bool enabled, dynamic_lights, render_lights;
    unsigned char : 8;
    long render_mode;
    long position_type, target_type, orientation_type;
    long reserved;
    long field_of_view_type;
    union {
        real_point3d position;
        struct { long object_index, marker_index; };
    };
    union {
        real_vector3d position_offset;
        real_point3d position_offset_point;
    };
    union {
        real_point3d target;
        struct { long target_object_index, target_marker_name; };
    };
    real_vector3d camera_forward;
    real_vector3d camera_up;
    float field_of_view;
    union { float projection_depth; float aspect_ratio; };
    long width, height;
};

extern "C" {
extern __declspec(thread) s_render_texture_camera_globals *g_render_texture_camera_globals;
}

#endif // RENDER_RENDER_TEXTURE_CAMERA_GLOBALS_H
