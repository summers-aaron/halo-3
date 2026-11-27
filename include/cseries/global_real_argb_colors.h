#ifndef CSERIES_GLOBAL_REAL_ARGB_COLORS_H
#define CSERIES_GLOBAL_REAL_ARGB_COLORS_H

#include "math/real_math_types.h"

// The shared colour table in .rdata; the global_real_argb_* pointers in cseries.h point into it.
enum e_global_real_argb_color
{
    _real_argb_white,
    _real_argb_grey,
    _real_argb_black,
    _real_argb_red,
    _real_argb_green,
    _real_argb_blue,
    _real_argb_cyan,
    _real_argb_yellow,
    _real_argb_magenta,
    _real_argb_pink,
    _real_argb_lightblue,
    _real_argb_orange,
    _real_argb_purple,
    _real_argb_aqua,
    _real_argb_darkgreen,
    _real_argb_salmon,
    _real_argb_violet,

    k_global_real_argb_color_count
};

extern "C" const real_argb_color global_real_argb_colors[k_global_real_argb_color_count];

#endif // CSERIES_GLOBAL_REAL_ARGB_COLORS_H
