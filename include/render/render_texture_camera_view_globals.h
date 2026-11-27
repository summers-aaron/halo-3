#ifndef RENDER_RENDER_TEXTURE_CAMERA_VIEW_GLOBALS_H
#define RENDER_RENDER_TEXTURE_CAMERA_VIEW_GLOBALS_H

class c_rasterizer_texture_ref;

extern "C" unsigned char g_texture_camera_render_active;
extern "C" float g_texture_camera_exposure;
extern "C" float g_texture_camera_exposure_multiplier;
extern "C" float g_texture_camera_near_clip;
extern c_rasterizer_texture_ref g_albedo_buffer_source_texture;
extern c_rasterizer_texture_ref g_normal_buffer_source_texture;

#endif // RENDER_RENDER_TEXTURE_CAMERA_VIEW_GLOBALS_H
