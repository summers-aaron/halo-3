#ifndef PORT_ZLIB_HEAP_H
#define PORT_ZLIB_HEAP_H

#if defined(__linux__) && !defined(_WIN32)
#include <stddef.h>
#include "port_abi.h"

extern "C" {
void *PORT_MS_ABI GetProcessHeap(void);
void *PORT_MS_ABI RtlAllocateHeap(void *heap, unsigned int flags, size_t bytes);
int PORT_MS_ABI RtlFreeHeap(void *heap, unsigned int flags, void *address);
}
#else
#include <xtl.h>
#endif

#endif
