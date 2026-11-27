#ifndef OBJECTS_OBJECT_SOFT_CEILING_QUERIES_H
#define OBJECTS_OBJECT_SOFT_CEILING_QUERIES_H

#include "math/real_math_types.h"

struct objects_soft_ceiling_hit {
    long bsp_index;
    unsigned short plane_index;
    unsigned short record_index;
    void *plane;
    float plane_distance;
    float clearance;
    real_point3d point;
};

struct objects_soft_ceiling_results {
    long best_index;
    float best_clearance;
    real_point3d best_point;
    long count;
    objects_soft_ceiling_hit hits[256];
};

struct objects_soft_ceiling_indices {
    long count;
    long indices[3];
    __forceinline objects_soft_ceiling_indices() : count(0) {}
};

extern "C" void scenario_soft_ceilings_query_triangles_in_sphere(
    objects_soft_ceiling_results *, unsigned long *, long, bool, float, const real_point3d *, const real_point3d *, bool, float);
extern "C" bool scenario_soft_ceilings_compute_collision_normal(const objects_soft_ceiling_results *, const real_point3d *,
    objects_soft_ceiling_indices *, real_vector3d *, float *, float *, bool);

#endif
