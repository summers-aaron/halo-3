// OBJ: from the records 0x419d68 0x419f10 0x419f30 0x41a0f0 0x41a340 0x41a350 0x41a4b8 0x41a568 0x41a570 0x41a578
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/delivery/network_delivery_types.h"
#include "cseries/crt_xdk.h"
#include <string.h>
#include "networking/network_memory.h"
#include <new>
#include "memory/bitstream.h"
#include "networking/replication/replication_types.h"
#include "networking/network_memory_types.h"
#include "simulation/simulation_event_handler_internals.h"
#include "networking/replication/replication_event.h"
#include "networking/network_time_since_access.h"
#include "networking/replication/replication_entity.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "networking/replication/replication_event_manager.h"
#include "networking/replication/replication_event_manager_view.h"
// @address 0x419d68 ?reset@c_replication_event_manager_view@@QAAXXZ
// @address 0x419f10 ?has_data_to_transmit@c_replication_event_manager_view@@EAA_NXZ
// @address 0x42d380 ?terminator_required_bits@c_replication_event_manager_view@@EBAJXZ
// @address 0x419f30 ?build_outgoing_requests@c_replication_event_manager_view@@EAAJPBXJPAUs_replication_outgoing_request@@@Z
// @address 0x41a0f0 ?write_to_packet@c_replication_event_manager_view@@EAAXJJPBXJPAVc_bitstream@@J@Z
// @address 0x41a340 ?write_terminator_to_packet@c_replication_event_manager_view@@EAAXJPAVc_bitstream@@@Z
// @address 0x41a350 ?read_from_packet@c_replication_event_manager_view@@EAA?AW4e_network_read_result@@JPAVc_bitstream@@JPAUs_replication_incoming_request@@PAJ@Z
// @address 0x41a568 ?notify_packet_acknowledged@c_replication_event_manager_view@@EAAXJ@Z
// @address 0x41a570 ?notify_packet_retired@c_replication_event_manager_view@@EAAXJ_N@Z
// @address 0x41a578 ?mark_packet_delivered@c_replication_event_manager_view@@AAAXJ_N@Z

// the 64-bit window internals (no name in the March 2007 build)

void c_replication_event_manager_view::reset()
{
    c_packet_record *packet = m_packet_records;
    m_packet_records = 0;
    if (packet)
    {
        do
        {
            c_packet_record *next_packet = packet->m_next;
            c_event_record *event_record = packet->m_events;
            if (event_record)
            {
                do
                {
                    c_event_record *next_event = event_record->m_next;
                    event_record->m_event->m_pending_acknowledgement_mask &= ~(1 << m_view_index);

                    int details;
                    if (!network_shared_memory_globals.heap->m_rockall_front_end->Details(event_record, &details))
                    {
                        details = -1;
                    }
                    c_rockall_heap *heap = network_shared_memory_globals.heap;
                    heap->m_rockall_front_end->Delete(event_record, -1);
                    heap->m_allocation_count--;

                    event_record = next_event;
                } while (event_record);
            }

            int details;
            if (!network_shared_memory_globals.heap->m_rockall_front_end->Details(packet, &details))
            {
                details = -1;
            }
            c_rockall_heap *heap = network_shared_memory_globals.heap;
            heap->m_rockall_front_end->Delete(packet, -1);
            heap->m_allocation_count--;

            packet = next_packet;
        } while (packet);
    }

    c_replication_outgoing_event *event = m_manager->m_outgoing_events;
    if (event)
    {
        do
        {
            c_replication_outgoing_event *next_event = event->m_next;
            unsigned long bit = 1 << m_view_index;
            if ((event->m_outgoing_mask & bit) != 0)
            {
                unsigned long mask = event->m_outgoing_mask & ~bit;
                c_replication_event_manager *manager = m_manager;
                event->m_outgoing_mask = mask;
                if (mask == 0)
                {
                    manager->m_client->c_simulation_event_handler::notify_outgoing_event_retired(event);
                    manager->delete_outgoing_event(event);
                }
            }
            event = next_event;
        } while (event);
    }

    m_events_pending = 0;
    m_events_delivered = 0;
    m_events_in_transit = 0;
}

bool c_replication_event_manager_view::has_data_to_transmit()
{
    return m_events_pending > 0;
}

long c_replication_event_manager_view::terminator_required_bits() const
{
    return 1;
}

