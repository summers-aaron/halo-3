// networkinglibxbox/simulation_event_handler.obj: from the records 0x41c1e8 0x41c300 0x41c468 0x41c560 0x446548
// @compiler xdk6130
// @flags /TP /O2
#include "networking/delivery/network_delivery_types.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include <string.h>
#include <stddef.h>
// @address 0x446e00 ?entity_is_active@c_simulation_view_telemetry_provider@@QBA_NJ@Z
#include <stdlib.h>
#include "networking/network_memory.h"
#include "simulation/simulation_queue_events.h"
#include "simulation/simulation_event_handler.h"
#include "networking/simulation/simulation_entity_database_operations.h"
#include "simulation/entity_definition_types.h"
#include "networking/network_memory_types.h"
#include "networking/simulation/simulation_type_collection.h"
#include "networking/replication/replication_types.h"


#include "networking/replication/replication_event.h"
#include "networking/replication/replication_event_manager.h"
#include "game/game_engine_event_data.h"
#include "memory/bitstream.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "simulation/simulation_world_types.h"
#include "simulation/simulation_event_handler_types.h"
#include "simulation/simulation_event_handler_internals.h"
#include "simulation/simulation_view.h"
#include "simulation/simulation_distributed_view.h"
#include "networking/replication/replication_entity_manager.h"
// @address 0x416990 ?delete_outgoing_event@c_replication_event_manager@@QAAXPAVc_replication_outgoing_event@@@Z
// @address 0x416a20 ?send_event@c_replication_event_manager@@QAAXKPAVc_replication_outgoing_event@@@Z
// @address 0x8087a8 ?g_network_configuration@@3Us_network_configuration@@A

__forceinline void *c_replication_outgoing_event::operator new(size_t size)
{
    return network_shared_memory_globals.heap->allocate((long)size);
}

__forceinline bool c_replication_outgoing_event::set_payload(const void *payload, long payload_size)
{
    void *payload_data = network_shared_memory_globals.heap->allocate(payload_size);
    bool result = false;
    if (payload_data) {
        m_payload_size = payload_size;
        m_payload_data = payload_data;
        memcpy(payload_data, payload, payload_size);
        result = true;
    }
    return result;
}

__forceinline void c_simulation_entity_database::event_reference_remove(long entity_index, const c_replication_outgoing_event *event) {
    long event_type = event->get_type();
    s_simulation_entity *entity = entity_try_and_get(entity_index);
    if (entity)
        --entity->event_reference_count;
}
__forceinline void c_simulation_entity_database::event_reference_add(long entity_index, const c_replication_outgoing_event *event) {
    s_simulation_entity *entity = entity_get(entity_index);
    ++entity->event_reference_count;
}

// @address 0x41c1e8 ?read_incoming_event@c_simulation_event_handler@@QAA?AW4e_network_read_result@@JPBJJPAJPAUs_replication_allocation_block@@PAVc_bitstream@@@Z
e_network_read_result c_simulation_event_handler::read_incoming_event(
    long event_type,
    const long *entity_reference_indices,
    long maximum_block_count,
    long *block_count,
    s_replication_allocation_block *payload_blocks,
    c_bitstream *packet)
{
    void *payload = 0;
    long result = 0;
    if (event_type >= 0 && event_type < m_type_collection->get_event_definition_count()) {
        c_simulation_event_definition *definition = m_type_collection->get_event_definition((e_simulation_event_type)event_type);
        long payload_size = definition->payload_size();
        if (payload_size > 0) {
            payload = network_shared_memory_globals.heap->allocate(payload_size);
            if (payload) {
                if (definition->event_payload_decode(payload_size, payload, packet, true)) {
                    s_replication_allocation_block *block = &payload_blocks[*block_count];
                    ++*block_count;
                    block->block_size = (short)payload_size;
                    block->block_type = 16;
                    block->block_data = payload;
                    payload = 0;
                } else {
                    result = 3;
                }
            } else {
                result = 2;
            }
        }
    } else {
        result = 3;
    }
    if (payload) {
        ::network_heap_free_block((void *)(payload));
        payload = 0;
    }
    return (e_network_read_result)result;
}

