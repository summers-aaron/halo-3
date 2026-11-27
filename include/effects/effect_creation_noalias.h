#ifndef EFFECTS_EFFECT_CREATION_NOALIAS_H
#define EFFECTS_EFFECT_CREATION_NOALIAS_H
#include "effects/effect_creation.h"
// match: 0x35cf48: item_detonate keeps datum base in r3, spill at 68h(r1); without it base moves to r29, spill to 60h(r1), stores reschedule.
__declspec(noalias) long effect_new_from_object(c_tag_index tag_index, const s_damage_owner *damage_owner,
    long flags, long object_index, float scale_a, float scale_b, const real_rgb_color *color,
    const effect_vector_field *vector_field, long mode);
#endif
