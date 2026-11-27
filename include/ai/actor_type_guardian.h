#ifndef AI_ACTOR_TYPE_GUARDIAN_H
#define AI_ACTOR_TYPE_GUARDIAN_H

// Functions defined in tu/ailibxbox/actor_type_guardian.cpp.

bool guardian_is_surging(long actor_index);

class c_behavior_state;

short guardian_isolation_surge_get_impulse(long actor_index, c_behavior_state *state);
short guardian_danger_surge_get_impulse(long actor_index, c_behavior_state *state);
short behavior_surge_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state);


#endif // AI_ACTOR_TYPE_GUARDIAN_H
