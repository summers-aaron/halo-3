#ifndef ANIMATIONS_ANIMATION_SCALAR_NODE_ORIENTATIONS_H
#define ANIMATIONS_ANIMATION_SCALAR_NODE_ORIENTATIONS_H

#include "animations/animation_id.h"
#include "math/real_math_types.h"

// match: keep scalar flags ABI; the canonical c_flags parameter emits extra code.
struct flags256;
extern "C" void animation_compute_node_orientations(long graph_index, c_animation_id animation_id, long node_count, float frame,
    float weight, const flags256 *node_flags, real_orientation *orientations, unsigned long computation_flags, bool blend,
    float blend_frame, float blend_weight);

#endif
