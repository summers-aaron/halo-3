#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGES_CONNECT_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGES_CONNECT_H

class c_bitstream;

struct s_network_message_connect_request
{
    unsigned long identifier;
    unsigned long flags;
};

struct s_network_message_connect_refuse
{
    unsigned long remote_identifier;
    long reason;
};

struct s_network_message_connect_establish
{
    unsigned long remote_identifier;
    unsigned long identifier;
};

struct s_network_message_connect_closed
{
    unsigned long remote_identifier;
    unsigned long identifier;
    long closure_reason;
};

class c_network_message_connect_request
{
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

class c_network_message_connect_refuse
{
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

class c_network_message_connect_establish
{
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

class c_network_message_connect_closed
{
public:
    static void encode(c_bitstream *stream, long message_storage_size, const void *message_storage);
    static bool decode(c_bitstream *stream, long message_storage_size, void *message_storage);
};

typedef char network_message_connect_request_size[(sizeof(s_network_message_connect_request) == 8) ? 1 : -1];
typedef char network_message_connect_refuse_size[(sizeof(s_network_message_connect_refuse) == 8) ? 1 : -1];
typedef char network_message_connect_establish_size[(sizeof(s_network_message_connect_establish) == 8) ? 1 : -1];
typedef char network_message_connect_closed_size[(sizeof(s_network_message_connect_closed) == 12) ? 1 : -1];

// Functions defined in tu/networkinglibxbox/network_messages_connect.cpp that other units call.

class c_network_message_type_collection;

extern "C" void network_message_types_register_connect(c_network_message_type_collection *message_collection);

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGES_CONNECT_H
