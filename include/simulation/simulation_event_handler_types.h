#ifndef SIMULATION_SIMULATION_EVENT_HANDLER_TYPES_H
#define SIMULATION_SIMULATION_EVENT_HANDLER_TYPES_H

#include "networking/delivery/network_delivery_types.h"
#include "networking/replication/replication_types.h"
#include "simulation/simulation_queue_events.h"
#include "networking/simulation/simulation_type_collection.h"

class c_bitstream;
class c_replication_event_manager;
class c_replication_outgoing_event;
class c_simulation_world;
class c_simulation_entity_database;

class c_simulation_event_handler
{
public:
    e_network_read_result read_incoming_event(long event_type, const long *entity_reference_indices,
        long maximum_block_count, long *block_count, s_replication_allocation_block *payload_blocks, c_bitstream *packet);
    void process_incoming_event(long event_type, const long *entity_reference_indices,
        long block_count, s_replication_allocation_block *payload_blocks);
    __forceinline void write_outgoing_event(long event_type, long payload_size, const void *payload, c_bitstream *packet)
    {
        c_simulation_event_definition *definition = m_type_collection->get_event_definition((e_simulation_event_type)event_type);
        definition->event_payload_encode(payload_size, payload, packet, true);
    }
    void notify_outgoing_event_retired(const c_replication_outgoing_event *event);
    // match: 0x41c300: returns the bit count, unlike March's output parameter.
    long calculate_requirements(c_replication_outgoing_event *event, const void *data, float *priority);
    void send_event(e_simulation_event_type event_type, long reference_count, long *references, unsigned long machine_mask,
        long payload_size, const void *payload, long cancel_timer) const;

private:
    friend class c_simulation_world;

    void initialize(c_simulation_world *world, c_replication_event_manager *event_manager,
        c_simulation_type_collection *type_collection, c_simulation_entity_database *entity_database);
    void destroy();

    bool m_initialized;
    c_simulation_world *m_world;
    c_replication_event_manager *m_event_manager;
    c_simulation_type_collection *m_type_collection;
    c_simulation_entity_database *m_entity_database;
};

#if defined(_M_PPCBE) || defined(_XBOX)
typedef char simulation_event_handler_size[(sizeof(void *) != 4 || sizeof(c_simulation_event_handler) == 0x14) ? 1 : -1];
#endif

#endif // SIMULATION_SIMULATION_EVENT_HANDLER_TYPES_H
