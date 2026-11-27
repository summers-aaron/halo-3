#ifndef SCENARIO_WIND_DEFINITIONS_H
#define SCENARIO_WIND_DEFINITIONS_H

#include "math/function_definitions.h"

struct s_wind_values
{
    long seconds;
    long fractional_seconds;
    long offset_x;
    long offset_y;
    float texture_x;
    float texture_y;
    float frequency;
    float zero;
    float velocity_x;
    float velocity_y;
    float turbulence;
    float time;
};

struct s_wind_definition
{
    c_function_definition direction;
    c_function_definition speed;
    c_function_definition bend;
    c_function_definition turbulence;
    c_function_definition texture_speed;
    float frequency;
    s_tag_reference gust_noise_bitmap;

    void update(s_wind_values *values) const;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char wind_definition_size[(sizeof(s_wind_definition) == 0x78) ? 1 : -1];
#endif
typedef char wind_values_size[(sizeof(s_wind_values) == 0x30) ? 1 : -1];

// @address 0x81dc34 ?g_wind_values@@3Us_wind_values@@A
extern s_wind_values g_wind_values;
// @address 0x58968 k_wind_texture_offset_scale
extern "C" const float k_wind_texture_offset_scale;
// @address 0x5896c k_wind_speed_to_feet_per_second
extern "C" const float k_wind_speed_to_feet_per_second;

#endif // SCENARIO_WIND_DEFINITIONS_H
