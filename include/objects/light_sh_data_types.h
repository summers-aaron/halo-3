#ifndef OBJECTS_LIGHT_SH_DATA_TYPES_H
#define OBJECTS_LIGHT_SH_DATA_TYPES_H

#include "math/matrix_math_types.h"
#include "math/real_math_types.h"
#include "render/render_lighting_types.h"

struct s_light_sh_data {
    long object_index;
    render_lighting lighting;
    float scale;
    float sh0[9];
    float sh1[9];
    float sh2[9];
    long definition_index;
    real_matrix4x3 matrix;
};

typedef char light_sh_data_scale_offset[(offsetof(s_light_sh_data, scale) == 0x21c) ? 1 : -1];

#endif // OBJECTS_LIGHT_SH_DATA_TYPES_H
