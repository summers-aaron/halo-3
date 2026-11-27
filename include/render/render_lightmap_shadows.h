#ifndef RENDER_RENDER_LIGHTMAP_SHADOWS_H
#define RENDER_RENDER_LIGHTMAP_SHADOWS_H

// Functions defined in tu/graphicslibxbox/render_lightmap_shadows.cpp.

struct s_oriented_bounding_box;

bool obb_visible(s_oriented_bounding_box *obb);

extern "C" long g_first_person_object_index;
extern "C" long g_first_person_physics_object_index;
extern "C" bool g_motion_blur_enabled;
extern "C" const long k_lightmap_shadows_quality_shader_indices[4];
extern "C" float g_shadow_fade_distance_scale;
extern "C" float g_shadow_reduce_distance_scale;
extern "C" bool g_debug_shadow_bounds_solid;
extern "C" bool g_debug_shadow_histencil;

#endif // RENDER_RENDER_LIGHTMAP_SHADOWS_H
