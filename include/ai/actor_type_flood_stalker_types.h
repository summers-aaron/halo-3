#ifndef INCLUDE_AI_ACTOR_TYPE_FLOOD_STALKER_TYPES_H
#define INCLUDE_AI_ACTOR_TYPE_FLOOD_STALKER_TYPES_H

#include "ai/behavior.h"
#include "math/real_math_types.h"

class c_stealth_stalk_data : public c_behavior_state
{
public:
    real_point3d target_position;
    unsigned char stalk_close;
    short stalk_timer;
};

#endif
