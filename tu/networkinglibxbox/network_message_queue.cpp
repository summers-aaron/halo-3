// networkinglibxbox/network_message_queue.obj: from the records 0x4153c0 0x4155c8 0x4156b8 0x415740 0x415938 0x415b50 0x415d00 0x415e18 0x416048 0x416338 0x4163c0
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include <string.h>
#include "networking/network_memory.h"
#include "simulation/simulation.h"
#include "memory/crc.h"
#include "memory/bitstream.h"
#include "networking/messages/network_message_type_collection.h"
#include "networking/messages/network_messages_connect.h"
#include "memory/sliding_window.h"
#include "networking/delivery/network_delivery_types.h"
#include "networking/network_memory_types.h"
#include "networking/messages/network_message_queue.h"
#include "networking/delivery/network_channel.h"
// @address 0x4163c0 ?handle_fragment@c_network_message_queue@@AAA_NJ_NJJPBX@Z
// @address 0x415740 ?write_to_packet@c_network_message_queue@@UAA_NJPAVc_bitstream@@JJ@Z
// @address 0x415938 ?read_from_packet@c_network_message_queue@@UAA?AW4e_network_read_result@@PAJPAVc_bitstream@@@Z
// @address 0x416048 ?retrieve_message@c_network_message_queue@@QAA_NPAW4e_network_message_type@@PAJJPAX@Z
// @address 0x4155c8 ?reset_for_channel_connection@c_network_message_queue@@QAAXXZ
// @address 0x3eec58 ?decode_message_header@c_network_message_type_collection@@QBA_NPAVc_bitstream@@PAW4e_network_message_type@@PAJ@Z
// @address 0x183ef0 ?crc_checksum_buffer@@YAKKPBXJ@Z
// @address 0x2210e8 ?reset@c_bitstream@@QAAXJ@Z
// @address 0x2212e0 ?read_bits_internal@c_bitstream@@AAAXPAEJ@Z
// @address 0x221768 ?read_value_internal@c_bitstream@@AAAKJ@Z
// @address 0x221648 ?read_bit_internal@c_bitstream@@AAA_NXZ
// @address 0x3eebb8 ?decode_message@c_network_message_type_collection@@QBA_NPAVc_bitstream@@PAW4e_network_message_type@@PAJPAX@Z
// @address 0x3eeb48 ?encode_message@c_network_message_type_collection@@QBAXPAVc_bitstream@@W4e_network_message_type@@JPBX@Z
// @address 0xa2788 ?begin_writing@c_bitstream@@QAAXJJ_N@Z
// @address 0xa2dc0 ?finish_writing@c_bitstream@@QAAXPAJ@Z
// @address 0x57ead8 ?allocate@c_rockall_heap@@QAAPAXJ@Z
// @address 0x57eb80 ?deallocate@c_rockall_heap@@QAAXPAX@Z
// @address 0x415e18 ?send_message@c_network_message_queue@@QAAXW4e_network_message_type@@JPBX@Z
#if defined(_XBOX) && !defined(__clang__)
#include "networking/network_memory_storage.h"
// @address 0x80877c network_shared_memory_storage
// match: 0x415b50: byte storage preserves heap reloads across fragment stores.
#define NETWORK_MESSAGE_QUEUE_HEAP (*(c_rockall_heap **)(network_shared_memory_storage + 36))
#else
#define NETWORK_MESSAGE_QUEUE_HEAP (network_shared_memory_globals.heap)
#endif

__forceinline void c_network_message_queue::free_first_incoming_fragment()
{
    s_incoming_fragment_record *fragment = m_first_incoming_fragment;
    if (fragment == m_last_incoming_fragment) {
        m_first_incoming_fragment = 0;
        m_last_incoming_fragment = 0;
    } else {
        m_first_incoming_fragment = fragment->next;
    }
#ifdef HALO3_64BIT
    m_incoming_fragment_bytes -= sizeof(s_incoming_fragment_record);
#else
    m_incoming_fragment_bytes -= 40;
#endif
    c_rockall_heap *heap = NETWORK_MESSAGE_QUEUE_HEAP;
    heap->get_allocation_size(fragment);
    heap = NETWORK_MESSAGE_QUEUE_HEAP;
    heap->deallocate(fragment);
}

