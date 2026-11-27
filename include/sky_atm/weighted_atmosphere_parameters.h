#ifndef SKY_ATM_WEIGHTED_ATMOSPHERE_PARAMETERS_H
#define SKY_ATM_WEIGHTED_ATMOSPHERE_PARAMETERS_H

#include <stddef.h>
#include "math/real_math_types.h"

// The atmosphere shader's parameters followed by the patchy-fog parameters.
struct s_weighted_atmosphere_parameters
{
    bool valid;
    real_rgb_color sun_color;
    real_vector3d sun_direction;
    float sun_intensity;
    float distance_bias;
    real_rgb_color rayleigh_scattering;
    real_rgb_color mie_scattering;
    real_rgb_color fog_color;
    real_rgb_color mie_color;
    float mie_anisotropy;
    float rayleigh_height;
    float fog_intensity;
    float mie_height;
    float patchy_density;
    float patchy_start_distance;
    float patchy_end_distance;
    real_vector3d patchy_velocity;
};

typedef char weighted_atmosphere_size[(sizeof(s_weighted_atmosphere_parameters) == 0x7c) ? 1 : -1];
typedef char weighted_atmosphere_rayleigh_height[(offsetof(s_weighted_atmosphere_parameters, rayleigh_height) == 0x58) ? 1 : -1];
typedef char weighted_atmosphere_patchy_density[(offsetof(s_weighted_atmosphere_parameters, patchy_density) == 0x64) ? 1 : -1];
typedef char weighted_atmosphere_patchy_velocity[(offsetof(s_weighted_atmosphere_parameters, patchy_velocity) == 0x70) ? 1 : -1];

#endif
