#include "rasterizer/rasterizer_device_states.h"
#ifndef INCLUDE_INTERFACE_CHUD_CHUD_RENDER_DEPENDENCIES_H
#define INCLUDE_INTERFACE_CHUD_CHUD_RENDER_DEPENDENCIES_H

#include "rasterizer/rasterizer_floating_point_depth.h"
#include "game/player_control_globals.h"

struct s_widget_geometry;
struct chud_script_flags;
struct s_saved_film_work_entry;
#include "game/campaign_metagame.h"
#include "game/players_global_data.h"
struct s_campaing_metagame_globals;
extern "C" void chud_draw_cinematic_strip(long strip_index, long strip_count, float x, float y, s_widget_geometry *geometry);
extern "C" bool chud_scripting_is_element_enabled(const chud_script_flags *flags, short index, long user_index);
extern "C" int saved_film_get_marker_count(const s_saved_film_work_entry *entries);
extern "C" bool campaign_metagame_is_enabled(void);
extern "C" float campaign_metagame_get_time_multiplier(long ticks, float *time);
extern "C" float campaign_metagame_get_score_scale(void);

// Shared renderer state and constant-pool anchors used by CHUD.
extern "C" {
extern long g_rasterizer_current_z_buffer_mode;
}

#endif // INCLUDE_INTERFACE_CHUD_CHUD_RENDER_DEPENDENCIES_H
