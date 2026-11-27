// networkinglibxbox/replication_scheduler.obj: from the records 0x4193b0 0x419420 0x419480 0x419488 0x419808 0x419a80 0x419ad0 0x419b28
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "networking/delivery/network_delivery_types.h"
#include "networking/simulation/simulation_view_telemetry.h"
#include "networking/simulation/simulation_accessors.h"
#include "memory/tls_globals.h"
#include "memory/bitstream.h"
#include "networking/replication/replication_scheduler.h"
#include "networking/network_configuration.h"
#include "networking/network_configuration_types.h"
#include "networking/network_memory_types.h"




class c_client_packet_bitstream : public c_bitstream
{
public:
    c_client_packet_bitstream() { reset(0); }
    ~c_client_packet_bitstream() {}
};

struct s_replication_sort_element
{
    union
    {
        struct
        {
            long key;
            long data0;
        };
        __int64 qword0;
    };
    __int64 qword1;
#ifdef HALO3_64BIT
    __int64 qword2; // the request is 24 bytes: its identifier/event slot is pointer-sized
#endif
};


#include "networking/replication/replication_types.h"

#ifdef HALO3_64BIT
typedef char replication_sort_element_size[(sizeof(s_replication_sort_element) == sizeof(s_replication_outgoing_request)) ? 1 : -1];
#endif

#include "simulation/simulation_world_types.h"
#include "memory/data_types.h"
#include "networking/delivery/network_channel_closure_reason_types.h"
#include "simulation/simulation_queue_types.h"



static __forceinline void replication_scheduler_simulation_gamestate_entity_delete(long entity_index)
{
    ::datum_delete(tls_simulation_gamestate_entity_data(), entity_index);
}


bool c_replication_scheduler::has_data_to_transmit(bool *has_urgent_data) const
{
    bool has_data = false;
    for (long client_index = 0; client_index < 3; ++client_index) {
        c_replication_scheduler_client *client = m_clients[client_index];
        if (client && client->has_data_to_transmit()) {
            has_data = true;
            *has_urgent_data = true;
        }
    }
    return has_data;
}

void c_replication_scheduler::register_client(long client_index, c_replication_scheduler_client *client)
{
    m_clients[client_index] = client;
    m_client_terminator_bits[client_index] = client->terminator_required_bits();
    m_total_terminator_bits += m_client_terminator_bits[client_index];
    client->m_client_index = client_index;
}

long c_replication_scheduler::space_required_bits(long packet_index, long packet_acknowledged_index)
{
    return m_total_terminator_bits;
}

