#ifndef AI_ACTOR_LOOKING_H
#define AI_ACTOR_LOOKING_H

// Functions defined in tu/ailibxbox/actor_looking.cpp.

void actor_look_affect_movement(long actor_index);
bool aiming_at_target(long actor_index);
void actor_look_update(long actor_index);

class c_ai_direction;
union real_vector3d;
union real_point3d;

bool actor_look_find_random_vector(
    const real_point3d *, const real_vector3d *, bool, float, float, float, float, real_vector3d *);

extern "C" float actor_compute_look_interest(long actor_index, const real_vector3d *reference, long object_index);
extern "C" bool actor_find_idle_look_direction(long actor_index, const real_vector3d *reference,
    bool direction_from_idle, bool aiming, c_ai_direction *direction_out);

// Idle-vector validation.
extern "C" bool actor_looking_valid_idle_vector(
    long, const real_vector3d *, unsigned char, real_vector3d *);
extern "C" unsigned char actor_looking_valid_idle_direction(
    long, const real_vector3d *, unsigned char, c_ai_direction *);

#endif // AI_ACTOR_LOOKING_H
