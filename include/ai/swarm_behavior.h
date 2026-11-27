#ifndef AI_SWARM_BEHAVIOR_H
#define AI_SWARM_BEHAVIOR_H

// Functions defined in tu/ailibxbox/swarm_behavior.cpp.

bool support_attackers(long actor_index,float threshold);
bool prop_ref_infectable(long prop_index);

class c_behavior_state;
struct s_behavior_input;
struct s_behavior_arguments;

short behavior_support_attack_get_desire(long actor_index, const s_behavior_input *input, const s_behavior_arguments *arguments);
void behavior_support_attack_control(long actor_index, c_behavior_state *state);


#endif // AI_SWARM_BEHAVIOR_H
