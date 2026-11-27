#ifndef MATH_CONVEX_HULLS_H
#define MATH_CONVEX_HULLS_H

// Functions defined in tu/blamlibxbox/convex_hulls.cpp that other units call.

union real_point2d;

bool convex_hull2d_test_point(short count, const real_point2d *points, const real_point2d *point, float epsilon);
short convex_hull2d_intersect_new(short p_count, const real_point2d *p, short q_count, const real_point2d *q,
    short maximum_count, real_point2d *result, float epsilon);

#endif // MATH_CONVEX_HULLS_H
