#ifndef OBJECTS_OBJECT_BROADPHASE_TYPES_H
#define OBJECTS_OBJECT_BROADPHASE_TYPES_H
#include "math/real_math_types.h"
#include "objects/object_cluster_payload.h"

struct s_object_broadphase_handle_data
{
    signed char object_type;
    unsigned short cull_flags;
    real_point3d center;
    float radius;
};
#endif
