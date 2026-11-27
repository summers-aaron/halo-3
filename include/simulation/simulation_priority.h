#ifndef SIMULATION_SIMULATION_PRIORITY_H
#define SIMULATION_SIMULATION_PRIORITY_H

// Functions defined in tu/networkinglibxbox/simulation_priority.cpp that other units call.

struct s_entity_update_data;
struct s_simulation_entity;

float simulation_calculate_entity_update_priority(const s_simulation_entity *entity,
    const s_entity_update_data *update_data, float *relevance_out, long *period_out);

struct s_simulation_view_telemetry_data;

extern "C" float simulation_calculate_entity_creation_priority(const s_simulation_entity *entity,
    const s_simulation_view_telemetry_data *telemetry, float *relevance_out);

#endif // SIMULATION_SIMULATION_PRIORITY_H
