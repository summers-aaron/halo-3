#ifndef RENDER_RENDER_SIMPLE_LIGHTS_H
#define RENDER_RENDER_SIMPLE_LIGHTS_H

#include "math/real_math_types.h"

struct simple_light {
    real_point3d position;
    float intensity;
    real_vector3d direction;
    float diffuse;
    real_rgb_color color;
    float exponent;
    float radius_scale, angle_scale, radius_offset, angle_offset, radius_squared;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
};
typedef char simple_light_size_check[(sizeof(simple_light) == 80) ? 1 : -1];

void render_lights_submit_simple_light_draw_list_to_shader(const float *lights, long count);

#endif
