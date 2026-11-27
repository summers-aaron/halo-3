// networkinglibxbox/replication_control_view.obj: from the records 0x41bc70 0x41bc98 0x41be00 0x41bf88 0x41c158
// @compiler xdk6130
#include "cseries/crt_xdk.h"
// @flags /TP /O2 /Oc
#include "networking/simulation/simulation_view_telemetry.h"
#include "memory/bitstream.h"
// @address 0x41be00 ?write_to_packet@c_replication_control_view@@UAAXJJPBXJPAVc_bitstream@@J@Z
// @address 0x41c158 ?process_incoming_request@c_replication_control_view@@UAAXPAUs_replication_incoming_request@@@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
// @address 0x221480 ?write_value_internal@c_bitstream@@QAAXKJ@Z
#include <string.h>
#include "networking/replication/replication_encoding.h"

#include "networking/replication/replication_motion.h"

#include "networking/replication/replication_types.h"



#include "networking/network_time_access.h"

#include "networking/replication/replication_control_view.h"
#include "networking/network_configuration_types.h"
#include "networking/network_configuration.h"
#include "networking/network_memory.h"
#include "networking/network_memory_types.h"

 static __forceinline long replication_control_view_player_motion_minimum_required_bits()
{
    return 53;
}

static __forceinline long replication_control_view_player_prediction_minimum_required_bits()
{
    return 135;
}





bool c_replication_control_view::has_data_to_transmit() {
    return m_motion_available_send != 0 || m_prediction_available_send != 0;
}

void c_replication_control_view::write_to_packet(
    LONG_PTR packet_index, long request_index, const void *request, long request_size, c_bitstream *packet, long space_bits)
{
    unsigned long control_mask = 1 << packet_index;
    packet->push_position();
    packet->write_bool("control-exists", true);
    packet->write_integer("control-index", (long)packet_index, 5);
    packet->write_bool("motion-exists", (m_motion_available_send & control_mask) != 0);
    if (m_motion_available_send & control_mask)
        player_motion_encode(packet, &m_motions[packet_index]);
    packet->write_bool("prediction-exists", (m_prediction_available_send & control_mask) != 0);
    if (m_prediction_available_send & control_mask)
        player_prediction_encode(packet, &m_predictions[packet_index], true);
    if (packet->get_space_left_in_bits() < space_bits)
    {
        packet->pop_position(true);
    }
    else
    {
        packet->pop_position(false);
        m_motion_available_send &= ~control_mask;
        m_prediction_available_send &= ~control_mask;
        m_control_times[packet_index] = network_time_get();
    }
}

void c_replication_control_view::write_terminator_to_packet(long packet_index, c_bitstream *packet)
{
    packet->write_bool("control-exists", false);
}

