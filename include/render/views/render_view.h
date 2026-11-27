#ifndef RENDER_VIEWS_RENDER_VIEW_H
#define RENDER_VIEWS_RENDER_VIEW_H

// Functions defined in tu/graphicslibxbox/render_view.cpp.

struct render_camera;
struct s_observer_result;

class c_ui_view;
extern "C" void render_view_initialize_surface(c_ui_view *view, long, long surface);
extern "C" void render_view_initialize_camera(render_camera *camera, const s_observer_result *observer);

union rectangle2d;
struct render_projection;
struct s_oriented_bounding_box;

void render_camera_build_orthogonal_projection(
    const s_oriented_bounding_box *bounds, const rectangle2d *window_bounds, render_projection *projection);

// The default horizontal field of view (placed by the // @address line in render_view.cpp).
extern "C" float k_render_default_horizontal_fov;

extern "C" void render_view_reset_camera_fx(void);

#endif // RENDER_VIEWS_RENDER_VIEW_H
