#ifndef GAME_RENDER_POSTPROCESS_COLOR_TWEAKING_TYPES_H
#define GAME_RENDER_POSTPROCESS_COLOR_TWEAKING_TYPES_H

#include "math/real_math_types.h"

struct s_render_postprocess_color_tweaking {
    float hue_shift;
    float saturation;
    real_rgb_color color_filter;
};

#endif // GAME_RENDER_POSTPROCESS_COLOR_TWEAKING_TYPES_H
