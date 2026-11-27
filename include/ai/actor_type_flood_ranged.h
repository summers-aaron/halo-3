#ifndef AI_ACTOR_TYPE_FLOOD_RANGED_H
#define AI_ACTOR_TYPE_FLOOD_RANGED_H

// Functions defined in tu/ailibxbox/actor_type_flood_ranged.cpp.

class c_behavior_state;

struct s_behavior_input;
struct s_behavior_arguments;

short ranged_turtle_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state);
short pureform_ranged_uncover_get_desire(long actor_index, const s_behavior_input *input, const s_behavior_arguments *arguments);

void ranged_turtle_control(long actor_index, c_behavior_state *behavior_state);

#endif // AI_ACTOR_TYPE_FLOOD_RANGED_H