static __forceinline void network_message_queue_begin_writing(c_bitstream *packet) {
    long zero = 0;
    packet->m_data_size_alignment = 1;
    packet->m_state = 1;
    packet->m_current.next_data = packet->m_data;
    packet->m_current.current_stream_bit_position = zero;
    packet->m_current.current_memory_bit_position = zero;
    packet->m_position_stack_depth = zero;
    packet->m_error_occurred = (bool)zero;
    packet->m_encoded_value_bits = zero;
    packet->m_current.window = zero;
    packet->m_current.window_bits_used = zero;
    packet->m_number_of_bits_rewound = zero;
    packet->m_number_of_position_resets = zero;
}

// @address 0x11cb040 ?g_network_connect_establish_buffer@@3Us_network_message_queue_buffer@@A
s_network_message_queue_buffer g_network_connect_establish_buffer;
// @address 0x11bb038 ?g_network_message_send_buffer@@3Us_network_message_queue_buffer@@A
s_network_message_queue_buffer g_network_message_send_buffer;
// @address 0x119b030 ?g_network_message_receive_buffer@@3Us_network_message_queue_buffer@@A
s_network_message_queue_buffer g_network_message_receive_buffer;
// @address 0x11ab038 ?g_network_message_decode_buffer@@3Us_network_message_queue_buffer@@A

void c_network_message_queue::reset() {
    if (m_initialized) {
        if (!m_outgoing_window.empty()) {
            for (long sequence = m_outgoing_window.oldest_sequence_number();
                 sequence <= m_outgoing_window.newest_sequence_number(); ++sequence)
                free_first_outgoing_fragment();
        }
        if (!m_incoming_window.empty()) {
            for (long sequence = m_incoming_window.oldest_sequence_number();
                 sequence <= m_incoming_window.newest_sequence_number(); ++sequence) {
                free_first_incoming_fragment();
                m_incoming_window.close(sequence);
            }
        }
    }
    m_connection_lost = false;
    m_connect_establish_pending = false;
    m_message_sent = false;
    m_connected = false;
    m_outgoing_window.reset(true, 0);
    m_incoming_window.reset(true, 0);
    m_outgoing_fragment_bytes = 0;
    m_incoming_fragment_bytes = 0;
}
e_network_read_result c_network_message_queue::read_from_packet(long *packet_sequence_number, c_bitstream *packet) {
    e_network_read_result result = _network_read_result_success;
    long previous_sequence = -1;
    bool reading = true;
    if (*packet_sequence_number != -1) {
        do {
            bool bit0 = packet->read_bool("bit0");
            bool bit1 = packet->read_bool("bit1");
            long sequence = -1;
            if (!bit0 && !bit1) {
                reading = false;
            } else if (!bit0 && bit1) {
                long encoded = packet->read_integer("sequence", 10);
                if (!packet->read_only_for_consistency()) {
                    long delta = encoded - (m_incoming_window.newest_sequence_number() & 1023);
                    if (delta > 512)
                        delta -= 1024;
                    else if (delta <= -512)
                        delta += 1024;
                    sequence = m_incoming_window.newest_sequence_number() + delta;
                    if (sequence - m_incoming_window.newest_sequence_number() > 384 ||
                        m_incoming_window.m_trailing_sequence_number - sequence > 384) {
                        result = _network_read_result_discard;
                        m_connection_lost = true;
                    }
                }
            } else if (bit0 && !bit1) {
                long encoded = packet->read_integer("sequence", 4);
                if (!packet->read_only_for_consistency()) {
                    if (previous_sequence != -1)
                        sequence = previous_sequence + 1 + encoded;
                    else
                        result = _network_read_result_error;
                }
            } else if (bit0 && bit1) {
                if (!packet->read_only_for_consistency()) {
                    if (previous_sequence != -1)
                        sequence = previous_sequence + 1;
                    else
                        result = _network_read_result_error;
                }
            }
            if (reading && result == _network_read_result_success) {
                previous_sequence = sequence;
                bool completes_message = packet->read_bool("completes-message");
                long fragment_size_bits;
                long fragment_size_bytes;
                if (completes_message) {
                    fragment_size_bits = packet->read_integer("size", 8) + 1;
                    fragment_size_bytes = (fragment_size_bits + 7) / 8;
                } else {
                    fragment_size_bits = 256;
                    fragment_size_bytes = 32;
                }
                long fragment_data[8];
                packet->read_raw_data("payload", fragment_data, fragment_size_bits);
                if (packet->m_state != 4 &&
                    !handle_fragment(sequence, completes_message, fragment_size_bits, fragment_size_bytes, fragment_data))
                    result = _network_read_result_discard;
            }
        } while (reading && result == _network_read_result_success);
    }
    return result;
}

