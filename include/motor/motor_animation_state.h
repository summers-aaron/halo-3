#ifndef MOTOR_MOTOR_ANIMATION_STATE_H
#define MOTOR_MOTOR_ANIMATION_STATE_H

#include "objects/objects.h"
#include "animations/animation_manager.h"

__forceinline long motor_animation_get_state(long i)
{
    object_datum *o = object_get(i);
    c_animation_manager *m = (c_animation_manager *)object_header_block_get(o, &o->animation);
    long state = -1;
    if ((bool)((m->m_flags >> 6) & 1))
        state = m->get_state_name();
    return state;
}

#endif
