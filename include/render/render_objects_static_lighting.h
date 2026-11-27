#ifndef RENDER_RENDER_OBJECTS_STATIC_LIGHTING_H
#define RENDER_RENDER_OBJECTS_STATIC_LIGHTING_H

#include "effects/transparent_sort_layer_types.h"
#include "math/real_math_trigonometry.h"

// Functions defined in tu/graphicslibxbox/render_objects_static_lighting.cpp.

union real_point3d;
struct render_lighting;
struct render_objects_static_lighting_hierarchy_levels;

void interpolate_scalar(float *value, float target, float rate);
bool render_object_is_first_person(long player_index, long object_index);
long object_get_cached_render_state(long object_index);
void render_object_update_change_colors(long object_index, bool force);
void render_object_invalidate_render_state(long object_index);
void __tls_set_g_cached_object_render_states_allocator(void *object);

template<long, void (*)(void *), void (*)(void *), void (*)(void *)>
class t_restricted_allocation_manager;
extern t_restricted_allocation_manager<2, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_cached_object_render_states_allocator> g_cached_object_render_states_allocator;
render_lighting *object_update_cached_render_lighting(long object_index, float value);
void render_object_calculate_alphas(long object_index, const real_point3d *position, float, long object_type,
    bool time_fade, bool cached_alpha, render_objects_static_lighting_hierarchy_levels *alphas);
extern "C" const render_lighting *object_get_cached_render_lighting(long object_index);

struct s_render_objects_static_lighting_setup;

extern "C" void render_object_initialize_setup(s_render_objects_static_lighting_setup *setup);

struct s_geometry_sample;

extern "C" void render_objects_set_cached_base_address(void *object);
extern "C" void render_objects_apply_cinematic_lighting(long object_index, render_lighting *lighting);
extern "C" bool render_objects_sample_lighting(long object_index, bool flag, s_geometry_sample *sample, long unused);
extern "C" void render_objects_refresh_lighting(long cached_index, long object_index, float value);
extern "C" void render_object_compute_fade(long object_index, float distance, unsigned char *fade);

// Data defined in tu/graphicslibxbox/render_objects_static_lighting.cpp that other units use.

struct s_render_object_first_person_globals;

extern s_render_object_first_person_globals render_object_globals;

struct real_plane3d;

// match: 0x162958: noalias keeps the 0x980 frame, r14/r15 global coloring and __savefpr_22 spills in submit_visibility
extern "C" __declspec(noalias) bool render_objects_add_transparent_element(const real_point3d *point, const real_plane3d *plane,
    float bias, e_transparent_sort_layer sort_layer, void (*callback)(const void *, long), const void *context, long user, float radius);

extern "C" render_lighting g_fallback_render_lighting;

extern "C" float g_render_object_time_fade_scale;

#endif // RENDER_RENDER_OBJECTS_STATIC_LIGHTING_H
