#ifndef SIMULATION_GAME_INTERFACE_SIMULATION_GAME_OBJECTS_H
#define SIMULATION_GAME_INTERFACE_SIMULATION_GAME_OBJECTS_H

#include "networking/simulation/simulation_event_definition.h"

// Functions defined in tu/networkinglibxbox/simulation_game_objects.cpp that other units call.

long simulation_object_get_replicated_object_from_entity(long entity_index);
long simulation_object_get_authoritative_entity_internal(long object_index, bool validate);
long simulation_object_get_predictive_entity_internal(long object_index, bool validate);

extern "C" const float k_object_interpolation_inverse_fraction;

// Event type 32, "object_refresh" (vtable 0x4732c). The Xbox link folds payload_size, the reference queries,
// event_requirements and the empty payload handlers into identical functions of other units, so only
// event_type_name, calculate_relevance, write_description_to_string and apply_game_event have their own code.
class c_simulation_object_refresh_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual const char *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *event,
        const s_simulation_view_telemetry_data *telemetry);
    virtual long minimum_required_bits(const c_replication_outgoing_event *event,
        const s_simulation_view_telemetry_data *telemetry);
    virtual float calculate_relevance(const c_replication_outgoing_event *event,
        const s_simulation_view_telemetry_data *telemetry, float relevance);
    virtual void write_description_to_string(const c_replication_outgoing_event *event,
        const s_simulation_view_telemetry_data *telemetry, float relevance, long buffer_size, char *buffer);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *event,
        const s_simulation_view_telemetry_data *telemetry);
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool force);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool force);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long reference_count, const long *references, long payload_size, const void *payload);
};

extern "C" c_simulation_object_refresh_event_definition g_simulation_object_refresh_event_definition;

#endif // SIMULATION_GAME_INTERFACE_SIMULATION_GAME_OBJECTS_H
