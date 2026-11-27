#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGES_SESSION_PARAMETERS_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGES_SESSION_PARAMETERS_H

#include "networking/transport/transport_security_types.h"
class c_bitstream;

struct s_network_message_parameters_update
{
    s_transport_secure_identifier session_id;
    bool initial_update;
    unsigned __int64 cleared_parameters;
    unsigned __int64 updated_parameters;
    union
    {
        unsigned char payload[6144];
        unsigned char parameters[6144];
    };
};

struct s_network_message_parameters_request
{
    s_transport_secure_identifier session_id;
    union
    {
        unsigned __int64 parameter_flags;
        unsigned __int64 change_request_parameters;
    };
    union
    {
        unsigned char data[6144];
        unsigned char payload[6144];
        unsigned char parameters[6144];
    };
};

class c_network_message_parameters_update
{
public:
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
};

class c_network_message_parameters_request
{
public:
    static void encode(c_bitstream *packet, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
};

extern "C" bool network_session_parameters_decode_change_request_message(c_bitstream *packet, s_network_message_parameters_request *message);

#endif
