#ifndef INCLUDE_PHYS_GEOMETRY_UTIL_H
#define INCLUDE_PHYS_GEOMETRY_UTIL_H

template <typename T> class phys_array;
class phys_vec4;
class phys_sphere;
class phys_transform;
struct phys_strided_vertices;
struct phys_geometry;
#include "phys/phys_hull_mode.h"
enum phys_geom_obb_mode {
    PHYS_OBB_MODE_INVALID = 0,
    PHYS_OBB_MODE_DEFAULT = 1,
    PHYS_OBB_MODE_BETTER_FIT_BUT_SLOWER = 2,
    PHYS_OBB_MODE_MAX_ID
};

class phys_geometry_util {
public:
    static void build_convex_geometry(const phys_strided_vertices &vertices, phys_geometry &geometry, phys_hull_mode mode);
    static void build_convex_geometry(const phys_strided_vertices &vertices, phys_geometry &geometry, phys_array<phys_vec4> &planes, phys_hull_mode mode);
    static void vertices_from_planes(const phys_array<phys_vec4> &planes, phys_array<phys_vec4> &vertices);
    static void geometry_from_planes(const phys_array<phys_vec4> &planes, phys_geometry &geometry);
    static void compute_obb(const phys_strided_vertices &vertices, phys_vec4 &box_half_extents, phys_transform &transform, phys_geom_obb_mode mode);
    static phys_sphere min_bounding_sphere(const phys_strided_vertices &vertices, float radius_epsilon);
    static void expand_planes(phys_array<phys_vec4> &planes, float plane_expansion);
};

#endif // HAVOK_HK_GEOMETRY_UTILITY_H
