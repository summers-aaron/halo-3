#ifndef INCLUDE_RENDER_RENDER_TEXTURE_CAMERA_CALLS_H
#define INCLUDE_RENDER_RENDER_TEXTURE_CAMERA_CALLS_H

class c_texture_camera_view;

extern "C" {
void player_view_render_lightmap_shadows(c_texture_camera_view *view);
void render_objects_update_visible_subparts(unsigned int p1);
}

#endif
