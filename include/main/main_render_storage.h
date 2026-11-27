#ifndef MAIN_MAIN_RENDER_STORAGE_H
#define MAIN_MAIN_RENDER_STORAGE_H
#include "cutscene/cinematic_new_globals_types.h"

struct s_main_render_cinematic_objects;
#include "render/screen_postprocess_settings.h"
#include "render/render_lightmap_shadows_globals.h"

static __forceinline unsigned long &main_render_timing_settings()
{
    return g_player_render_tuning.flags;
}

#endif
