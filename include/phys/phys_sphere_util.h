#ifndef INCLUDE_PHYS_SPHERE_UTIL_H
#define INCLUDE_PHYS_SPHERE_UTIL_H

#include "phys/phys_base_types.h"

struct phys_ray_input;
struct phys_ray_output;

class phys_sphere_util {
public:
    static phys_bool trace_sphere_ray(float radius, const phys_ray_input &input, phys_ray_output &output);
};

#endif
