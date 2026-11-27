#ifndef AI_ACTOR_FIRING_POSITION_QUERIES_H
#define AI_ACTOR_FIRING_POSITION_QUERIES_H

struct firing_position_evaluation_context;
struct firing_position_ref;

extern "C++" void actor_discard_firing_position(long actor_index, firing_position_ref reference, bool temporary);
extern "C" void actor_set_current_firing_position(int actor_index, int fpindex);

extern "C" int firing_position_get_owner(int fp_ref);
extern "C" void firing_position_valid(long actor_index, firing_position_ref reference);
extern "C" bool dynamic_firing_set_find_and_attach(
    long actor_index, firing_position_evaluation_context *context, bool use_primary, bool use_secondary);
extern "C" bool pursuit_area_discarded(long index, short first, short second);

#endif // AI_ACTOR_FIRING_POSITION_QUERIES_H
