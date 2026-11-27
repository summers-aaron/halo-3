#ifndef SIMULATION_SIMULATION_QUEUE_GLOBAL_EVENTS_SERVICES_H
#define SIMULATION_SIMULATION_QUEUE_GLOBAL_EVENTS_SERVICES_H
struct s_variant_multiplayer_object_properties_definition;
extern "C" void simulation_queue_game_simulation_insert(void);
extern "C" void game_engine_sandbox_set_variant_object_properties(
    long player_index, long variant_index, const s_variant_multiplayer_object_properties_definition *properties);

#endif
