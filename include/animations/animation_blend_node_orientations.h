#ifndef ANIMATIONS_ANIMATION_BLEND_NODE_ORIENTATIONS_H
#define ANIMATIONS_ANIMATION_BLEND_NODE_ORIENTATIONS_H

#include "animations/animation_codec_flags.h"
#include "animations/animation_inheritence.h"
#include "math/real_math_types.h"

template <long k_blend_mode, bool k_remap>
void animation_blend_node_orientations(long, const s_animation_inheritence *inheritance,
    const s_node_flag_sets *correction_flags, const real_orientation *corrections, const s_node_flag_sets *flags,
    const real_orientation *source, long, real_orientation *destination, real ratio, real inverse_ratio);

#endif
