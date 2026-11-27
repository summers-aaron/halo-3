#ifndef SCENARIO_SCENARIO_KILL_TRIGGER_VOLUMES_H
#define SCENARIO_SCENARIO_KILL_TRIGGER_VOLUMES_H

// Functions defined in tu/blamlibxbox/scenario_kill_trigger_volumes.cpp that other units call.

bool scenario_kill_trigger_volumes_test_object(long object_index, bool *always_kill);

void __tls_set_g_scenario_kill_trigger_volumes_state_allocator(void *allocator);

#include "cache/restricted_memory_regions.h"

extern t_restricted_allocation_manager<1,
    static_cast<void (*)(void *)>(0),
    static_cast<void (*)(void *)>(0),
    &__tls_set_g_scenario_kill_trigger_volumes_state_allocator> g_scenario_kill_trigger_volumes_state_allocator;

#endif // SCENARIO_SCENARIO_KILL_TRIGGER_VOLUMES_H
