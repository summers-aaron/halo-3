#ifndef NETWORKING_TRANSPORT_TRANSPORT_ENDPOINT_WINSOCK_H
#define NETWORKING_TRANSPORT_TRANSPORT_ENDPOINT_WINSOCK_H

// Functions defined in tu/networkinglibxbox/transport_endpoint_winsock.cpp that other units call.

#include <stddef.h>
#include "networking/transport/transport_endpoint_types.h"

struct transport_address;
struct transport_endpoint
{
    int socket;
    int type;
    int flags;
};

typedef char transport_endpoint_size[(sizeof(transport_endpoint) == 12) ? 1 : -1];
typedef char transport_endpoint_type_offset[(offsetof(transport_endpoint, type) == 4) ? 1 : -1];
typedef char transport_endpoint_flags_offset[(offsetof(transport_endpoint, flags) == 8) ? 1 : -1];

bool transport_endpoint_set_blocking(transport_endpoint *endpoint, bool blocking);
void transport_endpoint_disconnect(transport_endpoint *endpoint);
short transport_endpoint_read_from(transport_endpoint *endpoint, void *buffer, short length, transport_address *address);
short transport_endpoint_write_to(transport_endpoint *endpoint, const void *buffer, short length, const transport_address *destination);
bool transport_endpoint_writeable(transport_endpoint *endpoint);
bool transport_endpoint_async_connect(transport_endpoint *endpoint, const transport_address *address);
bool transport_endpoint_create_socket(transport_endpoint *endpoint, const transport_address *address);
bool transport_endpoint_get_socket_address(const transport_address *address, long *socket_address_size, unsigned char *const socket_address);
transport_endpoint *transport_endpoint_create(e_transport_type type);
extern "C" bool transport_endpoint_get_transport_address(
    long socket_address_length, const unsigned char *socket_address, transport_address *address);
extern "C" bool transport_endpoint_async_is_connected(transport_endpoint *endpoint, bool *is_connected);
extern "C" int transport_endpoint_read(transport_endpoint *read_endpoint, void *read_buffer, short read_length);

#endif // NETWORKING_TRANSPORT_TRANSPORT_ENDPOINT_WINSOCK_H
