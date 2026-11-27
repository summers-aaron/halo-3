#ifndef RENDER_RENDER_OBJECTS_H
#define RENDER_RENDER_OBJECTS_H

#include "objects/object_reference_types.h"
#include "math/real_math_types.h"

// Functions defined in tu/graphicslibxbox/render_objects.cpp.

void __tls_set_g_render_object_globals_allocator(void *allocator);
void render_objects_reset_render_state(long unused);

template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<4, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_render_object_globals_allocator> g_render_object_globals_allocator;

struct render_lighting;

extern "C" bool object_get_render_model_instance_bitfield_visible(long object_index, void **out_address);
extern "C" void render_objects_set_hiz_stencil_state(void *memory);
class c_player_view;
extern "C" void render_objects_render_transparent_pass(c_player_view *view);
extern "C" long render_objects_set_lighting_constants(const render_lighting *lighting, long result, long type);

struct s_mesh_part_counter;
struct s_render_object_info;

extern "C" void render_objects_add_instance_mesh_part(s_mesh_part_counter *counter, s_render_object_info *object_info, unsigned long flags);
extern "C" float render_objects_compute_falloff_weight(float p0, float p1, float p2, float p3);
extern "C" void render_objects_render_object_context(long object_index, long entry_point, long p2, char p3);
extern "C" void render_objects_submit_visible_hierarchy(long first_object, long object_count,
    long context_parameters, unsigned long flags, unsigned long submit_flags);
extern "C" void render_objects_render_shield_impact(const void *user_data, long user_context);

union real_point3d;
struct s_object_render_context;

extern "C" void render_objects_update_element_weights(s_cluster_reference cluster_reference, const real_point3d *point);
extern "C" void render_objects_submit_active_camo_parts(s_object_render_context *context, s_render_object_info *object_info,
    unsigned char transparent, unsigned char lighting_flag);

// Set while the hierarchical-Z stencil pass is in use: render_albedo, player_view_render_opaque_objects and the
// transparency pass set the hi-z stencil state only then.
extern "C" bool g_render_hiz_stencil_enabled;

struct render_objects_screen_effect_settings
{
    float channels[8];
    real_rgb_color color_filter;
    real_rgb_color color_floor;
};
extern "C" void render_objects_compute_screen_effect(
    const real_point3d *position, const real_vector3d *forward, render_objects_screen_effect_settings *values, long player_index);

extern "C" void *render_objects_get_cluster_element(s_cluster_reference reference, const char **out_name);

extern "C" void render_objects_reset_cached_indices(void);

#endif // RENDER_RENDER_OBJECTS_H