// match: 0x419f30: __restrict hoists the requests word load above the fmuls; without it c2 reloads 4(r29) after the multiply
long c_replication_event_manager_view::build_outgoing_requests(const void *data, long maximum_requests, s_replication_outgoing_request *__restrict requests)
{
    long request_count = 0;
    long view_bit = 1 << m_view_index;
    c_replication_outgoing_event *event = m_manager->m_outgoing_events;
    if (event != 0)
    {
        s_replication_outgoing_request *request = requests;
        do
        {
            c_replication_outgoing_event *next_event = event->m_next;
            if (request_count >= maximum_requests)
                break;
            if ((event->m_outgoing_mask & view_bit) != 0 && (event->m_pending_acknowledgement_mask & view_bit) == 0)
            {
                if (event->m_cancel_timer != -1 && network_time_since(event->m_creation_time) >= event->m_cancel_timer)
                {
                    m_events_pending = m_events_pending - 1;
                    m_manager->retire_outgoing_event(m_view_index, event, false);
                }
                else
                {
                    float priority;
                    c_replication_event_manager *manager = m_manager;
                    long size_in_bits = manager->m_client->calculate_requirements(event, data, &priority);
                    if (size_in_bits != -1)
                    {
                        long total_bits = size_in_bits + 7;
                        s_replication_outgoing_request *out_request = request;
                        request_count = request_count + 1;
                        request = request + 1;
                        for (long index = 0; index < 2; index++)
                        {
                            total_bits = total_bits + 1;
                            if (event->get_entity_reference(index) != -1)
                                total_bits = total_bits + 14;
                        }
                        out_request->event = event;
                        out_request->request_type = -1;
                        out_request->size_in_bits = total_bits;
                        out_request->client_index = m_client_index;
                        out_request->priority = (long)(priority * 65536.0f);
                    }
                    else
                    {
                        if (network_time_since(event->m_creation_time) >=
                                g_network_configuration.event_timeout &&
                            event->m_cancel_timer == -1)
                            event->m_cancel_timer = 0;
                    }
                }
            }
            event = next_event;
        } while (event != 0);
    }
    return request_count;
}

void c_replication_event_manager_view::write_to_packet(
    LONG_PTR event_index, long priority, const void *data, long sequence_number, c_bitstream *stream, long reserved_bits)
{
    c_packet_record *packet_record = 0;
    if (m_packet_records && m_packet_records->m_sequence_number == sequence_number)
        packet_record = m_packet_records;
    if (!m_client_disabled && !packet_record)
    {
        void *storage = network_shared_memory_globals.heap->allocate(sizeof(c_packet_record));
        packet_record = storage ? new (storage) c_packet_record : 0;
        if (packet_record)
        {
            packet_record->m_next = m_packet_records;
            m_packet_records = packet_record;
            ++m_packet_count;
            packet_record->m_events = 0;
            packet_record->m_sequence_number = sequence_number;
        }
        else
        {
            m_client_disabled = true;
        }
    }
    if (!m_client_disabled && packet_record)
    {
        c_replication_outgoing_event *event = (c_replication_outgoing_event *)event_index;
        c_event_record *event_record = (c_event_record *)network_shared_memory_globals.heap->allocate(sizeof(c_event_record));
        if (event_record)
        {
            event_record->m_event = 0;
            event_record->m_next = 0;
            stream->push_position();
            stream->write_bool("event-exists", true);
            stream->write_integer("event-type", event->m_type, 6);
            for (long index = 0; index < 2; ++index)
            {
                long entity_index = event->get_entity_reference(index);
                stream->write_bool("entity-reference-exists", entity_index != -1);
                if (entity_index != -1)
                {
                    stream->write_value_internal((unsigned long)entity_index & 0x3ff, 10);
                    stream->write_value_internal((unsigned long)entity_index >> 28, 4);
                }
            }
            if (event->m_payload_size > 0)
            {
                m_manager->m_client->c_simulation_event_handler::write_outgoing_event(
                    event->m_type, event->m_payload_size, event->m_payload_data, stream);
            }
            if (stream->get_space_left_in_bits() >= reserved_bits)
            {
                stream->pop_position(false);
                event_record->m_next = packet_record->m_events;
                packet_record->m_events = event_record;
                event_record->m_event = event;
                --m_events_pending;
                ++m_events_in_transit;
                event->m_pending_acknowledgement_mask |= 1UL << m_view_index;
            }
            else
            {
                stream->pop_position(true);
                ::network_heap_free_block((void *)(event_record));
            }
        }
        else
        {
            m_client_disabled = true;
        }
    }
}

