#ifndef NETWORKING_SIMULATION_SIMULATION_EVENT_DEFINITION_H
#define NETWORKING_SIMULATION_SIMULATION_EVENT_DEFINITION_H

#include "simulation/simulation_queue_event_types.h"

class c_bitstream;
class c_replication_outgoing_event;
struct s_simulation_view_telemetry_data;

// Packed transmission requirements returned by c_simulation_event_definition::event_requirements;
// the fields restate number_of_entity_references, reference_delays_entity_deletion,
// event_can_be_transmitted and minimum_required_bits in one word (first field is the high bits).
struct s_simulation_event_requirements
{
#if defined(_XBOX) && !defined(__clang__)
    unsigned long reference_count : 7;
    unsigned long reference_delays_entity_deletion : 1;
    unsigned long can_be_transmitted : 1;
    unsigned long minimum_required_bits : 23;
#else
    // The PC compilers allocate bit-fields from the least significant bit: reversed so the word matches the console one.
    unsigned long minimum_required_bits : 23;
    unsigned long can_be_transmitted : 1;
    unsigned long reference_delays_entity_deletion : 1;
    unsigned long reference_count : 7;
#endif
};

// Event definition interface: payload queries, network encode/decode,
// and gameworld prepare/apply handlers.
// event_requirements (slot 9) has no March counterpart: the March build reports
// minimum_required_bits through an output parameter instead.
// Each registered event definition supplies every interface slot on the PC.
#if defined(__clang__)
#define SIMULATION_EVENT_INTERFACE_PURE = 0
#else
#define SIMULATION_EVENT_INTERFACE_PURE
#endif

class c_simulation_event_definition
{
public:
    virtual e_simulation_event_type event_type() SIMULATION_EVENT_INTERFACE_PURE;
    virtual const char *event_type_name() SIMULATION_EVENT_INTERFACE_PURE;
    virtual long payload_size() SIMULATION_EVENT_INTERFACE_PURE;
    virtual long number_of_entity_references() SIMULATION_EVENT_INTERFACE_PURE;
    virtual bool reference_delays_entity_deletion() SIMULATION_EVENT_INTERFACE_PURE;
    virtual bool event_can_be_transmitted(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) SIMULATION_EVENT_INTERFACE_PURE;
    virtual long minimum_required_bits(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *) SIMULATION_EVENT_INTERFACE_PURE;
    virtual float calculate_relevance(const c_replication_outgoing_event *, const s_simulation_view_telemetry_data *, float) SIMULATION_EVENT_INTERFACE_PURE;
    virtual void write_description_to_string(const c_replication_outgoing_event *,
        const s_simulation_view_telemetry_data *, float, long, char *) SIMULATION_EVENT_INTERFACE_PURE;
    virtual s_simulation_event_requirements event_requirements(const c_replication_outgoing_event *,
        const s_simulation_view_telemetry_data *) SIMULATION_EVENT_INTERFACE_PURE;
    virtual void event_payload_encode(long payload_size, const void *payload, c_bitstream *packet, bool force) SIMULATION_EVENT_INTERFACE_PURE;
    virtual bool event_payload_decode(long payload_size, void *payload, c_bitstream *packet, bool force) SIMULATION_EVENT_INTERFACE_PURE;
    virtual void prepare_event_data_for_gameworld(long payload_size, void *payload) SIMULATION_EVENT_INTERFACE_PURE;
    virtual bool apply_game_event(long reference_count, const long *references, long payload_size, const void *payload) SIMULATION_EVENT_INTERFACE_PURE;
};

typedef char simulation_event_requirements_size[(sizeof(s_simulation_event_requirements) == 4) ? 1 : -1];

#endif
