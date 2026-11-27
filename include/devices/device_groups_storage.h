#ifndef DEVICES_DEVICE_GROUPS_STORAGE_H
#define DEVICES_DEVICE_GROUPS_STORAGE_H

// Concrete allocator storage; callers access its c_allocation_base interface.
#include "cache/restricted_memory.h"

typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_device_groups_data_allocator> c_device_groups_data_allocator;
extern c_device_groups_data_allocator g_device_groups_data_allocator;

#endif // DEVICES_DEVICE_GROUPS_STORAGE_H
