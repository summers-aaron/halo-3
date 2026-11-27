#ifndef RENDER_RENDER_LIGHTS_H
#define RENDER_RENDER_LIGHTS_H

// Functions defined in tu/graphicslibxbox/render_lights.cpp that other units call.

class c_lights_view;

extern "C" void render_lights_render_light(long light_index, long unused, float scale, c_lights_view *view);
extern "C" void render_lights_add_simple_light(int light_index, int view_index, float intensity_scale, void *context);
extern "C" void render_lights_iterate_lights(void *context, long unused_user_index, long view_index, long maximum,
    unsigned char direct, unsigned char indirect, float scale, void (*callback)(int, int, float, void *));

extern "C" long g_current_light_gel_index;

#include "math/real_math_types.h"

extern "C" void render_lights_set_single_light(c_lights_view *state, real_point3d position, real_rgb_color color, float intensity,
    float radius, real_vector3d direction, float angle, float exponent, float diffuse);

extern "C" {
extern const real_vector3d k_render_light_clip_plane_normals[6];
extern const float k_render_light_clip_plane_distances[6];
}

#endif // RENDER_RENDER_LIGHTS_H