void c_replication_event_manager_view::write_terminator_to_packet(long packet_index, c_bitstream *stream)
{
    stream->write_bool("event-exists", false);
}

e_network_read_result c_replication_event_manager_view::read_from_packet(
    long packet_index,
    c_bitstream *packet,
    long maximum_number_of_requests,
    s_replication_incoming_request *requests,
    long *number_of_requests)
{
    long result = 0;
    long count = 0;

    if (m_client_disabled)
    {
        result = 2;
    }

    while (result == 0 && packet->read_bool("event-exists"))
    {
        if (count >= maximum_number_of_requests)
        {
            result = 3;
            break;
        }

        long event_type = packet->read_integer("event-type", 6);
        if (event_type >= 0 && event_type < 64)
        {
            long block_count = 0;
            long entity_reference_indices[2];
            s_replication_allocation_block blocks[1];

            for (long index = 0; index < 2; index++)
            {
                if (packet->read_bool("entity-reference-exists"))
                {
                    replication_entity_index_decode(packet, &entity_reference_indices[index]);
                }
                else
                {
                    entity_reference_indices[index] = -1;
                }
            }

            result = m_manager->m_client->c_simulation_event_handler::read_incoming_event(
                event_type, entity_reference_indices, 1, &block_count, blocks, packet);
            if (result == 0)
            {
                s_replication_incoming_request *request = &requests[count];
                count++;
                request->entity_index = -1;
                request->request_type = event_type;
                request->block_count = block_count;
                if (block_count > 0)
                {
                    memcpy(request->blocks, blocks, block_count * sizeof(s_replication_allocation_block));
                }
                memcpy(request->entity_references, entity_reference_indices, sizeof(entity_reference_indices));
            }
        }
        else
        {
            result = 3;
        }
    }

    *number_of_requests = count;
    return (e_network_read_result)result;
}

__forceinline void c_replication_event_manager::process_incoming_event(long event_type,
    const long *entity_references, long block_count, s_replication_allocation_block *payload_blocks)
{
    m_client->c_simulation_event_handler::process_incoming_event(event_type, entity_references, block_count, payload_blocks);
}

// @address 0x41a4b8 ?process_incoming_request@c_replication_event_manager_view@@EAAXPAUs_replication_incoming_request@@@Z
void c_replication_event_manager_view::process_incoming_request(s_replication_incoming_request *request)
{
    const long *entity_references = request->entity_references;
    m_manager->process_incoming_event(request->request_type, entity_references, request->block_count, request->blocks);
}

void c_replication_event_manager_view::notify_packet_acknowledged(long packet_index)
{
    mark_packet_delivered(packet_index, true);
}

void c_replication_event_manager_view::notify_packet_retired(long packet_index, bool delivered)
{
    mark_packet_delivered(packet_index, delivered);
}

void c_replication_event_manager_view::mark_packet_delivered(long packet_index, bool delivered)
{
    c_packet_record **packet_ptr = &m_packet_records;
    c_packet_record *packet = *packet_ptr;
    while (packet && packet->m_sequence_number != packet_index)
    {
        packet_ptr = &packet->m_next;
        packet = *packet_ptr;
    }

    if (packet)
    {

    c_event_record *event_record = packet->m_events;
    if (event_record)
    {
        do
        {
            c_replication_outgoing_event *event = event_record->m_event;
            c_event_record *next_event_record = event_record->m_next;

            event->m_pending_acknowledgement_mask &= ~(1 << m_view_index);
            m_events_in_transit--;

            if (!delivered)
            {
                m_events_pending++;
            }
            else
            {
                c_replication_event_manager *manager = m_manager;
                m_events_delivered++;
                unsigned long outgoing_mask = event->m_outgoing_mask & ~(1 << m_view_index);
                event->m_outgoing_mask = outgoing_mask;
                if (outgoing_mask == 0)
                {
                    manager->m_client->c_simulation_event_handler::notify_outgoing_event_retired(event);
                    manager->delete_outgoing_event(event);
                }
            }

            ::network_heap_free_block((void *)(event_record));
            event_record = next_event_record;
        } while (event_record);
    }

        *packet_ptr = packet->m_next;
        m_packet_count--;
        ::network_heap_free_block((void *)(packet));
    }
}

