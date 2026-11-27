#ifndef SIMULATION_GAME_DAMAGE_EVENT_DEFINITIONS_H
#define SIMULATION_GAME_DAMAGE_EVENT_DEFINITIONS_H

#include "simulation/simulation_queue_event_types.h"
#include "networking/simulation/simulation_event_definition.h"
class c_bitstream;
class c_replication_outgoing_event;
struct s_simulation_view_telemetry_data;

class c_simulation_damage_aftermath_event_definition : public c_simulation_event_definition {
public:
    virtual e_simulation_event_type event_type();
    virtual const char *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *);
    virtual long minimum_required_bits(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *);
    virtual float calculate_relevance(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *, float);
    virtual void write_description_to_string(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *, float, long, char *);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *);
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool force);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool force);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long reference_count, const long *references, long payload_size, const void *payload);
};
class c_simulation_damage_section_response_event_definition : public c_simulation_event_definition {
public:
    virtual e_simulation_event_type event_type();
    virtual const char *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *);
    virtual long minimum_required_bits(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *);
    virtual float calculate_relevance(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *, float);
    virtual void write_description_to_string(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *, float, long, char *);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *);
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool force);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool force);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long reference_count, const long *references, long payload_size, const void *payload);
};
class c_simulation_breakable_surface_damage_event_definition : public c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type();
    virtual const char *event_type_name();
    virtual long payload_size();
    virtual long number_of_entity_references();
    virtual bool reference_delays_entity_deletion();
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *);
    virtual long minimum_required_bits(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *);
    virtual float calculate_relevance(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *, float);
    virtual void write_description_to_string(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *, float, long, char *);
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *);
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool force);
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool force);
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload);
    virtual bool apply_game_event(long reference_count, const long *references, long payload_size, const void *payload);
};

#endif
