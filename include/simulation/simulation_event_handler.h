#ifndef SIMULATION_SIMULATION_EVENT_HANDLER_H
#define SIMULATION_SIMULATION_EVENT_HANDLER_H

// Functions defined in tu/networkinglibxbox/simulation_event_handler.cpp.

class c_simulation_event_definition;
class c_replication_outgoing_event;
struct s_simulation_view_telemetry_data;

float simulation_event_compute_relevance(c_simulation_event_definition *definition, const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry);

#endif // SIMULATION_SIMULATION_EVENT_HANDLER_H
