#ifndef AI_ACTOR_FIRING_POSITION_EVALUATION_INLINE_H
#define AI_ACTOR_FIRING_POSITION_EVALUATION_INLINE_H
#include "ai/actor_firing_position.h"
static __forceinline bool actor_firing_position_reject(firing_position_evaluation_context *context, firing_position *position)
{
    position->rejected = true;
    return !context->debug;
}
#endif
