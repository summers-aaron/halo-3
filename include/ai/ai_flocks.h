#ifndef AI_AI_FLOCKS_H
#define AI_AI_FLOCKS_H

// Functions defined in tu/ailibxbox/ai_flocks.cpp.

#include "math/real_math_types.h"

// Perlin-noise wander state (perlin_state_update): flock boids and swarm creatures (swarm_creature_state::movement_noise).
struct s_perlin_state {
    short period;
    short elapsed;
    real_point3d target;
    real_point3d previous;
};

void perlin_state_update(s_perlin_state *state, float magnitude, float minimum_period, float maximum_period, bool planar, real_point3d *offset);
void flocks_handle_weapon_detonation(real_point3d *point);
void flock_handle_creature_delete(long creature_index);
long flock_get_by_name(long name);
extern "C" void flocks_dispose_from_old_structure_bsp(long flock_index);
extern "C" void flocks_dispose_from_bsp_mask(unsigned long mask);
extern "C" void flock_handle_boid_death(long creature_index);
extern "C" void flocks_dispose_from_old_map();
extern "C" void flock_reset_boid_update_timers(long flock_index);
extern "C" void flocks_update(void);

struct creature_datum;
struct flock_boid_iterator {
    long next_object_index;
    long current_object_index;
};

creature_datum *flock_boid_iterator_next(flock_boid_iterator *iterator);
bool flock_volume_test_point(long flock_index, real_point3d *point, float radius);
extern "C" bool flock_new_from_scenario(long scenario_index);
extern "C" void flock_update_oldest_boid_target(long flock_index);
extern "C" void boid_update_target(long flock_index, long object_index);
void boid_update(long flock_index, long boid_index);

struct s_flock_instance;

void boundary_force_bounding_volume(s_flock_instance *instance, real_point3d *point, float *strength, real_vector3d *force, bool horizontal_only, bool *moved);

#endif // AI_AI_FLOCKS_H
