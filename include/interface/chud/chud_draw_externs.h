#ifndef INTERFACE_CHUD_CHUD_DRAW_EXTERNS_H
#define INTERFACE_CHUD_CHUD_DRAW_EXTERNS_H

#include "rasterizer/rasterizer_device_states.h"
#include "main/main_render_loading_types.h"
#include "text/font_cache_lock.h"
#include "interface/chud/chud_manager.h"
#include "game/player_control_globals.h"
#include "cutscene/cinematic_globals_types.h"

#include "game/player_mapping_globals.h"

#include "cseries/cseries_unicode.h"

// Dependencies of the CHUD draw owner while other units adopt these layouts.

struct s_chud_cortana_effect_basis;

#include "game/game_time_types.h"
#include "camera/director_notification_types.h"
#include "game/player_mapping_types.h"
#include "game/game_globals_storage.h"
#include "camera/observer_globals_types.h"
struct s_cinematic_globals_definition;
struct D3DDevice;
class c_chud_persistent_user_data;
struct c_static_wchar_string_1024;
struct s_data_array;
struct s_player_control_view_globals;
struct s_main_render_loading_globals;
struct D3DTexture;

extern "C" {
void chud_cortana_effect_new_random_basis(s_chud_cortana_effect_basis *basis);
void text_group_get_string(long tag_index, long string_id, c_static_wchar_string_1024 *string);
}

#include "interface/chud/chud_sampler_filter_mode.h"

extern "C" {
extern long g_rasterizer_alpha_blend_mode;

void D3DDevice_SetRenderState_PointSpriteEnable(D3DDevice *, unsigned long);
}
#include "rasterizer/rasterizer_main_globals.h"
#include "rasterizer/rasterizer_sampler_filter_state.h"

#endif
