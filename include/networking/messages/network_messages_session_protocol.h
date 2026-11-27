#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGES_SESSION_PROTOCOL_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGES_SESSION_PROTOCOL_H

#include <stddef.h>
#include "networking/transport/transport_security_types.h"

class c_network_message_type_collection;
class c_bitstream;
enum e_network_join_refuse_reason;

struct s_network_message_join_refuse
{
    s_transport_secure_identifier session_id;
    e_network_join_refuse_reason reason;
};

struct s_network_message_peer_establish
{
    s_transport_secure_identifier session_id;
    bool session_exists;
    bool peer_exists;
    bool host_exists;
    unsigned char pad_b;
    s_transport_secure_address host_address;
};

struct s_network_message_session_boot
{
    s_transport_secure_identifier session_id;
    long reason;
};

typedef char network_message_join_refuse_size[(sizeof(s_network_message_join_refuse) == 12) ? 1 : -1];
typedef char network_message_peer_establish_size[(sizeof(s_network_message_peer_establish) == 48) ? 1 : -1];
typedef char network_message_peer_establish_address[(offsetof(s_network_message_peer_establish, host_address) == 12) ? 1 : -1];

struct s_network_message_time_synchronize
{
    s_transport_secure_identifier session_id;
    unsigned long client_timestamp[2];
    unsigned long authority_timestamp[2];
    long synchronization_stage;
};

struct s_network_message_host_handoff
{
    s_transport_secure_identifier session_id;
    s_transport_secure_address host_address;
    short host_peer_index;
};

struct s_network_message_host_reestablish
{
    s_transport_secure_identifier session_id;
    // The new-host description fills the wire fields as one 60-byte block (check_to_send_host_reestablish
    // copies previous_description over them; handle_host_reestablish reads them back as a description).
    union
    {
        s_transport_session_description previous_description;
        struct
        {
            s_transport_secure_identifier new_session_id;
            s_transport_secure_address new_session_host_address;
            s_transport_secure_key new_session_key;
        };
    };
    long secure_key_platform;
};

struct s_network_message_host_decline
{
    s_transport_secure_identifier session_id;
    bool session_exists;
    bool peer_exists;
    bool host_exists;
    unsigned char pad_0b;
    s_transport_secure_address host_address;
};

class c_network_message_session_boot
{
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

class c_network_message_time_synchronize
{
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
    static bool compare(long message_storage_size, void *message_storage, void *comparison_storage);
};

class c_network_message_host_handoff
{
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

class c_network_message_host_reestablish
{
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

class c_network_message_host_decline
{
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

typedef char network_message_session_boot_size[(sizeof(s_network_message_session_boot) == 12) ? 1 : -1];
typedef char network_message_time_synchronize_size[(sizeof(s_network_message_time_synchronize) == 28) ? 1 : -1];
typedef char network_message_host_handoff_size[(sizeof(s_network_message_host_handoff) == 46) ? 1 : -1];
typedef char network_message_host_reestablish_size[(sizeof(s_network_message_host_reestablish) == 72) ? 1 : -1];
typedef char network_message_host_decline_size[(sizeof(s_network_message_host_decline) == 48) ? 1 : -1];
typedef char network_message_host_decline_address_offset[(offsetof(s_network_message_host_decline, host_address) == 12) ? 1 : -1];

void network_message_types_register_session_protocol(c_network_message_type_collection *message_collection);

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGES_SESSION_PROTOCOL_H
