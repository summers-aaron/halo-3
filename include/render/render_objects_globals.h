#include "rasterizer/rasterizer_tiling_globals.h"
#include "scenario/scenario.h"
#include "rasterizer/rasterizer_surface_globals.h"
#include "cache/restricted_address.h"
#include "objects/light_globals.h"
#ifndef RENDER_RENDER_OBJECTS_GLOBALS_H
#define RENDER_RENDER_OBJECTS_GLOBALS_H
#include "render/camera_fx_globals_types.h"
#include "cutscene/cinematic_new_globals_types.h"
#include "interface/chud/chud_manager.h"
#include "game/player_control_globals.h"
#include "effects/particle_array_globals.h"

extern "C" {
extern __declspec(thread) struct s_render_texture_camera_globals *g_render_texture_camera_globals;
}

#include "render/render_object_storage.h"
#include "render/render_transparency_globals.h"

#include "math/real_math_types.h"
#include "rasterizer/rasterizer_depth_bias.h"

struct D3DXPLANE;
struct s_render_object_globals;
struct s_object_marker_globals;

struct s_effect_render_globals;
#include "game/game_time_types.h"
#include "render/screen_postprocess_settings.h"
struct s_rasterizer_tiling_globals;
#include "interface/chud/chud_navpoint_manager.h"
class c_render_surface_group;
struct s_camera_fx_globals;
struct s_render_texture_camera_globals;
struct s_light_render_globals;
struct s_rasterizer_render_globals;
struct cluster_order;
struct render_player_view_camera_settings;

extern "C" {
extern s_object_marker_globals g_object_marker_globals[];
extern const bool k_render_object_type_flag_16[];
extern long g_first_person_object_index;
extern long g_first_person_physics_object_index;
extern bool g_render_static_lighting_active;
extern bool g_decorator_albedo_active;
extern long g_lighting_debug_mode;
extern D3DXPLANE g_render_clip_planes[];
extern float g_decal_distance_inverse;
extern long g_decal_preplaced_render_counts[2];
extern long g_decal_runtime_render_counts[2];
extern bool g_render_water_flag;
extern float g_shadow_fade_distance_scale;
}

#endif // RENDER_RENDER_OBJECTS_GLOBALS_H
