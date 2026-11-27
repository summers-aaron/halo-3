#ifndef SKY_ATM_ATMOSPHERE_RENDER_INTERFACE_H
#define SKY_ATM_ATMOSPHERE_RENDER_INTERFACE_H

// Atmosphere-side views of the C render interface.
// Other rendering units retain their private setting and sky-index views.
struct s_cluster_reference;
struct c_atmosphere_setting;
struct s_weighted_atmosphere_parameters;
struct s_atmosphere_sky_index;
struct real_linear_rgb_color;
union real_vector3d;

extern "C" void render_objects_accumulate_atmosphere(
    s_cluster_reference cluster, const c_atmosphere_setting *setting, s_weighted_atmosphere_parameters *parameters, float weight);
extern "C" void atmosphere_get_sun_parameters(
    s_atmosphere_sky_index sky_index, const c_atmosphere_setting *setting, real_linear_rgb_color *color, real_vector3d *direction);

#endif // SKY_ATM_ATMOSPHERE_RENDER_INTERFACE_H
