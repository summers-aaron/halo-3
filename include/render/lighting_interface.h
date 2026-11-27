#ifndef RENDER_LIGHTING_INTERFACE_H
#define RENDER_LIGHTING_INTERFACE_H

// c_lighting_interface: static methods defined in tu/graphicslibxbox/lighting_interface.cpp (lighting_interface.obj
// in the March 2007 build, which also has get_default_sky_lightprobe, process_prt and the inline
// get_dominant_light_dir / get_dominant_light_intensity over the static m_dominant_light_dir and
// m_dominant_light_intensity; only the methods some unit declares are listed).

union real_rgb_color;
union real_vector3d;

extern "C" {
extern long g_lighting_debug_mode;
}

enum e_lighting_debug_mode
{
    _lighting_debug_disabled = 0,
    _lighting_debug_positive_x = 1,
    _lighting_debug_negative_x = 2,
    _lighting_debug_positive_y = 3,
    _lighting_debug_negative_y = 4,
    _lighting_debug_positive_z = 5,
    _lighting_debug_negative_z = 6,
    _lighting_debug_positive_xy = 7,
    _lighting_debug_negative_xy = 8,
    _lighting_debug_positive_xz = 9,
    _lighting_debug_negative_xz = 10,
    _lighting_debug_positive_yz = 11,
    _lighting_debug_negative_yz = 12,
    _lighting_debug_probe_0 = 13,
    _lighting_debug_probe_1 = 14,
    _lighting_debug_probe_2 = 15,
    _lighting_debug_probe_3 = 16,
    _lighting_debug_probe_4 = 17,
};

class c_lighting_interface
{
private:
    static void calculate_and_set_ravi_constants_internal(const float *red, const float *green, const float *blue, float scale);

public:
    static void calculate_and_set_ravi_constants(const float *red, const float *green, const float *blue, float scale);
    static void set_object_dominant_light_direction_and_intensity(const real_vector3d *direction, const real_rgb_color *intensity);
    static void setup_default_lighting();
};

#include "math/real_math_types.h"
extern "C" unsigned char lighting_interface_get_visible_sky_lighting(long *dst0, long *dst1, long *dst2, real_vector3d *dir, real_rgb_color *intensity);

#endif // RENDER_LIGHTING_INTERFACE_H
