// networkinglibxbox/network_messages_test.obj: from the records 0x3f4038 0x3f4078
// @compiler xdk6130
// @flags /TP /O2 /Oc
#include "memory/bitstream.h"
#include "networking/messages/network_messages_test.h"

// @address 0x3f4038 network_message_test_encode
extern "C" void network_message_test_encode(c_bitstream *packet, long message_storage_size, const void *message_storage)
{
    const s_network_message_test *message = (const s_network_message_test *)message_storage;
    packet->write_bool("in-sequence", message->in_sequence);
    packet->write_integer("id", message->id, 16);
}

bool c_network_message_test::decode(c_bitstream *packet, long message_storage_size, void *message_storage)
{
    s_network_message_test *message = (s_network_message_test *)message_storage;
    message->in_sequence = packet->read_bool("in-sequence");
    message->id = packet->read_integer("id", 16);
    bool success = !packet->error_occurred();
    return success;
}
