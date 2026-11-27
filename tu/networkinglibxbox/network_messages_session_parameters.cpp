// @address 0x8c4488 g_network_session_parameter_message_types
// networkinglibxbox/network_messages_session_parameters.obj: from the records 0x3f2ef0 0x3f2fb0 0x3f2fb8 0x3f3050
// @compiler xdk6130
// @flags /TP /O2 /Oc
// @address 0x221228 ?write_bits_internal@c_bitstream@@AAAXPBEJ@Z
// @address 0x221398 ?write_bit_internal@c_bitstream@@AAAX_N@Z
#include "memory/bitstream.h"
#include "networking/session/network_session_parameters.h"
#include "math/real_math_types.h"
#include "networking/messages/network_messages_session_parameters.h"
#include "networking/session/network_session_parameter_type_collection.h"



static __forceinline void network_session_parameters_encode_update_message(c_bitstream *packet, const s_network_message_parameters_update *message)
{
    packet->write_bool("initial-update", message->initial_update);
    packet->write_raw_data("session-id", &message->session_id, 64);
    packet->write_raw_data("cleared-parameters", &message->cleared_parameters, 64);
    packet->write_raw_data("updated-parameters", &message->updated_parameters, 64);
    unsigned char *payload = (unsigned char *)message->payload;
    for (long index = 0; index < 47; ++index)
    {
        if ((message->updated_parameters & (1ui64 << index)) != 0)
        {
            long update_size = g_network_session_parameter_message_types->get_update_size((e_network_session_parameter_type)index);
#ifdef HALO3_64BIT
            long parameter_size = update_size + sizeof(s_network_session_parameter_update);
#else
            long parameter_size = update_size + 4;
#endif
            s_network_session_parameter_update *update = (s_network_session_parameter_update *)payload;
            update->payload = payload + sizeof(*update);
            g_network_session_parameter_message_types->encode_update((e_network_session_parameter_type)index, packet, update->payload, update_size);
            update->payload = 0;
            payload += parameter_size;
        }
    }
}

void c_network_message_parameters_update::encode(c_bitstream *packet, long message_storage_size, const void *message_storage)
{
    const s_network_message_parameters_update *message = (const s_network_message_parameters_update *)message_storage;
    network_session_parameters_encode_update_message(packet, message);
}


bool c_network_message_parameters_update::decode(c_bitstream *packet, long message_storage_size, void *message_storage)
{
    bool result = true;
    s_network_message_parameters_update *message = (s_network_message_parameters_update *)message_storage;
    result = network_session_parameters_decode_update_message(packet, message);
    return result;
}

static __forceinline void network_session_parameters_encode_change_request_message(c_bitstream *packet, const s_network_message_parameters_request *message)
{
    packet->write_raw_data("session-id", &message->session_id, 64);
    packet->write_raw_data("change-request-parameters", &message->parameter_flags, 64);
    unsigned char *payload = (unsigned char *)message->payload;
    for (long index = 0; index < 47; ++index)
    {
        if ((message->parameter_flags & (1ui64 << index)) != 0)
        {
            long request_size = g_network_session_parameter_message_types->get_change_request_size((e_network_session_parameter_type)index);
            g_network_session_parameter_message_types->encode_change_request((e_network_session_parameter_type)index, packet, payload, request_size);
            payload += request_size;
        }
    }
}

void c_network_message_parameters_request::encode(c_bitstream *packet, long message_storage_size, const void *message_storage)
{
    const s_network_message_parameters_request *message = (const s_network_message_parameters_request *)message_storage;
    network_session_parameters_encode_change_request_message(packet, message);
}

bool c_network_message_parameters_request::decode(c_bitstream *packet, long message_storage_size, void *message_storage)
{
    bool result = true;
    s_network_message_parameters_request *message = (s_network_message_parameters_request *)message_storage;
    result = network_session_parameters_decode_change_request_message(packet, message);
    return result;
}
