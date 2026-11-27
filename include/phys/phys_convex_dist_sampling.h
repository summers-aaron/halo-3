#ifndef INCLUDE_PHYS_CONVEX_DIST_SAMPLING_H
#define INCLUDE_PHYS_CONVEX_DIST_SAMPLING_H

// Functions defined in tu/hkinternal/hkCvxCvxDistByHeuristicSampling.cpp that other units call.

class phys_hull_vertex;
class phys_convex_shape;
struct phys_simplex_out;
class phys_transform;

extern "C" void phys_convex_dist_by_sampling(const phys_convex_shape *penetration_shape_a,
	const phys_convex_shape *penetration_shape_b, const phys_transform &a_from_b,
	phys_hull_vertex *point_on_a_out, phys_hull_vertex *point_on_b_out, phys_simplex_out &out);

#endif // HAVOK_HK_CVX_CVX_DIST_BY_HEURISTIC_SAMPLING_H
