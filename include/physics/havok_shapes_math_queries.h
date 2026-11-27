#ifndef PHYSICS_HAVOK_SHAPES_MATH_QUERIES_H
#define PHYSICS_HAVOK_SHAPES_MATH_QUERIES_H

#include "math/real_math_types.h"

real_rectangle3d *real_rectangle3d_enclose_point(real_rectangle3d *rectangle, const real_point3d *point);

// match: 0x25f038: direction/plane stay in callee-saved FPRs across the call (removal: frame 0x150->0x160, FPR saves 24->25).
__declspec(noalias) float vector_intersect_plane3d(const real_point3d *point, const real_vector3d *vector, const real_plane3d *plane);
// match: 0x25f038: vertex loads stay in callee-saved FPRs across the call (removal: frame 0x150->0x160, fr27/26/25->fr28/27/26).
__declspec(noalias) bool convex_hull2d_test_point(short count, const real_point2d *points, const real_point2d *point, float epsilon);

#endif // PHYSICS_HAVOK_SHAPES_MATH_QUERIES_H
