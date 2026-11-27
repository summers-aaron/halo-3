#ifndef SKY_ATM_ATMOSPHERE_EXTERNAL_H
#define SKY_ATM_ATMOSPHERE_EXTERNAL_H

struct real_linear_rgb_color;
union real_vector3d;

// Sky lighting query used by atmosphere_get_sun_parameters.
extern "C" void atmosphere_query_sky_sun_parameters(long index, real_linear_rgb_color *color, real_vector3d *direction);

#endif
