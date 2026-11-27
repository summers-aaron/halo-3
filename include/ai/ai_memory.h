#ifndef AI_AI_MEMORY_H
#define AI_AI_MEMORY_H

#include "cache/restricted_memory_regions.h"
#include "ai/ai_tls_data.h"
#include "ai/ai_dialogue_globals.h"
#include "memory/data_types.h"
#include "ai/swarm_spawner_globals.h"

// AI runtime allocation managers, data names, and thread-local storage.
void __tls_set_g_ai_globals_allocator(void *);
void __tls_set_g_effect_data_allocator(void *object);
void __tls_set_g_objectives_data_allocator(void *object);
void __tls_set_g_tasks_records_allocator(void *address);
void __tls_set_g_squad_data_allocator(void *address);
void __tls_set_g_squad_group_data_allocator(void *address);
void __tls_set_g_command_script_allocator(void *object);
void __tls_set_g_tracking_data_allocator(void *object);
void __tls_set_g_contrail_system_allocator(void *object);
void __tls_set_g_dynamic_firing_set_allocator(void *object);

extern t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_ai_globals_allocator> g_ai_globals_layout_allocation;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_effect_data_allocator> g_ai_command_script_allocator;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_objectives_data_allocator> g_ai_objectives_allocator;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tasks_records_allocator> g_ai_tasks_layout_allocation;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_squad_data_allocator> g_ai_squads_allocator;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_squad_group_data_allocator> g_ai_squad_group_allocator;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_command_script_allocator> g_ai_props_allocator;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_tracking_data_allocator> g_ai_prop_ref_allocator;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_contrail_system_allocator> g_ai_prop_view_allocator;
extern t_restricted_allocation_manager<1, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_dynamic_firing_set_allocator> g_ai_dynamic_firing_set_allocator;

extern "C" {
extern char g_team_dialogue_state_layout, g_ai_dialogue_records_allocator;
extern char g_swarm_data_allocator, g_swarm_spawner_allocator, g_swarm_nondeterministic_counter_layout;
extern char g_ai_clumps_allocator, g_ai_joint_state_allocator, g_ai_flocks_allocator;
#include "ai/ai_player_tls.h"
}

#endif // AI_AI_MEMORY_H
