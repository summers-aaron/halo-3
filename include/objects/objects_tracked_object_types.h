#ifndef OBJECTS_OBJECTS_TRACKED_OBJECT_TYPES_H
#define OBJECTS_OBJECTS_TRACKED_OBJECT_TYPES_H

#include "math/real_math_types.h"
#include "memory/static_arrays.h"

// Layout-identical to s_object_acceleration_info: object_submit_acceleration
// memcpys one wholesale into objects[] and both flush paths cast elements to it.
struct s_objects_tracked_object {
    real_point3d point;
    real_vector3d acceleration;
    long object_index;
    short node_index;
    unsigned short flags;
};

struct s_objects_tracked_object_globals
{
    long count;
    s_objects_tracked_object objects[64];
    c_static_flags<64> changed_flags;
    bool active;
};

typedef char objects_tracked_globals_size[(sizeof(s_objects_tracked_object_globals) == 0x810) ? 1 : -1];

extern "C" {
extern __declspec(thread) s_objects_tracked_object_globals *g_tracked_object_globals;
}

#endif // OBJECTS_OBJECTS_TRACKED_OBJECT_TYPES_H
