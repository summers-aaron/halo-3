// blamlibxbox/color_math.obj: from the records 0x18b4d0 0x298540 0x298678 0x2987c8 0x298858
// @flags /TP /O2 /Oc
#include "math/real_math.h"
// @address 0x298540 real_rgb_color_to_real_hsv_color
// @address 0x298678 real_hsv_color_to_real_rgb_color
// @address 0x2987c8 ?pixel32_to_real_argb_color@@YAPATreal_argb_color@@KPAT1@@Z
// @address 0x18b4d0 ?real_argb_color_to_pixel32@@YAKPBTreal_argb_color@@@Z
// @address 0x298858 ?rgb_colors_interpolate@@YAPATreal_rgb_color@@PAT1@KPBT1@1M@Z

#include <math.h>
#include "math/color_math.h"



union real_hsv_color
{
    struct { float hue, saturation, value; };
    float n[3];
};



extern "C" real_hsv_color *real_rgb_color_to_real_hsv_color(const real_rgb_color *rgb, real_hsv_color *hsv)
{
    float maximum = rgb->red > (rgb->green > rgb->blue ? rgb->green : rgb->blue) ? rgb->red : (rgb->green > rgb->blue ? rgb->green : rgb->blue);
    float minimum = rgb->red > (rgb->green > rgb->blue ? rgb->blue : rgb->green) ? (rgb->green > rgb->blue ? rgb->blue : rgb->green) : rgb->red;
    float delta = maximum - minimum;

    hsv->value = maximum;
    hsv->saturation = maximum == 0.0f ? 0.0f : delta / maximum;

    if (hsv->saturation == 0.0f)
    {
        hsv->hue = 0.0f;
    }
    else
    {
        if (rgb->red == maximum)
            hsv->hue = 0.0f + (rgb->green - rgb->blue) / delta;
        else if (rgb->green == maximum)
            hsv->hue = 2.0f + (rgb->blue - rgb->red) / delta;
        else
            hsv->hue = 4.0f + (rgb->red - rgb->green) / delta;

        hsv->hue = hsv->hue / 6.0f;
        if (hsv->hue < 0.0f)
            hsv->hue = hsv->hue + 1.0f;
    }
    return hsv;
}

// match: 0x298678: holds hsv->value in registers across the rgb stores (removal adds lfs reloads of 8(r30) on every sector path)
extern "C" real_rgb_color *real_hsv_color_to_real_rgb_color(const real_hsv_color *__restrict hsv, real_rgb_color * rgb)
{
    float fl = (float)floor((double)hsv->hue);
    float h = 6.0f * (hsv->hue - fl);

    if (!(hsv->saturation != 0.0f))
    {
        rgb->blue = hsv->value;
        rgb->green = hsv->value;
        rgb->red = hsv->value;
    }
    else
    {
        int sector = (int)h;
        float f = h - (float)sector;
        float p = hsv->value * (1.0f - hsv->saturation);
        float q = hsv->value * (1.0f - f * hsv->saturation);
        float t = hsv->value * (1.0f - hsv->saturation * (1.0f - f));

        switch (sector)
        {
        case 0:
            rgb->red = hsv->value;
            rgb->green = t;
            rgb->blue = p;
            break;
        case 1:
            rgb->red = q;
            rgb->green = hsv->value;
            rgb->blue = p;
            break;
        case 2:
            rgb->red = p;
            rgb->green = hsv->value;
            rgb->blue = t;
            break;
        case 3:
            rgb->red = p;
            rgb->green = q;
            rgb->blue = hsv->value;
            break;
        case 4:
            rgb->red = t;
            rgb->green = p;
            rgb->blue = hsv->value;
            break;
        case 5:
            rgb->red = hsv->value;
            rgb->green = p;
            rgb->blue = q;
            break;
        default:
            // match: 0x298678: omits the sector range check (removal adds cmplwi r,5 + bgt to the switch epilogue)
            __assume(0);
        }
    }
    return rgb;
}

real_argb_color *pixel32_to_real_argb_color(unsigned long pixel, real_argb_color *color)
{
    float scale = 1.0f / 255.0f;
    color->alpha = (float)(pixel >> 24) * scale;
    color->rgb.red = (float)((pixel >> 16) & 255) * scale;
    color->rgb.green = (float)((pixel >> 8) & 255) * scale;
    color->rgb.blue = (float)(pixel & 255) * scale;
    return color;
}

real_rgb_color *rgb_colors_interpolate(real_rgb_color *result, unsigned long flags, const real_rgb_color *a, const real_rgb_color *b, float t)
{
    float inverse = 1.0f - t;

    if (flags & 1)
    {
        real_hsv_color hsv_a;
        real_hsv_color hsv_b;
        real_hsv_color hsv_result;

        real_rgb_color_to_real_hsv_color(a, &hsv_a);
        real_rgb_color_to_real_hsv_color(b, &hsv_b);

        if ((fabs(hsv_a.hue - hsv_b.hue) > 0.5f) != ((flags & 2) != 0))
        {
            if (hsv_a.hue < hsv_b.hue)
                hsv_a.hue = hsv_a.hue + 1.0f;
            else
                hsv_b.hue = hsv_b.hue + 1.0f;
        }

        hsv_result.hue = inverse * hsv_a.hue + t * hsv_b.hue;
        if (hsv_result.hue > 1.0f)
            hsv_result.hue = hsv_result.hue - 1.0f;
        hsv_result.saturation = inverse * hsv_a.saturation + t * hsv_b.saturation;
        hsv_result.value = inverse * hsv_a.value + t * hsv_b.value;
        real_hsv_color_to_real_rgb_color(&hsv_result, result);
    }
    else
    {
        result->red = inverse * a->red + t * b->red;
        result->green = inverse * a->green + t * b->green;
        result->blue = inverse * a->blue + t * b->blue;
    }
    return result;
}

unsigned long real_a_rgb_color_to_pixel32(float alpha, const real_rgb_color *color)
{
    unsigned long pixel = 0;
    float scale = 255.0f;

    pixel = ((long)(alpha * scale) << 24) | ((long)(color->red * scale) << 16) | ((long)(color->green * scale) << 8) | (long)(color->blue * scale);
    return pixel;
}

unsigned long real_argb_color_to_pixel32(const real_argb_color *color)
{
    unsigned long pixel = 0;
    pixel = real_a_rgb_color_to_pixel32(color->alpha, &color->rgb);
    return pixel;
}


