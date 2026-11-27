#ifndef OBJECTS_OBJECT_CLUSTER_PAYLOAD_H
#define OBJECTS_OBJECT_CLUSTER_PAYLOAD_H

#include "math/real_math_types.h"

struct s_object_cluster_payload
{
    signed char object_type;
    unsigned char flags;
    unsigned short cull_flags;
    union { real_point3d bounding_sphere_center; long center_words[3]; };
    float bounding_sphere_radius;
};

#endif