// @address 0x419488 ?write_to_packet@c_replication_scheduler@@UAA_NJPAVc_bitstream@@JJ@Z
bool c_replication_scheduler::write_to_packet(long packet_index, c_bitstream *packet, long reserved_space_bits, long must_leave_space_bits)
{
    s_replication_outgoing_request requests[2000];
    long request_count = 0;
    const void *telemetry = 0;
    bool overflow = false;

    telemetry = m_telemetry_provider->get_telemetry_for_for_view();
    for (long client_index = 0; client_index < 3; ++client_index)
    {
        c_replication_scheduler_client *client = m_clients[client_index];
        if (client != 0 && client->has_data_to_transmit())
        {
            request_count += client->build_outgoing_requests(telemetry, 2000 - request_count, &requests[request_count]);
        }
    }

    replication_scheduler_sort_requests((s_replication_sort_element *)requests, request_count);
    if (request_count > 256)
        request_count = 256;

    if (g_network_configuration.replication_request_limit != 0 && simulation_is_initialized())
    {
        c_simulation_world *world = simulation_get_world();
        if (world != 0 && world->m_world_type != 0 && world->view_count > 0)
        {
            long limit = g_network_configuration.replication_request_limit / world->view_count;
            if (request_count > limit)
                request_count = limit;
        }
    }

    long usable_remaining_bits = packet->get_space_left_in_bits() - must_leave_space_bits;
    struct s_client_packet_storage
    {
        c_client_packet_bitstream client_packets[3];
        unsigned char packet_buffer[4608];
    } client_storage;
    usable_remaining_bits -= m_total_terminator_bits;

    for (long client_index = 0; client_index < 3; ++client_index)
    {
        c_bitstream *client_packet = &client_storage.client_packets[client_index];
        long offset = client_index * 1536;
        unsigned char *buffer = &client_storage.packet_buffer[offset];
        client_packet->set_data(buffer, 1536);
        client_packet->begin_writing();
    }

    if (packet_index != -1)
    {
        for (long request_index = 0; request_index < request_count; ++request_index)
        {
            long client_index = requests[request_index].client_index;
            if (requests[request_index].size_in_bits <= usable_remaining_bits)
            {
                c_replication_scheduler_client *client = m_clients[client_index];
                c_bitstream *client_packet = &client_storage.client_packets[client_index];
                if (client != 0)
                {
                    long bits_used = client_packet->get_space_used_in_bits();
                    long leave_space_bits = client_packet->get_space_left_in_bits() - usable_remaining_bits;
                    client->write_to_packet(
                        requests[request_index].identifier,
                        requests[request_index].request_type,
                        telemetry,
                        packet_index,
                        client_packet,
                        leave_space_bits);
                    bits_used = client_packet->get_space_used_in_bits() - bits_used;
                    usable_remaining_bits -= bits_used;
                    if (usable_remaining_bits <= 0)
                        break;
                }
            }
            else
            {
                overflow = true;
            }
        }
    }

    for (long client_index = 0; client_index < 3; ++client_index)
    {
        c_replication_scheduler_client *client = m_clients[client_index];
        c_bitstream *client_packet = &client_storage.client_packets[client_index];
        if (client != 0)
        {
            client->write_terminator_to_packet(packet_index, client_packet);
        }
    }

    for (long client_index = 0; client_index < 3; ++client_index)
    {
        c_replication_scheduler_client *client = m_clients[client_index];
        c_bitstream *client_packet = &client_storage.client_packets[client_index];
        if (client != 0)
        {
            client_packet->finish_writing(0);
            packet->append(client_packet);
        }
    }

    return overflow;
}

e_network_read_result c_replication_scheduler::read_from_packet(long *packet_index, c_bitstream *packet)
{
    long number_of_requests = 0;
    e_network_read_result result = _network_read_result_success;
    s_replication_incoming_request requests[256];
    long client_indices[4];

    for (long client_index = 0; client_index < 3; ++client_index)
    {
        c_replication_scheduler_client *client = m_clients[client_index];
        client_indices[client_index] = number_of_requests;
        if (client != 0 && result == _network_read_result_success)
        {
            long request_count = 0;
            result = client->read_from_packet(
                *packet_index,
                packet,
                256 - number_of_requests,
                &requests[number_of_requests],
                &request_count);
            number_of_requests += request_count;
        }
    }
    client_indices[3] = number_of_requests;

    if (result == _network_read_result_success && !packet->read_only_for_consistency())
    {
        const long *request_indices = &client_indices[1];
        for (long client_index = 0; client_index < 3; ++client_index, ++request_indices)
        {
            c_replication_scheduler_client *client = m_clients[client_index];
            long first_request = *(request_indices - 1);
            long last_request = *request_indices;
            while (first_request < last_request)
            {
                client->process_incoming_request(&requests[first_request]);
                first_request++;
            }
        }
    }

    for (long request_index = 0; request_index < number_of_requests; ++request_index)
    {
        s_replication_incoming_request *request = &requests[request_index];
        for (long block_index = 0; block_index < request->block_count; ++block_index)
        {
            s_replication_allocation_block *block = &request->blocks[block_index];
            if (block->block_data != 0)
            {
                if (packet->read_only_for_consistency())
                {
                    void *allocation = block->block_data;
                    network_shared_memory_globals.heap->get_allocation_size(allocation);
                    network_shared_memory_globals.heap->deallocate(allocation);
                }
                else
                {
                    switch (block->block_type)
                    {
                    case _replication_entity_allocation_simulation_queue:
                    {
                        void *element = *(void **)block->block_data;
                        simulation_get_world()->simulation_queue_free((s_simulation_queue_element *)element);
                        break;
                    }
                    case _replication_entity_allocation_gamestate_entity:
                    {
                        long entity_index = *(long *)block->block_data;
                        replication_scheduler_simulation_gamestate_entity_delete(entity_index);
                        break;
                    }
                    }
                    void *allocation = block->block_data;
                    network_shared_memory_globals.heap->get_allocation_size(allocation);
                    network_shared_memory_globals.heap->deallocate(allocation);
                }
                block->block_data = 0;
            }
        }
    }

    return result;
}

