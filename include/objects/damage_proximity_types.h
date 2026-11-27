#ifndef OBJECTS_DAMAGE_PROXIMITY_TYPES_H
#define OBJECTS_DAMAGE_PROXIMITY_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"
#include "effects/effect_damage_batch_types.h"

// damage_proximity_object is c_render_batch viewed through proximity names (effects.cpp casts
// the live batch to this type): batch_entries/limit/entries/count/selected/radius/min_distance/
// max_distance overlay entries/count/records/record_count/selected_index/search_radius/
// minimum_distance/maximum_distance, and entry radius overlays the record radius.
struct damage_proximity_entry {
    real_point3d position;
    long index;
    float radius;
};

struct damage_proximity_object {
    s_effect_damage_batch_entry batch_entries[8];
    long limit;
    damage_proximity_entry entries[128];
    long count;
    long selected;
    float radius;
    float min_distance;
    float max_distance;
};
typedef char damage_proximity_object_size[(sizeof(damage_proximity_object) == 0xbb8) ? 1 : -1];
typedef char damage_proximity_limit_offset[(offsetof(damage_proximity_object, limit) == 0x1a0) ? 1 : -1];

#endif
