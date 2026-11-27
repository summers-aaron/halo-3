#ifndef NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_CHUNKED_TYPES_H
#define NETWORKING_SESSION_NETWORK_SESSION_PARAMETERS_CHUNKED_TYPES_H

enum { k_network_session_parameter_chunk_size = 128 };

// Wire record for a single chunk of a chunked session parameter update.
struct s_network_session_parameter_chunked_change_request
{
    long chunk_index;
    char chunk_data[k_network_session_parameter_chunk_size];
    long chunk_count;
    long transmit_size;
    unsigned long transmit_checksum;
};

typedef char network_session_parameter_chunked_request_size_check[
    (sizeof(s_network_session_parameter_chunked_change_request) == 0x90) ? 1 : -1];

#endif