// @address 0x41c300 ?calculate_requirements@c_simulation_event_handler@@QAAJPAVc_replication_outgoing_event@@PBXPAM@Z
long c_simulation_event_handler::calculate_requirements(c_replication_outgoing_event *event, const void *telemetry_data, float *priority_out)
{
    int result;
    result = -1;
    c_simulation_event_definition *definition = m_type_collection->get_event_definition((e_simulation_event_type)event->m_type);
    const s_simulation_view_telemetry_data *telemetry = (const s_simulation_view_telemetry_data *)telemetry_data;
    s_simulation_event_requirements requirements;
    requirements = definition->event_requirements(event, telemetry);
    bool has_requirements;
    has_requirements = requirements.can_be_transmitted;
    if (has_requirements != 0) {
        unsigned int reference_count;
        reference_count = requirements.reference_count;
        for (int reference_index = 0; reference_index < (int)reference_count; ++reference_index) {
            c_replication_entity_manager_view *table;
            int index;
            c_simulation_view_telemetry_provider *telemetry_provider;
            bool marked;
            unsigned char entry_flags;
            int entity_reference;
            entity_reference = event->m_entity_references[reference_index];
            if (entity_reference != -1) {
                telemetry_provider = telemetry->provider;
                if ((unsigned char)telemetry_provider->entity_is_active(entity_reference) == 0) {
                    if (requirements.reference_delays_entity_deletion) {
                        index = entity_reference & 1023;
                        marked = false;
                        table = &telemetry_provider->m_view->m_distributed_view->entities;
                        if (index >= 0 && (unsigned int)index < 1024 && table->entries[index].id == entity_reference) {
                            entry_flags = table->definitions->m_entity_data[index].flags;
                            if ((entry_flags & 4) != 0) {
                                marked = _rotl(entry_flags, 31) & 1;
                            }
                        }
                        if ((unsigned int)marked != 0)
                            continue;
                    }
                    has_requirements = false;
                    break;
                }
            }
        }
    }
    if ((unsigned int)has_requirements != 0) {
        result = requirements.minimum_required_bits;
        *priority_out = simulation_event_compute_relevance(definition, event, telemetry);
    } else {
        *priority_out = 0.0f;
    }
    return result;
}

// @address 0x41c468 ?notify_outgoing_event_retired@c_simulation_event_handler@@QAAXPBVc_replication_outgoing_event@@@Z
void c_simulation_event_handler::notify_outgoing_event_retired(const c_replication_outgoing_event *event) {
    c_simulation_event_definition *event_definition = m_type_collection->get_event_definition((e_simulation_event_type)event->get_type());
    if (event_definition->reference_delays_entity_deletion()) {
        for (long reference_index = 0; reference_index < 2; ++reference_index) {
            long entity_index = event->get_entity_reference(reference_index);
            if (entity_index != -1)
                m_entity_database->event_reference_remove(entity_index, event);
        }
    }
}


void c_simulation_event_handler::send_event(e_simulation_event_type event_type, long reference_count, long *references,
    unsigned long machine_mask, long payload_size, const void *payload, long cancel_timer) const
{
    s_simulation_world_view_iterator iterator;
    c_simulation_view *view;
    unsigned long view_mask = 0;
    bool success = true;
    const c_simulation_world *world = m_world;
    world->iterator_begin(&iterator, 0xffffffff);
    while (world->iterator_next(&iterator, &view)) {
        if (view->established()) {
            unsigned long machine_index = view->get_machine_index();
            if (machine_index <= 15 && ((1 << machine_index) & machine_mask))
                view_mask |= 1 << view->get_world_view_index();
        }
    }
    if (view_mask != 0) {
        c_replication_outgoing_event *event = m_event_manager->new_outgoing_event();
        if (event) {
            c_simulation_event_definition *event_definition = m_type_collection->get_event_definition(event_type);
            event->set_type(event_type);
            event->set_cancel_timer(cancel_timer);
            if (reference_count > 0 && references[0] != -1)
                event->set_entity_reference(0, references[0]);
            for (long reference_index = 1; reference_index < reference_count; reference_index++) {
                if (references[reference_index] != -1)
                    event->set_entity_reference(reference_index, references[reference_index]);
            }
            if (payload_size > 0 && !event->set_payload(payload, payload_size))
                success = false;
            if (success) {
                if (event_definition->reference_delays_entity_deletion()) {
                    for (long reference_index = 0; reference_index < reference_count; reference_index++) {
                        long entity_index = references[reference_index];
                        if (entity_index != -1)
                            m_entity_database->event_reference_add(entity_index, event);
                    }
                }
                m_event_manager->send_event(view_mask, event);
            } else {
                m_event_manager->delete_outgoing_event(event);
            }
        }
    }
}

static __forceinline const s_simulation_event_relevance_entry *simulation_event_relevance_entry(long type)
{
    return &g_network_configuration.event_relevance[type];
}

// @address 0x446548 ?simulation_event_compute_relevance@@YAMPAVc_simulation_event_definition@@PBVc_replication_outgoing_event@@PBUs_simulation_view_telemetry_data@@@Z
float simulation_event_compute_relevance(c_simulation_event_definition *definition, const c_replication_outgoing_event *event,
    const s_simulation_view_telemetry_data *telemetry)
{
    const s_simulation_event_relevance_entry *entry = simulation_event_relevance_entry(event->get_type());
    float relevance = entry->relevance;
    if (!(relevance > 0.0f)) {
        float range = entry->maximum - entry->minimum;
        float fraction = 0.0f;
        if (!(range <= 0.0f))
            fraction = definition->calculate_relevance(event, telemetry, entry->input);
        relevance = fraction * range + entry->minimum;
    }
    if (event->get_type() == 14 && (unsigned int)*(const signed char *)event->get_payload_data() != 0) {
        relevance = relevance - 0.02f;
        if (!(relevance > 0.0f))
            relevance = 0.0f;
    }
    return relevance;
}
