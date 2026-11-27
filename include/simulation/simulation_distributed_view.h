#ifndef SIMULATION_SIMULATION_DISTRIBUTED_VIEW_H
#define SIMULATION_SIMULATION_DISTRIBUTED_VIEW_H
#include <stddef.h>
#include "memory/data_types.h"
#include "networking/replication/replication_scheduler.h"
#include "networking/replication/replication_entity_manager_view.h"
#include "networking/replication/replication_event_manager_view.h"
#include "networking/replication/replication_control_view.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "game/game_results_replicator.h"

// Member order and member
// sizes and trailing results replicator: simulation_view.cpp's distributed view.
struct s_simulation_distributed_view
{
    // Element header of the distributed_views data array, written by datum_new_at_index.
    s_datum_header m_header;
    c_replication_scheduler scheduler;
    c_replication_entity_manager_view entities;
    c_replication_event_manager_view events;
    c_replication_control_view control;
    c_simulation_view_telemetry_provider telemetry_provider;
    long m_telemetry_alignment;
    c_game_results_replicator game_results;
};
typedef char simulation_distributed_view_control_offset[(sizeof(void *) != 4 || offsetof(s_simulation_distributed_view, control) == 0x4118) ? 1 : -1];
typedef char simulation_distributed_view_telemetry_offset[(sizeof(void *) != 4
    || offsetof(s_simulation_distributed_view, telemetry_provider) == 0x5438) ? 1 : -1];
typedef char simulation_distributed_view_results_offset[(sizeof(void *) != 4 || offsetof(s_simulation_distributed_view, game_results) == 0x54e0) ? 1 : -1];
#endif
