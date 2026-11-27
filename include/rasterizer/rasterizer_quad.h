#ifndef RASTERIZER_RASTERIZER_QUAD_H
#define RASTERIZER_RASTERIZER_QUAD_H

#include "math/real_math_types.h"
#include "rasterizer/rasterizer.h"
class c_rasterizer_texture_ref;
void rasterizer_quad_screenspace_explicit(const point2d (&points)[4], unsigned long color, c_rasterizer_texture_ref texture_ref,
    bool unknown, c_rasterizer::e_alpha_blend_mode alpha_blend_mode);

#endif // RASTERIZER_RASTERIZER_QUAD_H
