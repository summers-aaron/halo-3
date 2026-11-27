#ifndef AI_ACTOR_SENSES_H
#define AI_ACTOR_SENSES_H

// Definition-side ABI in actors.cpp; actor_queries.h retains long caller indices.
extern "C" bool actor_is_blind(unsigned short unit_index);
extern "C" unsigned char actor_is_deaf(unsigned short player_index);

#endif // AI_ACTOR_SENSES_H
