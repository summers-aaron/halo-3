#ifndef RENDER_RENDER_OBJECTS_DEPENDENCIES_H
#define RENDER_RENDER_OBJECTS_DEPENDENCIES_H

#include "math/real_math_types.h"

struct s_cluster_reference;
class c_player_view;
struct render_camera;
struct render_objects_screen_effect_settings;
struct render_player_view_observer_flags;
struct render_objects_static_lighting_hierarchy_levels;

// Call-site views retained until the render interface types are unified.
extern "C" {
long render_objects_add_visible_hierarchy(long object_index, const real_point3d *origin, float maximum_distance,
    unsigned long view_index, bool a, bool b, bool c, long index,
    render_objects_static_lighting_hierarchy_levels levels, bool d, bool e, long mask);
void atmosphere_get_sun_parameters(s_cluster_reference cluster, const void *setting, real_rgb_color *color, real_vector3d *direction);
void render_method_submit_draw_mesh_part(unsigned __int64 a, void *b, void *c, void *d, long e, unsigned long *f, void *g);
void player_view_render_lightmap_shadows(c_player_view *view);
void screen_postprocess_render_view(void *fx, const render_camera *camera, const render_objects_screen_effect_settings *color,
    long multires, const render_player_view_observer_flags *focus, long user_index);
}

#endif // RENDER_RENDER_OBJECTS_DEPENDENCIES_H
