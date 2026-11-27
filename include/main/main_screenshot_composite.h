#ifndef MAIN_MAIN_SCREENSHOT_COMPOSITE_H
#define MAIN_MAIN_SCREENSHOT_COMPOSITE_H
#include "rasterizer/rasterizer.h"
#include "math/real_math_types.h"
struct s_range_source;
// Preserve the full-register distortion argument used by screenshot rendering.
extern "C" bool screen_postprocess_render_composite(long shader, c_rasterizer::e_surface target, c_rasterizer::e_surface source, float brightness,
    long distortion, long unused, bool depth_effect, float near_depth, float far_depth, const rectangle2d *scissor,
    const real_rectangle2d *bounds, const s_range_source *range);
#endif
