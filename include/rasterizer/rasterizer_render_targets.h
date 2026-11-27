#include "rasterizer/rasterizer_surface_globals.h"
#include "rasterizer/rasterizer_bound_render_targets.h"
#ifndef RASTERIZER_RASTERIZER_RENDER_TARGETS_H
#define RASTERIZER_RASTERIZER_RENDER_TARGETS_H

#include "rasterizer/rasterizer_floating_point_depth.h"

#include "rasterizer/rasterizer_dynamic_render_targets.h"

// Functions defined in tu/graphicslibxbox/rasterizer_render_targets.cpp.

struct s_rasterizer_tiling_settings;

const s_rasterizer_tiling_settings *rasterizer_xenon_main_get_tiling_settings(long screen_resolution, long tiling_configuration);
extern "C" void rasterizer_set_default_render_targets(void);
extern "C" void rasterizer_clear_postprocess_target(unsigned long color);

class c_render_surface;

extern "C" void rasterizer_mark_surface_resolved(c_render_surface *surface);

struct D3DSurface;
union real_vector2d;

extern "C" void rasterizer_set_depth_stencil_surface_direct(D3DSurface *surface);
extern "C" int g_antialiasing_mode;
extern "C" unsigned long g_rasterizer_resolved_surface_mask[7];
extern "C" long g_rasterizer_surface_count;
extern "C" real_vector2d g_rasterizer_viewport_offset;

#include "rasterizer/rasterizer.h"

extern "C" bool rasterizer_is_surface_resolved(c_rasterizer::e_surface surface);
struct D3DBaseTexture;
struct s_render_targets_surface_desc;
extern "C" bool get_texture_level_desc(D3DBaseTexture *texture, int level, s_render_targets_surface_desc *desc);

class c_render_surface_group;
struct s_data_array;
extern "C" {
extern bool g_rasterizer_using_albedo_sampler;
}
#endif
