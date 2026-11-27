#ifndef INTERFACE_INTERFACE_CONSTANTS_INLINE_H
#define INTERFACE_INTERFACE_CONSTANTS_INLINE_H

#include "math/real_math_types.h"
#include "interface/interface_constants.h"

// March 83619cb8: update the cached dimensions, then return the scale by value.
__forceinline real_vector2d interface_get_authoring_to_target_dimensions_scaling()
{
    calculate_authoring_to_target_dimensions_scaling();
    return g_interface_authoring_to_target_dimensions_scaling;
}

#endif // INTERFACE_INTERFACE_CONSTANTS_INLINE_H
