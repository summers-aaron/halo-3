#ifndef AI_PROP_DIALOGUE_H
#define AI_PROP_DIALOGUE_H

#include "ai/dialogue_state_types.h"
#include "ai/prop_dialogue_event_types.h"

// Functions defined in tu/ailibxbox/prop_dialogue.cpp.

struct ai_information_data;

long clump_get_first_unit(long clump_index);
bool actor_dialogue_event(long actor_index,short event,long parameter,ai_information_data *information);
bool actor_dialogue_event(long actor_index, short event, long target, e_dialogue_state state, ai_information_data *information);
bool actor_dialogue_prop_event(long actor_index, short event, long prop_ref_index, e_dialogue_state state, ai_information_data *information);
void clump_dialogue_update(long clump_index);
void prop_calculate_dialogue_flags(long clump_index, long prop_index);
bool clump_dialogue_event_allowed( long clump_index, long prop_index, bool flag, e_prop_dialogue_event event, short *dialogue_index,
    e_dialogue_state *dialogue_state);
void prop_dialogue_event_internal(long actor_index,long prop_index,e_prop_dialogue_event event,short dialogue_index,float delay);
bool clump_dialogue_allowed(long clump_index,long prop_index,e_dialogue_state state);

#endif // AI_PROP_DIALOGUE_H
