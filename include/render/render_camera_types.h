#ifndef RENDER_RENDER_CAMERA_TYPES_H
#define RENDER_RENDER_CAMERA_TYPES_H

#include <stddef.h>
#include "math/matrix_math_types.h"

struct render_camera
{
    real_point3d position;
    real_vector3d forward;
    real_vector3d up;
    bool mirrored;
    float vertical_field_of_view;
    float field_of_view_scale;
    rectangle2d window_pixel_bounds;
    rectangle2d window_title_safe_pixel_bounds;
    point2d window_offset;
    rectangle2d fullscreen_pixel_bounds;
    rectangle2d fullscreen_title_safe_pixel_bounds;
    rectangle2d display_pixel_bounds;
    float near_clip;
    float far_clip;
    real_plane3d clip_plane;
    bool frustum_adjustment;
    real_point2d frustum_center;
    real_point2d frustum_scale;
};

struct render_projection
{
    real_matrix4x3 world_to_view;
    real_matrix4x3 view_to_world;
    real_rectangle2d frustum;
    float projection_matrix[4][4];
    real_vector2d world_to_screen_size;
};

typedef char render_camera_size_check[(sizeof(render_camera) == 0x88) ? 1 : -1];
typedef char render_projection_size_check[(sizeof(render_projection) == 0xc0) ? 1 : -1];
typedef char render_camera_window_offset_check[(offsetof(render_camera, window_offset) == 0x40) ? 1 : -1];
typedef char render_camera_near_clip_check[(offsetof(render_camera, near_clip) == 0x5c) ? 1 : -1];
typedef char render_camera_far_clip_check[(offsetof(render_camera, far_clip) == 0x60) ? 1 : -1];
typedef char render_camera_frustum_adjustment_check[(offsetof(render_camera, frustum_adjustment) == 0x74) ? 1 : -1];
typedef char render_projection_frustum_check[(offsetof(render_projection, frustum) == 0x68) ? 1 : -1];
typedef char render_projection_matrix_check[(offsetof(render_projection, projection_matrix) == 0x78) ? 1 : -1];
typedef char render_projection_screen_size_check[(offsetof(render_projection, world_to_screen_size) == 0xb8) ? 1 : -1];

#endif // RENDER_RENDER_CAMERA_TYPES_H
