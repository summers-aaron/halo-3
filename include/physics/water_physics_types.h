#ifndef PHYSICS_WATER_PHYSICS_TYPES_H
#define PHYSICS_WATER_PHYSICS_TYPES_H

#include "math/real_math_types.h"
#include "game/materials.h"

struct s_water_physics_test_sphere_result
{
    float depth;
    real_plane3d surface_plane;
    real_plane3d plane;
    real_vector3d flow;
    float fraction;
    long structure_bsp_index;
    long water_instance_index;
    c_global_material_type material_type;
};

typedef char water_physics_test_sphere_result_size[(sizeof(s_water_physics_test_sphere_result) == 0x40) ? 1 : -1];

struct s_water_physics_test_vector_result
{
    real_point3d point;
    real_vector3d normal;
    float fraction;
    long structure_bsp_index;
    long water_instance_index;
    c_global_material_type material_type;
    real_vector3d flow;
    real_vector3d velocity;
    __forceinline s_water_physics_test_vector_result() {}
};

#endif // PHYSICS_WATER_PHYSICS_TYPES_H
