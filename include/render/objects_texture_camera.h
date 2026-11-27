#ifndef RENDER_OBJECTS_TEXTURE_CAMERA_H
#define RENDER_OBJECTS_TEXTURE_CAMERA_H

#include "render/render_texture_camera_types.h"

class c_player_view;
typedef s_render_texture_camera objects_render_texture_camera;
extern "C" bool render_texture_camera_try_and_get(long camera_type, s_render_texture_camera *camera, c_player_view *player_view);

#endif
