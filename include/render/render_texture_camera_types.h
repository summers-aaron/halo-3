#ifndef RENDER_RENDER_TEXTURE_CAMERA_TYPES_H
#define RENDER_RENDER_TEXTURE_CAMERA_TYPES_H
#include <stddef.h>
#include "render/render_camera_types.h"

// March's frame_texture_get_source passes the leading render_camera to render_camera_mirror.
struct s_render_texture_camera : render_camera
{
    // The frame texture stores its own angles after the render camera.
    float vertical_field_of_view;
    float horizontal_field_of_view;
    short width;
    short height;
    char name[256];
};
typedef char render_texture_camera_size[(sizeof(s_render_texture_camera) == 0x194) ? 1 : -1];
typedef char render_texture_camera_fov_offset[(offsetof(s_render_texture_camera, vertical_field_of_view) == 0x88) ? 1 : -1];
#endif
