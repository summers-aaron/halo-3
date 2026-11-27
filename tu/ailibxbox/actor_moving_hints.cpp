// ailibxbox/actor_moving_hints.obj: from the records 0x5285f0 0x5286c8 0x528780
// @compiler xdk3424
// @flags /TP /O2 /J /Oc
#include "memory/tls_globals.h"
#include "game/game_time_types.h"
#include "game/game_time_inline.h"
#include "ai/recent_obstacle.h"
#include <string.h>
#include "ai/actor_moving_objects.h"
#include "ai/actor_moving_hints.h"
#include "ai/clump_datum.h"

#include "ai/actor_datum.h"



#include "memory/data.h"

#define actor_data tls_actor_data()

static __forceinline actor_datum *ai_hint_reserved_next_actor(long *next_index)
{
    actor_datum *actor = 0;
    if (*next_index != -1)
    {
        actor = datum_get<actor_datum>(actor_data, *next_index);
        *next_index = actor->next_clump_actor_index;
    }
    return actor;
}

// @address 0x5285f0 ?ai_hint_reserved@@YA_NJVc_hint_ref@@@Z
bool ai_hint_reserved(long actor_index, c_hint_ref hint)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool reserved = false;
    if (actor->clump_index != -1)
    {
        clump_datum *clump = datum_get<clump_datum>(clump_data, actor->clump_index);
        long next_index = clump->first_actor_index;
        actor_datum *other;
        while ((other = ai_hint_reserved_next_actor(&next_index)) != 0)
        {
            if (other != actor && other->interaction_target.valid() && other->interaction_target.type == 0)
            {
                bool same_hint = other->interaction_target.hint == hint;
                if (same_hint)
                {
                    reserved = true;
                }
            }
        }
    }
    return reserved;
}

bool actor_hint_reserve(long actor_index, c_hint_ref hint_ref, bool reserve)
{
    actor_datum *actor = datum_get<actor_datum>(actor_data, actor_index);
    bool success = true;
    if (reserve)
    {
        actor->interaction_target.set_hint(hint_ref);
    }
    else if (actor->interaction_target.type == 0 && actor->interaction_target.hint == hint_ref)
    {
        actor->interaction_target.clear();
    }
    else
    {
        success = false;
    }
    return false;
}

static __forceinline float inl_max(float a, float b) { return a > b ? a : b; }
// @address 0x528780 ai_handle_interaction_failure
extern "C" void ai_handle_interaction_failure(long actor_index, c_interaction_target *target)
{
    if (target->type) {
        c_recent_obstacle *obstacle = actor_find_recent_obstacle(actor_index, target->object_index);
        if (obstacle) {
            float ticks = (float)game_seconds_to_ticks_round(5.0f);
            obstacle->flags.set(_recent_obstacle_successful, false);
            obstacle->ticks = (short)inl_max((float)obstacle->ticks, ticks);
        }
    }
}
