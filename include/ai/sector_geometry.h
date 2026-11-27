#ifndef AI_SECTOR_GEOMETRY_H
#define AI_SECTOR_GEOMETRY_H

// Functions defined in tu/ailibxbox/sector_geometry.cpp.

#include "ai/sector_ref.h"

class c_ai_point3d;
struct collision_result;
union real_point3d;
union real_vector3d;

c_sector_ref giant_sector_from_point(const real_point3d *point);
c_sector_ref collision_surface_get_sector(short bsp, long surface, long instance_index, long object_index, unsigned long flags, const real_point3d *point);

struct path_collision_result;
struct path_error_info;

extern "C" bool pathfinding_test_vector(const real_point3d *point, c_sector_ref sector_ref, c_sector_ref sector_ref2, const real_vector3d *vector,
    float length, const path_error_info *error_info, path_collision_result *result);
extern "C" bool pathfinding_test_vector_with_flags(const real_point3d *point, c_sector_ref sector_ref, c_sector_ref sector_ref2,
    const real_vector3d *vector, float length, unsigned long flags, const path_error_info *error_info, path_collision_result *result);

// The collision result is the caller's scratch; its global material starts invalid (the default constructor).
extern "C" c_sector_ref pathfinding_find_ground_sector(
    const real_point3d *point, const real_vector3d *up, c_ai_point3d *ai_point, collision_result *collision, float offset);

struct pathfinding_data;
struct sector_link;
union real_vector2d;
union real_point2d;

// Cross a pathfinding link and fill the collision result.
extern "C" void sector_geometry_cross_link(pathfinding_data *data, sector_link *link, c_sector_ref sector_ref,
    unsigned long flags, long link_index, const real_point3d *point, const real_vector2d *vector,
    const void *plane, c_sector_ref *next_sector, pathfinding_data **next_data, bool forward,
    const real_point2d *p1, const real_point2d *p2, path_collision_result *result);

#endif // AI_SECTOR_GEOMETRY_H
