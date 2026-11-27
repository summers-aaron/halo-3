#ifndef AI_ACTOR_STIMULUS_H
#define AI_ACTOR_STIMULUS_H

#include "ai/behavior_stimulus_types.h"

// Functions defined in tu/ailibxbox/actor_stimulus.cpp.

union real_vector3d;

void actor_stimulus_allegiance_update(long actor_index, long prop_ref_index);
void actor_stimuli_update(long actor_index);
void actor_stimulus_prop_acknowledged(long actor_index, long prop_ref_index, bool new_prop);
void actor_stimulus_tracking_or_locking(long actor_index, long object_index, short flags);
void actor_stimulus_switched_danger_zone(long actor_index, long object_index);
void actor_stimulus_damage(long actor_index, short, long flags, long prop_index, float, const real_vector3d *vector);
void actor_stimulus_heard_shooting(long actor_index, long prop_ref_index, long shooter_actor_index);
void actor_stimulus_prop_just_killed(long actor_index, long prop_ref_index, long killed_actor_index);
void actor_stimulus_vehicle_eviction(long actor_index, long player_index);
void actor_stimulus_prop_sighted(long actor_index, long prop_ref_index, bool force);
void actor_stimulus_acknowledged_danger_zone(long actor_index, long prop_ref_index, unsigned char acknowledged);
void actor_stimulus_friend_body_sighted(long actor_index, long prop_ref_index);
bool stimulus_register(long actor_index, e_behavior_stimulus stimulus, short time, short priority);

#endif // AI_ACTOR_STIMULUS_H
