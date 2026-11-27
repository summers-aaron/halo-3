#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGE_QUEUE_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGE_QUEUE_H

#include "networking/delivery/network_channel.h"

// Functions defined in tu/networkinglibxbox/network_message_queue.cpp that other units call.

class c_network_message_queue;

extern "C" void network_message_queue_send_connect_establish(c_network_message_queue *queue, long a, long b, const void *args);

// The message buffers network_message_queue.cpp owns: one 64 KB payload with its checksum each.
struct s_network_message_queue_buffer
{
    unsigned long crc;
    unsigned char data[0x10000];
};

extern s_network_message_queue_buffer g_network_connect_establish_buffer;
extern s_network_message_queue_buffer g_network_message_send_buffer;
extern s_network_message_queue_buffer g_network_message_receive_buffer;
// Defined in tu/networkinglibxbox/network_channel.cpp (0x11ab038), not network_message_queue.cpp.
extern s_network_message_queue_buffer g_network_message_decode_buffer;

extern "C" c_network_message_queue::s_outgoing_fragment_record *network_message_queue_allocate_outgoing_fragment(c_network_message_queue *queue, long size);

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGE_QUEUE_H
