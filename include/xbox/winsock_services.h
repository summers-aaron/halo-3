#ifndef XBOX_WINSOCK_SERVICES_H
#define XBOX_WINSOCK_SERVICES_H

extern "C" long WSAGetLastError(void);
extern "C" long bind(long socket, const unsigned char *address, long address_size);
extern "C" long setsockopt(long socket, long level, long option, const void *value, long value_size);

#endif // XBOX_WINSOCK_SERVICES_H
