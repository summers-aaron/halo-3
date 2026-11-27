#ifndef AI_ACTOR_MOVING_HINTS_H
#define AI_ACTOR_MOVING_HINTS_H

// Functions defined in tu/ailibxbox/actor_moving_hints.cpp that other units call.

class c_interaction_target;
class c_hint_ref;

bool actor_hint_reserve(long actor_index, c_hint_ref hint_ref, bool reserve);

extern "C" void ai_handle_interaction_failure(long actor_index, c_interaction_target *target);

union real_vector3d;

extern "C" bool actor_moving_compute_jump_velocity(long actor_index, long object_index, unsigned char flag,
    float velocity, real_vector3d *out);

bool ai_hint_reserved(long actor_index, c_hint_ref hint);

#endif // AI_ACTOR_MOVING_HINTS_H
