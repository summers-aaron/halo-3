#ifndef RENDER_RENDER_OBJECTS_SUBMISSION_GLOBALS_H
#define RENDER_RENDER_OBJECTS_SUBMISSION_GLOBALS_H

#include "objects/cinematic_light_globals.h"

#include "game/game_engine_globals.h"
#include "game/player_mapping_globals.h"
#include "render/render_lightmap_shadows_globals.h"

#include "objects/light_cinematic_types.h"
#include "rasterizer/rasterizer_implicit_geometry.h"
struct s_game_engine_globals;
struct s_controllers;
struct s_user_globals;
#include "game/game_globals_storage.h"

struct blend_state_value { int value; };
void rasterizer_set_packed_blend_state(struct D3DDevice *device, unsigned long index, blend_state_value state);

extern "C" {
extern __declspec(thread) s_controllers *g_director_globals;
}

#endif
