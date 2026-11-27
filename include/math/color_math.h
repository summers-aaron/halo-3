#ifndef MATH_COLOR_MATH_H
#define MATH_COLOR_MATH_H

#include "math/real_math_types.h"

// Functions defined in tu/blamlibxbox/color_math.cpp that other units call.
real_argb_color *pixel32_to_real_argb_color(unsigned long pixel, real_argb_color *color);
unsigned long real_a_rgb_color_to_pixel32(float alpha, const real_rgb_color *color);
real_rgb_color *rgb_colors_interpolate(real_rgb_color *result, unsigned long flags, const real_rgb_color *a, const real_rgb_color *b, float t);
unsigned long real_argb_color_to_pixel32(const real_argb_color *color);

// Color arithmetic used inline by its callers.
inline void real_rgb_color_scale(const real_rgb_color *color, float scale, real_rgb_color *result)
{
    result->red = color->red * scale;
    result->green = color->green * scale;
    result->blue = color->blue * scale;
}
inline void real_rgb_color_bias(const real_rgb_color *color, float bias, real_rgb_color *result)
{
    result->red = color->red + bias;
    result->green = color->green + bias;
    result->blue = color->blue + bias;
}
inline void real_argb_color_scale(const real_argb_color *color, float scale, real_argb_color *result)
{
    real_rgb_color_scale(&color->rgb, scale, &result->rgb);
    result->alpha = color->alpha * scale;
}
inline void real_argb_color_bias(const real_argb_color *color, float bias, real_argb_color *result)
{
    real_rgb_color_bias(&color->rgb, bias, &result->rgb);
    result->alpha = color->alpha + bias;
}


#endif // MATH_COLOR_MATH_H
