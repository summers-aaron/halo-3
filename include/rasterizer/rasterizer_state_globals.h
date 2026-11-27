#include "rasterizer/rasterizer_device_states.h"
#include "render_methods/render_method_submit_cache.h"
#include "rasterizer/rasterizer_depth_bias_globals.h"
#ifndef RASTERIZER_RASTERIZER_STATE_GLOBALS_H
#define RASTERIZER_RASTERIZER_STATE_GLOBALS_H

#include "rasterizer/rasterizer_floating_point_depth.h"
#include "rasterizer/rasterizer_sampler_filter_state.h"

#include "rasterizer/rasterizer.h"

#include "render/shader_reference_types.h"


extern "C" {
extern long g_rasterizer_current_z_buffer_mode;
extern const float g_rasterizer_depth_bias_constant_pool[];
}

#endif
