#ifndef AI_ACTOR_FIRING_POSITION_SELECTION_H
#define AI_ACTOR_FIRING_POSITION_SELECTION_H

#include "ai/firing_position_ref.h"

struct firing_position;
struct firing_position_evaluation_context;
struct path_state;

firing_position_ref actor_active_select_firing_position(long actor_index, firing_position_evaluation_context *context,
    firing_position *position, long *index, path_state *path, bool *found);
firing_position_ref actor_change_firing_position(long actor_index, firing_position_ref reference,
    firing_position *position, long previous_owner, path_state *path, bool move);

bool area_valid_firing_positions(short zone_index, short area_index, firing_position_ref reference);

#endif // AI_ACTOR_FIRING_POSITION_SELECTION_H
