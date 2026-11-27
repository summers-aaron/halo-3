#ifndef RASTERIZER_DX9_DYNAMIC_SCREEN_GEOMETRY_TYPES_H
#define RASTERIZER_DX9_DYNAMIC_SCREEN_GEOMETRY_TYPES_H
#include "math/real_math_types.h"
#include "rasterizer/rasterizer_textures_cache.h"
#include "rasterizer/dx9/rasterizer_screenshot_types.h"
#include <stddef.h>

struct rasterizer_dynamic_screen_geometry_parameters : s_rasterizer_screenshot_context
{
    c_rasterizer_texture_ref textures[3];
    bool texture_address[16];
    real_vector2d scale0;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    real_vector2d scale1;
    real_vector2d secondary_scale;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    long : 32;
    short alpha_blend_mode;
    bool point_sampled;
    long explicit_shader_index;
};
typedef char dynamic_screen_geometry_secondary_scale_offset[
    (sizeof(void *) != 4 || ((offsetof(rasterizer_dynamic_screen_geometry_parameters, secondary_scale) == 0x44))) ? 1 : -1];
typedef char dynamic_screen_geometry_alpha_blend_mode_offset[
    (sizeof(void *) != 4 || ((offsetof(rasterizer_dynamic_screen_geometry_parameters, alpha_blend_mode) == 0x90))) ? 1 : -1];
typedef char dynamic_screen_geometry_size[(sizeof(void *) != 4 || ((sizeof(rasterizer_dynamic_screen_geometry_parameters) == 0x98))) ? 1 : -1];
#endif