void c_replication_scheduler::notify_packet_acknowledged(long packet_index)
{
    for (long client_index = 0; client_index < 3; ++client_index) {
        c_replication_scheduler_client *client = m_clients[client_index];
        if (client)
            client->notify_packet_acknowledged(packet_index);
    }
}

void c_replication_scheduler::notify_packet_retired(long packet_index, bool delivered)
{
    for (long client_index = 0; client_index < 3; ++client_index) {
        c_replication_scheduler_client *client = m_clients[client_index];
        if (client)
            client->notify_packet_retired(packet_index, delivered);
    }
}

static __forceinline bool replication_scheduler_sort_compare(const s_replication_sort_element *first, const s_replication_sort_element *second)
{
    return first->key < second->key;
}

static __forceinline void replication_scheduler_sort_swap(s_replication_sort_element *a, s_replication_sort_element *b)
{
    __int64 swap0 = a->qword0;
    __int64 swap1 = a->qword1;
#ifdef HALO3_64BIT
    __int64 swap2 = a->qword2;
#endif
    a->qword0 = b->qword0;
    a->qword1 = b->qword1;
#ifdef HALO3_64BIT
    a->qword2 = b->qword2;
#endif
    b->qword0 = swap0;
    b->qword1 = swap1;
#ifdef HALO3_64BIT
    b->qword2 = swap2;
#endif
}

static __forceinline void replication_scheduler_selection_sort(s_replication_sort_element *lo, s_replication_sort_element *hi)
{
    while (hi > lo)
    {
        s_replication_sort_element *maximum = lo;
        for (s_replication_sort_element *candidate = lo + 1; candidate <= hi; ++candidate)
            if (replication_scheduler_sort_compare(candidate, maximum))
                maximum = candidate;
        replication_scheduler_sort_swap(maximum, hi);
        --hi;
    }
}

// @address 0x419b28 replication_scheduler_sort_requests
extern "C" void replication_scheduler_sort_requests(s_replication_sort_element *base, unsigned long count)
{
    if (count < 2)
        return;
    long stack_index = 0;
    s_replication_sort_element *lo = base;
    s_replication_sort_element *hi = base + count - 1;
    s_replication_sort_element *lower_stack[30], *upper_stack[30];
    for (;;)
    {
        unsigned long size = hi - lo + 1;
        if (size <= 8)
            replication_scheduler_selection_sort(lo, hi);
        else
        {
            s_replication_sort_element *pivot = lo + size / 2;
            replication_scheduler_sort_swap(pivot, lo);
            s_replication_sort_element *low = lo;
            s_replication_sort_element *high = hi + 1;
            for (;;)
            {
                do { ++low; } while (low <= hi && !replication_scheduler_sort_compare(low, lo));
                do { --high; } while (high > lo && !replication_scheduler_sort_compare(lo, high));
                if (high < low)
                    break;
                replication_scheduler_sort_swap(low, high);
            }
            replication_scheduler_sort_swap(lo, high);
            if (high - 1 - lo >= hi - low)
            {
                if (lo + 1 < high) { lower_stack[stack_index] = lo; upper_stack[stack_index] = high - 1; ++stack_index; }
                if (low < hi) { lo = low; continue; }
            }
            else
            {
                if (low < hi) { lower_stack[stack_index] = low; upper_stack[stack_index] = hi; ++stack_index; }
                if (lo + 1 < high) { hi = high - 1; continue; }
            }
        }
        --stack_index;
        if (stack_index < 0)
            break;
        lo = lower_stack[stack_index];
        hi = upper_stack[stack_index];
    }
}
