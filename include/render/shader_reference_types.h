#ifndef RENDER_SHADER_REFERENCE_TYPES_H
#define RENDER_SHADER_REFERENCE_TYPES_H

#include "math/real_math_types.h"

struct s_shader_reference
{
    union
    {
        struct { long a, b, c, d; };
        unsigned long colors[4];
    };
};
typedef char shader_reference_size[(sizeof(s_shader_reference) == 16) ? 1 : -1];

#endif // RENDER_SHADER_REFERENCE_TYPES_H
