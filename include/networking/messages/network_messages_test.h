#ifndef NETWORKING_MESSAGES_NETWORK_MESSAGES_TEST_H
#define NETWORKING_MESSAGES_NETWORK_MESSAGES_TEST_H

// Functions defined in tu/networkinglibxbox/network_messages_test.cpp that other units call.

class c_bitstream;

struct s_network_message_test
{
    bool in_sequence;
    long id;
};

class c_network_message_test
{
public:
    static bool decode(c_bitstream *packet, long message_storage_size, void *message_storage);
};

typedef char network_message_test_size_check[(sizeof(s_network_message_test) == 8) ? 1 : -1];

extern "C" void network_message_test_encode(c_bitstream *packet, long message_storage_size, const void *message_storage);

#endif // NETWORKING_MESSAGES_NETWORK_MESSAGES_TEST_H
