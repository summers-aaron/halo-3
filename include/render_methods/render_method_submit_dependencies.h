#ifndef RENDER_METHODS_RENDER_METHOD_SUBMIT_DEPENDENCIES_H
#define RENDER_METHODS_RENDER_METHOD_SUBMIT_DEPENDENCIES_H

struct s_render_texture_camera;

class c_view;

extern "C" {
bool render_texture_camera_try_and_get(long camera_type, s_render_texture_camera *camera, long user_index);
}

#endif
