#include "render/shared_shader_constants.h"
#include "effects/decal_message_queue_tls.h"
#include "game/players_global_data.h"
#include "game/player_mapping_globals.h"
#include "math/real_math_constants.h"
#ifndef MAIN_MAIN_RENDER_GLOBALS_H
#define MAIN_MAIN_RENDER_GLOBALS_H
#include "cutscene/cinematic_new_globals_types.h"
#include "cutscene/cinematic_globals_types.h"
#include "render/screen_postprocess.h"

#include "main/game_tick_time_samples.h"
#include "render/water_interaction_tls.h"

#include "objects/object_name_list.h"

#include "rasterizer/rasterizer_shader_gpr_globals.h"
#include "interface/user_interface_dialog_globals.h"

class c_view;
struct s_main_pregame_frame_colors;

#if defined(_XBOX) && !defined(__clang__)
#include <stddef.h>
// Xbox placement spans the render_water timestamp and rasterizer_xenon_main GPR allocation.
struct main_render_shader_state
{
    long last_water_interaction;
    char padding4[0xd8 - 4];
    long gpr_allocation;
};
typedef char main_render_shader_state_gpr_offset[(offsetof(main_render_shader_state, gpr_allocation) == 0xd8) ? 1 : -1];
extern "C" main_render_shader_state g_main_render_shader_state;
#endif

extern "C" {
extern signed char g_main_render_rasterizer_reset;
extern c_view *g_view_stack[4];
extern const s_main_pregame_frame_colors k_main_pregame_frame_colors[];
extern float g_main_pregame_frame_scales[];
extern bool g_main_render_predict_first_person;
extern bool g_main_render_predict_structure;
extern bool g_main_render_predict_pvs;
extern bool g_main_render_predict_objects;
extern bool g_main_render_predict_cinematic;
extern bool g_main_render_fence_active;
extern unsigned long g_main_render_gpu_fence;
}

extern long g_main_render_lag_counter;

struct s_user_globals;
struct s_player_globals;
struct s_cinematic_globals_definition;
struct s_main_time_globals;
struct s_main_render_loading_globals;
struct s_levels_globals;
struct s_controllers;
struct fpw_first_person;
#include "game/game_globals_storage.h"
struct s_game_tick_time_samples;
struct main_render_postprocess_settings;
#include "game/game_time_types.h"
struct render_player_view_camera_settings;
struct D3DDevice;
class c_player_view;
union real_vector4d;

#include "main/main_time_globals.h"

extern "C" {
extern unsigned int g_effect_render_message_count;
#include "effects/effect_message_queue_tls.h"
extern signed char g_main_render_force_default_tuning;
extern unsigned long g_effect_render_location_count;
extern c_screen_postprocess::s_settings g_main_render_default_postprocess_settings;
extern bool g_main_render_fence_enabled;
#include "main/main_render_fence.h"
}

#endif // MAIN_MAIN_RENDER_GLOBALS_H
