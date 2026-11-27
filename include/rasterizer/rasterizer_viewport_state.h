#ifndef RASTERIZER_RASTERIZER_VIEWPORT_STATE_H
#define RASTERIZER_RASTERIZER_VIEWPORT_STATE_H

#include "math/real_math_types.h"
#include <stddef.h>

// The last scissor and viewport rectangles saved by rasterizer_main.cpp.
struct s_rasterizer_last_rectangles
{
    rectangle2d scissor;
    rectangle2d viewport;
};

typedef char rasterizer_last_rectangles_size[(sizeof(s_rasterizer_last_rectangles) == 16) ? 1 : -1];
typedef char rasterizer_last_viewport_offset[(offsetof(s_rasterizer_last_rectangles, viewport) == 8) ? 1 : -1];

extern "C" s_rasterizer_last_rectangles g_rasterizer_last_rectangles;

// On Xbox this is a separate symbol placed on g_rasterizer_last_rectangles.viewport;
// other builds name that field directly so both spellings read the same rectangle.
#if defined(_XBOX) && !defined(__clang__)
extern "C" rectangle2d g_rasterizer_viewport_bounds;
#else
#define g_rasterizer_viewport_bounds (g_rasterizer_last_rectangles.viewport)
#endif

#endif // RASTERIZER_RASTERIZER_VIEWPORT_STATE_H
