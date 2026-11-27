#ifndef OBJECTS_OBJECT_ANIMATION_H
#define OBJECTS_OBJECT_ANIMATION_H

template<long k_count> class c_static_flags;
struct render_model_definition;
class c_animation_manager;
struct real_orientation;

void internal_object_compute_animated_node_orientations(long object_index, float blend_ratio, const render_model_definition *render_model,
    c_animation_manager *animation_manager, const c_static_flags<255> *node_mask, long node_orientations_count,
    real_orientation *node_orientations, bool interpolate);

#if defined(_XBOX) && !defined(__clang__)
extern "C" {
extern c_static_flags<255> g_object_render_only_node_flags;
extern unsigned long g_object_render_only_node_flags_initialization;
}
#endif

class c_interpolator_control;
extern "C" void object_type_preprocess_node_orientations(const c_interpolator_control *control, short node_count, const c_static_flags<255> *flags,
    const real_orientation *orientations, real_orientation *result_orientations);

#endif // OBJECTS_OBJECT_ANIMATION_H
