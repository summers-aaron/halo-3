#ifndef NETWORKING_NETWORK_MEMORY_STORAGE_H
#define NETWORKING_NETWORK_MEMORY_STORAGE_H

// Byte-addressable shared heap storage; preserve the queue's alias behavior.
extern "C" { extern char network_shared_memory_storage[]; }

#endif
