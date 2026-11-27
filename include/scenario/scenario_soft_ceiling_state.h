#ifndef SCENARIO_SOFT_CEILING_STATE_H
#define SCENARIO_SOFT_CEILING_STATE_H

// TLS slot shared by the soft-ceiling allocator and its update callback.
extern __declspec(thread) unsigned long *g_scenes_played;
// Preserve direct TLS evaluation and compiler scheduling, as for g_restricted_address.
#define TLS_SCENARIO_SOFT_CEILING_STATE g_scenes_played

#endif
