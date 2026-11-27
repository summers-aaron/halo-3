#ifndef RASTERIZER_LENS_FLARE_GLOBALS_H
#define RASTERIZER_LENS_FLARE_GLOBALS_H

#include "rasterizer/lens_flare_globals_types.h"

extern "C" {
extern lens_flare_globals g_lens_flare_globals;
// Frame texture flags saved before lens_flares_update clears the current frame.
extern int g_lens_flare_previous_frame_texture_flags;
}

#endif // RASTERIZER_LENS_FLARE_GLOBALS_H
