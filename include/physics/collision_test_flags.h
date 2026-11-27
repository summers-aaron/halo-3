#ifndef PHYSICS_COLLISION_TEST_FLAGS_H
#define PHYSICS_COLLISION_TEST_FLAGS_H

#include <stddef.h>

// Collision test flags pair a collision mask with an object-type mask.
// March has no s_collision_test_flags or e_collision_test_flag instantiation.
struct s_collision_test_flags {
    unsigned long collision_flags;
    unsigned long object_flags;
};

static inline s_collision_test_flags collision_test_flags_new(unsigned long collision_flags, unsigned long object_flags)
{
    s_collision_test_flags result = { collision_flags, object_flags };
    return result;
}

typedef char collision_flags_size[(sizeof(s_collision_test_flags) == 8) ? 1 : -1];
typedef char collision_flags_object_offset[(offsetof(s_collision_test_flags, object_flags) == 4) ? 1 : -1];

// Named flag sets defined (with their startup initializers) in collisions.cpp and real_math.cpp.
extern s_collision_test_flags k_collision_test_all_objects;
extern s_collision_test_flags k_projectiles_collision_test_flags;
extern s_collision_test_flags k_projectiles_fat_collision_test_flags;
extern s_collision_test_flags k_ai_melee_collision_flags;
extern s_collision_test_flags k_aim_assist_los_collision_flags;
extern s_collision_test_flags k_collision_test_visibility;
extern s_collision_test_flags k_collision_test_visibility_first_person;
extern s_collision_test_flags k_unit_access_collision_flags;
extern s_collision_test_flags k_biped_camera_collision_flags;
extern s_collision_test_flags k_unit_enter_seat_collision_flags;
extern s_collision_test_flags k_biped_footstep_collision_flags;
extern s_collision_test_flags k_placement_test_collision_flags;
extern s_collision_test_flags k_flying_vehicle_collision_flags;
extern s_collision_test_flags k_ai_line_of_sight_default_flags;
extern s_collision_test_flags k_collision_test_none;
extern s_collision_test_flags k_collision_test_structure;
extern s_collision_test_flags k_biped_ground_collision_flags;
extern s_collision_test_flags k_collision_test_structure_and_water;
extern s_collision_test_flags k_collision_test_camera;

#endif // PHYSICS_COLLISION_TEST_FLAGS_H
