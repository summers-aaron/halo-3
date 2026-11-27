#ifndef RENDER_RENDER_LENS_FLARES_H
#define RENDER_RENDER_LENS_FLARES_H

// Functions defined in tu/graphicslibxbox/render_lens_flares.cpp.

union real_vector3d;

bool valid_real_vector3d_axes2(const real_vector3d *vector, const real_vector3d *vector2);
extern "C" void lens_flares_update(void);
extern "C" void lens_flares_submit_occlusions(long user_index, int query_type);
void lens_flares_render(long user_index);
extern "C" void lens_flare_render(int user_index, int lens_flare_index, float occlusion_factor, int query, int screen_space);

union real_point3d;
union real_vector2d;
union real_vector4d;

bool lens_flare_project_billboard(const real_point3d *point, float radius, const real_vector2d *size,
	real_vector4d *projected_point, real_vector2d *projected_size);


#endif // RENDER_RENDER_LENS_FLARES_H
