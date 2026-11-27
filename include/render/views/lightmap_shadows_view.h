#ifndef RENDER_VIEWS_LIGHTMAP_SHADOWS_VIEW_H
#define RENDER_VIEWS_LIGHTMAP_SHADOWS_VIEW_H

#include "render/views/world_view.h"

struct light_geometry;
struct s_oriented_bounding_box
{
    real_matrix4x3 matrix;
    union
    {
        float extents[3];
        real_vector3d half_extents;
    };
};

class c_lightmap_shadows_view : public c_world_view
{
public:
    __forceinline c_lightmap_shadows_view();
    virtual void render_submit_visibility();
    void submit_visibility_and_render(float shadow_fade, float scale);
    void render(long ignored_object_index);
    void submit_visibility_and_render(float shadow_fade, float resolution_scale, long object_index, long model_index);
    void compute_visibility(long object_index, long light_index);
    static bool object_shadow_visible(long object_index, s_oriented_bounding_box *obb, light_geometry *geometry);
    static void setup_camera(long user_index);
    void setup_camera(long width, long height);
    void render_setup_internal() const;

    s_oriented_bounding_box m_object_bounding_box;
};

typedef char lightmap_shadows_box_size[(sizeof(s_oriented_bounding_box) == 0x40) ? 1 : -1];
typedef char lightmap_shadows_box_extents_offset[(offsetof(s_oriented_bounding_box, half_extents) == 0x34) ? 1 : -1];
typedef char lightmap_shadows_view_box_offset[(sizeof(void *) != 4 || ((offsetof(c_lightmap_shadows_view, m_object_bounding_box) == 0x298))) ? 1 : -1];
typedef char lightmap_shadows_view_size[(sizeof(void *) != 4 || ((sizeof(c_lightmap_shadows_view) == 0x2d8))) ? 1 : -1];

#endif // RENDER_VIEWS_LIGHTMAP_SHADOWS_VIEW_H
