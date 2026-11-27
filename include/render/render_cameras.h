#ifndef RENDER_RENDER_CAMERAS_H
#define RENDER_RENDER_CAMERAS_H

// Functions defined in tu/graphicslibxbox/render_cameras.cpp.

extern "C" const float render_cameras_fsel_sign_table[2];

union rectangle2d;
struct render_camera;

void render_view_compute_fullscreen_bounds(struct render_camera * camera);
void render_view_compute_window_bounds(long screen, long arrangement, rectangle2d *bounds, rectangle2d *clipped);

union real_rectangle2d;
struct render_projection;
extern "C" void render_camera_build_projection(const render_camera *camera, const real_rectangle2d *bounds, render_projection *projection, float depth);

union real_point2d;

extern "C" void rectangle2d_scale_around_origin(rectangle2d *bounds, const real_point2d *scale);

struct render_camera_projection;

bool render_camera_build_viewport_frustum_bounds(const render_camera * camera, real_rectangle2d *frustum_bounds);
extern "C" void render_camera_compute_projection_parameters(const render_camera *camera, const real_rectangle2d *bounds,
    render_camera_projection * projection, float aspect_override);

extern "C" const float k_render_camera_width_scale;
extern "C" const float k_render_camera_height_scale;
extern "C" bool g_render_camera_cinematic_widescreen_scale_enabled;
extern "C" const float k_render_camera_cinematic_widescreen_scale;

#endif // RENDER_RENDER_CAMERAS_H
