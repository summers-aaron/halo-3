#ifndef SCENARIO_SCENARIO_KILL_TRIGGER_VOLUMES_STATE_H
#define SCENARIO_SCENARIO_KILL_TRIGGER_VOLUMES_STATE_H

#include "scenario/scenario_kill_trigger_volumes.h"

#include "scenario/scenario_kill_trigger_volumes_state_types.h"

void scenario_kill_trigger_volumes_initialize(void);
void scenario_kill_trigger_volumes_dispose(void);
void scenario_kill_trigger_volumes_initialize_for_new_map(void);
void scenario_kill_trigger_volumes_dispose_from_old_map(void);

typedef t_restricted_allocation_manager<
    1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_kill_trigger_volumes_state_allocator> c_kill_trigger_volumes_allocation_manager;

template <>
void *t_restricted_allocation_manager<
    1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_kill_trigger_volumes_state_allocator>::reserve_memory(
    const char *name_str, const char *type_str, unsigned int size, long alignment_bits);

template <>
void *t_restricted_allocation_manager<
    1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_kill_trigger_volumes_state_allocator>::allocate(
    unsigned int size, const char *name);

#endif // SCENARIO_SCENARIO_KILL_TRIGGER_VOLUMES_STATE_H
