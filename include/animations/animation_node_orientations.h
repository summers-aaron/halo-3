#ifndef ANIMATIONS_ANIMATION_NODE_ORIENTATIONS_H
#define ANIMATIONS_ANIMATION_NODE_ORIENTATIONS_H

#include "cseries/cseries.h"
#include "animations/animation_id.h"
#include "animations/mixing_board/channels/anim_computation_flags_types.h"
#include "math/real_math_types.h"

extern "C" void animation_compute_node_orientations(long graph_index, c_animation_id animation_id, long node_count, float frame,
    float weight, const c_static_flags_no_init<255> *node_flags, real_orientation *orientations,
    c_flags<e_anim_computation_flags, unsigned long, 1> computation_flags, bool blend, float blend_frame, float blend_weight);

#endif // ANIMATIONS_ANIMATION_NODE_ORIENTATIONS_H
