#ifndef PHYSICS_HAVOK_VEHICLE_PHYSICS_INTERNALS_H
#define PHYSICS_HAVOK_VEHICLE_PHYSICS_INTERNALS_H
#include "physics/physics_constants_tls.h"

#include <stddef.h>
#include "game/materials.h"
#include "math/real_math_types.h"
#include "physics/havok_vehicle_physics.h"
#include "physics/impact_contact.h"

struct s_havok_sphere_query;
struct s_havok_sphere_test_cache;
struct s_contact_constraint_reference;

struct s_havok_sphere_test
{
    s_havok_sphere_test() {}
    union { unsigned char hit; bool hit_found; };
    float distance;
    c_global_material_type material;
    real_point3d position;
    real_vector3d normal;
    long surface_index;
    long object_index;
    long rigid_body_index;
    unsigned char flag;
    real_vector3d contact_velocity;
};

typedef char havok_sphere_test_size[(sizeof(s_havok_sphere_test) == 0x40) ? 1 : -1];
typedef char havok_sphere_test_normal_offset[(offsetof(s_havok_sphere_test, normal) == 0x18) ? 1 : -1];

void havok_vehicle_physics_remove_impacts_for_friction_point_index(
    const s_havok_vehicle_physics_instance *havok_vehicle_physics_instance, long friction_point_index, bool disconnected_from_physics);
void compute_ground_plane(s_havok_vehicle_physics_instance *instance, const s_havok_sphere_test *test, long friction_point_index);
long havok_vehicle_physics_update_contact_description(s_havok_vehicle_physics_instance *havok_vehicle_physics_instance,
    const c_impact::s_contact_description *contact_description, const real_vector3d *effect_direction, float volume, bool vehicle);
extern "C" float havok_vehicle_physics_get_update_scale(unsigned long object_index);


#endif // PHYSICS_HAVOK_VEHICLE_PHYSICS_INTERNALS_H
