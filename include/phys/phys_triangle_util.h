#ifndef INCLUDE_PHYS_TRIANGLE_UTIL_H
#define INCLUDE_PHYS_TRIANGLE_UTIL_H

// Triangle queries defined in tu/hkcollide/hkTriangleUtil.cpp.

#include "phys/phys_math.h"

class phys_triangle_util {
public:
    static phys_bool ray_intersect(float &t, float &u, float &v, const phys_vec4 &origin, const phys_vec4 &direction,
        const phys_vec4 &a, const phys_vec4 &b, const phys_vec4 &c, phys_bool cull_backfaces);
    static phys_bool is_degenerate(const phys_vec4 &a, const phys_vec4 &b, const phys_vec4 &c, float tolerance);

protected:
    phys_triangle_util();
};

#endif // HAVOK_HK_TRIANGLE_UTIL_H
