#ifndef AI_ACTOR_FIRING_POSITION_HASH_H
#define AI_ACTOR_FIRING_POSITION_HASH_H

#include "ai/firing_position_ref.h"

struct s_hash_table;

extern "C" {
int firing_position_ref_hash_function(firing_position_ref ref);
extern __declspec(thread) s_hash_table *g_actor_firing_position_hash;
}

#endif // AI_ACTOR_FIRING_POSITION_HASH_H