void c_network_message_queue::reset_for_channel_connection() {
    if (!m_incoming_window.empty()) {
        for (long sequence = m_incoming_window.oldest_sequence_number();
             sequence <= m_incoming_window.newest_sequence_number(); ++sequence) {
            s_incoming_fragment_record *fragment = m_first_incoming_fragment;
            if (fragment != 0) {
                if (fragment == m_last_incoming_fragment) {
                    m_first_incoming_fragment = 0;
                    m_last_incoming_fragment = 0;
                } else {
                    m_first_incoming_fragment = fragment->next;
                }
#ifdef HALO3_64BIT
                m_incoming_fragment_bytes -= sizeof(s_incoming_fragment_record);
#else
                m_incoming_fragment_bytes -= 40;
#endif
                ::network_heap_free_block((void *)(fragment));
            }
            m_incoming_window.close(sequence);
        }
    }
    m_incoming_window.reset(true, 0);
}
bool c_network_message_queue::has_data_to_transmit(bool *urgent) const {
    bool result = false;
    if (!m_outgoing_window.empty()) {
        const s_outgoing_fragment_record *fragment = get_first_outgoing_fragment();
        for (long sequence = m_outgoing_window.oldest_sequence_number();
             sequence <= m_outgoing_window.newest_sequence_number(); ++sequence) {
            if ((fragment->flags & 4) != 0 && (m_connected || sequence == 1)) {
                result = true;
                break;
            }
            fragment = fragment->next;
        }
    }
    if (result)
        *urgent = true;
    return result;
}
// @address 0x415b50 ?notify_packet_retired@c_network_message_queue@@UAAXJ_N@Z
void c_network_message_queue::notify_packet_retired(long identifier, bool acknowledged) {
    if (!m_outgoing_window.empty()) {
        c_network_message_queue::s_outgoing_fragment_record *fragment = m_first_outgoing_fragment;
        for (long sequence = m_outgoing_window.oldest_sequence_number();
             sequence <= m_outgoing_window.newest_sequence_number(); ++sequence) {
            if (fragment->identifier == identifier) {
                fragment->identifier = -1;
                if (acknowledged)
                    fragment->flags |= 1;
                else
                    fragment->flags |= 4;
            }
            fragment = fragment->next;
        }
    }
    while (!m_outgoing_window.empty()) {
        c_network_message_queue::s_outgoing_fragment_record *fragment = m_first_outgoing_fragment;
        long sequence = m_outgoing_window.oldest_sequence_number();
        if ((fragment->flags & 1) == 0)
            break;
        if (fragment == m_last_outgoing_fragment) {
            m_first_outgoing_fragment = 0;
            m_last_outgoing_fragment = 0;
        } else {
            m_first_outgoing_fragment = fragment->next;
        }
#ifdef HALO3_64BIT
        m_outgoing_fragment_bytes -= fragment->size + offsetof(s_outgoing_fragment_record, data);
#else
        m_outgoing_fragment_bytes -= fragment->size + 12;
#endif
        c_rockall_heap *heap = NETWORK_MESSAGE_QUEUE_HEAP;
        heap->get_allocation_size(fragment);
        heap = NETWORK_MESSAGE_QUEUE_HEAP;
        heap->deallocate(fragment);
        m_outgoing_window.close(sequence);
    }
}
// @address 0x415d00 network_message_queue_send_connect_establish
void network_message_queue_send_connect_establish(c_network_message_queue *queue, long a, long b, const void *args) {
    if (queue->m_connect_establish_pending) {
        c_network_message_queue::s_outgoing_fragment_record *fragment = queue->m_first_outgoing_fragment;
        c_bitstream packet;
        bitstream_construct(&packet, g_network_connect_establish_buffer.data, sizeof(g_network_connect_establish_buffer.data));
        packet.begin_writing(1, 0, false);
        queue->m_message_types->encode_message(&packet, _network_message_type_connect_establish, sizeof(s_network_message_connect_establish), args);
        packet.finish_writing(0);
        long total_bits = packet.get_space_used_in_bits() + 32;
        g_network_connect_establish_buffer.crc = (unsigned long)-1;
        long byte_count = (total_bits + 7) / 8;
        g_network_connect_establish_buffer.crc = crc_checksum_buffer(-1, &g_network_connect_establish_buffer, byte_count);
        fragment->flags = 12;
        fragment->bits = (unsigned short)total_bits;
        fragment->identifier = -1;
        memcpy(fragment->data, &g_network_connect_establish_buffer, byte_count);
        queue->m_connect_establish_pending = false;
    } else {
        queue->m_connection_lost = true;
        queue->m_connect_establish_pending = false;
    }
}
void c_network_message_queue::send_message(e_network_message_type network_message_type, long size, const void *message) {
    c_bitstream packet;
    bitstream_construct(&packet, g_network_message_send_buffer.data, sizeof(g_network_message_send_buffer.data));
    bool message_sent = false;
    network_message_queue_begin_writing(&packet);
    m_message_sent = true;
    m_message_types->encode_message(&packet, network_message_type, size, message);
    long total_bits = packet.get_space_used_in_bits() + 32;
    packet.finish_writing(0);
    g_network_message_send_buffer.crc = (unsigned long)-1;
    long byte_count = (total_bits + 7) / 8;
    g_network_message_send_buffer.crc = crc_checksum_buffer(-1, &g_network_message_send_buffer, byte_count);

    long remaining_bits = total_bits;
    const char *buffer = (const char *)&g_network_message_send_buffer;
    while (!message_sent) {
        if (!connection_lost(0)) {
            if (!m_outgoing_window.full()) {
                bool last;
                long fragment_bits;
                long fragment_bytes;
                if (remaining_bits <= 256) {
                    last = true;
                    fragment_bits = remaining_bits;
                    fragment_bytes = (remaining_bits + 7) / 8;
                } else {
                    last = false;
                    fragment_bits = 256;
                    fragment_bytes = 32;
                }
                s_outgoing_fragment_record *fragment = network_message_queue_allocate_outgoing_fragment(this, fragment_bytes);
                if (fragment != 0) {
                    m_outgoing_window.open(m_outgoing_window.newest_sequence_number() + 1);
                    memset(fragment, 0, sizeof(*fragment));
                    fragment->flags = 4;
                    if (last)
                        fragment->flags |= 8;
                    else
                        fragment->flags &= ~8;
                    fragment->bits = (unsigned short)fragment_bits;
                    fragment->size = (unsigned char)fragment_bytes;
                    fragment->identifier = -1;
                    memcpy(fragment->data, buffer, fragment_bytes);
                    remaining_bits -= 256;
                    buffer += 32;
                    if (last)
                        message_sent = true;
                } else {
                    m_connection_lost = true;
                }
            } else {
                m_connection_lost = true;
            }
            continue;
        } else {
            break;
        }
    }
}
// @address 0x416338 network_message_queue_allocate_outgoing_fragment
c_network_message_queue::s_outgoing_fragment_record *network_message_queue_allocate_outgoing_fragment(c_network_message_queue *queue, long size) {
#ifdef HALO3_64BIT
    long total_size = (long)offsetof(c_network_message_queue::s_outgoing_fragment_record, data) + size;
    c_network_message_queue::s_outgoing_fragment_record *fragment =
        (c_network_message_queue::s_outgoing_fragment_record *)NETWORK_MESSAGE_QUEUE_HEAP->allocate(total_size);
    if (fragment != 0) {
        queue->m_outgoing_fragment_bytes += total_size;
#else
    c_network_message_queue::s_outgoing_fragment_record *fragment =
        (c_network_message_queue::s_outgoing_fragment_record *)NETWORK_MESSAGE_QUEUE_HEAP->allocate(size + 12);
    if (fragment != 0) {
        queue->m_outgoing_fragment_bytes += size + 12;
#endif
        fragment->next = 0;
        if (queue->m_first_outgoing_fragment == 0)
            queue->m_first_outgoing_fragment = fragment;
        else
            queue->m_last_outgoing_fragment->next = fragment;
        queue->m_last_outgoing_fragment = fragment;
    }
    return fragment;
}


static __forceinline c_network_message_queue::s_incoming_fragment_record *network_message_queue_allocate_last_incoming_fragment(
    c_network_message_queue *queue, long size)
{
#ifdef HALO3_64BIT
    long total_size = (long)offsetof(c_network_message_queue::s_incoming_fragment_record, data) + size;
    c_network_message_queue::s_incoming_fragment_record *fragment =
        (c_network_message_queue::s_incoming_fragment_record *)NETWORK_MESSAGE_QUEUE_HEAP->allocate(total_size);
    if (fragment != 0) {
        queue->m_incoming_fragment_bytes += total_size;
#else
    c_network_message_queue::s_incoming_fragment_record *fragment =
        (c_network_message_queue::s_incoming_fragment_record *)NETWORK_MESSAGE_QUEUE_HEAP->allocate(size + 8);
    if (fragment != 0) {
        queue->m_incoming_fragment_bytes += size + 8;
#endif
        fragment->next = 0;
        if (queue->m_first_incoming_fragment == 0)
            queue->m_first_incoming_fragment = fragment;
        else
            queue->m_last_incoming_fragment->next = fragment;
        queue->m_last_incoming_fragment = fragment;
    }
    return fragment;
}

static __forceinline void network_message_queue_free_incoming_block(void *block) {
    c_rockall_heap *heap = NETWORK_MESSAGE_QUEUE_HEAP;
    heap->get_allocation_size(block);
    heap = NETWORK_MESSAGE_QUEUE_HEAP;
    heap->m_rockall_front_end->Delete(block, -1);
    if (block != 0)
        --heap->m_allocation_count;
}

bool c_network_message_queue::retrieve_message(
    e_network_message_type *message_type, long *raw_message_size, long raw_message_buffer_size, void *raw_message_buffer)
{
    bool result = false;
    if (!m_incoming_window.empty()) {
        s_incoming_fragment_record *fragment = m_first_incoming_fragment;
        for (long sequence_number = m_incoming_window.oldest_sequence_number();
             sequence_number <= m_incoming_window.newest_sequence_number(); ++sequence_number) {
            if ((fragment->flags & 1) == 0)
                break;
            if ((fragment->flags & 2) != 0)
                result = true;
            fragment = fragment->next;
        }
    }
    if (result) {
        c_bitstream packet;
        bitstream_construct(&packet, g_network_message_receive_buffer.data, sizeof(g_network_message_receive_buffer.data));
        long message_size = 0;
        long fragment_count = 0;
        bool last_fragment_found = false;
        s_incoming_fragment_record *fragment = m_first_incoming_fragment;
        long sequence_number = m_incoming_window.oldest_sequence_number();
        for (; !last_fragment_found && sequence_number <= m_incoming_window.newest_sequence_number(); ++sequence_number) {
            long fragment_bytes;
            if ((fragment->flags & 2) != 0) {
                last_fragment_found = true;
                fragment_bytes = (fragment->bits + 7) / 8;
            } else {
                fragment_bytes = 32;
            }
            memcpy((unsigned char *)&g_network_message_receive_buffer + message_size, fragment->data, fragment_bytes);
            fragment = fragment->next;
            message_size += fragment_bytes;
            ++fragment_count;
        }
        unsigned long expected_crc = g_network_message_receive_buffer.crc;
        g_network_message_receive_buffer.crc = (unsigned long)-1;
        g_network_message_receive_buffer.crc = crc_checksum_buffer(-1, &g_network_message_receive_buffer, message_size);
        if (expected_crc == g_network_message_receive_buffer.crc) {
            long packet_size = message_size - 4;
            packet.m_data = g_network_message_receive_buffer.data;
            packet.m_data_max = packet.m_data + packet_size;
            packet.m_data_size_bytes = packet_size;
            packet.reset(0);
            packet.begin_reading();
            if (m_message_types->decode_message(&packet, message_type, raw_message_size, &g_network_message_decode_buffer))
                packet.finish_reading();
            else {
                m_connection_lost = true;
                result = false;
            }
        } else {
            m_connection_lost = true;
            result = false;
        }
        long oldest_sequence_number = m_incoming_window.oldest_sequence_number();
        for (long i = 0; i < fragment_count; ++i) {
            s_incoming_fragment_record *first = m_first_incoming_fragment;
            if (first == m_last_incoming_fragment) {
                m_first_incoming_fragment = 0;
                m_last_incoming_fragment = 0;
            } else {
                m_first_incoming_fragment = first->next;
            }
#ifdef HALO3_64BIT
            m_incoming_fragment_bytes -= sizeof(s_incoming_fragment_record);
#else
            m_incoming_fragment_bytes -= 40;
#endif
            network_message_queue_free_incoming_block(first);
            m_incoming_window.close(oldest_sequence_number);
            ++oldest_sequence_number;
        }
    }
    return result;
}

bool c_network_message_queue::handle_fragment(
    long fragment_sequence_number, bool last_fragment, long fragment_size_bits, long fragment_size_bytes, const void *fragment_data)
{
    bool result = false;
    if (fragment_sequence_number == 1 && last_fragment) {
        const s_network_message_queue_buffer *message = (const s_network_message_queue_buffer *)fragment_data;
        c_bitstream packet;
        bitstream_initialize(&packet, message->data, sizeof(message->data));
        packet.begin_reading();
        e_network_message_type message_type;
        long message_size;
        if (m_message_types->decode_message_header(&packet, &message_type, &message_size) && message_type == _network_message_type_connect_establish) {
            reset_for_channel_connection();
            m_connected = true;
        }
    }
    if (fragment_sequence_number <= m_incoming_window.m_trailing_sequence_number) {
        result = true;
    } else {
        s_incoming_fragment_record *fragment_record = 0;
        if (m_incoming_window.newest_sequence_number() < fragment_sequence_number) {
            while (m_incoming_window.newest_sequence_number() < fragment_sequence_number) {
                if (m_incoming_window.full()) {
                    fragment_record = 0;
                    break;
                } else {
                    long sequence_number = m_incoming_window.newest_sequence_number() + 1;
                    m_incoming_window.open(sequence_number);
                    fragment_record = network_message_queue_allocate_last_incoming_fragment(this, 32);
                    if (fragment_record == 0) {
                        break;
                    } else {
                        memset(fragment_record, 0, offsetof(s_incoming_fragment_record, data));
                    }
                }
            }
        } else if (fragment_sequence_number >= m_incoming_window.oldest_sequence_number()) {
            fragment_record = m_first_incoming_fragment;
            for (long sequence_number = m_incoming_window.oldest_sequence_number();
                 sequence_number <= m_incoming_window.newest_sequence_number(); ++sequence_number) {
                if (sequence_number == fragment_sequence_number)
                    break;
                else
                    fragment_record = fragment_record->next;
            }
        }
        if (fragment_record != 0) {
            if ((fragment_record->flags & 1) != 0) {
                result = true;
            } else {
                fragment_record->flags |= 1;
                if (last_fragment)
                    fragment_record->flags |= 2;
                else
                    fragment_record->flags &= ~2;
                fragment_record->bits = (unsigned short)fragment_size_bits;
                fragment_record->size = (unsigned char)fragment_size_bytes;
                memcpy(fragment_record->data, fragment_data, fragment_size_bytes);
                result = true;
            }
        }
    }
    return result;
}

bool c_network_message_queue::write_to_packet(long packet_sequence_number, c_bitstream *packet, long reserved_space_bits, long must_leave_space_bits) {
    bool result = false;
    if (packet_sequence_number != -1) {
        if (!m_outgoing_window.empty()) {
            long previous_sequence = -1;
            s_outgoing_fragment_record *fragment = m_first_outgoing_fragment;
            for (long sequence = m_outgoing_window.oldest_sequence_number();
                 sequence <= m_outgoing_window.newest_sequence_number(); ++sequence) {
                if (!m_connected && sequence != 1)
                    break;
                if ((fragment->flags & 4) != 0) {
                    long sequence_offset = sequence - previous_sequence;
                    packet->push_position();
                    if (previous_sequence != -1 && sequence_offset <= 16) {
                        if (sequence_offset == 1) {
                            packet->write_bool("bit0", true);
                            packet->write_bool("bit1", true);
                        } else {
                            packet->write_bool("bit0", true);
                            packet->write_bool("bit1", false);
                            packet->write_integer("sequence", sequence_offset - 1, 4);
                        }
                    } else {
                        packet->write_bool("bit0", false);
                        packet->write_bool("bit1", true);
                        packet->write_integer("sequence", sequence & 0x3ff, 10);
                    }
                    if ((fragment->flags & 8) != 0) {
                        packet->write_bool("completes-message", true);
                        packet->write_integer("size", fragment->bits - 1, 8);
                    } else {
                        packet->write_bool("completes-message", false);
                    }
                    packet->write_raw_data("payload", fragment->data, fragment->bits);
                    if ((packet->m_data_size_bytes << 3) - packet->m_current.window_bits_used -
                            packet->m_current.current_stream_bit_position <
                        must_leave_space_bits + 2)
                    {
                        if (previous_sequence == -1)
                            m_connection_lost = true;
                        result = true;
                        --packet->m_position_stack_depth;
                        XMemCpy(&packet->m_current, &packet->m_position_stack[packet->m_position_stack_depth], sizeof(packet->m_current));
                        break;
                    } else {
                        previous_sequence = sequence;
                        --packet->m_position_stack_depth;
                        fragment->flags &= ~4;
                        fragment->flags |= 2;
                        fragment->identifier = packet_sequence_number;
                    }
                }
                fragment = fragment->next;
            }
        }
        packet->write_bool("bit0", false);
        packet->write_bool("bit1", false);
    }
    return result;
}
