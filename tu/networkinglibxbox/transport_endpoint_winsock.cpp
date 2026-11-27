// networkinglibxbox/transport_endpoint_winsock.obj: from the records 0x8f1c0 0x441e18 0x441ea8 0x441f68 0x441fe8 0x4420b8 0x442190 0x442260 0x442360 0x442520 0x4425f8 0x4426a8
// @flags /TP /O2 /J /Oc
// @address 0x441f68 ?transport_endpoint_disconnect@@YAXPAUtransport_endpoint@@@Z
// @address 0x442190 ?transport_endpoint_writeable@@YA_NPAUtransport_endpoint@@@Z
// @address 0x441e18 ?transport_endpoint_create@@YAPAUtransport_endpoint@@W4e_transport_type@@@Z
// @address 0x441ea8 ?transport_endpoint_set_blocking@@YA_NPAUtransport_endpoint@@_N@Z
// @address 0x442520 ?transport_endpoint_create_socket@@YA_NPAUtransport_endpoint@@PBUtransport_address@@@Z
// @address 0x4425f8 ?transport_endpoint_get_socket_address@@YA_NPBUtransport_address@@PAJQAE@Z
// @address 0x441fe8 transport_endpoint_read
// @address 0x442360 transport_endpoint_async_is_connected
// @address 0x58e5c0 memset
#define NOD3D
#include <xtl.h>
#include "cseries/crt_xdk.h"
#include "networking/transport/transport_globals.h"
#include "cseries/cseries_types.h"
#include <string.h>
#include "networking/transport/transport_endpoint_winsock.h"
#include "networking/transport/transport_address.h"
#include "networking/transport/transport_available.h"

struct transport_in6_addr
{
    unsigned short word[8];
};

struct transport_sockaddr_in6
{
    unsigned short sin6_family;
    unsigned short sin6_port;
    unsigned long sin6_flowinfo;
    transport_in6_addr sin6_addr;
    unsigned long sin6_scope_id;
};

namespace {
static __forceinline void transport_socket_set_add(unsigned int socket, fd_set *set)
{
 unsigned int index;
 for (index = 0; index < set->fd_count; ++index) {
  if (set->fd_array[index] == socket) break;
 }
 if (index == set->fd_count && set->fd_count < FD_SETSIZE) {
  set->fd_array[index] = socket;
  ++set->fd_count;
 }
}
}

transport_endpoint *transport_endpoint_create(e_transport_type type)
{
  transport_endpoint *result = 0;
  if (transport_available()) {
    result = (transport_endpoint *)RtlAllocateHeap(GetProcessHeap(), 0, sizeof(transport_endpoint));
    if (result != 0) {
      result->type = type;
      result->socket = -1;
      result->flags = 0;
    }
  }
  return result;
}

bool transport_endpoint_set_blocking(transport_endpoint *endpoint, bool blocking)
{
    unsigned long arg;
    bool result = true;
    if (transport_available()) {
        int sock = endpoint->socket;
        if (sock != -1) {
            if ((unsigned char)(((unsigned)endpoint->flags >> 4) & 1)) {
                arg = 1;
                if (ioctlsocket(sock, FIONBIO, &arg) == 0) {
                    endpoint->flags &= ~0x10;
                } else {
                    WSAGetLastError();
                    result = false;
                }
            }
        } else {
            result = false;
        }
    }
    return result;
}


void transport_endpoint_disconnect(transport_endpoint *endpoint)
{
 if (endpoint->socket != -1) {
  if ((endpoint->flags & 1) != 0) {
   if (shutdown(endpoint->socket, SD_BOTH) != 0) {
    int error = WSAGetLastError();
   }
  }
  if (closesocket(endpoint->socket) != 0) {
   int error = WSAGetLastError();
  }
 }
 endpoint->socket = -1;
 endpoint->flags = 0;
}

extern "C" int transport_endpoint_read(transport_endpoint *endpoint, void *buffer, short length)
{
    int result = 0;
    if (transport_available() && (endpoint->flags & 1) != 0) {
        result = (short)recv(endpoint->socket, (char *)buffer, length, 0);
        short bytes_read = (short)result;
        if (bytes_read == -1)
            return (WSAGetLastError() == WSAEWOULDBLOCK) - 3;
        if (bytes_read == 0)
            endpoint->flags &= ~0x25;
    }
    return result;
}

