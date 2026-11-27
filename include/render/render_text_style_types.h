#ifndef RENDER_RENDER_TEXT_STYLE_TYPES_H
#define RENDER_RENDER_TEXT_STYLE_TYPES_H

#include "math/real_math_types.h"

struct s_render_text_style
{
    const char *string;
    const real_rgb_color *color;
    const real_rgb_color *shadow_color;
    float scale;
};

#endif // RENDER_RENDER_TEXT_STYLE_TYPES_H
