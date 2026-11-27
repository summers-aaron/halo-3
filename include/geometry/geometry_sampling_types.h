#ifndef GEOMETRY_GEOMETRY_SAMPLING_TYPES_H
#define GEOMETRY_GEOMETRY_SAMPLING_TYPES_H

#include <stddef.h>

#include "math/real_math_types.h"

// The light sample shared by the sampler and object lighting.
struct s_geometry_sample
{
    real_point3d position;
    float red[36];
    float green[36];
    float blue[36];
    real_vector3d diffuse;
    real_vector3d normal;
    real_vector3d dominant_light_direction;
    real_rgb_color dominant_light_color;
    float dominant_light_intensity;
    bool valid;
    float light_scale;
};

enum e_geometry_sample_result
{
    _geometry_sample_result_sampled = 0,
    _geometry_sample_result_no_lightprobes,
    _geometry_sample_result_no_geometry,
    _geometry_sample_result_unlit_geometry,
    _geometry_sample_result_no_lightmap,
    _geometry_sample_result_no_lighting_data,
};

typedef char geometry_sample_size[(sizeof(s_geometry_sample) == 0x1f8) ? 1 : -1];
typedef char geometry_sample_red_offset[(offsetof(s_geometry_sample, red) == 0xc) ? 1 : -1];
typedef char geometry_sample_green_offset[(offsetof(s_geometry_sample, green) == 0x9c) ? 1 : -1];
typedef char geometry_sample_blue_offset[(offsetof(s_geometry_sample, blue) == 0x12c) ? 1 : -1];
typedef char geometry_sample_valid_offset[(offsetof(s_geometry_sample, valid) == 0x1f0) ? 1 : -1];
 typedef char geometry_sample_light_scale_offset[(offsetof(s_geometry_sample, light_scale) == 0x1f4) ? 1 : -1];

#endif // GEOMETRY_GEOMETRY_SAMPLING_TYPES_H
