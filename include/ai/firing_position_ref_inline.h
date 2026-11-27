#ifndef AI_FIRING_POSITION_REF_INLINE_H
#define AI_FIRING_POSITION_REF_INLINE_H

#include "ai/firing_position_ref.h"

static __forceinline bool fp_ref_equals(firing_position_ref a, firing_position_ref b)
{
    return a.value == b.value;
}

// A dynamic firing position: the block's top bit is set and the reference is not FP_REF_CURRENT.
static __forceinline bool fp_ref_is_dynamic(firing_position_ref reference)
{
    return (reference.block & 0x8000) != 0 && !fp_ref_equals(reference, FP_REF_CURRENT);
}

#endif // AI_FIRING_POSITION_REF_INLINE_H
