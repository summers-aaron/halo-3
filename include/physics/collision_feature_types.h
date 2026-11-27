#ifndef PHYSICS_COLLISION_FEATURE_TYPES_H
#define PHYSICS_COLLISION_FEATURE_TYPES_H

#include <stddef.h>
#include "math/real_math_types.h"

struct collision_sphere
{
    long user0;
    unsigned long user1;
    long user2;
    long user3;
    unsigned char user4;
    unsigned char user5;
    short user6;
    real_point3d center;
    float radius;
};

struct collision_cylinder
{
    long user0;
    unsigned long user1;
    long user2;
    long user3;
    unsigned char user4;
    unsigned char user5;
    short user6;
    real_point3d base;
    real_vector3d height;
    float radius;
};

struct collision_prism
{
    long user0;
    unsigned long user1;
    long user2;
    long user3;
    unsigned char user4;
    unsigned char user5;
    short user6;
    real_plane3d plane;
    float height;
    short projection;
    bool sign;
    long count;
    real_point2d points[8];
};

struct collision_feature_list
{
    union
    {
        struct
        {
            short sphere_count;
            short cylinder_count;
            short prism_count;
        };
        short counts[3];
    };
    collision_sphere spheres[256];
    collision_cylinder cylinders[256];
    collision_prism prisms[256];
};

struct collision_plane
{
    long user0;
    unsigned long user1;
    long user2;
    unsigned char user4;
    unsigned char user5;
    short user6;
    float t;
    real_point3d point;
    real_plane3d plane;
};

typedef char collision_sphere_size[(sizeof(collision_sphere) == 0x24) ? 1 : -1];
typedef char collision_cylinder_size[(sizeof(collision_cylinder) == 0x30) ? 1 : -1];
typedef char collision_prism_size[(sizeof(collision_prism) == 0x70) ? 1 : -1];
typedef char collision_prism_count_offset[(offsetof(collision_prism, count) == 0x2c) ? 1 : -1];
typedef char collision_feature_list_size[(sizeof(collision_feature_list) == 0xc408) ? 1 : -1];
typedef char collision_feature_spheres_offset[(offsetof(collision_feature_list, spheres) == 8) ? 1 : -1];
typedef char collision_plane_size[(sizeof(collision_plane) == 0x30) ? 1 : -1];

#endif // PHYSICS_COLLISION_FEATURE_TYPES_H
