#ifndef AI_ACTOR_TYPE_SCARAB_H
#define AI_ACTOR_TYPE_SCARAB_H

// Functions defined in tu/ailibxbox/actor_type_scarab.cpp.

bool giant_find_pursuit_position(long actor_index, long prop_index);

class c_behavior_state;

struct s_behavior_input;
struct s_behavior_arguments;

short behavior_scarab_combat_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state);
void behavior_scarab_combat_deactivate(long actor_index, c_behavior_state *state);
short behavior_scarab_search_get_desire(long actor_index, const s_behavior_input *input, const s_behavior_arguments *arguments);
short behavior_scarab_pause_next(long actor_index, c_behavior_state *state, c_behavior_state *next_state);

void behavior_scarab_search_deactivate(long actor_index, c_behavior_state *);

#endif // AI_ACTOR_TYPE_SCARAB_H
