#include "rasterizer/rasterizer_device_states.h"
#include "rasterizer/rasterizer_tiling_globals.h"
#ifndef RENDER_RENDER_LIGHTMAP_SHADOWS_GLOBALS_H
#define RENDER_RENDER_LIGHTMAP_SHADOWS_GLOBALS_H

struct s_effect_render_globals;
class c_lightmap_shadows_view;
struct s_rasterizer_tiling_state;
#include "game/game_tuning_types.h"

extern "C" {
extern __declspec(thread) s_effect_render_globals *g_effect_render_globals;
extern s_player_tuning g_player_render_tuning;
}

#endif
