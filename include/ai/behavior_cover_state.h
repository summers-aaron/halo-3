#ifndef AI_BEHAVIOR_COVER_STATE_H
#define AI_BEHAVIOR_COVER_STATE_H

#include "ai/behavior.h"
#include "math/real_math_types.h"

// Cover control, destination testing and next-state selection share this payload.
class c_cover_state_data : public c_behavior_state
{
public:
    short flags;
    short timer;
    short cover_timer;
    short counter;
    short state;
    short desired_ticks;
    real_vector3d direction;
};

typedef char cover_state_layout_1[(offsetof(c_cover_state_data, flags) == 0x24) ? 1 : -1];
typedef char cover_state_layout_2[(offsetof(c_cover_state_data, cover_timer) == 0x28) ? 1 : -1];
typedef char cover_state_layout_3[(offsetof(c_cover_state_data, state) == 0x2c) ? 1 : -1];
typedef char cover_state_layout_4[(offsetof(c_cover_state_data, direction) == 0x30) ? 1 : -1];
typedef char cover_state_layout_5[(sizeof(c_cover_state_data) == 0x3c) ? 1 : -1];

#endif // AI_BEHAVIOR_COVER_STATE_H
