#ifndef AI_ACTOR_TYPE_FLOOD_TANK_H
#define AI_ACTOR_TYPE_FLOOD_TANK_H

// Functions defined in tu/ailibxbox/actor_type_flood_tank.cpp.

class c_behavior_state;

short tank_morph_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state);
short pureform_tank_throttle_control_get_impulse(long actor_index, c_behavior_state *state);

#endif // AI_ACTOR_TYPE_FLOOD_TANK_H
