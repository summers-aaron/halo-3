#ifndef INCLUDE_PHYS_BVH_SPHERE_QUERY_H
#define INCLUDE_PHYS_BVH_SPHERE_QUERY_H

#include "phys/phys_collide.h"

extern "C" void phys_bvh_sphere_vm_query_sphere(const phys_bvh_bytecode *code, const phys_sphere &sphere, phys_array<phys_bvh_primitive_info> *hits);

#endif
