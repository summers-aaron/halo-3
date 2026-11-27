#ifndef AI_ACTOR_BEHAVIOR_STATE_H
#define AI_ACTOR_BEHAVIOR_STATE_H

#include "ai/actor_datum.h"
#include "ai/behavior.h"
#include "memory/tls_globals.h"

static __forceinline bool actor_path_executing(long actor_index)
{
    bool executing = false;
    actor_datum *actor = ((actor_datum *)((char *)actor_data->data + ((actor_index) & 0xffff) * sizeof(actor_datum)));
    if (actor->path.valid && actor->path_state == 1)
        executing = true;
    return executing;
}

static __forceinline c_behavior_state *actor_behavior_state_get(actor_datum *actor, short layer)
{
    c_behavior_state *result = 0;
    if (layer >= 0 && layer < 8)
        result = (c_behavior_state *)(actor->behavior_state_data + actor->state_offsets[layer]);
    return result;
}

static __forceinline short actor_behavior_state_offset_get(actor_datum *actor, short layer)
{
    short result = 0;
    if (layer >= 0 && layer < 8)
        result = actor->state_offsets[layer];
    return result;
}

static __forceinline short actor_behavior_index_get(actor_datum *actor, short layer)
{
    short result = NONE;
    if (layer >= 0 && layer < 8)
        result = actor_behavior_state_get(actor, layer)->behavior_index;
    return result;
}

#endif // AI_ACTOR_BEHAVIOR_STATE_H
