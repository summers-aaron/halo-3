#ifndef GPU_PARTICLE_EFFECT_CONSTANTS_TYPES_H
#define GPU_PARTICLE_EFFECT_CONSTANTS_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

// GPU effect state: one 16-byte register per scalar/vector, then ten overrides.
struct s_effect_constants
{
    real_vector4d count;
    real_vector4d scale;
    real_vector4d offset;
    real_vector4d length_times_density;
    real_vector4d inverse_density;
    real_vector4d profile_length;
    real_point3d point;
    float point_w;
    real_vector3d vector;
    float vector_w;
    real_vector4d parameters[10];
};

struct s_handle
{
    long identifier;
    unsigned long index;
};

typedef char effect_constants_size[(sizeof(s_effect_constants) == 0x120) ? 1 : -1];
typedef char effect_constants_point_offset[(offsetof(s_effect_constants, point) == 0x60) ? 1 : -1];
typedef char effect_constants_vector_offset[(offsetof(s_effect_constants, vector) == 0x70) ? 1 : -1];
typedef char effect_constants_parameters_offset[(offsetof(s_effect_constants, parameters) == 0x80) ? 1 : -1];
typedef char effect_gpu_handle_size[(sizeof(s_handle) == 8) ? 1 : -1];
typedef char effect_gpu_handle_identifier_offset[(offsetof(s_handle, identifier) == 0) ? 1 : -1];
typedef char effect_gpu_handle_index_offset[(offsetof(s_handle, index) == 4) ? 1 : -1];

#endif // GPU_PARTICLE_EFFECT_CONSTANTS_TYPES_H
