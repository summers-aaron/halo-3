#ifndef MAIN_MAIN_RENDER_LOADING_TYPES_H
#define MAIN_MAIN_RENDER_LOADING_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

struct declaration;
struct D3DTexture;
struct D3DVertexBuffer;
struct s_rasterizer_main_loading_band {
    float start[6];
    float end[6];
};

// loading screen render resources (s_render_globals + 0x34)
struct s_rasterizer_main_noise_context {
    unsigned char *texture;                     // 256x122 atlas the five texture definitions decompress into
    declaration *declarations[3];
    D3DTexture *noise_texture;                  // 64x32x4 volume texture
    D3DTexture *bar_texture;                    // texture definition 1
    D3DTexture *glyph_texture;                  // texture definition 0
    D3DTexture *glow_texture;                   // 16x16
    D3DVertexBuffer *grid_vertex_buffer;
    D3DVertexBuffer *noise_vertex_buffer;
    D3DVertexBuffer *edge_vertex_buffer;
    D3DVertexBuffer *glyph_vertex_buffer;
    D3DTexture *logo_textures[3];               // texture definitions 2..4
    float fade_offsets[32];
    float fade_scales[32];
    float clear_values[2048];
    float clear_value;
    s_rasterizer_main_loading_band bands[32];
};

struct s_main_render_camera_curves {
    const real_point2d *position_angle;
    const real_point2d *position_radius;
    const real_point2d *position_height;
    const real_point2d *direction_angle;
    const real_point2d *direction_radius;
    const real_point2d *direction_height;
    const real_point2d *field_of_view;
    const real_point2d *near_plane;
    const real_point2d *far_plane;
};
struct s_main_render_loading_globals {
    bool initialized;
    bool started;
    // Genuine gap: no lifted function in the image references globals+4, while every
    // other header offset (+0..+52) has references; March has no loading-screen system.
    char alignment[4];
    long owner_thread;
    unsigned long start_time;
    unsigned long last_time;
    float first_value;
    float last_value;
    long completion_time;
    float smoothed_progress;
    float progress;
    float last_elapsed;
    float rate;
    float delta_elapsed;
    union {
        s_rasterizer_main_noise_context loading;
        struct {
            // First 316 bytes of s_chud_loading_screen (initialized through
            // profile); chud reads them here and samples through clear_values.
            char chud_loading_screen_header[316];
            float clear_values[2048];
        };
    };
    s_main_render_camera_curves camera_curves;
    float history[90];
    long history_index;
};

typedef char main_render_loading_started_offset[(offsetof(s_main_render_loading_globals, started) == 1) ? 1 : -1];
typedef char main_render_loading_owner_offset[(offsetof(s_main_render_loading_globals, owner_thread) == 8) ? 1 : -1];
typedef char main_render_loading_resources_offset[(sizeof(void *) != 4 || ((offsetof(s_main_render_loading_globals, loading) == 0x34))) ? 1 : -1];
typedef char main_render_loading_progress_offset[(offsetof(s_main_render_loading_globals, smoothed_progress) == 0x20) ? 1 : -1];
typedef char main_render_loading_history_offset[(sizeof(void *) != 4 || ((offsetof(s_main_render_loading_globals, history) == 0x2798))) ? 1 : -1];
typedef char main_render_loading_history_index_offset[(sizeof(void *) != 4 || ((offsetof(s_main_render_loading_globals, history_index) == 0x2900))) ? 1 : -1];

extern "C" s_main_render_loading_globals g_main_render_loading_globals;

#endif
