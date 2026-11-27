#ifndef ANIMATIONS_ANIMATION_COMPUTE_ORIENTATIONS_H
#define ANIMATIONS_ANIMATION_COMPUTE_ORIENTATIONS_H

#include "cseries/cseries.h"
#include "animations/animation_id.h"
#include "math/real_math_types.h"

#include "animations/animation_node_flags.h"
extern "C" void animation_compute_node_orientations(long graph_index, c_animation_id animation_id,
    long node_count, real frame, real weight, const c_static_flags_no_init<255> *node_flags,
    real_orientation *orientations, bool computation_flags, bool blend, real blend_frame, real blend_weight);

#endif // ANIMATIONS_ANIMATION_COMPUTE_ORIENTATIONS_H
