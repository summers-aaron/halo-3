#ifndef OBJECTS_OBJECT_ACCELERATION_INFO_TYPES_H
#define OBJECTS_OBJECT_ACCELERATION_INFO_TYPES_H

#include "math/real_math_types.h"
#include "memory/data_types.h"

struct s_object_acceleration_info {
    real_point3d point;
    real_vector3d acceleration;
    long object_index;
    short node_index;
    unsigned short flags;
};

#endif // OBJECTS_OBJECT_ACCELERATION_INFO_TYPES_H