// @address 0x41bf88 ?read_from_packet@c_replication_control_view@@UAA?AW4e_network_read_result@@JPAVc_bitstream@@JPAUs_replication_incoming_request@@PAJ@Z
e_network_read_result c_replication_control_view::read_from_packet(
    long packet_index, c_bitstream *packet, long maximum_requests, s_replication_incoming_request *requests, long *request_count)
{
    e_network_read_result result = _network_read_result_success;
    long count = 0;
    s_replication_incoming_request *request = requests;
    do {
        if (!packet->read_bool("control-exists"))
            break;
        unsigned long control_index = packet->read_integer("control-index", 5);
        if (control_index > 31 || count >= maximum_requests) {
            result = _network_read_result_error;
            break;
        }
        void *motion = 0;
        void *prediction = 0;
        bool motion_exists = packet->read_bool("motion-exists");
        if (motion_exists) {
            motion = network_shared_memory_globals.heap->allocate(sizeof(s_player_motion));
            if (motion) {
                if (!player_motion_decode(packet, (s_player_motion *)motion))
                    result = _network_read_result_error;
            } else {
                result = _network_read_result_discard;
            }
        }
        bool prediction_exists = packet->read_bool("prediction-exists");
        if (prediction_exists) {
            prediction = network_shared_memory_globals.heap->allocate(sizeof(s_player_prediction));
            if (prediction) {
                if (!player_prediction_decode(packet, (s_player_prediction *)prediction, true))
                    result = _network_read_result_error;
            } else {
                result = _network_read_result_discard;
            }
        }
        if (result == _network_read_result_success) {
            if (motion_exists || prediction_exists) {
                ++count;
                s_replication_incoming_request *current_request = request;
                ++request;
                current_request->entity_index = control_index;
                current_request->request_type = -1;
                current_request->block_count = 2;
                current_request->blocks[0].block_type = 9;
                current_request->blocks[0].block_data = motion;
                current_request->blocks[1].block_type = 10;
                current_request->blocks[1].block_data = prediction;
                current_request->blocks[0].block_size = motion ? sizeof(s_player_motion) : 0;
                current_request->blocks[1].block_size = prediction ? sizeof(s_player_prediction) : 0;
                continue;
            }
            result = _network_read_result_error;
        }
        if (motion)
            ::network_heap_free_block(motion);
        if (prediction)
            ::network_heap_free_block(prediction);
        if (result != _network_read_result_success)
            break;
    } while (result == _network_read_result_success);
    *request_count = count;
    return result;
}

void c_replication_control_view::process_incoming_request(s_replication_incoming_request *request) {
    long entity_index = request->entity_index;
    if (request->blocks[0].block_data != 0) {
        memcpy(&m_motions[entity_index], request->blocks[0].block_data, sizeof(s_player_motion));
        m_pending_motion_mask |= (1 << entity_index);
    }
    if (request->blocks[1].block_data != 0) {
        memcpy(&m_predictions[entity_index], request->blocks[1].block_data, sizeof(s_player_prediction));
        m_pending_prediction_mask |= (1 << entity_index);
    }
}

// @address 0x41bc98 ?build_outgoing_requests@c_replication_control_view@@UAAJPBXJPAUs_replication_outgoing_request@@@Z
long c_replication_control_view::build_outgoing_requests(const void *data, long maximum_requests, s_replication_outgoing_request *requests)
{
    long request_count = 0;
    unsigned long control_mask = 1;
    s_replication_outgoing_request *request = requests;
    for (long control_index = 0; control_index < 32; control_index++, control_mask <<= 1)
    {
        if (request_count >= maximum_requests)
            break;
        long motion_available = m_motion_available_send & control_mask;
        long prediction_available = m_prediction_available_send & control_mask;
        if (motion_available == 0 && prediction_available == 0)
            continue;
        s_replication_outgoing_request *current = request;
        request_count++;
        request++;
        long size_in_bits = 6;
        size_in_bits++;
        if (motion_available != 0)
            size_in_bits += replication_control_view_player_motion_minimum_required_bits();
        size_in_bits++;
        if (prediction_available != 0)
            size_in_bits += replication_control_view_player_prediction_minimum_required_bits();
        current->client_index = m_client_index;
        float priority = g_network_configuration.default_priority;
        if (motion_available != 0)
        {
            float relevance;
            long transmission_period;
            m_telemetry_provider->calculate_control_relevance(control_index, data, &relevance, &transmission_period);
            long elapsed = network_time_since_inline(m_control_times[control_index]);
            if (elapsed >= g_network_configuration.stale_timeout)
            {
                priority = g_network_configuration.stale_priority;
            }
            else if (elapsed >= transmission_period)
            {
                priority = g_network_configuration.stale_scale * relevance + g_network_configuration.stale_offset;
            }
            else
            {
                priority = g_network_configuration.current_scale * relevance + g_network_configuration.current_offset;
            }
        }
        current->identifier = control_index;
        current->request_type = -1;
        current->size_in_bits = size_in_bits;
        current->priority = (long)(priority * 65536.0f);
    }
    return request_count;
}
