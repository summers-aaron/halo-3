#ifndef RASTERIZER_RASTERIZER_HUE_SATURATION_H
#define RASTERIZER_RASTERIZER_HUE_SATURATION_H

#include "game/render_postprocess_color_tweaking_types.h"

// Functions and data defined in tu/graphicslibxbox/rasterizer_hue_saturation.cpp that other units use.

extern "C" void rasterizer_set_hue_saturation_constants(const s_render_postprocess_color_tweaking *parameters, float hue_delta,
    float saturation_delta, float brightness, const real_rgb_color *filter, const real_rgb_color *offset);

// The neutral color tweaking (no hue shift, full saturation, white filter) that resets copy from.
extern s_render_postprocess_color_tweaking g_default_hue_saturation_parameters;

extern "C" {
extern s_render_postprocess_color_tweaking g_render_postprocess_color_tweaking;
extern __declspec(thread) s_render_postprocess_color_tweaking *g_screenshot_hue_saturation;
}

#endif // RASTERIZER_RASTERIZER_HUE_SATURATION_H
