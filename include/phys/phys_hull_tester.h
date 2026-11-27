#ifndef INCLUDE_PHYS_HULL_TESTER_H
#define INCLUDE_PHYS_HULL_TESTER_H

#include "phys/phys_hull_builder.h"

class phys_hull_tester {
public:
    static phys_bool find_same_edges(const phys_hull_builder::plane_and_points &plane_points1,
        const phys_hull_builder::plane_and_points &plane_points2, phys_bool &found_edge01, phys_bool &found_edge02, phys_bool &found_edge12);
    static phys_bool check_plane_equations(phys_array<phys_vec4> &initial_verts, phys_array<phys_vec4> &used_vertices,
        phys_array<phys_vec4> &hull_plane_equations, float coplanar_eps);
    static phys_bool is_valid_hull(const phys_hull_tolerances &hull_tolerances, const phys_vec4 *initial_verts,
        int initial_vertex_count, phys_geom_hull &geom_hull, phys_array<phys_vec4> &used_vertices);
public:
  static class phys_bool is_valid_planar_hull(class phys_array<class phys_vec4> &p0, class phys_geom_hull &p1, class phys_array<class phys_vec4> &p2,
      class phys_array<class phys_vec4> &p3, class phys_array<class phys_hull_builder::plane_and_points> &p4, float p5);
public:
  static class phys_bool is_valid_nonplanar_hull(class phys_array<class phys_vec4> &p0, class phys_geom_hull &p1, class phys_array<class phys_vec4> &p2,
      class phys_array<class phys_vec4> &p3, class phys_array<class phys_hull_builder::plane_and_points> &p4, float p5);
};

#endif
