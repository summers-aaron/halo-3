#ifndef MATH_FUNCTION_DEFINITIONS_CONSTANTS_H
#define MATH_FUNCTION_DEFINITIONS_CONSTANTS_H

#include "math/real_math.h"

// The shipped image forms this address as pool base + 6652; other platforms read the same
// white from the shared color table.
#if defined(_XBOX) && !defined(__clang__)
struct s_function_constant_pool_table
{
    float prior_constants[1663];
    real_argb_color default_white_color;
};

extern "C" {
void k_function_constant_pool_anchor();
}

inline const real_argb_color *function_get_default_white_color()
{
    const s_function_constant_pool_table *table = (const s_function_constant_pool_table *)k_function_constant_pool_anchor;
    return &table->default_white_color;
}
#else
#include "cseries/global_real_argb_colors.h"

inline const real_argb_color *function_get_default_white_color()
{
    return &global_real_argb_colors[_real_argb_white];
}
#endif

#endif // MATH_FUNCTION_DEFINITIONS_CONSTANTS_H
