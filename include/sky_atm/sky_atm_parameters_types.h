#ifndef SKY_ATM_SKY_ATM_PARAMETERS_TYPES_H
#define SKY_ATM_SKY_ATM_PARAMETERS_TYPES_H

#include "math/real_math_types.h"
#include "tag_files/tag_types.h"

struct c_atmosphere_setting
{
    unsigned short flags;
    unsigned short : 16;
    string_id name;
    float pitch;
    float yaw;
    real_rgb_color color;
    float intensity;
    float sea_level;
    float rayleigh_height_scale;
    float mie_height_scale;
    float rayleigh_multiplier;
    float mie_multiplier;
    float sun_phase_function;
    float desaturation;
    float distance_bias;
    float max_fog_thickness;
    real_vector3d beta_m;
    real_vector3d beta_p;
    real_vector3d beta_m_theta_prefix;
    real_vector3d beta_p_theta_prefix;
    float sheet_density;
    float full_intensity_height;
    float half_intensity_height;
    real_vector3d wind_direction;
    s_tag_reference weather_effect;
    float weight;
    float cluster_weight;
};

struct s_water_material_parameters
{
    long name;
    float murkiness;
    real_rgb_color color;
};

// The scenario sky_parameters reference selects this 'skya' tag in both
// atmosphere and water rendering. The unmodeled prefix precedes the two blocks.
struct s_sky_atm_parameters_definition
{
    unsigned short flags;
    unsigned short : 16;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    unsigned long : 32;
    float falloff_distance;
    float falloff_start;
    float falloff_exponent;
    float transparent_sort_distance;
    unsigned char transparent_sort_layer;
    s_tag_block atmosphere_settings;
    c_typed_tag_block<s_water_material_parameters> materials;
};

typedef char sky_atmosphere_setting_size[(sizeof(c_atmosphere_setting) == 0xa4) ? 1 : -1];
typedef char sky_atmosphere_weight_offset[(offsetof(c_atmosphere_setting, weight) == 0x9c) ? 1 : -1];
typedef char sky_atmosphere_falloff_offset[(offsetof(s_sky_atm_parameters_definition, falloff_distance) == 0x20) ? 1 : -1];
typedef char sky_atmosphere_sort_layer_offset[(offsetof(s_sky_atm_parameters_definition, transparent_sort_layer) == 0x30) ? 1 : -1];
typedef char sky_water_material_size[(sizeof(s_water_material_parameters) == 0x14) ? 1 : -1];
typedef char sky_atmosphere_settings_offset[(offsetof(s_sky_atm_parameters_definition, atmosphere_settings) == 0x34) ? 1 : -1];
typedef char sky_water_materials_offset[(offsetof(s_sky_atm_parameters_definition, materials) == 0x40) ? 1 : -1];

#endif
