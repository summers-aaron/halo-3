#include "rasterizer/rasterizer_surface_globals.h"
#include "rasterizer/rasterizer_bound_render_targets.h"
#ifndef RASTERIZER_RASTERIZER_MAIN_RUNTIME_GLOBALS_H
#define RASTERIZER_RASTERIZER_MAIN_RUNTIME_GLOBALS_H

#include "rasterizer/rasterizer_floating_point_depth.h"

#include "rasterizer/rasterizer_main_globals.h"
#include "rasterizer/rasterizer.h"
#include "rasterizer/rasterizer_implicit_geometry.h"
struct main_globals;
class c_render_surface_group;

extern "C" {
extern const float g_rasterizer_depth_bias_constant_pool[];
extern long g_rasterizer_current_z_buffer_mode;
extern long g_rasterizer_alpha_blend_mode;
}

#endif
