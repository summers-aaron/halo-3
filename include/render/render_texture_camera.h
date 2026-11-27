#ifndef RENDER_RENDER_TEXTURE_CAMERA_H
#define RENDER_RENDER_TEXTURE_CAMERA_H

class c_texture_camera_view;

extern "C" void texture_camera_view_render_texture_camera(c_texture_camera_view *view, long view_index);

void render_texture_camera_initialize(void);
void render_texture_camera_dispose(void);
void render_texture_camera_initialize_for_new_map(void);
void render_texture_camera_dispose_from_old_map(void);

#endif
