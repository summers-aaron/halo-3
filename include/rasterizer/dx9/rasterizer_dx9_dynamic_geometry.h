#ifndef RASTERIZER_DX9_RASTERIZER_DX9_DYNAMIC_GEOMETRY_H
#define RASTERIZER_DX9_RASTERIZER_DX9_DYNAMIC_GEOMETRY_H

#include "main/main_screenshot.h"

// Functions defined in tu/graphicslibxbox/rasterizer_dx9_dynamic_geometry.cpp.

struct rasterizer_vertex_screen;
union real_vector2d;

extern "C" unsigned char rasterizer_tiling_initialize(short width, short height);
extern "C" void rasterizer_scale_screen_vertices(rasterizer_vertex_screen *vertices, long vertex_count, const real_vector2d *scale);
extern "C" void rasterizer_render_water(bool enabled);
extern "C" void rasterizer_draw_screen_bitmap(const rasterizer_vertex_screen *vertices, long bitmap_group_index,
    long bitmap_index, float amount, float progress, float offset);
extern "C" void rasterizer_composite_tile(const void *parameters, long column, long row);

struct rasterizer_dx9_dynamic_geometry_screenshot_parameters;
struct s_rasterizer_screenshot_context;

extern "C" void rasterizer_render_tiled_grid(rasterizer_dx9_dynamic_geometry_screenshot_parameters * p0);

extern "C" bool g_motion_blur_cache_valid;
extern "C" long g_motion_blur_taps;
extern "C" float g_motion_blur_expected_dt;
extern "C" float g_motion_blur_max_x;
extern "C" float g_motion_blur_max_y;
extern "C" float g_motion_blur_scale_x;
extern "C" float g_motion_blur_scale_y;
extern "C" float g_motion_blur_center_falloff;
extern "C" const rasterizer_vertex_screen k_screen_bitmap_quad_vertices[];
extern "C" bool g_cinematic_letterbox_enabled;

extern "C" void rasterizer_resolve_accumulation_targets(const s_rasterizer_screenshot_context *parameters);
#endif // RASTERIZER_DX9_RASTERIZER_DX9_DYNAMIC_GEOMETRY_H
