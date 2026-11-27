#ifndef SIMULATION_SIMULATION_EVENT_HANDLER_INTERNALS_H
#define SIMULATION_SIMULATION_EVENT_HANDLER_INTERNALS_H

#include "networking/network_memory.h"
#include "simulation/simulation_event_handler_types.h"

__forceinline void c_simulation_event_handler::process_incoming_event(long event_type,
    const long *entity_references, long block_count, s_replication_allocation_block *payload_blocks)
{
    c_simulation_event_definition *definition = m_type_collection->get_event_definition((e_simulation_event_type)event_type);
    long reference_count = definition->number_of_entity_references();

    long payload_size;
    void *payload_data;
    if (definition->payload_size() > 0)
    {
        payload_size = payload_blocks[0].block_size;
        payload_data = payload_blocks[0].block_data;
        payload_blocks[0].block_data = 0;
    }
    else
    {
        payload_size = 0;
        payload_data = 0;
    }

    simulation_queue_event_insert(event_type, reference_count, entity_references, payload_size, payload_data);
    if (payload_data)
        network_heap_free_block(payload_data);
}

#endif // SIMULATION_SIMULATION_EVENT_HANDLER_INTERNALS_H
