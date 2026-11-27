#ifndef AI_RECENT_OBSTACLE_H
#define AI_RECENT_OBSTACLE_H

#include "cseries/cseries_wrapped.h"

enum e_recent_obstacle_flags {
    _recent_obstacle_successful = 0,
    _recent_obstacle_valid = 1,
    k_recent_obstacle_flags_count = 2
};

// March 82e5abb0 constructs the c_flags member. The other methods are emitted
// in actor_moving_objects.obj (82f84c18..82f84d7c); no assert attests a header.
class c_recent_obstacle {
public:
    long object_index;
    short ticks;
    c_flags<e_recent_obstacle_flags, short, k_recent_obstacle_flags_count> flags;

    __forceinline void clear() {
        object_index = -1;
        ticks = 0;
        flags.clear();
    }

    __forceinline bool successful() { return flags.test(_recent_obstacle_successful); }
    __forceinline bool valid() { return flags.test(_recent_obstacle_valid); }
    void set(long index, float seconds, bool success);
    static c_recent_obstacle *find_recent_obstacle(c_recent_obstacle *obstacles, short count, long object_index);
};

typedef char recent_obstacle_size[(sizeof(c_recent_obstacle) == 8) ? 1 : -1];

__forceinline c_recent_obstacle *c_recent_obstacle::find_recent_obstacle(c_recent_obstacle *obstacles, short count, long object_index)
{
    c_recent_obstacle *result = 0;
    for (short slot_index = 0; slot_index < count; ++slot_index) {
        c_recent_obstacle *obstacle = &obstacles[slot_index];
        if (obstacle->valid() && obstacle->object_index == object_index) {
            result = obstacle;
            break;
        }
    }
    return result;
}

#endif // AI_RECENT_OBSTACLE_H
