#include "rasterizer/rasterizer_device_states.h"
#include "rasterizer/rasterizer_surface_globals.h"
#ifndef RENDER_METHODS_RENDER_METHOD_SUBMIT_GLOBALS_H
#define RENDER_METHODS_RENDER_METHOD_SUBMIT_GLOBALS_H

#include "rasterizer/rasterizer_dynamic_render_targets.h"

#include "objects/cluster_reference.h"
#include "render/shader_reference_types.h"
#include "rasterizer/global_shader_parameter_types.h"

// Shared declarations for render method submission globals.
extern "C" {
extern long g_render_method_submit_cached_definition_index;
extern const s_global_shader_parameter k_global_shader_parameters[49];
extern const long k_mesh_part_type_entry_points[];
extern float g_render_method_submit_loop_phase;
extern long g_render_method_submit_loop_phase_tick;
}

#include "render_methods/render_method_submit_cache.h"

#include "math/real_math_types.h"
#include "memory/static_arrays.h"

struct s_data_array;
class c_view;
class c_render_surface_group;

extern "C" {
extern unsigned char g_render_method_submit_cached_state_enabled;
extern unsigned char g_render_method_submit_state_dirty;
extern real_vector4d g_render_method_submit_light_direction, g_render_method_submit_extern_constant_47;
extern real_rgb_color g_render_method_submit_light_intensity;
extern real_argb_color g_render_method_submit_cluster_color;
}

#endif // RENDER_METHODS_RENDER_METHOD_SUBMIT_GLOBALS_H
