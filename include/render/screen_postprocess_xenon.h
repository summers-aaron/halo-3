#ifndef RENDER_SCREEN_POSTPROCESS_XENON_H
#define RENDER_SCREEN_POSTPROCESS_XENON_H

// Functions defined in tu/graphicslibxbox/screen_postprocess_xenon.cpp that other units call.

class c_player_view;

extern "C" void screen_postprocess_downsample_surface(float *width_scale, float *height_scale, float width, float height, long surface);
extern "C" void screen_postprocess_blit_scaled_surface(long surface, long surface2, long width, long height, long shader);
extern "C" void screen_postprocess_blit_player_view(const c_player_view *view);

#include "rasterizer/rasterizer.h"
#include "math/real_math_types.h"

struct s_range_source;

#endif // RENDER_SCREEN_POSTPROCESS_XENON_H
