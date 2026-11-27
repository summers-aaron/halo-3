#ifndef RENDER_RENDER_PLAYER_VIEW_GLOBALS_H
#define RENDER_RENDER_PLAYER_VIEW_GLOBALS_H
#include "render/camera_fx_globals_types.h"
#include "render/render_lightmap_shadows.h"
#include "cutscene/cinematic_globals_types.h"

#include "rasterizer/rasterizer_dynamic_render_targets.h"
#include "game/player_mapping_globals.h"

#include "render/render_transparency_globals.h"
#include "rasterizer/rasterizer_shader_gpr_types.h"

#include "math/real_math_types.h"

struct render_player_view_camera_settings;
struct s_camera_fx_globals;

extern "C" {
extern unsigned long g_frame_texture_flags, g_uses_scene_texture;
extern long g_current_light_gel_index;
extern real_rectangle2d g_active_camo_texture_bounds;
extern rectangle2d g_active_camo_resolve_bounds;
extern bool g_active_camo_capture_pending;
extern bool g_active_camo_resolve_pending;
extern bool g_main_render_fence_active;
extern unsigned long g_main_render_gpu_fence;
extern long g_depth_render_mode;
extern bool g_cinematic_letterbox_enabled;
extern unsigned long g_scene_texture_flags;
extern unsigned long g_scene_texture_flags_per_view[];
extern long g_last_lighting_view_index;
extern long g_distortion_occlusion_context;
extern float g_postprocess_exposure_offset;
extern const float g_rasterizer_depth_bias_constant_pool[];
extern __declspec(thread) render_player_view_camera_settings *g_player_view_camera_settings;
extern long g_transparent_sort_partition_count;
extern unsigned short g_transparent_sort_mode;
extern unsigned char g_render_method_submit_cache_bypass;
extern unsigned char g_render_method_submit_cached_state_enabled;
extern unsigned char g_render_first_person_active;
extern bool g_rasterizer_distortion_visible;
extern bool g_chud_text_render_target_dirty;
}

struct s_data_array;
struct s_visibility_globals;
struct s_cinematic_globals_definition;
struct s_user_globals;
struct s_controllers;
#include "memory/static_arrays.h"
#include "game/game_globals_storage.h"
#include "main/main_screenshot.h"

#endif // RENDER_RENDER_PLAYER_VIEW_GLOBALS_H
