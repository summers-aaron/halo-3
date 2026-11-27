#ifndef NETWORKING_SIMULATION_SIMULATION_GAME_ENGINE_EVENT_DEFINITIONS_H
#define NETWORKING_SIMULATION_SIMULATION_GAME_ENGINE_EVENT_DEFINITIONS_H

#include "game/game_engine_event_data.h"
#include "networking/simulation/simulation_view_telemetry.h"
class c_replication_outgoing_event;
class c_bitstream;
#include "networking/simulation/simulation_event_definition.h"

// Game engine event payloads are serialized with encode/decode; apply_game_event
// reports whether the event was applied.
class c_simulation_game_engine_event_definition : public c_simulation_event_definition
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

class c_simulation_game_engine_request_boot_player_event_definition : public c_simulation_event_definition
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