short transport_endpoint_read_from(transport_endpoint *endpoint, void *buffer, short length, transport_address *address)
{
    unsigned char socket_address[28];
    int socket_address_length;
    short result;
    socket_address[0] = 0;
    memset(&socket_address[1], 0, sizeof(socket_address) - 1);
    result = -3;
    socket_address_length = sizeof(socket_address);
    if (transport_available()) {
        short received = (short)recvfrom(endpoint->socket, (char *)buffer, 0x1000, 0, (sockaddr *)socket_address, &socket_address_length);
        if (received == -1) {
            result = (WSAGetLastError() == WSAEWOULDBLOCK) - 3;
        } else {
            transport_endpoint_get_transport_address(socket_address_length, socket_address, address);
            result = received;
        }
    }
    return result;
}

short transport_endpoint_write_to(transport_endpoint *endpoint, const void *buffer, short length, const transport_address *destination)
{
    short bytes_written=-3;
    unsigned char socket_address[28];
    long socket_address_size;
    if(transport_available() && transport_endpoint_get_socket_address(destination, &socket_address_size, socket_address)) {
        bytes_written=(short)sendto(endpoint->socket, (const char *)buffer, length, 0, (const sockaddr *)socket_address, socket_address_size);
        if(bytes_written == -1) {
            long error=WSAGetLastError();
            if(error == WSAEWOULDBLOCK)
                bytes_written=-2;
            else if(error == WSAEHOSTUNREACH)
                bytes_written=-1;
            else
                bytes_written=-3;
        }
    }
    return bytes_written;
}

bool transport_endpoint_writeable(transport_endpoint *endpoint)
{
 bool result = false;
 if (transport_available() && endpoint->socket != -1) {
  timeval timeout;
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  fd_set write_set;
  write_set.fd_count = 0;
  transport_socket_set_add(endpoint->socket, &write_set);
  result = select(1, 0, &write_set, 0, &timeout) == 1
   && __WSAFDIsSet(endpoint->socket, &write_set) != 0;
 }
 return result;
}

bool transport_endpoint_async_connect(transport_endpoint *endpoint, const transport_address *address)
{
    bool result=false;
    unsigned char socket_address[28];
    long socket_address_size;
    if(transport_available() && transport_endpoint_get_socket_address(address, &socket_address_size, socket_address)
        && transport_endpoint_create_socket(endpoint, address)) {
        if(transport_endpoint_set_blocking(endpoint, false)) {
            if(connect(endpoint->socket, (const sockaddr *)socket_address, socket_address_size) == 0) {
                endpoint->flags |= 1;
                endpoint->flags |= 32;
                result=true;
            } else {
                long error=WSAGetLastError();
                if(error == WSAEWOULDBLOCK)
                    result=true;
            }
        } else {
            long error=WSAGetLastError();
        }
    }
    return result;
}

extern "C" bool transport_endpoint_async_is_connected(transport_endpoint *endpoint, bool *is_connected)
{
    bool result = false;
    timeval timeout;
    timeout.tv_sec = 0;
    long *microseconds = &timeout.tv_usec;
    memset(microseconds, 0, sizeof(*microseconds));
    fd_set write_set;
    write_set.fd_count = 0;
    memset(write_set.fd_array, 0, sizeof(write_set.fd_array));
    fd_set except_set;
    except_set.fd_count = 0;
    memset(except_set.fd_array, 0, sizeof(except_set.fd_array));
    *is_connected = false;
    if (transport_available()) {
        if (endpoint->socket != -1) {
            unsigned int index;
            for (index = 0; index < write_set.fd_count; index++) {
                if (write_set.fd_array[index] == (unsigned int)endpoint->socket)
                    break;
            }
            if (index == write_set.fd_count && write_set.fd_count < FD_SETSIZE) {
                write_set.fd_array[index] = endpoint->socket;
                write_set.fd_count++;
            }
            unsigned int except_index;
            for (except_index = 0; except_index < except_set.fd_count; except_index++) {
                if (except_set.fd_array[except_index] == (unsigned int)endpoint->socket)
                    break;
            }
            if (except_index == except_set.fd_count && except_set.fd_count < FD_SETSIZE) {
                except_set.fd_array[except_index] = endpoint->socket;
                except_set.fd_count++;
            }
            if (select(0, 0, &write_set, &except_set, &timeout) != SOCKET_ERROR) {
                if (!__WSAFDIsSet(endpoint->socket, &except_set)) {
                    result = true;
                    if (__WSAFDIsSet(endpoint->socket, &write_set)) {
                        unsigned long flags = endpoint->flags;
                        *is_connected = true;
                        endpoint->flags = flags | 33;
                    }
                }
            } else {
                WSAGetLastError();
            }
        }
    }
    return result;
}


