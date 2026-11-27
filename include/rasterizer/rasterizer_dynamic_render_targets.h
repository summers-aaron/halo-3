#ifndef RASTERIZER_RASTERIZER_DYNAMIC_RENDER_TARGETS_H
#define RASTERIZER_RASTERIZER_DYNAMIC_RENDER_TARGETS_H

#include "memory/static_arrays.h"

struct s_data_array;

// Functions defined in tu/graphicslibxbox/rasterizer_dynamic_render_targets.cpp that other units call.

class c_rasterizer_texture_ref;
struct D3DSurface;

extern "C" {
extern c_rasterizer_texture_ref g_rasterizer_accumulation_texture;
extern c_rasterizer_texture_ref g_rasterizer_albedo_texture;
extern c_rasterizer_texture_ref g_rasterizer_normal_texture;
extern D3DSurface *g_rasterizer_accumulation_surface;
extern D3DSurface *g_rasterizer_albedo_surface;
extern D3DSurface *g_rasterizer_normal_surface;
extern D3DSurface *g_rasterizer_depth_surface;
extern long g_dynamic_render_target_alloc_mask;
#if defined(_XBOX) && !defined(__clang__)
extern c_static_array<long, 6> g_dynamic_render_target_indices;
extern s_data_array *g_dynamic_render_targets;
#endif
}

extern "C" void dynamic_render_targets_set_accumulation_texture();

extern "C" {
extern const double k_dynamic_render_target_tile_width, k_dynamic_render_target_tile_height;
}
#endif
