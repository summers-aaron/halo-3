#ifndef AI_SEAT_ENTRY_INFO_TYPES_H
#define AI_SEAT_ENTRY_INFO_TYPES_H

#include "math/real_math_types.h"

struct seat_entry_info {
    long unit_index;
    short seat_index;
    short pad6;
    real_point3d entrance;
    real_vector3d approach;
    float preference;
    float score;
    bool aligned;
    bool can_enter;
    bool close;
    char pad2b;
};

#endif // AI_SEAT_ENTRY_INFO_TYPES_H
