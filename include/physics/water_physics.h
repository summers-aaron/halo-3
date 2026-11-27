#ifndef PHYSICS_WATER_PHYSICS_H
#define PHYSICS_WATER_PHYSICS_H

// Functions defined in tu/blamlibxbox/water_physics.cpp.

union real_point3d;
union real_vector3d;
struct s_water_physics_test_sphere_result;
struct s_water_physics_test_vector_result;

bool water_physics_test_sphere(bool point_test, const real_point3d *point, float radius, s_water_physics_test_sphere_result *result);
bool water_physics_test_vector(const real_point3d *point, const real_vector3d *vector, s_water_physics_test_vector_result *result);
bool water_physics_update(long object_index);

class c_global_material_type;

extern "C" bool water_physics_try_and_get_material(const real_point3d *point, c_global_material_type *material_type, long *material_name);
extern "C" float water_physics_get_depth_at_point(const real_point3d *point);

struct s_water_physics_model;

extern "C" unsigned char water_physics_test_ripple_spawn(const s_water_physics_model *model, long object_index, float depth, float speed,
    long sequence, long scale);

#endif // PHYSICS_WATER_PHYSICS_H
