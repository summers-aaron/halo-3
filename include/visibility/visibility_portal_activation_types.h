#ifndef VISIBILITY_PORTAL_ACTIVATION_TYPES_H
#define VISIBILITY_PORTAL_ACTIVATION_TYPES_H

// Sixteen BSPs, each with 512 portal activation bits.
extern "C" { extern __declspec(thread) unsigned long *g_active_portal_bitvectors; }

#include "cache/restricted_memory_regions.h"
void __tls_set_g_active_portal_bitvectors_allocator(void *value);
typedef t_restricted_allocation_manager<3, static_cast<void (*)(void *)>(0), static_cast<void (*)(void *)>(0),
    &__tls_set_g_active_portal_bitvectors_allocator> active_portal_bitvectors_allocation_manager;

#endif