bool transport_endpoint_create_socket(transport_endpoint *endpoint, const transport_address *address)
{
    bool success = false;

    if (endpoint->socket == -1) {
        int type = 0;
        int protocol = 0;
        int family = -1;

        switch ((unsigned int)endpoint->type) {
        case _transport_type_udp:
            type = SOCK_DGRAM;
            protocol = IPPROTO_UDP;
            break;
        case _transport_type_vdp:
            type = SOCK_DGRAM;
            protocol = IPPROTO_VDP;
            break;
        case _transport_type_tcp:
            type = SOCK_STREAM;
            protocol = IPPROTO_TCP;
            break;
        }

        switch (address->address_length) {
        case sizeof(address->address.ipv4_address):
            family = AF_INET;
            break;
        case sizeof(address->address.ipv6_address):
            family = 23;
            break;
        }

        endpoint->socket = socket(family, type, protocol);
    }

    if (endpoint->socket != -1) {
        endpoint->flags |= 16;
        success = true;
    } else {
        WSAGetLastError();
    }

    return success;
}

bool transport_endpoint_get_socket_address(const transport_address *address, long *socket_address_size, unsigned char *const socket_address)
{
    bool success = false;

    short address_length = address->address_length;
    *socket_address_size = 0;

    switch (address_length) {
    case sizeof(address->address.ipv4_address): {
        sockaddr_in *ipv4_address = (sockaddr_in *)socket_address;
        ipv4_address->sin_family = AF_INET;
        unsigned long sin_addr = address->address.ipv4_address;
        *socket_address_size = sizeof(sockaddr_in);
        ipv4_address->sin_addr.s_addr = sin_addr;
        ipv4_address->sin_port = address->port;
        success = true;
        break;
    }
    case sizeof(address->address.ipv6_address): {
        transport_sockaddr_in6 *ipv6_address = (transport_sockaddr_in6 *)socket_address;
        ipv6_address->sin6_family = 23;
        unsigned short first_address_word = address->address.ipv6_address[0];
        *socket_address_size = sizeof(transport_sockaddr_in6);
        ipv6_address->sin6_addr.word[0] = first_address_word;
        ipv6_address->sin6_addr.word[1] = address->address.ipv6_address[1];
        ipv6_address->sin6_addr.word[2] = address->address.ipv6_address[2];
        ipv6_address->sin6_addr.word[3] = address->address.ipv6_address[3];
        ipv6_address->sin6_addr.word[4] = address->address.ipv6_address[4];
        ipv6_address->sin6_addr.word[5] = address->address.ipv6_address[5];
        ipv6_address->sin6_addr.word[6] = address->address.ipv6_address[6];
        ipv6_address->sin6_addr.word[7] = address->address.ipv6_address[7];
        ipv6_address->sin6_port = address->port;
        success = true;
        break;
    }
    }

    return success;
}


// @address 0x4426a8 transport_endpoint_get_transport_address
// match: 0x4426a8: pointer __restrict qualifier prevents alias reload
bool transport_endpoint_get_transport_address(long socket_address_length, const unsigned char *const socket_address, transport_address *__restrict address)
{
    bool success = false;

    switch (socket_address_length)
    {
    case sizeof(sockaddr_in):
    {
        const sockaddr_in *ipv4_address = (const sockaddr_in *)socket_address;
        address->address.ipv4_address = ipv4_address->sin_addr.s_addr;
        address->port = ipv4_address->sin_port;
        address->address_length = sizeof(address->address.ipv4_address);
        success = true;
        break;
    }
    case sizeof(transport_sockaddr_in6):
    {
        const transport_sockaddr_in6 *ipv6_address = (const transport_sockaddr_in6 *)socket_address;
        address->address.ipv6_address[0] = ipv6_address->sin6_addr.word[0];
        address->address.ipv6_address[1] = ipv6_address->sin6_addr.word[1];
        address->address.ipv6_address[2] = ipv6_address->sin6_addr.word[2];
        address->address.ipv6_address[3] = ipv6_address->sin6_addr.word[3];
        address->address.ipv6_address[4] = ipv6_address->sin6_addr.word[4];
        address->address.ipv6_address[5] = ipv6_address->sin6_addr.word[5];
        address->address.ipv6_address[6] = ipv6_address->sin6_addr.word[6];
        address->address.ipv6_address[7] = ipv6_address->sin6_addr.word[7];
        address->port = ipv6_address->sin6_port;
        address->address_length = sizeof(address->address.ipv6_address);
        success = true;
        break;
    }
    default:
        memset(address, 0, sizeof(transport_address));
        break;
    }

    return success;
}
