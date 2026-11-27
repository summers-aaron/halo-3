#include "rasterizer/rasterizer_device_states.h"
#include "rasterizer/rasterizer_surface_globals.h"
#include "rasterizer/rasterizer_bound_render_targets.h"
#ifndef RASTERIZER_DX9_DYNAMIC_GEOMETRY_GLOBALS_H
#define RASTERIZER_DX9_DYNAMIC_GEOMETRY_GLOBALS_H

#include "rasterizer/rasterizer_floating_point_depth.h"

#include "math/real_math_types.h"
#include "rasterizer/rasterizer_render_target_types.h"
#include "rasterizer/rasterizer_render_globals_types.h"
#include "main/main_flag_state.h"
#include "gpu_particle/contrail_layout_types.h"
class c_player_view;
struct render_objects_static_lighting_screen_effect_settings;
extern "C" {
extern s_contrail_layout g_rasterizer_particle_submission_state;
extern long g_rasterizer_current_z_buffer_mode;
extern bool g_rasterizer_distortion_visible;
// Both spellings name the single player-view array both subsystems share (same
// macro as render_globals.h; identical redefinition is benign if both meet).
#define g_rasterizer_player_views c_player_view::x_global_player_views
extern float g_rasterizer_tiling_alpha;
extern const float k_rasterizer_screen_x_offset_scale;
void render_objects_compute_screen_effect(const real_point3d *position, const real_vector3d *forward,
    render_objects_static_lighting_screen_effect_settings *settings, long player_index);
}

#endif
