#ifndef INCLUDE_PHYS_WORLD_H
#define INCLUDE_PHYS_WORLD_H

class phys_bool;
class phys_vec4;
class phys_broadphase;

extern "C" {
extern phys_broadphase *(*g_phys_broadphase_default_factory)(const phys_vec4 &min, const phys_vec4 &max, int sweep_marker_count);
extern phys_bool g_phys_world_force_mt_simulation;
}

// Functions defined in tu/hkdynamics/hkWorld.cpp.

class phys_physics_system;
class phys_island;
class phys_world;

phys_bool enumerate_inactive_entities(const phys_world *world, phys_physics_system *system);
phys_bool enumerate_active_entities(const phys_world *world, phys_physics_system *system);

void enumerate_island_constraints(phys_island *island, phys_physics_system *system);
void enumerate_world_constraints(const phys_world *world, phys_physics_system *system);
void enumerate_world_actions(const phys_world *world, phys_physics_system *system);
void enumerate_world_volumes(const phys_world *world, phys_physics_system *system);

#endif // HAVOK_HK_WORLD_H
