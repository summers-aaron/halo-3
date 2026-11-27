#ifndef PHYSICS_WATER_PHYSICS_QUERIES_H
#define PHYSICS_WATER_PHYSICS_QUERIES_H

class phys_bvh_bytecode;
class phys_sphere;
class phys_bvh_primitive_info;
template<class T> class phys_array;

extern "C" void phys_bvh_sphere_vm_query_sphere(const phys_bvh_bytecode *code, const phys_sphere &sphere, phys_array<phys_bvh_primitive_info> *hits);
#if defined(_XBOX) && !defined(__clang__)
extern "C" long object_get_water_properties_index(long object_index);
#else
extern "C" bool object_get_water_properties_index(long object_index);
#endif

#include "game/game_time_types.h"
#include "cache/restricted_address.h"
#include "objects/object_header_data.h"
#include "physics/impact.h"
#include "physics/physics_constants_globals.h"
struct s_data_array;
extern "C" {
extern __declspec(thread) s_data_array *g_impact_data;
extern __declspec(thread) s_data_array *g_impact_array_data;
}


extern "C" {
#ifdef HALO3_64BIT
int effect_spawn_impact_effects(long *, const real_point3d *, const real_vector3d *, const real_vector3d *, const long *, const void *);
#else
int effect_spawn_impact_effects(long *, const real_point3d *, const real_vector3d *, const real_vector3d *, const long *, long);
#endif
void impact_initialize_contact_description(c_impact::s_contact_description *, long, bool, long, long, c_global_material_type,
    long, long, c_global_material_type, const real_point3d *, const real_vector3d *, long, const void *);
long impact_new_from_contact(const c_impact::s_contact_description *, long);
}

#endif // PHYSICS_WATER_PHYSICS_QUERIES_H
