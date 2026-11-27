#ifndef AI_ACTOR_TYPES_H
#define AI_ACTOR_TYPES_H

// Functions defined in tu/ailibxbox/actor_types.cpp that other units call.

bool actor_is_leader(long actor_index, long leader_index);

// Data defined in tu/ailibxbox/actor_types.cpp that other units use.

extern const short g_actor_type_default_team[];
extern const short g_actor_type_leader_type[];

#endif // AI_ACTOR_TYPES_H
