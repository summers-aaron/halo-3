#ifndef EFFECTS_EFFECT_VECTOR_TYPES_H
#define EFFECTS_EFFECT_VECTOR_TYPES_H

#include "math/real_math_types.h"

struct s_effect_vector
{
    real_point3d point;
    real_vector3d vector;
    long marker_name;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char effect_vector_size[(sizeof(void *) != 4 || sizeof(s_effect_vector) == 0x1c) ? 1 : -1];
#endif

#endif // EFFECTS_EFFECT_VECTOR_TYPES_H
