// networkinglibxbox/network_message_gateway.obj: from the records 0x126ac8 0x3ee730 0x3ee788 0x3ee8a8
// @flags /TP /O2 /Oc
#include "cseries/crt_xdk.h"
#include "memory/bitstream.h"
// @address 0x3ee730 ?push_position@c_bitstream@@QAAXXZ
#include "networking/transport/transport_address.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/delivery/network_link.h"
#include "networking/messages/network_message_handler.h"
#include "networking/messages/network_message_gateway.h"

// @address 0x126ac8 ?send_all_pending_messages@c_network_message_gateway@@QAAXXZ
void c_network_message_gateway::send_all_pending_messages()
{
    if (m_outgoing_packet_pending)
    {
        m_outgoing_packet.write_bool("has_message", false);
        m_outgoing_packet.finish_writing(0);
        m_link->send_out_of_band(&m_outgoing_packet, &m_outgoing_packet_address, 0);
        m_outgoing_packet_pending = false;
    }
}

// @address 0x3ee788 ?receive_out_of_band_packet@c_network_message_gateway@@UAA_NPBUtransport_address@@PAVc_bitstream@@@Z
bool c_network_message_gateway::receive_out_of_band_packet(const transport_address *address, c_bitstream *packet)
{
    bool success = true;
    packet->begin_reading();
    bool valid = true;
    valid = valid && !packet->error_occurred();
    if (valid)
    {
        char message_data[65536];
        while (packet->read_bool("has_message"))
        {
            long message_size = 0;
            e_network_message_type message_type;
            success = ((c_network_message_type_collection *)m_message_types)->decode_message(packet, &message_type, &message_size, message_data);
            if (!success)
                break;
            if (m_message_handler)
                m_message_handler->handle_out_of_band_message(address, message_type, message_size, message_data);
        }
        if (success)
            packet->finish_reading();
    }
    else
    {
        success = false;
    }
    return success;
}

// @address 0x3ee8a8 ?send_message_directed@c_network_message_gateway@@QAA_NPBUtransport_address@@W4e_network_message_type@@JPBX@Z
bool c_network_message_gateway::send_message_directed(
    const transport_address *outgoing, e_network_message_type message_type, long message_size, const void *message)
{
    bool success = false;
    if (m_outgoing_packet_pending && !transport_address_equivalent(&m_outgoing_packet_address, outgoing, false))
    {
        send_all_pending_messages();
    }
    while (!success)
    {
        bool new_packet = false;
        if (!m_outgoing_packet_pending)
        {
            long packet_size = 1536;
            if (!c_network_link::adjust_packet_size(true, 0, &packet_size))
            {
                break;
            }
            m_outgoing_packet.set_data(m_outgoing_packet_storage, packet_size);
            m_outgoing_packet.begin_writing();
            m_outgoing_packet_address = *outgoing;
            m_outgoing_packet_pending = true;
            new_packet = true;
        }
        m_outgoing_packet.push_position();
        m_outgoing_packet.write_bool("has_message", true);
        s_network_message_type_definition *definition = &m_message_types[message_type];
        m_outgoing_packet.write_integer("type", message_type, 8);
        m_outgoing_packet.write_integer("size", message_size, 16);
        if (!(definition->flags & 2))
        {
            definition->encode(&m_outgoing_packet, message_size, message);
        }
        if (m_outgoing_packet.would_overflow(1))
        {
            if (new_packet)
            {
                m_outgoing_packet.pop_position(true);
                break;
            }
            m_outgoing_packet.pop_position(true);
            flush_outgoing_packet();
        }
        else
        {
            m_outgoing_packet.pop_position(false);
            success = true;
        }
    }
    return success;
}
